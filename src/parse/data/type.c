#include <stdio.h> /* puts */

#include "type.h"

/* ignored unusued parameter */
#pragma GCC diagnostic ignored "-Wunused-parameter"

/* alloc and init a new type
 *
 * FIXME unknown params
 *
 * returns new type on success
 * returns 0 on error
 */
struct ic_type * ic_type_new(void){
    puts("ic_type_new: unimplemented");
    return 0;
}

/* init an existing type
 *
 * FIXME unknown params
 *
 * returns 1 on success
 * returns 0 on error
 */
unsigned int ic_type_init(struct ic_type *type){
    puts("ic_type_init: unimplemented");
    return 0;
}

/* destroy a type
 *
 * will only free the type if `free_type` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_destroy(struct ic_type *type, unsigned int free_tytpe){
    puts("ic_type_destroy: unimplemented");
    return 0;
}

