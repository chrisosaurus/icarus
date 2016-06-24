#include <stdlib.h> /* calloc, free */
#include <stdio.h> /* puts */

#include "tbody.h"

//struct ic_transform_body {
//  struct ic_pvector *tstmts;
//};

/* allocate and initialise a new tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
struct ic_transform_body * ic_transform_body_new(void){
  struct ic_transform_body *tbody = 0;

  tbody = calloc( sizeof(struct ic_transform_body), 1 );
  if( ! tbody ){
    puts("ic_transform_body_new: call to calloc failed");
    return 0;
  }

  if( ! ic_transform_body_init(tbody) ){
    puts("ic_transform_body_new: call to ic_transform_body_init failed");
    return 0;
  }

  return tbody;
}

/* initialise existing tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_body_init(struct ic_transform_body *tbody){
  if( ! tbody ){
    puts("ic_transform_body_init: tbody was null");
    return 0;
  }

  if( ! ic_pvector_init( &(tbody->tstmts), 0 ) ){
    puts("ic_transform_body_init: call to ic_pvector_init failed");
    return 0;
  }

  return 1;
}

/* destroy tbody
 *
 * will only free tbody if `free_tbody` is truthy
 *
 * will always call ic_pvector_destroy on tstmts
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_body_destroy(struct ic_transform_body *tbody, unsigned int free_tbody){
  if( ! tbody ){
    puts("ic_transform_body_destroy: tbody was null");
    return 0;
  }

  /* do not free as tstmts is a part of me
   * FIXME final arg should be a destructor for tstmt
   */
  if( ! ic_pvector_destroy( &(tbody->tstmts), 0, 0 ) ){
    puts("ic_transform_body_destroy: call to ic_pvector_destroy failed");
    return 0;
  }

  if( free_tbody ){
    free(tbody);
  }

  return 1;
}

/* append tstmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_body_append(struct ic_transform_body *tbody, struct ic_transform_ir_stmt *tstmt){
  if( ! tbody ){
    puts("ic_transform_body_append: tbody was null");
    return 0;
  }

  if( ! tstmt ){
    puts("ic_transform_body_append: tstmt was null");
    return 0;
  }

  if( ! ic_pvector_append( &(tbody->tstmts), tstmt ) ){
    puts("ic_transform_body_append: call to ic_pvector_append failed");
    return 0;
  }

  return 1;
}


/* get length of this tbody
 *
 * FIXME no real failure mode
 *
 * returns count on success
 * returns 0 on failure
 */
unsigned int ic_transform_body_length(struct ic_transform_body *tbody){
  if( ! tbody ){
    puts("ic_transform_body_length: tbody was null ");
    return 0;
  }

  return ic_pvector_length( &(tbody->tstmts) );
}

/* get tstmt at index `index`
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_stmt * ic_transform_body_get(struct ic_transform_body *tbody, unsigned int index){
  struct ic_transform_ir_stmt *tstmt = 0;

  if( ! tbody ){
    puts("ic_transform_body_get: tbody was null");
    return 0;
  }

  tstmt = ic_pvector_get( &(tbody->tstmts), index );
  if( ! tstmt ){
    puts("ic_transform_body_get: call to ic_pvector_get failed");
    return 0;
  }

  return tstmt;
}


