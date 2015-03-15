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
	@rm -rf bin
	@echo removing gcov files
	@find . -iname '*.gcda' -delete
	@find . -iname '*.gcov' -delete
	@find . -iname '*.gcno' -delete

example: icarus
	./icarus example/simple.ic

test: run_tests

run_tests: compile_tests
	@echo "\n\nrunning test_read"
	./bin/test_read
	@echo "\n\nrunning test_parray"
	./bin/test_parray
	@echo "\n\nrunning test_carray"
	./bin/test_carray
	@echo "\n\nrunning test_pvector"
	./bin/test_pvector
	@echo "\n\nrunning test_string"
	./bin/test_string
	@echo "\n\nrunning test_symbol"
	./bin/test_symbol
	@echo "\n\nrunning test_field"
	./bin/test_field
	@echo "\n\nrunning test_tdecl"
	./bin/test_tdecl
	@echo "\n\nrunning test_fdecl"
	./bin/test_fdecl
	@echo "\n\nrunning test_decl"
	./bin/test_decl
	@echo "\n\nrunning test_ast"
	./bin/test_ast
	@echo "\n"

compile_tests: clean ${OBJ}
	@echo "making bin directory for testing binaries"
	@mkdir bin
	@echo "compiling tests"
	@${CC} t/unit/test_read.c -o bin/test_read ${LDFLAGS} ${OBJ}
	@${CC} t/unit/test_parray.c -o bin/test_parray ${LDFLAGS} ${OBJ}
	@${CC} t/unit/test_carray.c -o bin/test_carray ${LDFLAGS} ${OBJ}
	@${CC} t/unit/test_pvector.c -o bin/test_pvector ${LDFLAGS} ${OBJ}
	@${CC} t/unit/test_string.c -o bin/test_string ${LDFLAGS} ${OBJ}
	@${CC} t/unit/test_symbol.c -o bin/test_symbol ${LDFLAGS} ${OBJ}
	@${CC} t/unit/test_field.c -o bin/test_field ${LDFLAGS} ${OBJ}
	@${CC} t/unit/test_tdecl.c -o bin/test_tdecl ${LDFLAGS} ${OBJ}
	@${CC} t/unit/test_fdecl.c -o bin/test_fdecl ${LDFLAGS} ${OBJ}
	@${CC} t/unit/test_decl.c -o bin/test_decl ${LDFLAGS} ${OBJ}
	@${CC} t/unit/test_ast.c -o bin/test_ast ${LDFLAGS} ${OBJ}
	@make -s cleanobj

.PHONY: all clean cleanobj icarus test example

