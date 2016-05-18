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

  if( ! input_kludge ){
    puts("ic_b2c_compile_expr_fcall: input_kludge was null");
    return 0;
  }

  if( ! expr ){
    puts("ic_b2c_compile_expr_fcall: expr was null");
    return 0;
  }

  if( ! out ){
    puts("ic_b2c_compile_expr_fcall: out was null");
    return 0;
  }

  if( expr->tag != ic_expr_type_func_call ){
    puts("ic_b2c_compile_expr_fcall: expr was not of type func_call");
    return 0;
  }

  puts("ic_b2c_compile_expr_fcall: call for");
  ic_expr_print(expr, &indent_level);
  puts("");

  puts("ic_b2c_compile_expr_fcall: unimplemented");
  return 0;
}

unsigned int ic_b2c_compile_expr_identifier(struct ic_kludge *input_kludge, struct ic_expr *expr, FILE *out){
  unsigned int indent_level = 1;

  if( ! input_kludge ){
    puts("ic_b2c_compile_expr_identifier: input_kludge was null");
    return 0;
  }

  if( ! expr ){
    puts("ic_b2c_compile_expr_identifier: expr was null");
    return 0;
  }

  if( ! out ){
    puts("ic_b2c_compile_expr_identifier: out was null");
    return 0;
  }

  if( expr->tag != ic_expr_type_identifier ){
    puts("ic_b2c_compile_expr_identifier: expr was not of type identifier");
    return 0;
  }

  puts("ic_b2c_compile_expr_identifier: call for");
  ic_expr_print(expr, &indent_level);
  puts("");

  puts("ic_b2c_compile_expr_identifier: unimplemented");
  return 0;
}

unsigned int ic_b2c_compile_expr_constant(struct ic_kludge *input_kludge, struct ic_expr *expr, FILE *out){
  unsigned int indent_level = 1;

  if( ! input_kludge ){
    puts("ic_b2c_compile_expr_constant: input_kludge was null");
    return 0;
  }

  if( ! expr ){
    puts("ic_b2c_compile_expr_constant: expr was null");
    return 0;
  }

  if( ! out ){
    puts("ic_b2c_compile_expr_constant: out was null");
    return 0;
  }

  if( expr->tag != ic_expr_type_constant ){
    puts("ic_b2c_compile_expr_constant: expr was not of type constant");
    return 0;
  }

  puts("ic_b2c_compile_expr_constant: call for");
  ic_expr_print(expr, &indent_level);
  puts("");

  puts("ic_b2c_compile_expr_constant: unimplemented");
  return 0;
}

unsigned int ic_b2c_compile_expr_operator(struct ic_kludge *input_kludge, struct ic_expr *expr, FILE *out){
  unsigned int indent_level = 1;

  if( ! input_kludge ){
    puts("ic_b2c_compile_expr_operator: input_kludge was null");
    return 0;
  }

  if( ! expr ){
    puts("ic_b2c_compile_expr_operator: expr was null");
    return 0;
  }

  if( ! out ){
    puts("ic_b2c_compile_expr_operator: out was null");
    return 0;
  }

  if( expr->tag != ic_expr_type_operator ){
    puts("ic_b2c_compile_expr_operator: expr was not of type operator");
    return 0;
  }


  puts("ic_b2c_compile_expr_operator: call for");
  ic_expr_print(expr, &indent_level);
  puts("");

  puts("ic_b2c_compile_expr_operator: unimplemented");
  return 0;
}

unsigned int ic_b2c_compile_expr_faccess(struct ic_kludge *input_kludge, struct ic_expr *expr, FILE *out){
  unsigned int indent_level = 1;

  if( ! input_kludge ){
    puts("ic_b2c_compile_expr_faccess: input_kludge was null");
    return 0;
  }

  if( ! expr ){
    puts("ic_b2c_compile_expr_faccess: expr was null");
    return 0;
  }

  if( ! out ){
    puts("ic_b2c_compile_expr_faccess: out was null");
    return 0;
  }

  if( expr->tag != ic_expr_type_field_access ){
    puts("ic_b2c_compile_expr_faccess: expr was not of type field_access");
    return 0;
  }

  puts("ic_b2c_compile_expr_faccess: call for");
  ic_expr_print(expr, &indent_level);
  puts("");

  puts("ic_b2c_compile_expr_faccess: unimplemented");
  return 0;
}

