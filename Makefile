CFLAGS = -O2 -Werror
CC = gcc

default: magdec 

magdec: magdec.c earth.c
	$(CC) $(CFLAGS) magdec.c earth.c earth2020.c -o $@

clean:
	rm -f *.o magdec 
