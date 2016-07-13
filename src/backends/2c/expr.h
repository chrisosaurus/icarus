#ifndef IC_BACKEND_2C_EXPR_H
#define IC_BACKEND_2C_EXPR_H

#include <stdio.h> /* FILE */

/* predeclarations */
struct ic_kludge;
struct ic_transform_ir_expr;
struct ic_expr_constant;

/* compile a given expr to specified file
 *
 * generate a c program
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_b2c_compile_expr(struct ic_kludge *input_kludge, struct ic_transform_ir_expr *texp, FILE *out);

unsigned int ic_b2c_compile_expr_constant(struct ic_kludge *input_kludge, struct ic_expr_constant *constant, FILE *out);

#endif
