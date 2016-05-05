#ifndef IC_BACKEND_2C_STMT_H
#define IC_BACKEND_2C_STMT_H

#include <stdio.h> /* FILE */

#include "../../parse/data/stmt.h"
#include "../../analyse/data/kludge.h"

/* compile a given statement to specified file
 *
 * generate a c program
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_b2c_compile_stmt(struct ic_kludge *input_kludge, struct ic_stmt *stmt, FILE *out);

#endif

