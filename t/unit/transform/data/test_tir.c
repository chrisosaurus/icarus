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
  struct ic_transform_ir_assign *let_p = 0;
  struct ic_transform_ir_assign let_l;

  puts("basic tir assign testing");

  let_p = ic_transform_ir_assign_new();
  assert( let_p );

  assert( ic_transform_ir_assign_init(&let_l) );

  assert( ic_transform_ir_assign_destroy( &let_l, 0 ) );
  assert( ic_transform_ir_assign_destroy( let_p, 1 ) );

  puts("success");
}

void basic_expr(void){
  struct ic_transform_ir_expr *let_p = 0;
  struct ic_transform_ir_expr let_l;

  puts("basic tir expr testing");

  let_p = ic_transform_ir_expr_new();
  assert( let_p );

  assert( ic_transform_ir_expr_init(&let_l) );

  assert( ic_transform_ir_expr_destroy( &let_l, 0 ) );
  assert( ic_transform_ir_expr_destroy( let_p, 1 ) );

  puts("success");
}

void basic_ret(void){
  struct ic_transform_ir_ret *let_p = 0;
  struct ic_transform_ir_ret let_l;

  puts("basic tir ret testing");

  let_p = ic_transform_ir_ret_new();
  assert( let_p );

  assert( ic_transform_ir_ret_init(&let_l) );

  assert( ic_transform_ir_ret_destroy( &let_l, 0 ) );
  assert( ic_transform_ir_ret_destroy( let_p, 1 ) );

  puts("success");
}

void basic_fcall(void){
  struct ic_transform_ir_fcall *let_p = 0;
  struct ic_transform_ir_fcall let_l;

  puts("basic tir fcall testing");

  let_p = ic_transform_ir_fcall_new();
  assert( let_p );

  assert( ic_transform_ir_fcall_init(&let_l) );

  assert( ic_transform_ir_fcall_destroy( &let_l, 0 ) );
  assert( ic_transform_ir_fcall_destroy( let_p, 1 ) );

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
