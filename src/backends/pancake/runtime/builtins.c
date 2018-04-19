#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtins.h"

/* all functions in here take the same single argument to match a single
 * interface, so there is no use in warning on unused-param
 */
#pragma GCC diagnostic ignored "-Wunused-parameter"
/* some functions pop unnecessary values for the side effects (e.g. the Unit functions) */
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

void panic(char *ch);
void panic(char *ch) {
    puts("Panic!");
    if (ch) {
        puts(ch);
    }
    exit(1);
}

#define INIT(type)                              \
    struct ic_backend_pancake_value *value = 0; \
    if (!value_stack) {                         \
        puts("value_stack was null");           \
        return 0;                               \
    }

#define READ(name, type)                                           \
    value = ic_backend_pancake_value_stack_peek(value_stack);      \
    if (!value) {                                                  \
        puts("stack_peek failed");                                 \
        return 0;                                                  \
    }                                                              \
    if (value->tag != ic_backend_pancake_value_type_##type) {      \
        fputs("read: value was not of expected type\n, found:", stdout); \
        ic_backend_pancake_value_print(stdout, value);             \
        return 0;                                                  \
    }                                                              \
    if (!ic_backend_pancake_value_stack_pop(value_stack)) {        \
        puts("stack_pop failed");                                  \
        return 0;                                                  \
    }                                                              \
    name = value->u.type;

#define RESULT(result, type)                                       \
    value = ic_backend_pancake_value_stack_push(value_stack);      \
    if (!value) {                                                  \
        puts("stack_push failed");                                 \
        return 0;                                                  \
    }                                                              \
    value->tag = ic_backend_pancake_value_type_##type;             \
    value->u.type = result;

/* TODO FIXME we probably never want to actually do this */
#define PUSH_UNIT()                                                \
    value = ic_backend_pancake_value_stack_push(value_stack);      \
    if (!value) {                                                  \
        puts("stack_push failed");                                 \
        return 0;                                                  \
    }                                                              \
    value->tag = ic_backend_pancake_value_type_unit;

#define CONSUME_UNIT()                                             \
    value = ic_backend_pancake_value_stack_peek(value_stack);      \
    if (!value) {                                                  \
        puts("stack_peek failed");                                 \
        return 0;                                                  \
    }                                                              \
    if (value->tag != ic_backend_pancake_value_type_unit) {        \
        fputs("consume_unit: value was not of expected type 'unit'\n, found:", stdout); \
        ic_backend_pancake_value_print(stdout, value);             \
        return 0;                                                  \
    }                                                              \

unsigned int Unit(struct ic_backend_pancake_value_stack *value_stack);
unsigned int print_string(struct ic_backend_pancake_value_stack *value_stack);
unsigned int print_signed(struct ic_backend_pancake_value_stack *value_stack);
unsigned int print_unsigned(struct ic_backend_pancake_value_stack *value_stack);
unsigned int print_bool(struct ic_backend_pancake_value_stack *value_stack);
unsigned int print_unit(struct ic_backend_pancake_value_stack *value_stack);
unsigned int println(struct ic_backend_pancake_value_stack *value_stack);
unsigned int println_string(struct ic_backend_pancake_value_stack *value_stack);
unsigned int println_signed(struct ic_backend_pancake_value_stack *value_stack);
unsigned int println_unsigned(struct ic_backend_pancake_value_stack *value_stack);
unsigned int println_bool(struct ic_backend_pancake_value_stack *value_stack);
unsigned int println_unit(struct ic_backend_pancake_value_stack *value_stack);
unsigned int plus_unsigned_unsigned(struct ic_backend_pancake_value_stack *value_stack);
unsigned int plus_signed_signed(struct ic_backend_pancake_value_stack *value_stack);
unsigned int minus_unsigned_unsigned(struct ic_backend_pancake_value_stack *value_stack);
unsigned int minus_signed_signed(struct ic_backend_pancake_value_stack *value_stack);
unsigned int multiply_unsigned_unsigned(struct ic_backend_pancake_value_stack *value_stack);
unsigned int multiply_signed_signed(struct ic_backend_pancake_value_stack *value_stack);
unsigned int lessthan_equal_unsigned_unsigned(struct ic_backend_pancake_value_stack *value_stack);
unsigned int lessthan_equal_signed_signed(struct ic_backend_pancake_value_stack *value_stack);
unsigned int greaterthan_equal_unsigned_unsigned(struct ic_backend_pancake_value_stack *value_stack);
unsigned int greaterthan_equal_signed_signed(struct ic_backend_pancake_value_stack *value_stack);
unsigned int lessthan_unsigned_unsigned(struct ic_backend_pancake_value_stack *value_stack);
unsigned int lessthan_signed_signed(struct ic_backend_pancake_value_stack *value_stack);
unsigned int greaterthan_unsigned_unsigned(struct ic_backend_pancake_value_stack *value_stack);
unsigned int greaterthan_signed_signed(struct ic_backend_pancake_value_stack *value_stack);
unsigned int to_str_bool(struct ic_backend_pancake_value_stack *value_stack);
unsigned int to_str_unit(struct ic_backend_pancake_value_stack *value_stack);
unsigned int length_string(struct ic_backend_pancake_value_stack *value_stack);
unsigned int equal_signed_signed(struct ic_backend_pancake_value_stack *value_stack);
unsigned int equal_unsigned_unsigned(struct ic_backend_pancake_value_stack *value_stack);
unsigned int equal_signed_unsigned(struct ic_backend_pancake_value_stack *value_stack);
unsigned int equal_unsigned_signed(struct ic_backend_pancake_value_stack *value_stack);
unsigned int equal_unit_unit(struct ic_backend_pancake_value_stack *value_stack);
unsigned int concat_string_string(struct ic_backend_pancake_value_stack *value_stack);
unsigned int modulo_signed_signed(struct ic_backend_pancake_value_stack *value_stack);
unsigned int assert_bool(struct ic_backend_pancake_value_stack *value_stack);

#define ic_backend_pancake_builtins_table_len 37

/* table mapping user-land names to internal names */
struct ic_backend_pancake_builtins_table_type {
    char *str;
    unsigned int (*func)(struct ic_backend_pancake_value_stack *value_stack);
} ic_backend_pancake_builtins_table[ic_backend_pancake_builtins_table_len] = {
    {"Unit()", Unit},
    {"print(String)", print_string},
    {"print(Unsigned)", print_unsigned},
    {"print(Signed)", print_signed},
    {"print(Bool)", print_bool},
    {"print(Unit)", print_unit},
    {"println()", println},
    {"println(String)", println_string},
    {"println(Unsigned)", println_unsigned},
    {"println(Signed)", println_signed},
    {"println(Bool)", println_bool},
    {"println(Unit)", println_unit},
    {"plus(Unsigned,Unsigned)", plus_unsigned_unsigned},
    {"plus(Signed,Signed)", plus_signed_signed},
    {"minus(Unsigned,Unsigned)", minus_unsigned_unsigned},
    {"minus(Signed,Signed)", minus_signed_signed},
    {"multiply(Unsigned,Unsigned)", multiply_unsigned_unsigned},
    {"multiply(Signed,Signed)", multiply_signed_signed},
    {"lessthan(Unsigned,Unsigned)", lessthan_unsigned_unsigned},
    {"lessthan(Signed,Signed)", lessthan_signed_signed},
    {"greaterthan(Unsigned,Unsigned)", greaterthan_unsigned_unsigned},
    {"greaterthan(Signed,Signed)", greaterthan_signed_signed},
    {"lessthan_equal(Unsigned,Unsigned)", lessthan_equal_unsigned_unsigned},
    {"lessthan_equal(Signed,Signed)", lessthan_equal_signed_signed},
    {"greaterthan_equal(Unsigned,Unsigned)", greaterthan_equal_unsigned_unsigned},
    {"greaterthan_equal(Signed,Signed)", greaterthan_equal_signed_signed},
    {"to_str(Bool)", to_str_bool},
    {"to_str(Unit)", to_str_unit},
    {"length(String)", length_string},
    {"equal(Signed,Signed)", equal_signed_signed},
    {"equal(Unsigned,Unsigned)", equal_unsigned_unsigned},
    {"equal(Unsigned,Signed)", equal_unsigned_signed},
    {"equal(Signed,Unsigned)", equal_signed_unsigned},
    {"equal(Unit,Unit)", equal_unit_unit},
    {"concat(String,String)", concat_string_string},
    {"modulo(Signed,Signed)", modulo_signed_signed},
    {"assert(Bool)", assert_bool},
};

/* alloc size bytes
 *
 * bytes can be 0
 *
 * returns * on success
 * panics on failure (doesn't return, exits)
 */
void *ic_alloc(size_t size) {
    void *v = 0;

    /* malloc/calloc allow a size of 0 but state
     * "if size is 0, then malloc() returns either NULL, or a unique pointer
     * value that can later be successfully passed to free()."
     *
     * we want to guarantee the second behavior
     * so we default to a minimum size of 1
     */
    if (size == 0) {
        size = 1;
    }

    v = calloc(1, size);
    if (!v) {
        panic("ic_alloc: call to calloc failed");
        exit(1);
    }

    return v;
}

/* get builtin function for user-land name
 *
 * returns * on success
 * returns 0 on failure
 */
ic_backend_function_sig ic_backend_pancake_builtins_table_get(char *str) {
    ic_backend_function_sig ret = 0;
    unsigned int i = 0;
    unsigned int len = 0;
    char *candidate_str = 0;

    if (!str) {
        puts("ic_backend_pancake_builtins_table_get: str was null");
        return 0;
    }

    len = ic_backend_pancake_builtins_table_len;

    for (i = 0; i < len; ++i) {
        candidate_str = ic_backend_pancake_builtins_table[i].str;
        if (!strcmp(str, candidate_str)) {
            /* found a string */
            ret = ic_backend_pancake_builtins_table[i].func;
            break;
        }
    }

    return ret;
}

/* Push a new Unit onto the stack
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int Unit(struct ic_backend_pancake_value_stack *value_stack) {
    INIT();

    /* TODO FIXME we probably never want to actually do this */
    PUSH_UNIT();

    return 1;
}

/* print a string
 *
 * pops string from value_stack
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int print_string(struct ic_backend_pancake_value_stack *value_stack) {
    char *str = 0;
    INIT();

    READ(str, string);

    fputs(str, stdout);

    return 1;
}

/* print a uint
 *
 * pops unsigned_integer from value_stack
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int print_unsigned(struct ic_backend_pancake_value_stack *value_stack) {
    unsigned int unsigned_integer = 0;
    INIT();

    READ(unsigned_integer, unsigned_integer);

    printf("%u", unsigned_integer);

    return 1;
}

/* print a sint
 *
 * pops signed_integer from value_stack
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int print_signed(struct ic_backend_pancake_value_stack *value_stack) {
    int signed_integer = 0;
    INIT();

    READ(signed_integer, signed_integer);

    printf("%d", signed_integer);

    return 1;
}

/* print a bool
 *
 * pops bool from value_stack
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int print_bool(struct ic_backend_pancake_value_stack *value_stack) {
    bool boolean = 0;
    INIT();

    READ(boolean, boolean);

    if (boolean) {
        fputs("True", stdout);
    } else {
        fputs("False", stdout);
    }

    return 1;
}

/* print a unit
 *
 * pops unit from value_stack
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int print_unit(struct ic_backend_pancake_value_stack *value_stack) {
    INIT();

    CONSUME_UNIT();

    fputs("Unit()", stdout);

    return 1;
}

/* print a \n
 *
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int println(struct ic_backend_pancake_value_stack *value_stack) {
    /* ignores argument, only needed to match interface */
    puts("");

    return 1;
}

/* print a string followed by a \n
 *
 * pops string from value_stack
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int println_string(struct ic_backend_pancake_value_stack *value_stack) {
    if (!print_string(value_stack)) {
        return 0;
    }
    puts("");

    return 1;
}

/* print a unsigned_integer followed by a \n
 *
 * pops unsigned_integer from value_stack
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int println_unsigned(struct ic_backend_pancake_value_stack *value_stack) {
    if (!print_unsigned(value_stack)) {
        return 0;
    }
    puts("");

    return 1;
}

/* print a signed_integer followed by a \n
 *
 * pops signed_integer from value_stack
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int println_signed(struct ic_backend_pancake_value_stack *value_stack) {
    if (!print_signed(value_stack)) {
        return 0;
    }
    puts("");

    return 1;
}

/* print a bool followed by a \n
 *
 * pops bool from value_stack
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int println_bool(struct ic_backend_pancake_value_stack *value_stack) {
    if (!print_bool(value_stack)) {
        return 0;
    }
    puts("");

    return 1;
}

/* print a unit followed by a \n
 *
 * pops unit from value_stack
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int println_unit(struct ic_backend_pancake_value_stack *value_stack) {
    if (!print_unit(value_stack)) {
        return 0;
    }
    puts("");

    return 1;
}

/* add two uints
 *
 * pops 2 uints from stack
 * pushes return back on
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int plus_unsigned_unsigned(struct ic_backend_pancake_value_stack *value_stack) {
    unsigned int unsigned_integer_one = 0;
    unsigned int unsigned_integer_two = 0;
    unsigned int answer = 0;
    INIT();

    READ(unsigned_integer_two, unsigned_integer);
    READ(unsigned_integer_one, unsigned_integer);

    answer = unsigned_integer_one + unsigned_integer_two;

    RESULT(answer, unsigned_integer);

    return 1;
}

/* add two sints
 *
 * pops 2 sints from stack
 * pushes return back on
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int plus_signed_signed(struct ic_backend_pancake_value_stack *value_stack) {
    int signed_integer_one = 0;
    int signed_integer_two = 0;
    int answer = 0;
    INIT();

    READ(signed_integer_two, signed_integer);
    READ(signed_integer_one, signed_integer);

    answer = signed_integer_one + signed_integer_two;

    RESULT(answer, signed_integer);

    return 1;
}

/* subtract two uints
 *
 * pops 2 uints from stack
 * pushes return back on
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int minus_unsigned_unsigned(struct ic_backend_pancake_value_stack *value_stack) {
    unsigned int unsigned_integer_one = 0;
    unsigned int unsigned_integer_two = 0;
    unsigned int answer = 0;
    INIT();

    READ(unsigned_integer_two, unsigned_integer);
    READ(unsigned_integer_one, unsigned_integer);

    /* if we would rollover then stop at 0 */
    if (unsigned_integer_two > unsigned_integer_one) {
        answer = 0;
    } else {
        answer = unsigned_integer_one - unsigned_integer_two;
    }

    RESULT(answer, unsigned_integer);

    return 1;
}

/* subtract two sints
 *
 * pops 2 sints from stack
 * pushes return back on
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int minus_signed_signed(struct ic_backend_pancake_value_stack *value_stack) {
    int signed_integer_one = 0;
    int signed_integer_two = 0;
    int answer = 0;
    INIT();

    READ(signed_integer_two, signed_integer);
    READ(signed_integer_one, signed_integer);

    answer = signed_integer_one - signed_integer_two;

    RESULT(answer, signed_integer);

    return 1;
}

/* multiply two uints
 *
 * pops 2 uints from stack
 * pushes return back on
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int multiply_unsigned_unsigned(struct ic_backend_pancake_value_stack *value_stack) {
    unsigned int unsigned_integer_one = 0;
    unsigned int unsigned_integer_two = 0;
    unsigned int answer = 0;
    INIT();

    READ(unsigned_integer_two, unsigned_integer);
    READ(unsigned_integer_one, unsigned_integer);

    answer = unsigned_integer_one * unsigned_integer_two;

    RESULT(answer, unsigned_integer);

    return 1;
}

/* multiply two sints
 *
 * pops 2 sints from stack
 * pushes return back on
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int multiply_signed_signed(struct ic_backend_pancake_value_stack *value_stack) {
    int signed_integer_one = 0;
    int signed_integer_two = 0;
    int answer = 0;
    INIT();

    READ(signed_integer_two, signed_integer);
    READ(signed_integer_one, signed_integer);

    answer = signed_integer_one * signed_integer_two;

    RESULT(answer, signed_integer);

    return 1;
}

unsigned int lessthan_equal_unsigned_unsigned(struct ic_backend_pancake_value_stack *value_stack) {
    unsigned int unsigned_integer_one = 0;
    unsigned int unsigned_integer_two = 0;
    int answer = 0;
    INIT();

    READ(unsigned_integer_two, unsigned_integer);
    READ(unsigned_integer_one, unsigned_integer);

    answer = unsigned_integer_one <= unsigned_integer_two;

    RESULT(answer, boolean);
    return 1;
}

unsigned int lessthan_equal_signed_signed(struct ic_backend_pancake_value_stack *value_stack) {
    int signed_integer_one = 0;
    int signed_integer_two = 0;
    int answer = 0;
    INIT();

    READ(signed_integer_two, signed_integer);
    READ(signed_integer_one, signed_integer);

    answer = signed_integer_one <= signed_integer_two;

    RESULT(answer, boolean);
    return 1;
}

unsigned int greaterthan_equal_unsigned_unsigned(struct ic_backend_pancake_value_stack *value_stack) {
    unsigned int unsigned_integer_one = 0;
    unsigned int unsigned_integer_two = 0;
    int answer = 0;
    INIT();

    READ(unsigned_integer_two, unsigned_integer);
    READ(unsigned_integer_one, unsigned_integer);

    answer = unsigned_integer_one >= unsigned_integer_two;

    RESULT(answer, boolean);
    return 1;
}

unsigned int greaterthan_equal_signed_signed(struct ic_backend_pancake_value_stack *value_stack) {
    int signed_integer_one = 0;
    int signed_integer_two = 0;
    int answer = 0;
    INIT();

    READ(signed_integer_two, signed_integer);
    READ(signed_integer_one, signed_integer);

    answer = signed_integer_one >= signed_integer_two;

    RESULT(answer, boolean);
    return 1;
}

unsigned int lessthan_unsigned_unsigned(struct ic_backend_pancake_value_stack *value_stack) {
    unsigned int unsigned_integer_one = 0;
    unsigned int unsigned_integer_two = 0;
    int answer = 0;
    INIT();

    READ(unsigned_integer_two, unsigned_integer);
    READ(unsigned_integer_one, unsigned_integer);

    answer = unsigned_integer_one < unsigned_integer_two;

    RESULT(answer, boolean);
    return 1;
}

unsigned int lessthan_signed_signed(struct ic_backend_pancake_value_stack *value_stack) {
    int signed_integer_one = 0;
    int signed_integer_two = 0;
    int answer = 0;
    INIT();

    READ(signed_integer_two, signed_integer);
    READ(signed_integer_one, signed_integer);

    answer = signed_integer_one < signed_integer_two;

    RESULT(answer, boolean);
    return 1;
}

unsigned int greaterthan_unsigned_unsigned(struct ic_backend_pancake_value_stack *value_stack) {
    unsigned int unsigned_integer_one = 0;
    unsigned int unsigned_integer_two = 0;
    int answer = 0;
    INIT();

    READ(unsigned_integer_two, unsigned_integer);
    READ(unsigned_integer_one, unsigned_integer);

    answer = unsigned_integer_one > unsigned_integer_two;

    RESULT(answer, boolean);
    return 1;
}

unsigned int greaterthan_signed_signed(struct ic_backend_pancake_value_stack *value_stack) {
    int signed_integer_one = 0;
    int signed_integer_two = 0;
    int answer = 0;
    INIT();

    READ(signed_integer_two, signed_integer);
    READ(signed_integer_one, signed_integer);

    answer = signed_integer_one > signed_integer_two;

    RESULT(answer, boolean);
    return 1;
}

unsigned int to_str_bool(struct ic_backend_pancake_value_stack *value_stack) {
    unsigned int boolean = 0;
    char *answer = 0;
    INIT();

    READ(boolean, boolean);

    if (boolean) {
        answer = "True";
    } else {
        answer = "False";
    }

    RESULT(answer, string);
    return 1;
}

unsigned int to_str_unit(struct ic_backend_pancake_value_stack *value_stack) {
    char *answer = 0;
    INIT();

    answer = "";

    RESULT(answer, string);
    return 1;
}

unsigned int length_string(struct ic_backend_pancake_value_stack *value_stack) {
    char *str = 0;
    unsigned int answer = 0;
    INIT();

    READ(str, string);

    answer = strlen(str);
    RESULT(answer, unsigned_integer);
    return 1;
}

unsigned int equal_signed_signed(struct ic_backend_pancake_value_stack *value_stack) {
    int signed_integer_one = 0;
    int signed_integer_two = 0;
    bool answer = 0;
    INIT();

    READ(signed_integer_two, signed_integer);
    READ(signed_integer_one, signed_integer);

    answer = signed_integer_one == signed_integer_two;

    RESULT(answer, boolean);
    return 1;
}

unsigned int equal_unsigned_unsigned(struct ic_backend_pancake_value_stack *value_stack) {
    int unsigned_integer_one = 0;
    int unsigned_integer_two = 0;
    bool answer = 0;
    INIT();

    READ(unsigned_integer_two, unsigned_integer);
    READ(unsigned_integer_one, unsigned_integer);

    answer = unsigned_integer_one == unsigned_integer_two;

    RESULT(answer, boolean);
    return 1;
}

unsigned int equal_signed_unsigned(struct ic_backend_pancake_value_stack *value_stack) {
    unsigned int unsigned_integer = 0;
    int signed_integer = 0;
    int answer = 0;
    unsigned int tmp = 0;
    INIT();

    READ(unsigned_integer, unsigned_integer);
    READ(signed_integer, signed_integer);

    if (signed_integer < 0) {
        answer = 0;
    } else {
        tmp = signed_integer;
        if (tmp == unsigned_integer) {
            answer = 1;
        } else {
            answer = 0;
        }
    }

    RESULT(answer, boolean);
    return 1;
}

unsigned int equal_unsigned_signed(struct ic_backend_pancake_value_stack *value_stack) {
    int signed_integer = 0;
    unsigned int unsigned_integer = 0;
    int answer = 0;
    unsigned int tmp = 0;
    INIT();

    READ(signed_integer, signed_integer);
    READ(unsigned_integer, unsigned_integer);

    if (signed_integer < 0) {
        answer = 0;
    } else {
        tmp = signed_integer;
        if (tmp == unsigned_integer) {
            answer = 1;
        } else {
            answer = 0;
        }
    }

    RESULT(answer, boolean);
    return 1;
}

unsigned int equal_unit_unit(struct ic_backend_pancake_value_stack *value_stack) {
    int answer = 0;
    INIT();

    CONSUME_UNIT();
    CONSUME_UNIT();

    answer = 1;

    RESULT(answer, boolean);
    return 1;
}

unsigned int concat_string_string(struct ic_backend_pancake_value_stack *value_stack) {
    char *str1 = 0;
    char *str2 = 0;
    char *answer = 0;
    size_t len = 0;
    INIT();

    READ(str2, string);
    READ(str1, string);

    len = strlen(str1);
    len += strlen(str2);
    len += 1; /* \0 */

    answer = ic_alloc(len);

    strcpy(answer, str1);
    strcat(answer, str2);

    RESULT(answer, string);
    return 1;
}

unsigned int modulo_signed_signed(struct ic_backend_pancake_value_stack *value_stack) {
    int signed_integer_one = 0;
    int signed_integer_two = 0;
    int answer = 0;
    INIT();

    READ(signed_integer_two, signed_integer);
    READ(signed_integer_one, signed_integer);

    answer = signed_integer_one % signed_integer_two;

    RESULT(answer, signed_integer);
    return 1;
}

unsigned int assert_bool(struct ic_backend_pancake_value_stack *value_stack) {
    bool boolean = 0;
    INIT();

    READ(boolean, boolean);
    if (!boolean) {
        panic("assertion failed");
    }

    return 1;
}
