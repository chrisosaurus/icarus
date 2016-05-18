#include "stmt.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"

/* compile a given stmt to specified file
 *
 * generate a c program
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_b2c_compile_stmt(struct ic_kludge *input_kludge, struct ic_stmt *stmt, FILE *out){
  unsigned int indent_level = 1;

  puts("ic_b2c_compile_stmt: called for");
  ic_stmt_print(stmt, &indent_level);

  puts("ic_b2c_compile_stmt: unimplemented");
  return 0;
}

