#ifndef IC_BACKEND_2C_TIR_H
#define IC_BACKEND_2C_TIR_H

#include <stdio.h> /* FILE */

/* predeclarations */
struct ic_kludge;
struct ic_transform_ir_stmt;
struct ic_transform_body;

/* compile a given body to specified file
 *
 * generate a c program
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_b2c_compile_body(struct ic_kludge *input_kludge, struct ic_transform_body *tbody, FILE *out);

/* compile a given statement to specified file
 *
 * generate a c program
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_b2c_compile_stmt(struct ic_kludge *input_kludge, struct ic_transform_ir_stmt *tstmt, FILE *out);

#endif
