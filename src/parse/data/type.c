#include <stdio.h>

#include "type.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"

/* allocate and intialise a new type
 * this will set type.type to unknown
 *
 * returns new type on success
 * returns 0 on failure
 */
struct ic_type * ic_type_new(void){
    puts("ic_type_ : unimplemented");
    return 0;
}

/* intialise a type
 * this will set type.type to unknown
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_type_init(struct ic_type *type){
    puts("ic_type_init : unimplemented");
    return 1;
}

/* destroy type
 *
 * this will only free type if `free_type` is truthy
 *
 * returns 0 on sucess
 * returns 1 on error
 */
unsigned int ic_type_destroy(struct ic_type *type, unsigned int free_type){
    puts("ic_type_destroy : unimplemented");
    return 1;
}

/* set the sym on this type from the provided string
 * this will change type.type to sym
 *
 * Note that this is ONLY allowed if a tdecl hasn't already been set
 *
 * if type.type is tdecl then calling this function is an error
 * as that would be going 'backwards'
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_type_set_symbol(struct ic_type *type, char *type_str, unsigned int type_len){
    puts("ic_type_set_symbol : unimplemented");
    return 1;
}

/* set the *tdecl on this type
 * this will change type.type to tdecl
 *
 * FIXME decide on what happens to sym
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_type_set_tdecl(struct ic_type *type, struct ic_type_decl *tdecl){
    puts("ic_type_set_tdecl : unimplemented");
    return 1;
}

