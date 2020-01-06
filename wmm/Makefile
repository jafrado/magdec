CC = gcc
CFLAGS = -g 
CFLAGS = -g -O2 -Wall -W
LDFLAGS =
LIBS = -lm

all: wmm_nav wmm_all

wmm_nav: wmm_nav.o GeomagnetismLibrary.o
	${CC} ${LDFLAGS} -o $@ wmm_nav.o GeomagnetismLibrary.o ${LIBS}

wmm_all: wmm_globe.o GeomagnetismLibrary.o
	${CC} ${LDFLAGS} -o $@ wmm_globe.o GeomagnetismLibrary.o ${LIBS}

tags:
	etags -a *.c *.h

clean:
	rm -f *.o
	rm -f wmm_nav wmm_all core TAGS
