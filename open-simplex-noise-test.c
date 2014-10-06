#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <png.h>

#include "open-simplex-noise.h"

#define WIDTH 512
#define HEIGHT 512
#define FEATURE_SIZE 24

static int write_png_image(const char *filename, unsigned char *pixels, int w, int h, int has_alpha)
{
	png_structp png_ptr;
	png_infop info_ptr;
	png_byte **row;
	int x, y, rc, colordepth = 8;
	int bytes_per_pixel = has_alpha ? 4 : 3;
	FILE *f;

	f = fopen(filename, "w");
	if (!f) {
		fprintf(stderr, "fopen: %s:%s\n", filename, strerror(errno));
		return -1;
	}
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
		goto cleanup1;
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
		goto cleanup2;
	if (setjmp(png_jmpbuf(png_ptr))) /* oh libpng, you're old as dirt, aren't you. */
		goto cleanup2;

	png_set_IHDR(png_ptr, info_ptr, (size_t) w, (size_t) h, colordepth,
			has_alpha ? PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB,
			PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
			PNG_FILTER_TYPE_DEFAULT);

	row = png_malloc(png_ptr, h * sizeof(*row));
	for (y = 0; y < h; y++) {
		row[y] = png_malloc(png_ptr, w * bytes_per_pixel);
		for (x = 0; x < w; x++) {
			unsigned char *r = (unsigned char *) row[y];
			unsigned char *src = (unsigned char *)
				&pixels[y * w * bytes_per_pixel + x * bytes_per_pixel];
			unsigned char *dest = &r[x * bytes_per_pixel];
			memcpy(dest, src, bytes_per_pixel);
		}
	}

	png_init_io(png_ptr, f);
	png_set_rows(png_ptr, info_ptr, row);
	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_PACKING, NULL);

	for (y = 0; y < h; y++)
		png_free(png_ptr, row[y]);
	png_free(png_ptr, row);
	rc = 0;
cleanup2:
	png_destroy_write_struct(&png_ptr, &info_ptr);
cleanup1:
	fclose(f);
	return rc;
}

int main(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[])
{
	int x, y;
	double value;
	uint32_t rgb;
	uint32_t image[HEIGHT][WIDTH];
	struct osn_context *ctx;

	open_simplex_noise(77374, &ctx);

	for (y = 0; y < HEIGHT; y++) {
		for (x = 0; x < WIDTH; x++) {
			//value = open_simplex_noise4(ctx, (double) x / FEATURE_SIZE, (double) y / FEATURE_SIZE, 0.0, 0.0);
			//value = open_simplex_noise2(ctx, (double) x / FEATURE_SIZE, (double) y / FEATURE_SIZE);
			value = open_simplex_noise3(ctx, (double) x / FEATURE_SIZE, (double) y / FEATURE_SIZE, 0.0);
			rgb = 0x010101 * (uint32_t) ((value + 1) * 127.5);
			image[y][x] = (0x0ff << 24) | (rgb);
		}
	}
	write_png_image("test.png", (unsigned char *) image, WIDTH, HEIGHT, 1);
	return 0;
}

