#include "expr.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"

unsigned int ic_b2c_compile_expr_fcall(struct ic_kludge *input_kludge, struct ic_expr *expr, FILE *out);
unsigned int ic_b2c_compile_expr_identifier(struct ic_kludge *input_kludge, struct ic_expr *expr, FILE *out);
unsigned int ic_b2c_compile_expr_constant(struct ic_kludge *input_kludge, struct ic_expr *expr, FILE *out);
unsigned int ic_b2c_compile_expr_operator(struct ic_kludge *input_kludge, struct ic_expr *expr, FILE *out);
unsigned int ic_b2c_compile_expr_faccess(struct ic_kludge *input_kludge, struct ic_expr *expr, FILE *out);

/* compile a given expr to specified file
 *
 * generate a c program
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_b2c_compile_expr(struct ic_kludge *input_kludge, struct ic_expr *expr, FILE *out){
  unsigned int indent_level = 1;

  if( ! input_kludge ){
    puts("ic_b2c_compile_expr: input_kludge was null");
    return 0;
  }

  if( ! expr ){
    puts("ic_b2c_compile_expr: expr was null");
    return 0;
  }

  if( ! out ){
    puts("ic_b2c_compile_expr: out was null");
    return 0;
  }

  switch( expr->tag ){
    case ic_expr_type_func_call:
      return ic_b2c_compile_expr_fcall(input_kludge, expr, out);
      break;

    case ic_expr_type_identifier:
      return ic_b2c_compile_expr_identifier(input_kludge, expr, out);
      break;

    case ic_expr_type_constant:
      return ic_b2c_compile_expr_identifier(input_kludge, expr, out);
      break;

    case ic_expr_type_operator:
      return ic_b2c_compile_expr_operator(input_kludge, expr, out);
      break;

    case ic_expr_type_field_access:
      return ic_b2c_compile_expr_faccess(input_kludge, expr, out);
      break;

    default:
      puts("ic_b2c_compile_expr: impossible tag");

      puts("ic_b2c_compile_expr: called for");
      ic_expr_print(expr, &indent_level);
      /* caller of ic_expr_print must add \n */
      puts("");
      return 0;

      break;
  }

  puts("ic_b2c_compile_expr: impossible");
  return 0;
}

unsigned int ic_b2c_compile_expr_fcall(struct ic_kludge *input_kludge, struct ic_expr *expr, FILE *out){
  unsigned int indent_level = 1;

  puts("ic_b2c_compile_expr_fcall: call for");
  ic_expr_print(expr, &indent_level);
  puts("");

  puts("ic_b2c_compile_expr_fcall: unimplemented");
  return 0;
}

unsigned int ic_b2c_compile_expr_identifier(struct ic_kludge *input_kludge, struct ic_expr *expr, FILE *out){
  unsigned int indent_level = 1;

  puts("ic_b2c_compile_expr_identifier: call for");
  ic_expr_print(expr, &indent_level);
  puts("");

  puts("ic_b2c_compile_expr_identifier: unimplemented");
  return 0;
}

unsigned int ic_b2c_compile_expr_constant(struct ic_kludge *input_kludge, struct ic_expr *expr, FILE *out){
  unsigned int indent_level = 1;

  puts("ic_b2c_compile_expr_constant: call for");
  ic_expr_print(expr, &indent_level);
  puts("");

  puts("ic_b2c_compile_expr_constant: unimplemented");
  return 0;
}

unsigned int ic_b2c_compile_expr_operator(struct ic_kludge *input_kludge, struct ic_expr *expr, FILE *out){
  unsigned int indent_level = 1;

  puts("ic_b2c_compile_expr_operator: call for");
  ic_expr_print(expr, &indent_level);
  puts("");

  puts("ic_b2c_compile_expr_operator: unimplemented");
  return 0;
}

unsigned int ic_b2c_compile_expr_faccess(struct ic_kludge *input_kludge, struct ic_expr *expr, FILE *out){
  unsigned int indent_level = 1;

  puts("ic_b2c_compile_expr_faccess: call for");
  ic_expr_print(expr, &indent_level);
  puts("");

  puts("ic_b2c_compile_expr_faccess: unimplemented");
  return 0;
}



