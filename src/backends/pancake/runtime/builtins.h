#ifndef IC_backend_pancake_BUILTINS_H
#define IC_backend_pancake_BUILTINS_H

#include "../data/value_stack.h"

typedef unsigned int (*ic_backend_function_sig)(struct ic_backend_pancake_value_stack *);

/* get builtin function for user-land name
 *
 * returns * on success
 * returns 0 on failure
 */
ic_backend_function_sig ic_backend_pancake_builtins_table_get(char *str);

#endif /* IC_backend_pancake_BUILTINS_H */
