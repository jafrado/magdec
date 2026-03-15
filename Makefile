CFLAGS = -O2 -Werror
CC = gcc

default: magdec magdec2

magdec: magdec.c earth.c test.c
	$(CC) $(CFLAGS) test.c magdec.c earth2026.c -o $@

magdec2: magdec2.c wmmcof2025.c
	$(CC) $(CFLAGS) -std=c99 magdec2.c wmmcof2025.c -lm -o $@

all: magdec magdec2

clean:
	rm -f *.o magdec magdec2
