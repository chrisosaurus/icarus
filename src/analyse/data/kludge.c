#include <stdio.h> /* puts */
#include <stdlib.h> /* calloc */

#include "kludge.h"
#include "../../data/pvector.h"

/* alloc and init a new kludge
 *
 * returns pointer on success
 * returns 0 on error
 */
struct ic_kludge * ic_kludge_new(struct ic_ast *ast){
    struct ic_kludge *kludge = 0;

    if( ! ast ){
        puts("ic_kludge_new: ast was null");
        return 0;
    }

    /* alloc */
    kludge = calloc(1, sizeof(struct ic_kludge));
    if( ! kludge ){
        puts("ic_kludge_new: call to calloc failed");
        return 0;
    }

    /* init */
    if( ic_kludge_init(kludge, ast) ){
        puts("ic_kludge_new: call to ic_klude_init failed");
        return 0;
    }

    /* success */
    return kludge;
}

/* init an existing kludge
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_kludge_init(struct ic_kludge *kludge, struct ic_ast *ast){
    if( ! kludge ){
        puts("ic_kludge_init: kludge was null");
        return 1;
    }
    if( ! ast ){
        puts("ic_kludge_init: ast was null");
        return 1;
    }

    /* FIXME dict_tname hash */

    /* FIXME dict_fsig hash */

    /* pvector tdecls */
    if( ic_pvector_init( &(kludge->tdecls), 0 ) ){
        puts("ic_kludge_init: tdecl: call to ic_pvector_init failed");
        return 1;
    }

    /* pvector fdecls */
    if( ic_pvector_init( &(kludge->fdecls), 0 ) ){
        puts("ic_kludge_init: fdecl: call to ic_pvector_init failed");
        return 1;
    }

    /* pvector error */
    if( ic_pvector_init( &(kludge->errors), 0 ) ){
        puts("ic_kludge_init: errors: call to ic_pvector_init failed");
        return 1;
    }

    /* ast ast */
    kludge->aast = ast;

    return 0;
}

/* add a new type decl to this kludge
 * this will insert into dict_tname and also
 * into tdecls
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_klude_add_tdecl(struct ic_kludge *kludge, struct ic_type_decl *tdecl){
    if( ! kludge ){
        puts("ic_kludge_add_tdecl: kludge was null");
        return 1;
    }
    if( ! tdecl ){
        puts("ic_kludge_add_tdecl: tdecl was null");
        return 1;
    }

    /* FIXME need to insert into hash */

    /* insert into list of tdecls */
    if( ic_pvector_append( &(kludge->tdecls), tdecl ) == -1 ){
        puts("ic_kludge_add_tdecl: call to ic_pvector_append failed");
        return 1;
    }

    return 0;
}

/* add a new func decl to this kludge
 * this will insert into dict_fname and also
 * into fsigs
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_klude_add_fdecl(struct ic_kludge *kludge, struct ic_func_decl *fdecl){
    if( ! kludge ){
        puts("ic_kludge_add_fdecl: kludge was null");
        return 1;
    }
    if( ! fdecl ){
        puts("ic_kludge_add_fdecl: fdecl was null");
        return 1;
    }


    /* FIXME need to insert into hash */

    /* insert into list of fdecls */
    if( ic_pvector_append( &(kludge->fdecls), fdecl ) == -1 ){
        puts("ic_kludge_add_fdecl: call to ic_pvector_append failed");
        return 1;
    }

    return 0;
}

/* add a new error to error list
 *
 * FIXME no error type
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_klude_add_error(struct ic_kludge *kludge, void *error){
    if( ! kludge ){
        puts("ic_kludge_add_error: kludge was null");
        return 1;
    }
    if( ! error ){
        puts("ic_kludge_add_error: error was null");
        return 1;
    }

    /* just a simple pvector of errors */
    if( ic_pvector_append(&(kludge->errors), error) == -1 ){
        puts("ic_kludge_add_error: call to ic_pvector_append failed");
        return 1;
    }

    return 0;
}

