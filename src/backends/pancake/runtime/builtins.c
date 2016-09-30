#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtins.h"

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
        fputs("value was not of expected type\n, found:", stdout); \
        ic_backend_pancake_value_print(stdout, value);             \
        return 0;                                                  \
    }                                                              \
    if (!ic_backend_pancake_value_stack_pop(value_stack)) {        \
        puts("stack_pop failed");                                  \
        return 0;                                                  \
    }                                                              \
    name = value->u.type;

#define RESULT(result, type)                                  \
    value = ic_backend_pancake_value_stack_push(value_stack); \
    if (!value) {                                             \
        puts("stack_push failed");                            \
        return 0;                                             \
    }                                                         \
    value->tag = ic_backend_pancake_value_type_##type;        \
    value->u.type = result;

unsigned int i_println_string(struct ic_backend_pancake_value_stack *value_stack);
unsigned int i_println_sint(struct ic_backend_pancake_value_stack *value_stack);
unsigned int i_println_uint(struct ic_backend_pancake_value_stack *value_stack);
unsigned int i_println_bool(struct ic_backend_pancake_value_stack *value_stack);
unsigned int i_plus_uint_uint(struct ic_backend_pancake_value_stack *value_stack);
unsigned int i_plus_sint_sint(struct ic_backend_pancake_value_stack *value_stack);
unsigned int i_minus_uint_uint(struct ic_backend_pancake_value_stack *value_stack);
unsigned int i_minus_sint_sint(struct ic_backend_pancake_value_stack *value_stack);
unsigned int i_lessthan_equal_uint_uint(struct ic_backend_pancake_value_stack *value_stack);
unsigned int i_lessthan_equal_sint_sint(struct ic_backend_pancake_value_stack *value_stack);
unsigned int i_greaterthan_equal_uint_uint(struct ic_backend_pancake_value_stack *value_stack);
unsigned int i_greaterthan_equal_sint_sint(struct ic_backend_pancake_value_stack *value_stack);
unsigned int i_lessthan_uint_uint(struct ic_backend_pancake_value_stack *value_stack);
unsigned int i_lessthan_sint_sint(struct ic_backend_pancake_value_stack *value_stack);
unsigned int i_greaterthan_uint_uint(struct ic_backend_pancake_value_stack *value_stack);
unsigned int i_greaterthan_sint_sint(struct ic_backend_pancake_value_stack *value_stack);
unsigned int i_to_str_bool(struct ic_backend_pancake_value_stack *value_stack);
unsigned int i_length_string(struct ic_backend_pancake_value_stack *value_stack);
unsigned int i_equal_sint_sint(struct ic_backend_pancake_value_stack *value_stack);
unsigned int i_equal_uint_uint(struct ic_backend_pancake_value_stack *value_stack);
unsigned int i_equal_sint_uint(struct ic_backend_pancake_value_stack *value_stack);
unsigned int i_equal_uint_sint(struct ic_backend_pancake_value_stack *value_stack);
unsigned int i_concat_string_string(struct ic_backend_pancake_value_stack *value_stack);
unsigned int i_modulo_sint_sint(struct ic_backend_pancake_value_stack *value_stack);

#define ic_backend_pancake_builtins_table_len 24

/* table mapping user-land names to internal names */
struct ic_backend_pancake_builtins_table_type {
    char *str;
    unsigned int (*func)(struct ic_backend_pancake_value_stack *value_stack);
} ic_backend_pancake_builtins_table[ic_backend_pancake_builtins_table_len] = {
    {"println(String)", i_println_string},
    {"println(Uint)", i_println_uint},
    {"println(Sint)", i_println_sint},
    {"println(Bool)", i_println_bool},
    {"plus(Uint,Uint)", i_plus_uint_uint},
    {"plus(Sint,Sint)", i_plus_sint_sint},
    {"minus(Uint,Uint)", i_minus_uint_uint},
    {"minus(Sint,Sint)", i_minus_sint_sint},
    {"lessthan(Uint,Uint)", i_lessthan_uint_uint},
    {"lessthan(Sint,Sint)", i_lessthan_sint_sint},
    {"greaterthan(Uint,Uint)", i_greaterthan_uint_uint},
    {"greaterthan(Sint,Sint)", i_greaterthan_sint_sint},
    {"lessthan_equal(Uint,Uint)", i_lessthan_equal_uint_uint},
    {"lessthan_equal(Sint,Sint)", i_lessthan_equal_sint_sint},
    {"greaterthan_equal(Uint,Uint)", i_greaterthan_equal_uint_uint},
    {"greaterthan_equal(Sint,Sint)", i_greaterthan_equal_sint_sint},
    {"to_str(Bool)", i_to_str_bool},
    {"length(String)", i_length_string},
    {"equal(Sint,Sint)", i_equal_sint_sint},
    {"equal(Uint,Uint)", i_equal_uint_uint},
    {"equal(Uint,Sint)", i_equal_uint_sint},
    {"equal(Sint,Uint)", i_equal_sint_uint},
    {"concat(String,String)", i_concat_string_string},
    {"modulo(Sint,Sint)", i_modulo_sint_sint},
};

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

/* print a string followed by a \n
 *
 * pops string from value_stack
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int i_println_string(struct ic_backend_pancake_value_stack *value_stack) {
    char *str = 0;
    INIT();

    READ(str, string);

    puts(str);
    return 1;
}

/* print a uint followed by a \n
 *
 * pops uint from value_stack
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int i_println_uint(struct ic_backend_pancake_value_stack *value_stack) {
    unsigned int uint = 0;
    INIT();

    READ(uint, uint);

    printf("%u\n", uint);
    return 1;
}

/* print a sint followed by a \n
 *
 * pops sint from value_stack
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int i_println_sint(struct ic_backend_pancake_value_stack *value_stack) {
    int sint = 0;
    INIT();

    READ(sint, sint);

    printf("%d\n", sint);
    return 1;
}

/* print a bool followed by a \n
 *
 * pops bool from value_stack
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int i_println_bool(struct ic_backend_pancake_value_stack *value_stack) {
    bool boolean = 0;
    INIT();

    READ(boolean, boolean);

    if (boolean) {
        puts("True");
    } else {
        puts("False");
    }
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
unsigned int i_plus_uint_uint(struct ic_backend_pancake_value_stack *value_stack) {
    unsigned int uint_one = 0;
    unsigned int uint_two = 0;
    unsigned int answer = 0;
    INIT();

    READ(uint_two, uint);
    READ(uint_one, uint);

    answer = uint_one + uint_two;

    RESULT(answer, uint);

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
unsigned int i_plus_sint_sint(struct ic_backend_pancake_value_stack *value_stack) {
    int sint_one = 0;
    int sint_two = 0;
    int answer = 0;
    INIT();

    READ(sint_two, sint);
    READ(sint_one, sint);

    answer = sint_one + sint_two;

    RESULT(answer, sint);

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
unsigned int i_minus_uint_uint(struct ic_backend_pancake_value_stack *value_stack) {
    unsigned int uint_one = 0;
    unsigned int uint_two = 0;
    unsigned int answer = 0;
    INIT();

    READ(uint_two, uint);
    READ(uint_one, uint);

    /* if we would rollover then stop at 0 */
    if (uint_two > uint_one) {
        answer = 0;
    } else {
        answer = uint_one - uint_two;
    }

    RESULT(answer, uint);

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
unsigned int i_minus_sint_sint(struct ic_backend_pancake_value_stack *value_stack) {
    int sint_one = 0;
    int sint_two = 0;
    int answer = 0;
    INIT();

    READ(sint_two, sint);
    READ(sint_one, sint);

    answer = sint_one - sint_two;

    RESULT(answer, sint);

    return 1;
}

unsigned int i_lessthan_equal_uint_uint(struct ic_backend_pancake_value_stack *value_stack) {
    unsigned int uint_one = 0;
    unsigned int uint_two = 0;
    int answer = 0;
    INIT();

    READ(uint_two, uint);
    READ(uint_one, uint);

    answer = uint_one <= uint_two;

    RESULT(answer, boolean);
    return 1;
}

unsigned int i_lessthan_equal_sint_sint(struct ic_backend_pancake_value_stack *value_stack) {
    int sint_one = 0;
    int sint_two = 0;
    int answer = 0;
    INIT();

    READ(sint_two, sint);
    READ(sint_one, sint);

    answer = sint_one <= sint_two;

    RESULT(answer, boolean);
    return 1;
}

unsigned int i_greaterthan_equal_uint_uint(struct ic_backend_pancake_value_stack *value_stack) {
    unsigned int uint_one = 0;
    unsigned int uint_two = 0;
    int answer = 0;
    INIT();

    READ(uint_two, uint);
    READ(uint_one, uint);

    answer = uint_one >= uint_two;

    RESULT(answer, boolean);
    return 1;
}

unsigned int i_greaterthan_equal_sint_sint(struct ic_backend_pancake_value_stack *value_stack) {
    int sint_one = 0;
    int sint_two = 0;
    int answer = 0;
    INIT();

    READ(sint_two, sint);
    READ(sint_one, sint);

    answer = sint_one >= sint_two;

    RESULT(answer, boolean);
    return 1;
}

unsigned int i_lessthan_uint_uint(struct ic_backend_pancake_value_stack *value_stack) {
    unsigned int uint_one = 0;
    unsigned int uint_two = 0;
    int answer = 0;
    INIT();

    READ(uint_two, uint);
    READ(uint_one, uint);

    answer = uint_one < uint_two;

    RESULT(answer, boolean);
    return 1;
}

unsigned int i_lessthan_sint_sint(struct ic_backend_pancake_value_stack *value_stack) {
    int sint_one = 0;
    int sint_two = 0;
    int answer = 0;
    INIT();

    READ(sint_two, sint);
    READ(sint_one, sint);

    answer = sint_one < sint_two;

    RESULT(answer, boolean);
    return 1;
}

unsigned int i_greaterthan_uint_uint(struct ic_backend_pancake_value_stack *value_stack) {
    unsigned int uint_one = 0;
    unsigned int uint_two = 0;
    int answer = 0;
    INIT();

    READ(uint_two, uint);
    READ(uint_one, uint);

    answer = uint_one > uint_two;

    RESULT(answer, boolean);
    return 1;
}

unsigned int i_greaterthan_sint_sint(struct ic_backend_pancake_value_stack *value_stack) {
    int sint_one = 0;
    int sint_two = 0;
    int answer = 0;
    INIT();

    READ(sint_two, sint);
    READ(sint_one, sint);

    answer = sint_one > sint_two;

    RESULT(answer, boolean);
    return 1;
}

unsigned int i_to_str_bool(struct ic_backend_pancake_value_stack *value_stack) {
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

unsigned int i_length_string(struct ic_backend_pancake_value_stack *value_stack) {
    char *str = 0;
    unsigned int answer = 0;
    INIT();

    READ(str, string);

    answer = strlen(str);
    RESULT(answer, uint);
    return 1;
}

unsigned int i_equal_sint_sint(struct ic_backend_pancake_value_stack *value_stack) {
    int sint_one = 0;
    int sint_two = 0;
    bool answer = 0;
    INIT();

    READ(sint_two, sint);
    READ(sint_one, sint);

    answer = sint_one == sint_two;

    RESULT(answer, boolean);
    return 1;
}

unsigned int i_equal_uint_uint(struct ic_backend_pancake_value_stack *value_stack) {
    int uint_one = 0;
    int uint_two = 0;
    bool answer = 0;
    INIT();

    READ(uint_two, sint);
    READ(uint_one, sint);

    answer = uint_one == uint_two;

    RESULT(answer, boolean);
    return 1;
}

unsigned int i_equal_sint_uint(struct ic_backend_pancake_value_stack *value_stack) {
    unsigned int uint = 0;
    int sint = 0;
    int answer = 0;
    unsigned int tmp = 0;
    INIT();

    READ(uint, uint);
    READ(sint, sint);

    if (sint < 0) {
        answer = 0;
    } else {
        tmp = sint;
        if (tmp == uint) {
            answer = 1;
        } else {
            answer = 0;
        }
    }

    RESULT(answer, boolean);
    return 1;
}

unsigned int i_equal_uint_sint(struct ic_backend_pancake_value_stack *value_stack) {
    int sint = 0;
    unsigned int uint = 0;
    int answer = 0;
    unsigned int tmp = 0;
    INIT();

    READ(sint, sint);
    READ(uint, uint);

    if (sint < 0) {
        answer = 0;
    } else {
        tmp = sint;
        if (tmp == uint) {
            answer = 1;
        } else {
            answer = 0;
        }
    }

    RESULT(answer, boolean);
    return 1;
}

unsigned int i_concat_string_string(struct ic_backend_pancake_value_stack *value_stack) {
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

    answer = calloc(len, sizeof(char));
    if (!answer) {
        puts("concat(String,String): call to calloc failed");
        return 0;
    }

    strcpy(answer, str1);
    strcat(answer, str2);

    RESULT(answer, string);
    return 1;
}

unsigned int i_modulo_sint_sint(struct ic_backend_pancake_value_stack *value_stack) {
    int sint_one = 0;
    int sint_two = 0;
    int answer = 0;
    INIT();

    READ(sint_two, sint);
    READ(sint_one, sint);

    answer = sint_one % sint_two;

    RESULT(answer, sint);
    return 1;
}
