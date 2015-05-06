#include <stdio.h> /* puts */
#include <stdlib.h> /* calloc */

#include "kludge.h"
#include "../../data/pvector.h"
#include "../../parse/data/decl.h"

/* takes an ast and breaks it down to populate the supplied kludge
 *
 * returns 0 on success
 * return 1 on failure
 */
static unsigned int ic_kludge_populate_from_ast(struct ic_kludge *kludge, struct ic_ast *ast){
    /* offset */
    unsigned int i = 0;
    /* cached len */
    unsigned int len = 0;

    /* current decl we are considering */
    struct ic_decl *decl = 0;

    /* iterate through each declaration
     * in the ast and add to kludge
     */
    len = ic_ast_length(ast);
    for( i=0; i<len; ++i ){
        decl = ic_ast_get(ast, i);
        if( ! decl ){
            puts("ic_kludge_populate_from_ast: call to ic_ast_get failed");
            return 1;
        }

        /* dispatch on type to appropriate kludge_add function */
        switch(decl->type){
            case ic_decl_func_decl:
                if( ic_kludge_add_fdecl(kludge, &(decl->u.fdecl)) ){
                    puts("ic_kludge_populate_from_ast: call to ic_kludge_add_fdecl failed");
                    return 1;
                }
                break;

            case ic_decl_type_decl:
                if( ic_kludge_add_tdecl(kludge, &(decl->u.tdecl)) ){
                    puts("ic_kludge_populate_from_ast: call to ic_kludge_add_tdecl failed");
                    return 1;
                }
                break;

            default:
                puts("ic_kludge_populate_from_ast: decl had impossible type");
                return 1;
                break;
        }
    }

    return 0;
}

/* alloc and init a new kludge
 *
 * this call will break apart the ast to populate the
 * fields stored on kludge
 *
 * this will NOT perform any analysis
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
        puts("ic_kludge_new: call to ic_kludge_init failed");
        free(kludge);
        return 0;
    }

    /* success */
    return kludge;
}

/* init an existing kludge
 *
 * this call will break apart the ast to populate the
 * fields stored on kludge
 *
 * this will NOT perform any analysis
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

    /* dict_tname hash */
    if( ic_dict_init(&(kludge->dict_tname)) ){
        puts("ic_kludge_init: dict_tname: call to ic_dict_init failed");
        return 1;
    }

    /* dict_fsig hash */
    if( ic_dict_init(&(kludge->dict_fsig)) ){
        puts("ic_kludge_init: dict_fsig: call to ic_dict_init failed");
        return 1;
    }

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

    /* populate kludge fiels from ast:
     *      dict_tname
     *      dict_fsig
     *      tdecls
     *      fdecls
     */
    if( ic_kludge_populate_from_ast(kludge, ast) ){
        puts("ic_kludge_init: errors: call to ic_kludge_populate_from_ast failed");
        return 1;
    }


    return 0;
}

/* destroy kludge
 *
 * will only free kludge if `free_kludge` is truthy
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_kludge_destroy(struct ic_kludge *kludge, unsigned int free_kludge){
    /* index into various pvectors */
    unsigned int i = 0;
    /* cached len */
    unsigned int len = 0;

    /* items we are currently considering inside each pvector */
    struct ic_func_decl *fdecl = 0;
    struct ic_type_decl *tdecl = 0;

    if( !kludge ){
        puts("ic_kludge_destroy: kludge was null");
        return 1;
    }

    /* NOTE we have to be careful as various parts of the kludge share the same
     * objects
     *
     * for example each fdecl will be in both fict_fsig and fdecls
     *
     * destroying twice would be an error
     *
     * so we properly destroy each element except for:
     *      dict_tname
     *      dict_fsig
     */

    /* iterate through tdecls destroying each tdecl
     *      list of Type decls
     *      struct ic_pvector tdecls;
     */
    len = ic_pvector_length( &(kludge->tdecls) );
    for( i=0; i<len; ++i ){
        tdecl = ic_pvector_get( &(kludge->tdecls), i );
        if( ! tdecl ){
            puts("ic_kludge_destroy: tdecl - call to ic_pvector_get failed");
            return 1;
        }

        if( ic_type_decl_destroy(tdecl, 1) ){
            puts("ic_kludge_destroy: tdecl - call to ic_type_decl_destroy failed");
            return 1;
        }
    }

    /* iterate through fdecls destroying each fdecl
     *      list of Func decls
     *      struct ic_pvector fdecls;
     */
    len = ic_pvector_length( &(kludge->fdecls) );
    for( i=0; i<len; ++i ){
        fdecl = ic_pvector_get( &(kludge->fdecls), i );
        if( ! fdecl ){
            puts("ic_kludge_destroy: fdecl - call to ic_pvector_get failed");
            return 1;
        }

        if( ic_func_decl_destroy(fdecl, 1) ){
            puts("ic_kludge_destroy: fdecl - call to ic_func_decl_destroy failed");
            return 1;
        }
    }

    /* iterate through errors destroying each error
     *      list of Errors
     *      struct ic_pvector errors;
     */
    len = ic_pvector_length( &(kludge->errors) );
    if( len ){
        /* FIXME
         * here we just throw a fit as we have no way of cleaning up these errors
         */
        puts("ic_kludge_destroy: found an error we wanted to destroy, but no way to destroy it");
        return 1;
    }

    /* call ast_destroy on our annotated ast
     *       annotated AST
     *       struct ic_ast *aast;
     */
    if( ic_ast_destroy( kludge->aast, 1 ) ){
        puts("ic_kludge_destroy: asst - call to ic_ast_destroy failed");
        return 1;
    }

    /* if asked nicely, destroy destroy destroy! */
    if( free_kludge ){
        free(kludge);
    }

    return 0;
}

/* add a new type decl to this kludge
 * this will insert into dict_tname and also
 * into tdecls
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_kludge_add_tdecl(struct ic_kludge *kludge, struct ic_type_decl *tdecl){
    if( ! kludge ){
        puts("ic_kludge_add_tdecl: kludge was null");
        return 1;
    }
    if( ! tdecl ){
        puts("ic_kludge_add_tdecl: tdecl was null");
        return 1;
    }

    /* insert into dict tname
     * returns 0 on failure
     */
    if( ! ic_dict_insert(&(kludge->dict_tname), ic_type_decl_str(tdecl), tdecl) ){
        puts("ic_kludge_add_tdecl: call to ic_dict_insert failed");
        return 1;
    }

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
unsigned int ic_kludge_add_fdecl(struct ic_kludge *kludge, struct ic_func_decl *fdecl){
    if( ! kludge ){
        puts("ic_kludge_add_fdecl: kludge was null");
        return 1;
    }
    if( ! fdecl ){
        puts("ic_kludge_add_fdecl: fdecl was null");
        return 1;
    }

    /* insert into dict tname
     * returns 0 on failure
     */
    if( ! ic_dict_insert(&(kludge->dict_fsig), ic_func_decl_str(fdecl), fdecl) ){
        puts("ic_kludge_add_fdecl: call to ic_dict_insert failed");
        return 1;
    }

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
unsigned int ic_kludge_add_error(struct ic_kludge *kludge, void *error){
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

