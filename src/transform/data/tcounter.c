#include <stdlib.h> /* calloc, free */
#include <stdio.h> /* puts */

#include "tcounter.h"

/* allocate and initialise a new transform_counter
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_counter * ic_transform_counter_new(void){
  struct ic_transform_counter *tcounter = 0;

  tcounter = calloc( sizeof(struct ic_transform_counter), 1 );
  if( ! tcounter ){
    puts("ic_transform_counter_new: call to calloc failed");
    return 0;
  }

  if( ! ic_transform_counter_init(tcounter) ){
    puts("ic_transform_counter_new: call to ic_transform_counter_init failed");
    return 0;
  }

  return tcounter;
}

/* initialise an existing transform_counter
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_counter_init(struct ic_transform_counter *tcounter){
  if( ! tcounter ){
    puts("ic_transform_counter_init: tcounter was null");
    return 0;
  }

  tcounter->literal_count = 0;
  tcounter->temporary_count = 0;

  return 1;
}

/* destroy a transform_counter
 *
 * will only free if `free_tcounter` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_counter_destroy(struct ic_transform_counter *tcounter, unsigned int free_tcounter){
  if( ! tcounter ){
    puts("ic_transform_counter_destroy: tcounter was null");
    return 0;
  }

  if( free_tcounter ){
    free(tcounter);
  }

  return 1;
}

/* get a unique literal number within this counter
 *
 * returns count > 0 on success
 * return 0 on failure
 */
unsigned int ic_transform_counter_register_literal(struct ic_transform_counter *tcounter){
  unsigned int ncount = 0;

  if( ! tcounter ){
    puts("ic_transform_counter_register_literal: tcounter was null");
    return 0;
  }

  tcounter->literal_count += 1;
  ncount = tcounter->literal_count;
  return ncount;
}

/* get a unique temporary number within this counter
 *
 * returns count > 0 on success
 * returns 0 on failure
 */
unsigned int ic_transform_counter_register_temporary(struct ic_transform_counter *tcounter){
  unsigned int ncount = 0;

  if( ! tcounter ){
    puts("ic_transform_counter_temporary: tcounter was null");
    return 0;
  }

  tcounter->temporary_count += 1;
  ncount = tcounter->temporary_count;
  return ncount;
}

