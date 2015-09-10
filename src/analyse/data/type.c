#include <stdlib.h> /* calloc */
#include <stdio.h> /* puts */

#include "type.h"

/* alloc and init a new type representing a tdecl
 *
 * returns new type on success
 * returns 0 on failure
 */
struct ic_type * ic_type_new_tdecl(struct ic_type_decl *decl){
    struct ic_type *type = 0;

    if( ! decl ){
        puts("ic_type_new_tdecl: decl was null");
        return 0;
    }

    type = calloc(1, sizeof(struct ic_type));
    if( ! type ){
        puts("ic_type_new_tdecl: call to calloc failed");
        return 0;
    }

    if( ! ic_type_init_tdecl(type, decl) ){
        puts("ic_type_new_tdecl: call to ic_type_init_tdecl failed");
        return 0;
    }

    return type;
}

/* init an existing type representing a tdecl
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_init_tdecl(struct ic_type *type, struct ic_type_decl *decl){
    if( ! type ){
        puts("ic_type_init_tdecl: type was null");
        return 0;
    }

    if( ! decl ){
        puts("ic_type_init_tdecl: decl was null");
        return 0;
    }

    type->type = ic_type_user;
    type->u.decl = decl;

    return 1;
}

/* destroy a type
 *
 * will only free the type if `free_type` is truthy
 *
 * will NOT free any decl or builtin objects
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_destroy(struct ic_type *type, unsigned int free_type){
    if( ! type ){
        puts("ic_type_destroy: type was null");
        return 0;
    }

    switch( type->type ){
        case ic_type_builtin:
            break;

        case ic_type_user:
            type->u.decl = 0;
            break;

        default:
            break;
    }

    if( free_type ){
        free(type);
    }

    return 1;
}

