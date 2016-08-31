#include <stdio.h>
#include <string.h>

#include "builtins.h"

#define ic_backend_pancake_builtins_table_len 1

/* table mapping user-land names to internal names */
struct ic_backend_pancake_builtins_table_type {
    char *str;
    unsigned int (*func)(struct ic_backend_pancake_value_stack *value_stack);
} ic_backend_pancake_builtins_table[ic_backend_pancake_builtins_table_len] = {
    {"println(String)", ic_backend_pancake_builtin_println_string},
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
unsigned int ic_backend_pancake_builtin_println_string(struct ic_backend_pancake_value_stack *value_stack) {
    struct ic_backend_pancake_value *value = 0;
    char *str = 0;

    if (!value_stack) {
        puts("ic_backend_pancake_builtin_println_string: value_stack was null");
        return 0;
    }

    value = ic_backend_pancake_value_stack_peek(value_stack);

    if (!value) {
        puts("ic_backend_pancake_builtin_println_string: call to ic_backend_pancake_value_stack_peek failed");
        return 0;
    }

    if (value->tag != ic_backend_pancake_value_type_string) {
        puts("ic_backend_pancake_builtin_println_string: value was not of expected type string");
        fputs("found: ", stdout);
        ic_backend_pancake_value_print(value, stdout);
        return 0;
    }

    if (!ic_backend_pancake_value_stack_pop(value_stack)) {
        puts("ic_backend_pancake_builtin_println_string: call to ic_backend_pancake_value_stack_pop failed");
        return 0;
    }

    str = value->u.string;

    puts(str);
    return 1;
}
