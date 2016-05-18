#include "expr.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"

/* compile a given expr to specified file
 *
 * generate a c program
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_b2c_compile_expr(struct ic_kludge *input_kludge, struct ic_expr *expr, FILE *out){
  unsigned int indent_level = 1;

  puts("ic_b2c_compile_expr: called for");
  ic_expr_print(expr, &indent_level);
  /* caller of ic_expr_print must add \n */
  puts("");

  puts("ic_b2c_compile_expr: unimplemented");
  return 0;
}

