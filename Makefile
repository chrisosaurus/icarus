.POSIX:

include config.mk

SRC = $(shell find src -name '*.c' | grep -v src/main.c)
OBJ = ${SRC:.c=.o}

EXTRAFLAGS =

# default to compiling without debug symbols
all: icarus

%.o: %.c
	@echo COMPILING CC $< with extra flags \"${EXTRAFLAGS}\"
	@${CC} -g -c ${CFLAGS} $< ${EXTRAFLAGS} -o $@

icarus: ${OBJ}
	@echo more compiling CC -o $@ with extra flags \"${EXTRAFLAGS}\"
	@${CC} src/main.c -o $@ ${LDFLAGS} ${EXTRAFLAGS} ${OBJ}
	@make -s cleanobj

# build icarus with debug output
debug:
	@make -s EXTRAFLAGS="${DEBUGFLAGS}" icarus

cleanobj:
	@echo cleaning objects and temporary files
	@rm -f src/*.o src/*.to t/*.o t/component/*.o src/*.su test

clean: cleanobj
	@echo cleaning executables
	@rm -f icarus
	@echo cleaning tests
	@rm -f test_read test_parray

example: icarus
	./icarus example/simple.ic

test: run_tests

run_tests: compile_tests
	@echo "running test_read"
	./test_read
	@echo "running test_parray"
	./test_parray

compile_tests: clean ${OBJ}
	@echo "compiling tests"
	@${CC} t/unit/test_read.c -o test_read ${LDFLAGS} ${OBJ}
	@${CC} t/unit/test_parray.c -o test_parray ${LDFLAGS} ${OBJ}
	@make -s cleanobj

.PHONY: all clean cleanobj icarus test example

