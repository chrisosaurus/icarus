.POSIX:

include config.mk

# search src/ and libs/ for c files
# ignore any path that contains '/test_' or 'src/main.c"
SRC = $(shell find src libs -name '*.c' | grep -v src/main.c | grep -v "/test_")
OBJ = ${SRC:.c=.o}

# tests live in t/ and libs
# tests must begin with test_
# we only want to automatically build and run t/unit and any tests in libs
# t/custom is managed manually by test_custom
TESTS = $(shell find t/unit libs -name 'test_*.c' )
# output location for tests
TESTOUT = bin
TESTO = $(patsubst %.c, $(TESTOUT)/%, $(TESTS))

# only defined when `make debug`
EXTRAFLAGS =

# default to compiling without debug symbols
all: icarus

# compile each component, linking is handled by `icarus` rule
%.o: %.c
	@echo COMPILING CC $< with extra flags \"${EXTRAFLAGS}\"
	@${CC} -g -c ${CFLAGS} $< ${EXTRAFLAGS} -o $@

# compile and link main executable
icarus: ${OBJ}
	@echo more compiling CC -o $@ with extra flags \"${EXTRAFLAGS}\"
	@${CC} src/main.c -o $@ ${LDFLAGS} ${EXTRAFLAGS} ${OBJ}

display-cc-version:
	@echo using compiler:
	${CC} -v

# build icarus with debug output
debug:
	@make -s EXTRAFLAGS="${DEBUGFLAGS}" icarus

# only clean up temporary files (.o)
cleanobj:
	@echo cleaning objects and temporary files
	@find . -iname '*.o' -delete

# clean up everything managed by make
clean: cleanobj
	@echo cleaning executables
	@rm -f icarus
	@echo cleaning tests
	@rm -rf $(TESTOUT)
	@echo removing gcov files
	@find . -iname '*.gcda' -delete
	@find . -iname '*.gcov' -delete
	@find . -iname '*.gcno' -delete
	@rm -f out.c

# run icarus over our simple example
example: icarus
	./icarus example/simple.ic

# clean
# build all OBJ
# run all TESTO
# cleanobj
#   this ensures that we do a fresh build every time
test: display-cc-version clean $(OBJ) $(TESTO) test_custom test_success test_milestones cleanobj


test_custom: $(OBJ) icarus
	@echo -e "\n\ncompiling t/custom/test_token_print.c to bin/t/custom/test_token_print"
	@mkdir -p `dirname bin/t/custom/test_token_print`
	@${CC} t/custom/test_token_print.c -o bin/t/custom/test_token_print ${CFLAGS} ${LDFLAGS} ${OBJ}
	@echo running test_token_print.pl
	t/custom/test_token_print.pl

	@echo running test_lex_simple.pl
	t/custom/test_lex_simple.pl

	@echo running test_parse_simple.pl
	t/custom/test_parse_simple.pl

	@echo running test_parse_union.pl
	t/custom/test_parse_union.pl

	@echo running test_parse_generics.pl
	t/custom/test_parse_generics.pl

	@echo -e "\n"
	@echo running test_arguments_version.pl
	t/custom/test_arguments_version.pl

	@echo -e "\n"
	@echo running test_arguments_help.pl
	t/custom/test_arguments_help.pl

	@echo -e "\n"
	@echo running test_analyse_simple.pl
	t/custom/test_analyse_simple.pl

	@echo -e "\n"
	@echo running test_analyse_union.pl
	t/custom/test_analyse_union.pl

	@echo -e "\n"
	@echo running test_analyse_invalid.pl
	t/custom/test_analyse_invalid.pl

	@echo -e "\n"
	@echo running test_transform_hello.pl
	t/custom/test_transform_hello.pl

	@echo -e "\n"
	@echo running test_backend_2c_hello.pl
	t/custom/test_backend_2c_hello.pl

	@echo -e "\n"
	@echo running test_backend_2c_simple.pl
	t/custom/test_backend_2c_simple.pl

	@echo -e "\n"
	@echo running test_backend_2c_end_to_end_output_only.pl
	t/custom/test_backend_2c_end_to_end_output_only.pl

	@echo -e "\n"
	@echo running test_backend_2c_end_to_end.pl
	t/custom/test_backend_2c_end_to_end.pl

	@echo -e "\n"
	@echo running test_backend_pancake_compile_hello.pl
	t/custom/test_backend_pancake_compile_hello.pl

	@echo -e "\n"
	@echo running test_backend_pancake_interpret_hello.pl
	t/custom/test_backend_pancake_interpret_hello.pl

	@echo running test_backend_pancake_interpret_instructions.pl
	t/custom/test_backend_pancake_interpret_instructions.pl

	@echo -e "\n"
	@echo running test_transform_simple.pl
	t/custom/test_transform_simple.pl

	@echo -e "\n"
	@echo running test_transform_generics.pl
	t/custom/test_transform_generics.pl

	@echo -e "\n"
	@echo running test_backend_pancake_end_to_end.pl
	t/custom/test_backend_pancake_end_to_end.pl

	@echo -e "\n"
	@echo running test_backend_pancake_end_to_end_output_only.pl
	t/custom/test_backend_pancake_end_to_end_output_only.pl

	@echo -e "\n"
	@echo running test_backends_output_equivalence.pl
	t/custom/test_backends_output_equivalence.pl

	@echo -e "\n"
	@echo running test_generics_progress.pl
	t/custom/test_generics_progress.pl

# compile and run each test
$(TESTO) : $(TESTOUT)/% : %.c
	@echo -e "\n\ncompiling $< to $@"
	@mkdir -p `dirname $@`
	@${CC} $< -o $@ ${LDFLAGS} ${OBJ}
	@echo running test
	$@

test_milestones:
	t/milestones/test_milestones.pl

# signal test success
test_success:
	@echo -e "\n\ntesting success\n"

compile: clean icarus
	rm -f out.c
	./icarus example/simple.ic out.c

format:
	find src/ t/ backends/ -iname '*.[ch]' | xargs clang-format -style=file -i

man:
	man -l icarus.1

analyze: clean
	scan-build make

update-libs: clean
	./scripts/update_libs.pl

.PHONY: all clean cleanobj test test_custom example compile format man analyze update-libs display-cc-version

