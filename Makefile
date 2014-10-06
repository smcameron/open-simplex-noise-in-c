all:	open-simplex-noise.o	open-simplex-noise-test

CFLAGS=-W -Wall -Wextra -O3
#CFLAGS=-W -Wall -Wextra -g

open-simplex-noise-test:	open-simplex-noise-test.c open-simplex-noise.o
	gcc ${CFLAGS} -o open-simplex-noise-test open-simplex-noise.o open-simplex-noise-test.c -lpng

open-simplex-noise.o:	open-simplex-noise.h open-simplex-noise.c Makefile
	gcc ${CFLAGS} -c open-simplex-noise.c

clean:
	rm -f open-simplex-noise.o open-simplex-noise-test test2d.png test3d.png test4d.png

