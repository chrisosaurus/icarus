.POSIX:

include config.mk

SRC =
OBJ = ${SRC:.c=.o}

EXTRAFLAGS =

# default to compiling without debug symbols
all: icarus

%.o: %.c
	@echo COMPILING CC $<
	@${CC} -g -c ${CFLAGS} $< ${EXTRAFLAGS} -o $@

icarus: ${OBJ}
	@echo more compiling CC -o $@
	@${CC} src/main.c -o $@ ${LDFLAGS} ${OBJ}
	@make -s cleanobj

# build icarus with debug output
debug:
	@make -s EXTRAFLAGS="-DICARUS_DEBUG" icarus

cleanobj:
	@echo cleaning objects and temporary files
	@rm -f src/*.o src/*.to t/*.o t/component/*.o src/*.su test

clean: cleanobj
	@echo cleaning executables
	@rm -f icarus

.PHONY: all clean cleanobj icarus

