#include "stmt.h"
#include "expr.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"

unsigned int ic_b2c_compile_stmt_ret(struct ic_kludge *input_kludge, struct ic_stmt *stmt, FILE *out);
unsigned int ic_b2c_compile_stmt_let(struct ic_kludge *input_kludge, struct ic_stmt *stmt, FILE *out);
unsigned int ic_b2c_compile_stmt_assign(struct ic_kludge *input_kludge, struct ic_stmt *stmt, FILE *out);
unsigned int ic_b2c_compile_stmt_if(struct ic_kludge *input_kludge, struct ic_stmt *stmt, FILE *out);
unsigned int ic_b2c_compile_stmt_for(struct ic_kludge *input_kludge, struct ic_stmt *stmt, FILE *out);
unsigned int ic_b2c_compile_stmt_while(struct ic_kludge *input_kludge, struct ic_stmt *stmt, FILE *out);
unsigned int ic_b2c_compile_stmt_expr(struct ic_kludge *input_kludge, struct ic_stmt *stmt, FILE *out);

/* compile a given stmt to specified file
 *
 * generate a c program
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_b2c_compile_stmt(struct ic_kludge *input_kludge, struct ic_stmt *stmt, FILE *out){
  unsigned int indent_level = 1;

  if( ! input_kludge ){
    puts("ic_b2c_compile_stmt: input_kludge was null");
    return 0;
  }

  if( ! stmt ){
    puts("ic_b2c_compile_stmt: stmt was null");
    return 0;
  }

  if( ! out ){
    puts("ic_b2c_compile_stmt: out was null");
    return 0;
  }

  /* dispatch to appropriate sub handler based on stmt type */
  switch( stmt->tag ){
    case ic_stmt_type_ret:
      return ic_b2c_compile_stmt_ret(input_kludge, stmt, out);
      break;

    case ic_stmt_type_let:
      return ic_b2c_compile_stmt_let(input_kludge, stmt, out);
      break;

    case ic_stmt_type_assign:
      return ic_b2c_compile_stmt_assign(input_kludge, stmt, out);
      break;

    case ic_stmt_type_if:
      return ic_b2c_compile_stmt_if(input_kludge, stmt, out);
      break;

    case ic_stmt_type_for:
      return ic_b2c_compile_stmt_for(input_kludge, stmt, out);
      break;

    case ic_stmt_type_while:
      return ic_b2c_compile_stmt_while(input_kludge, stmt, out);
      break;

    case ic_stmt_type_expr:
      return ic_b2c_compile_stmt_expr(input_kludge, stmt, out);
      break;

    default:
      puts("ic_b2c_compile_stmt: stmt had impossible tag:");
      ic_stmt_print(stmt, &indent_level);

      return 0;
      break;
  }

  puts("ic_b2c_compile_stmt: impossible...");
  return 0;
}

unsigned int ic_b2c_compile_stmt_ret(struct ic_kludge *input_kludge, struct ic_stmt *stmt, FILE *out){
  unsigned int indent_level = 1;

  if( ! input_kludge ){
    puts("ic_b2c_compile_stmt_ret: input_kludge was null");
    return 0;
  }

  if( ! stmt ){
    puts("ic_b2c_compile_stmt_ret: stmt was null");
    return 0;
  }

  if( ! out ){
    puts("ic_b2c_compile_stmt_ret: out was null");
    return 0;
  }

  if( stmt->tag != ic_stmt_type_ret ){
    puts("ic_b2c_compile_stmt_ret: stmt was not of type ret");
    return 0;
  }

  puts("ic_b2c_compile_stmt_ret: called on");
  ic_stmt_print(stmt, &indent_level);

  puts("ic_b2c_compile_stmt_ret: unimplemented");
  return 0;
}

unsigned int ic_b2c_compile_stmt_let(struct ic_kludge *input_kludge, struct ic_stmt *stmt, FILE *out){
  unsigned int indent_level = 1;

  if( ! input_kludge ){
    puts("ic_b2c_compile_stmt_let: input_kludge was null");
    return 0;
  }

  if( ! stmt ){
    puts("ic_b2c_compile_stmt_let: stmt was null");
    return 0;
  }

  if( ! out ){
    puts("ic_b2c_compile_stmt_let: out was null");
    return 0;
  }

  if( stmt->tag != ic_stmt_type_let ){
    puts("ic_b2c_compile_stmt_let: stmt was not of type let");
    return 0;
  }

  puts("ic_b2c_compile_stmt_let: called on");
  ic_stmt_print(stmt, &indent_level);

  puts("ic_b2c_compile_stmt_let: unimplemented");
  return 0;
}

unsigned int ic_b2c_compile_stmt_assign(struct ic_kludge *input_kludge, struct ic_stmt *stmt, FILE *out){
  unsigned int indent_level = 1;

  if( ! input_kludge ){
    puts("ic_b2c_compile_stmt_assign: input_kludge was null");
    return 0;
  }

  if( ! stmt ){
    puts("ic_b2c_compile_stmt_assign: stmt was null");
    return 0;
  }

  if( ! out ){
    puts("ic_b2c_compile_stmt_assign: out was null");
    return 0;
  }

  if( stmt->tag != ic_stmt_type_assign ){
    puts("ic_b2c_compile_stmt_assign: stmt was not of type assign");
    return 0;
  }

  puts("ic_b2c_compile_stmt_assign: called on");
  ic_stmt_print(stmt, &indent_level);

  puts("ic_b2c_compile_stmt_assign: unimplemented");
  return 0;
}

unsigned int ic_b2c_compile_stmt_if(struct ic_kludge *input_kludge, struct ic_stmt *stmt, FILE *out){
  unsigned int indent_level = 1;

  if( ! input_kludge ){
    puts("ic_b2c_compile_stmt_if: input_kludge was null");
    return 0;
  }

  if( ! stmt ){
    puts("ic_b2c_compile_stmt_if: stmt was null");
    return 0;
  }

  if( ! out ){
    puts("ic_b2c_compile_stmt_if: out was null");
    return 0;
  }

  if( stmt->tag != ic_stmt_type_if ){
    puts("ic_b2c_compile_stmt_if: stmt was not of type if");
    return 0;
  }

  puts("ic_b2c_compile_stmt_if: called on");
  ic_stmt_print(stmt, &indent_level);

  puts("ic_b2c_compile_stmt_if: unimplemented");
  return 0;
}

unsigned int ic_b2c_compile_stmt_for(struct ic_kludge *input_kludge, struct ic_stmt *stmt, FILE *out){
  unsigned int indent_level = 1;

  if( ! input_kludge ){
    puts("ic_b2c_compile_stmt_for: input_kludge was null");
    return 0;
  }

  if( ! stmt ){
    puts("ic_b2c_compile_stmt_for: stmt was null");
    return 0;
  }

  if( ! out ){
    puts("ic_b2c_compile_stmt_for: out was null");
    return 0;
  }

  if( stmt->tag != ic_stmt_type_for ){
    puts("ic_b2c_compile_stmt_for: stmt was not of type for");
    return 0;
  }

  puts("ic_b2c_compile_stmt_for: called on");
  ic_stmt_print(stmt, &indent_level);

  puts("ic_b2c_compile_stmt_for: unimplemented");
  return 0;
}

unsigned int ic_b2c_compile_stmt_while(struct ic_kludge *input_kludge, struct ic_stmt *stmt, FILE *out){
  unsigned int indent_level = 1;

  if( ! input_kludge ){
    puts("ic_b2c_compile_stmt_while: input_kludge was null");
    return 0;
  }

  if( ! stmt ){
    puts("ic_b2c_compile_stmt_while: stmt was null");
    return 0;
  }

  if( ! out ){
    puts("ic_b2c_compile_stmt_while: out was null");
    return 0;
  }

  if( stmt->tag != ic_stmt_type_while ){
    puts("ic_b2c_compile_stmt_while: stmt was not of type while");
    return 0;
  }

  puts("ic_b2c_compile_stmt_while: called on");
  ic_stmt_print(stmt, &indent_level);

  puts("ic_b2c_compile_stmt_while: unimplemented");
  return 0;
}

unsigned int ic_b2c_compile_stmt_expr(struct ic_kludge *input_kludge, struct ic_stmt *stmt, FILE *out){
  if( ! input_kludge ){
    puts("ic_b2c_compile_stmt_expr: input_kludge was null");
    return 0;
  }

  if( ! stmt ){
    puts("ic_b2c_compile_stmt_expr: stmt was null");
    return 0;
  }

  if( ! out ){
    puts("ic_b2c_compile_stmt_expr: out was null");
    return 0;
  }

  if( stmt->tag != ic_stmt_type_expr ){
    puts("ic_b2c_compile_stmt_expr: stmt was not of type expr");
    return 0;
  }

  if( ! ic_b2c_compile_expr(input_kludge, stmt->u.expr, out) ){
    puts("ic_b2c_compile_stmt_expr: call to ic_b2c_compile_expr failed");
    return 0;
  }

  return 1;
}



