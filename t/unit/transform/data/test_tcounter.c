#include <assert.h> /* assert */
#include <stdio.h> /* puts */

#include "../../../../src/transform/data/tcounter.h"

/* very 'dumb' unit testing of tir interface
 * at this point this is just the cons/init/decons
 */

void basic_tcounter(void){
  struct ic_transform_counter tcounter;
  struct ic_transform_counter *tcounter_p = 0;
  unsigned int lit_c = 0;
  unsigned int tmp_c = 0;

  puts("basic tir tcounter testing");

  assert( ic_transform_counter_init(&tcounter) );

  tcounter_p = ic_transform_counter_new();
  assert( tcounter_p );


  lit_c = ic_transform_counter_register_literal(tcounter_p);
  assert( 1 == lit_c );

  lit_c = ic_transform_counter_register_literal(tcounter_p);
  assert( 2 == lit_c );

  lit_c = ic_transform_counter_register_literal(tcounter_p);
  assert( 3 == lit_c );


  tmp_c = ic_transform_counter_register_temporary(tcounter_p);
  assert( 1 == tmp_c );

  tmp_c = ic_transform_counter_register_temporary(tcounter_p);
  assert( 2 == tmp_c );

  tmp_c = ic_transform_counter_register_temporary(tcounter_p);
  assert( 3 == tmp_c );


  assert( ic_transform_counter_destroy( &tcounter, 0 ) );
  assert( ic_transform_counter_destroy( tcounter_p, 1 ) );

  puts("success");
}

int main(void){
  basic_tcounter();

  return 0;
}
