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

int main(void){
  basic_let_literal();
  basic_let_expr();

  /* FIXME TODO: ic_transform_ir_let */

  basic_assign();
  basic_expr();
  basic_ret();
  basic_fcall();

  /* FIXME TODO: ic_transform_ir_stmt */

  return 0;
}
