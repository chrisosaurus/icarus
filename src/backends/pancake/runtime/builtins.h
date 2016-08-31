#ifndef IC_backend_pancake_BUILTINS_H
#define IC_backend_pancake_BUILTINS_H

#include "../data/value_stack.h"

/* print a string followed by a \n
 *
 * pops string from value_stack
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_builtin_println_string(struct ic_backend_pancake_value_stack *value_stack);

typedef unsigned int (*ic_backend_function_sig)(struct ic_backend_pancake_value_stack *);

/* get builtin function for user-land name
 *
 * returns * on success
 * returns 0 on failure
 */
ic_backend_function_sig ic_backend_pancake_builtins_table_get(char *str);

#endif /* IC_backend_pancake_BUILTINS_H */
