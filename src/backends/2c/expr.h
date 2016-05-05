#ifndef IC_BACKEND_2C_EXPR_H
#define IC_BACKEND_2C_EXPR_H

#include <stdio.h> /* FILE */

#include "../../parse/data/expr.h"
#include "../../analyse/data/kludge.h"

/* compile a given expr to specified file
 *
 * generate a c program
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_b2c_compile_expr(struct ic_kludge *input_kludge, struct ic_expr *expr, FILE *out);

#endif

