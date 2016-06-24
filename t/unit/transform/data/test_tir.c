#include <assert.h> /* assert */
#include <stdio.h> /* puts */

#include "../../../../src/transform/data/tir.h"

/* very 'dumb' unit testing of tir interface
 * at this point this is just the cons/init/decons
 */

void basic_let_literal(void){
  struct ic_transform_ir_let_literal *let_p = 0;
  struct ic_transform_ir_let_literal let_l;

  puts("basic tir let_literal testing");

  let_p = ic_transform_ir_let_literal_new();
  assert( let_p );

  assert( ic_transform_ir_let_literal_init(&let_l) );

  assert( ic_transform_ir_let_literal_destroy( &let_l, 0 ) );
  assert( ic_transform_ir_let_literal_destroy( let_p, 1 ) );

  puts("success");
}

void basic_let_expr(void){
  struct ic_transform_ir_let_expr *let_p = 0;
  struct ic_transform_ir_let_expr let_l;

  puts("basic tir let_expr testing");

  let_p = ic_transform_ir_let_expr_new();
  assert( let_p );

  assert( ic_transform_ir_let_expr_init(&let_l) );

  assert( ic_transform_ir_let_expr_destroy( &let_l, 0 ) );
  assert( ic_transform_ir_let_expr_destroy( let_p, 1 ) );

  puts("success");
}

void basic_let(void){
  struct ic_transform_ir_let let;
  struct ic_transform_ir_let *let_p = 0;
  struct ic_transform_ir_let_literal *let_lit = 0;
  struct ic_transform_ir_let_expr *let_expr = 0;

  puts("basic tir let testing");

  let_p = ic_transform_ir_let_new(ic_transform_ir_let_type_literal);
  assert( let_p );

  let_lit = ic_transform_ir_let_get_literal(let_p);
  assert(let_lit);


  assert( ic_transform_ir_let_init(&let, ic_transform_ir_let_type_fcall) );

  let_expr = ic_transform_ir_let_get_expr(&let);
  assert(let_expr);

  assert( ic_transform_ir_let_destroy( &let, 0 ) );
  assert( ic_transform_ir_let_destroy( let_p, 1 ) );

  puts("success");
}

void basic_assign(void){
  struct ic_transform_ir_assign *assign_p = 0;
  struct ic_transform_ir_assign assign_l;

  puts("basic tir assign testing");

  assign_p = ic_transform_ir_assign_new();
  assert( assign_p );

  assert( ic_transform_ir_assign_init(&assign_l) );

  assert( ic_transform_ir_assign_destroy( &assign_l, 0 ) );
  assert( ic_transform_ir_assign_destroy( assign_p, 1 ) );

  puts("success");
}

void basic_expr(void){
  struct ic_transform_ir_expr *expr_p = 0;
  struct ic_transform_ir_expr expr_l;

  puts("basic tir expr testing");

  expr_p = ic_transform_ir_expr_new();
  assert( expr_p );

  assert( ic_transform_ir_expr_init(&expr_l) );

  assert( ic_transform_ir_expr_destroy( &expr_l, 0 ) );
  assert( ic_transform_ir_expr_destroy( expr_p, 1 ) );

  puts("success");
}

void basic_ret(void){
  struct ic_transform_ir_ret *ret_p = 0;
  struct ic_transform_ir_ret ret_l;

  puts("basic tir ret testing");

  ret_p = ic_transform_ir_ret_new();
  assert( ret_p );

  assert( ic_transform_ir_ret_init(&ret_l) );

  assert( ic_transform_ir_ret_destroy( &ret_l, 0 ) );
  assert( ic_transform_ir_ret_destroy( ret_p, 1 ) );

  puts("success");
}

void basic_fcall(void){
  struct ic_transform_ir_fcall *fcall_p = 0;
  struct ic_transform_ir_fcall fcall_l;

  puts("basic tir fcall testing");

  fcall_p = ic_transform_ir_fcall_new();
  assert( fcall_p );

  assert( ic_transform_ir_fcall_init(&fcall_l) );

  assert( ic_transform_ir_fcall_destroy( &fcall_l, 0 ) );
  assert( ic_transform_ir_fcall_destroy( fcall_p, 1 ) );

  puts("success");
}

void basic_stmt(void){
  struct ic_transform_ir_stmt stmt_expr;
  struct ic_transform_ir_stmt *stmt_let = 0;
  struct ic_transform_ir_stmt *stmt_ret = 0;

  struct ic_transform_ir_expr *expr = 0;
  struct ic_transform_ir_let *let = 0;
  struct ic_transform_ir_ret *ret = 0;

  puts("basic tir stmt testing");

  assert( ic_transform_ir_stmt_init(&stmt_expr, ic_transform_ir_type_expr) );
  expr = ic_transform_ir_stmt_get_expr(&stmt_expr);
  assert(expr);

  stmt_let = ic_transform_ir_stmt_new(ic_transform_ir_type_let);
  assert(stmt_let);
  let = ic_transform_ir_stmt_get_let(stmt_let);
  assert(let);

  stmt_ret = ic_transform_ir_stmt_new(ic_transform_ir_type_ret);
  assert(stmt_ret);
  ret = ic_transform_ir_stmt_get_ret(stmt_ret);
  assert(ret);

  assert( ic_transform_ir_stmt_destroy( &stmt_expr, 0 ) );
  assert( ic_transform_ir_stmt_destroy( stmt_let, 1 ) );
  assert( ic_transform_ir_stmt_destroy( stmt_ret, 1 ) );

  puts("success");
}


int main(void){
  basic_let_literal();
  basic_let_expr();

  basic_let();


  basic_assign();
  basic_expr();
  basic_ret();
  basic_fcall();

  basic_stmt();

  return 0;
}
