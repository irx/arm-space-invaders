# 2021, Maksymilian Mruszczak
# jpeg2c make file

.PHONY: all clean install

PREFIX = /usr/local

CC = cc
CFLAGS = -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE \
	 -Wno-deprecated-declarations

I_FLAGS = -I/usr/local/include
L_FLAGS = -L/usr/local/lib
LD_FLAGS = -ljpeg

BIN = jpeg2c
OBJ = libimgio.o main.o
HDR = imgiolib.h

all: ${BIN}

${BIN}: ${OBJ}
	@echo LD $@
	@${CC} -o $@ ${CFLAGS} ${L_FLAGS} ${LD_FLAGS} ${OBJ}

.c.o: ${HDR}
	@echo CC $<
	@${CC} -c ${CFLAGS} ${I_FLAGS} $<

install: all
	@echo installing to ${PREFIX}
	mkdir -p ${PREFIX}/bin
	cp ${BIN} ${PREFIX}/bin/${BIN}

clean:
	rm ${BIN} ${OBJ}
