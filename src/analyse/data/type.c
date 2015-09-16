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

/* alloc and init a new type representing a builtin
 *
 * returns new type on success
 * returns 0 on failure
 */
struct ic_type * ic_type_new_builtin(struct ic_type_builtin *builtin){
    struct ic_type *type = 0;

    if( ! builtin ){
        puts("ic_type_new_builtin: builtin was null");
        return 0;
    }

    type = calloc(1, sizeof(struct ic_type));
    if( ! type ){
        puts("ic_type_new_builtin: call to calloc failed");
        return 0;
    }

    if( ! ic_type_init_builtin(type, builtin) ){
        puts("ic_type_new_builtin: call to ic_type_init_tdecl failed");
        return 0;
    }

    return type;
}

/* init an existing type representing a builtin
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_init_builtin(struct ic_type *type, struct ic_type_builtin *builtin){
    if( ! type ){
        puts("ic_type_init_builtin: type was null");
        return 0;
    }

    if( ! builtin ){
        puts("ic_type_init_builtin: builtin was null");
        return 0;
    }

    type->type = ic_type_builtin;
    type->u.builtin = builtin;

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
            /* FIXME builtin leaked */
            type->u.builtin = 0;
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

/* return a symbol representing the name of this type
 *
 * returns ic_symbol * on sucess
 * returns 0 on failure
 */
struct ic_symbol * ic_type_name(struct ic_type *type){
    if( ! type ){
        puts("ic_type_name: type was null");
        return 0;
    }

    switch( type->type ){
        case ic_type_builtin:
            return type->u.builtin->name;
            break;

        case ic_type_user:
            return &(type->u.decl->name);
            break;

        default:
            puts("ic_type_name: impossible type->type");
            return 0;
            break;
    }


    puts("ic_type_name: impossible case");
    return 0;
}

/* is this type void
 *
 * returns 1 if provided type is Void
 * returns 0 if provided type is NOT void
 */
unsigned int ic_type_isvoid(struct ic_type *type){
    if( ! type ){
        puts("ic_type_isvoid: type was null");
        return 0;
    }

    if( type->type != ic_type_builtin ){
        return 0;
    }

    return type->u.builtin->isvoid;
}

/* are these 2 types the equal
 *
 * returns 1 if they are equal
 * returns 0 of they are NOT equal
 */
unsigned int ic_type_equal(struct ic_type *a, struct ic_type *b){
    if( ! a ){
        puts("ic_type_equal: type a was null");
        return 0;
    }

    if( ! b ){
        puts("ic_type_equal: type b was null");
        return 0;
    }

    return a == b;
}

