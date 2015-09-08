#include <stdio.h> /* puts */
#include <stdlib.h> /* calloc */

#include "kludge.h"
#include "../../data/pvector.h"
#include "../../parse/data/decl.h"

/* takes an ast and breaks it down to populate the supplied kludge
 *
 * returns 1 on success
 * return 0 on failure
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
            return 0;
        }

        /* dispatch on type to appropriate kludge_add function */
        switch(decl->type){
            case ic_decl_func_decl:
                if( ! ic_kludge_add_fdecl(kludge, &(decl->u.fdecl)) ){
                    puts("ic_kludge_populate_from_ast: call to ic_kludge_add_fdecl failed");
                    return 0;
                }
                break;

            case ic_decl_type_decl:
                if( ! ic_kludge_add_tdecl(kludge, &(decl->u.tdecl)) ){
                    puts("ic_kludge_populate_from_ast: call to ic_kludge_add_tdecl failed");
                    return 0;
                }
                break;

            default:
                puts("ic_kludge_populate_from_ast: decl had impossible type");
                return 0;
                break;
        }
    }

    return 1;
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
    if( ! ic_kludge_init(kludge, ast) ){
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
 * returns 1 on success
 * returns 0 on error
 */
unsigned int ic_kludge_init(struct ic_kludge *kludge, struct ic_ast *ast){
    if( ! kludge ){
        puts("ic_kludge_init: kludge was null");
        return 0;
    }
    if( ! ast ){
        puts("ic_kludge_init: ast was null");
        return 0;
    }

    /* dict_tname hash */
    if( ! ic_dict_init(&(kludge->dict_tname)) ){
        puts("ic_kludge_init: dict_tname: call to ic_dict_init failed");
        return 0;
    }

    /* dict_fsig hash */
    if( ! ic_dict_init(&(kludge->dict_fsig)) ){
        puts("ic_kludge_init: dict_fsig: call to ic_dict_init failed");
        return 0;
    }

    /* pvector tdecls */
    if( ! ic_pvector_init( &(kludge->tdecls), 0 ) ){
        puts("ic_kludge_init: tdecl: call to ic_pvector_init failed");
        return 0;
    }

    /* pvector fdecls */
    if( ! ic_pvector_init( &(kludge->fdecls), 0 ) ){
        puts("ic_kludge_init: fdecl: call to ic_pvector_init failed");
        return 0;
    }

    /* pvector error */
    if( ! ic_pvector_init( &(kludge->errors), 0 ) ){
        puts("ic_kludge_init: errors: call to ic_pvector_init failed");
        return 0;
    }

    /* ast ast */
    kludge->aast = ast;

    /* populate kludge fiels from ast:
     *      dict_tname
     *      dict_fsig
     *      tdecls
     *      fdecls
     */
    if( ! ic_kludge_populate_from_ast(kludge, ast) ){
        puts("ic_kludge_init: errors: call to ic_kludge_populate_from_ast failed");
        return 0;
    }


    return 1;
}

/* destroy kludge
 *
 * will only free kludge if `free_kludge` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_kludge_destroy(struct ic_kludge *kludge, unsigned int free_kludge){
    /* cached len */
    unsigned int len = 0;

    if( !kludge ){
        puts("ic_kludge_destroy: kludge was null");
        return 0;
    }

    /* NOTE we have to be careful as various parts of the kludge share the same
     * objects
     *
     * for example each fdecl will be in:
     *  fict_fsig
     *  fdecls
     *  aast
     *
     * destroying all three would be an error
     *
     * this means that we do not need to destroy the items stored in the following as
     *  their elements will be freed via ic_ast_destroy on kludge->aast :
     *      dict_tname
     *      dict_fsig
     *      tdecls
     *      fdecls
     *
     * note that we must still ensure to free the above data structures themselves
     */

    /* cleanup dict_tname
     * ic_dict_destroy(*dict, free_dict, free_data);
     * do not free_dict as it is a member of kludge
     * do not free_data as it is freed when aast is freed
     */
    if( ! ic_dict_destroy( &(kludge->dict_tname), 0, 0 ) ){
        puts("ic_kludge_destroy: call to ic_dict_destroy for dict_tname failed");
        return 0;
    }

    /* cleanup dict_fsig
     * ic_dict_destroy(*dict, free_dict, free_data);
     * do not free_dict as it is a member of kludge
     * do not free_data as it is freed when aast is freed
     */
    if( ! ic_dict_destroy( &(kludge->dict_fsig), 0, 0 ) ){
        puts("ic_kludge_destroy: call to ic_dict_destroy for dict_fsig failed");
        return 0;
    }

    /* cleanup tdecls
     * ic_pvector_destroy(*vec, free_vec, (*destroy_item)());
     * do not free_vec as it is a member of kludge
     * no need for destroy_item funcion
     */
    if( ! ic_pvector_destroy( &(kludge->tdecls), 0, 0 ) ){
        puts("ic_kludge_destroy: call to ic_pvector_destroy for tdecls failed");
        return 0;
    }

    /* cleanup fdecls
     * ic_pvector_destroy(*vec, free_vec, (*destroy_item)());
     * do not free_vec as it is a member of kludge
     * no need for destroy_item funcion
     */
    if( ! ic_pvector_destroy( &(kludge->fdecls), 0, 0 ) ){
        puts("ic_kludge_destroy: call to ic_pvector_destroy for fdecls failed");
        return 0;
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
        return 0;
    }

    /* call ast_destroy on our annotated ast
     *       annotated AST
     *       struct ic_ast *aast;
     *
     */
    if( ! ic_ast_destroy( kludge->aast, 1 ) ){
        puts("ic_kludge_destroy: asst - call to ic_ast_destroy failed");
        return 0;
    }

    /* if asked nicely, destroy destroy destroy! */
    if( free_kludge ){
        free(kludge);
    }

    return 1;
}

/* add a new type decl to this kludge
 * this will insert into dict_tname and also
 * into tdecls
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_kludge_add_tdecl(struct ic_kludge *kludge, struct ic_type_decl *tdecl){
    char * str = 0;

    if( ! kludge ){
        puts("ic_kludge_add_tdecl: kludge was null");
        return 0;
    }
    if( ! tdecl ){
        puts("ic_kludge_add_tdecl: tdecl was null");
        return 0;
    }

    /* cache str
     * do not need to free as this char* is stored on the tdecl
     */
    str = ic_type_decl_str(tdecl); if( ! str ){
        puts("ic_kludge_add_tdecl: call to ic_type_decl_str failed");
        return 0;
    }

    /* check for exists first to aid diagnostics */
    if( ic_dict_exists( &(kludge->dict_tname), str ) ){
        printf("ic_kludge_add_tdecl: type '%s' already exists on this kludge\n", str);
        return 0;
    }

    /* insert into dict tname
     * returns 0 on failure
     */
    if( ! ic_dict_insert(&(kludge->dict_tname), str, tdecl) ){
        puts("ic_kludge_add_tdecl: call to ic_dict_insert failed");
        return 0;
    }

    /* insert into list of tdecls */
    if( -1 == ic_pvector_append( &(kludge->tdecls), tdecl ) ){
        return 0;
    }

    return 1;
}

/* add a new func decl to this kludge
 * this will insert into dict_fname and also
 * into fsigs
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_kludge_add_fdecl(struct ic_kludge *kludge, struct ic_func_decl *fdecl){
    char * str = 0;

    if( ! kludge ){
        puts("ic_kludge_add_fdecl: kludge was null");
        return 0;
    }
    if( ! fdecl ){
        puts("ic_kludge_add_fdecl: fdecl was null");
        return 0;
    }

    /* cache str
     * do not need to free as this char* is stored on the fdecl
     */
    str = ic_func_decl_str(fdecl);
    if( ! str ){
        puts("ic_kludge_add_fdecl: call to ic_func_decl_str failed");
        return 0;
    }

    /* check for exists first to aid diagnostics */
    if( ic_dict_exists( &(kludge->dict_fsig), str ) ){
        printf("ic_kludge_add_fdecl: function signature '%s' already exists on this kludge\n", str);
        return 0;
    }

    /* insert into dict tname
     * returns 0 on failure
     */
    if( ! ic_dict_insert(&(kludge->dict_fsig), str, fdecl) ){
        puts("ic_kludge_add_fdecl: call to ic_dict_insert failed");
        return 0;
    }

    /* insert into list of fdecls */
    if( -1 == ic_pvector_append( &(kludge->fdecls), fdecl )  ){
        puts("ic_kludge_add_fdecl: call to ic_pvector_append failed");
        return 0;
    }

    return 1;
}

/* retrieve type decl by string
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_type_decl * ic_kludge_get_tdecl(struct ic_kludge *kludge, char *tdecl_str){
    if( ! kludge ){
        puts("ic_kludge_get_tdecl: kludge was null");
        return 0;
    }

    if( ! tdecl_str ){
        puts("ic_kludge_get_tdecl: tdecl_str was null");
        return 0;
    }

    return ic_dict_get( &(kludge->dict_tname), tdecl_str );
}

/* retrieve type decl by string
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_func_decl * ic_kludge_get_fdecl(struct ic_kludge *kludge, char *fdecl_str){
    if( ! kludge ){
        puts("ic_kludge_get_fdecl: kludge was null");
        return 0;
    }

    if( ! fdecl_str ){
        puts("ic_kludge_get_fdecl: fdecl_str was null");
        return 0;
    }

    return ic_dict_get( &(kludge->dict_fsig), fdecl_str );
}

/* add a new error to error list
 *
 * FIXME no error type
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_kludge_add_error(struct ic_kludge *kludge, void *error){
    if( ! kludge ){
        puts("ic_kludge_add_error: kludge was null");
        return 0;
    }
    if( ! error ){
        puts("ic_kludge_add_error: error was null");
        return 0;
    }

    /* just a simple pvector of errors */
    if( -1 == ic_pvector_append(&(kludge->errors), error) ){
        puts("ic_kludge_add_error: call to ic_pvector_append failed");
        return 0;
    }

    return 1;
}

