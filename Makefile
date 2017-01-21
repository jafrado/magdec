CFLAGS = -O2
CC = gcc

default: magdec magdec_full

magdec: magdec.c
	$(CC) $(CFLAGS) magdec.c -o $@

magdec_full: magdec.c earth.c
	$(CC) $(CFLAGS) -DFULL_TABLE magdec.c earth.c -o $@


clean:
	rm -f *.o magdec magdec_full
