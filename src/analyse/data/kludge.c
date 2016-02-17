#include <stdio.h> /* puts */
#include <stdlib.h> /* calloc */
#include <string.h> /* strncmpy */

#include "../../data/pvector.h"
#include "../../parse/data/decl.h"
#include "kludge.h"
#include "type.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"

/* populate the kludge from the provided ast
 * this call will break apart the ast to populate the
 * fields stored on kludge
 *
 * this will NOT perform any analysis
 */
unsigned int ic_kludge_populate(struct ic_kludge *kludge, struct ic_ast *ast){
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
            puts("ic_kludge_populate: call to ic_ast_get failed");
            return 0;
        }

        /* dispatch on type to appropriate kludge_add function */
        switch(decl->tag){
            case ic_decl_tag_func:
            case ic_decl_tag_builtin_func:
                if( ! ic_kludge_add_fdecl(kludge, &(decl->u.fdecl)) ){
                    puts("ic_kludge_populate: call to ic_kludge_add_fdecl failed");
                    return 0;
                }
                break;

            case ic_decl_tag_type:
            case ic_decl_tag_builtin_type:
                if( ! ic_kludge_add_tdecl(kludge, &(decl->u.tdecl)) ){
                    puts("ic_kludge_populate: call to ic_kludge_add_tdecl failed");
                    return 0;
                }
                break;

            case ic_decl_tag_builtin_op:
                if( ! ic_kludge_add_op(kludge, &(decl->u.op)) ){
                    puts("ic_kludge_populate: call to ic_kludge_add_op failed");
                    return 0;
                }
                break;

            default:
                puts("ic_kludge_populate: decl had impossible type");
                return 0;
                break;
        }
    }

    return 1;
}

/* alloc and init a new kludge
 *
 * this will NOT perform any analysis
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_kludge * ic_kludge_new(void){
    struct ic_kludge *kludge = 0;

     /* alloc */
    kludge = calloc(1, sizeof(struct ic_kludge));
    if( ! kludge ){
        puts("ic_kludge_new: call to calloc failed");
        return 0;
    }

    /* init */
    if( ! ic_kludge_init(kludge) ){
        puts("ic_kludge_new: call to ic_kludge_init failed");
        free(kludge);
        return 0;
    }

    /* success */
    return kludge;
}

/* init an existing kludge
 *
 * this will NOT perform any analysis
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_kludge_init(struct ic_kludge *kludge){
    if( ! kludge ){
        puts("ic_kludge_init: kludge was null");
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

    /* dict_op hash */
    if( ! ic_dict_init(&(kludge->dict_op)) ){
        puts("ic_kludge_init: dict_op: call to ic_dict_init failed");
        return 0;
    }

    /* pvector tdecls */
    if( ! ic_pvector_init( &(kludge->tdecls), 0 ) ){
        puts("ic_kludge_init: tdecl: call to ic_pvector_init failed");
        return 0;
    }

    /* pvector tbuiltins */
    if( ! ic_pvector_init( &(kludge->tbuiltins), 0 ) ){
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
     *
     * destroying all three would be an error
     *
     * this means that we do not need to destroy the items stored in the following as
     *  their elements will be freed via ic_ast_destroy:
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
     * do not free_data as it is freed when ast is freed
     */
    if( ! ic_dict_destroy( &(kludge->dict_tname), 0, 0 ) ){
        puts("ic_kludge_destroy: call to ic_dict_destroy for dict_tname failed");
        return 0;
    }

    /* cleanup dict_fsig
     * ic_dict_destroy(*dict, free_dict, free_data);
     * do not free_dict as it is a member of kludge
     * do not free_data as it is freed when ast is freed
     */
    if( ! ic_dict_destroy( &(kludge->dict_fsig), 0, 0 ) ){
        puts("ic_kludge_destroy: call to ic_dict_destroy for dict_fsig failed");
        return 0;
    }

    /* cleanup dict_op
     * ic_dict_destroy(*dict, free_dict, free_data);
     * do not free_dict as it is a member of kludge
     * do not free_data as it is freed when ast is freed
     *
     * FIXME currently leaking symbols in value
     */
    if( ! ic_dict_destroy( &(kludge->dict_op), 0, 0 ) ){
        puts("ic_kludge_destroy: call to ic_dict_destroy for dict_op failed");
        return 0;
    }

    /* cleanup tdecls
     * ic_pvector_destroy(*vec, free_vec, (*destroy_item)());
     * do not free_vec as it is a member of kludge
     * no need for destroy_item function
     */
    if( ! ic_pvector_destroy( &(kludge->tdecls), 0, 0 ) ){
        puts("ic_kludge_destroy: call to ic_pvector_destroy for tdecls failed");
        return 0;
    }

    /* cleanup tbuiltins
     * ic_pvector_destroy(*vec, free_vec, (*destroy_item)());
     * do not free_vec as it is a member of kludge
     * no need for destroy_item function
     *
     * FIXME currently leaking builtins
     */
    if( ! ic_pvector_destroy( &(kludge->tbuiltins), 0, 0 ) ){
        puts("ic_kludge_destroy: call to ic_pvector_destroy for tdecls failed");
        return 0;
    }


    /* cleanup fdecls
     * ic_pvector_destroy(*vec, free_vec, (*destroy_item)());
     * do not free_vec as it is a member of kludge
     * no need for destroy_item function
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

    /* if asked nicely, destroy destroy destroy! */
    if( free_kludge ){
        free(kludge);
    }

    return 1;
}

/* add a new type decl to this kludge
 * this will insert into tdecls
 * it will also construct a new ic_type and insert into dict_tname
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_kludge_add_tdecl(struct ic_kludge *kludge, struct ic_decl_type *tdecl){
    char * str = 0;
    struct ic_type *type = 0;

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
    str = ic_decl_type_str(tdecl);
    if( ! str ){
        puts("ic_kludge_add_tdecl: call to ic_decl_type_str failed");
        return 0;
    }

    /* check for exists first to aid diagnostics */
    if( ic_dict_exists( &(kludge->dict_tname), str ) ){
        printf("ic_kludge_add_tdecl: type '%s' already exists on this kludge\n", str);
        return 0;
    }

    /* construct new ic_type */
    type = ic_type_new_tdecl(tdecl);
    if( ! type ){
        puts("ic_kludge_add_tdecl: call to ic_type_new_tdecl failed");
        return 0;
    }

    if( ! strncmp(str, "Void", 4) ){
        /* if this is the void type then mark it as so */
        if( ! ic_decl_type_mark_void(tdecl) ){
            puts("ic_kludge_add_tdecl: call to ic_decl_type_mark_void failed");
            return 0;
        }
    } else if( ! strncmp(str, "Bool", 4) ){
        /* if this is the bool type then mark it as so */
        if( ! ic_decl_type_mark_bool(tdecl) ){
            puts("ic_kludge_add_tdecl: call to ic_decl_type_mark_bool failed");
            return 0;
        }
    }

    /* insert into dict tname
     * returns 0 on failure
     */
    if( ! ic_dict_insert(&(kludge->dict_tname), str, type) ){
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
unsigned int ic_kludge_add_fdecl(struct ic_kludge *kludge, struct ic_decl_func *fdecl){
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
    str = ic_decl_func_sig_call(fdecl);
    if( ! str ){
        puts("ic_kludge_add_fdecl: call to ic_decl_func_sig_call failed");
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

/* add a new op decl to this kludge
 * this will insert into dict_op and also
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_kludge_add_op(struct ic_kludge *kludge, struct ic_decl_op *op){
    char *from_str = 0;
    struct ic_symbol *to_sym = 0;

    if( ! kludge ){
        puts("ic_kludge_add_op: kludge was null");
        return 0;
    }

    if( ! op ){
        puts("ic_kludge_add_op: kludge was null");
        return 0;
    }

    /* cache str
     * do not need to free as this char* is stored on the fdecl
     */
    from_str = ic_symbol_contents( &(op->from) );
    if( ! from_str ){
        puts("ic_kludge_add_op: call to ic_symbol_contents failed");
        return 0;
    }

    /* check for exists first to aid diagnostics */
    if( ic_dict_exists( &(kludge->dict_op), from_str ) ){
        printf("ic_kludge_add_op: operator '%s' already exists on this kludge\n", from_str);
        return 0;
    }

    /* create symbol for function, this is our value */
    to_sym = &(op->to);
    if( ! to_sym ){
        puts("ic_kludge_add_op: fetching to_sym failed");
        return 0;
    }

    /* insert into dict_op */
    if( ! ic_dict_insert(&(kludge->dict_op), from_str, to_sym) ){
        puts("ic_kludge_add_op: ic_dict_insert failed");
        return 0;
    }

    return 1;
}


/* retrieve ic_type by string
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_type * ic_kludge_get_type(struct ic_kludge *kludge, char *tdecl_str){
    if( ! kludge ){
        puts("ic_kludge_get_type: kludge was null");
        return 0;
    }

    if( ! tdecl_str ){
        puts("ic_kludge_get_type: tdecl_str was null");
        return 0;
    }

    return ic_dict_get( &(kludge->dict_tname), tdecl_str );
}

/* retrieve ic_type by symbol
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_type * ic_kludge_get_type_from_symbol(struct ic_kludge *kludge, struct ic_symbol *type){
    char * type_str = 0;

    if( ! kludge ){
        puts("ic_kludge_get_type_from_symbol: kludge was null");
        return 0;
    }

    if( ! type ){
        puts("ic_kludge_get_type_from_symbol: type was null");
        return 0;
    }

    type_str = ic_symbol_contents(type);
    if( ! type_str ){
        puts("ic_kludge_get_type_from_symbol: call to ic_symbol_contents failed");
        return 0;
    }

    return ic_kludge_get_type(kludge, type_str);
}

/* retrieve ic_type by type_ref
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_type * ic_kludge_get_type_from_typeref(struct ic_kludge *kludge, struct ic_type_ref *type_ref){
    if( ! kludge ){
        puts("ic_kludge_get_type_from_typeref: kludge was null");
        return 0;
    }

    if( ! type_ref ){
        puts("ic_kludge_get_type_from_typeref: type was null");
        return 0;
    }

    if( type_ref->tag != ic_type_ref_symbol ){
        puts("ic_kludge_get_type_from_typeref: type ref was not of type symbol");
        return 0;
    }

    return ic_kludge_get_type_from_symbol(kludge, &(type_ref->u.sym));
}

/* retrieve func decl by string
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_decl_func * ic_kludge_get_fdecl(struct ic_kludge *kludge, char *fdecl_str){
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

/* retrieve func decl by symbol
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_decl_func * ic_kludge_get_fdecl_from_symbol(struct ic_kludge *kludge, struct ic_symbol *fdecl){
    char *fdecl_str = 0;

    if( ! kludge ){
        puts("ic_kludge_get_fdecl_from_symbol: kludge was null");
        return 0;
    }

    if( ! fdecl ){
        puts("ic_kludge_get_fdecl_from_symbol: fdecl was null");
        return 0;
    }

    fdecl_str = ic_symbol_contents(fdecl);
    if( ! fdecl_str ){
        puts("ic_kludge_get_fdecl_from_symbol: call to ic_symbol_contents failed");
        return 0;
    }

    return ic_kludge_get_fdecl(kludge, fdecl_str);
}

/* retrieve the function name that this operator maps to
 *
 * '+' -> symbol('plus')
 *
 * returns * on success
 * returns 0 on error
 */
struct ic_symbol * ic_kludge_get_operator(struct ic_kludge *kludge, char *sym_str){
    struct ic_symbol *symbol = 0;

    if( ! kludge ){
        puts("ic_kludge_get_operator: kludge was null");
        return 0;
    }

    if( ! sym_str ){
        puts("ic_kludge_get_operator: sym_str was null");
        return 0;
    }

    symbol = ic_dict_get( &(kludge->dict_op), sym_str );
    if( ! symbol ){
        puts("ic_kludge_get_operator: call to ic_dict_get failed");
        return 0;
    }

    return symbol;
}

/* retrieve the function name that this operator maps to
 *
 * symbol('+') -> symbol('plus')
 *
 * returns * on success
 * returns 0 on error
 */
struct ic_symbol * ic_kludge_get_operator_from_symbol(struct ic_kludge *kludge, struct ic_symbol *symbol){
    struct ic_symbol *result = 0;
    char * str = 0;

    if( ! kludge ){
        puts("ic_kludge_get_operator: kludge was null");
        return 0;
    }

    if( ! symbol ){
        puts("ic_kludge_get_operator: symbol was null");
        return 0;
    }

    str = ic_symbol_contents(symbol);
    if( ! str ){
        puts("ic_kludge_get_operator: call to ic_symbol_contents failed");
        return 0;
    }

    result = ic_kludge_get_operator(kludge, str);
    if( ! result ){
        puts("ic_kludge_get_operator: call to ic_kludge_get_operator failed");
        return 0;
    }

    return result;
}

/* check if an existing identifier is taken either within the kludge or the provided scope
 *
 * if scope is not provided (null) then it will not be checked (and no error will be raised)
 *
 * this checks:
 *  already an existing an identifier within the scope
 *  already in use as a type name
 *  already in use as a function name
 *  already in use as an operator
 *
 * returns 1 if the identifier exists
 * returns 0 if the identifier does not exist
 * returns -1 on failure
 */
int ic_kludge_identifier_exists(struct ic_kludge *kludge, struct ic_scope *scope, char *identifier){
    if( ! kludge ){
        puts("ic_kludge_identifier_exists: kludge was null");
        return -1;
    }

    if( ! identifier ){
        puts("ic_kludge_identifier_exists: identifier was null");
        return -1;
    }

    /* only check in scope if provided, scope is optional */
    if( scope ){
        /* check if identifier is used within scope */
        if( ic_scope_get(scope, identifier) ){
            /* identifier was found in scope */
            return 1;
        }
    }

    /* check for existing type */
    if( ic_kludge_get_type(kludge, identifier) ){
        /* identifier was found as a type in kludge */
        return 1;
    }

    /* check for existing func */
    if( ic_kludge_get_fdecl(kludge, identifier) ){
        /* identifier was found as a func in kludge */
        return 1;
    }

    /* FIXME check for operator */

    return 0;
}

/* check if an existing identifier is taken either within the kludge or the provided scope
 *
 * if scope is not provided (null) then it will not be checked (and no error will be raised)
 *
 * this checks:
 *  already an existing an identifier within the scope
 *  already in use as a type name
 *  already in use as a function name
 *  already in use as an operator
 *
 * returns 1 if the identifier exists
 * returns 0 if the identifier does not exist
 * returns -1 on failure
 */
int ic_kludge_identifier_exists_symbol(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_symbol *identifier){
    if( ! kludge ){
        puts("ic_kludge_identifier_exists_symbol: kludge was null");
        return -1;
    }

    if( ! identifier ){
        puts("ic_kludge_identifier_exists_symbol: identifier was null");
        return -1;
    }

    /* only check in scope if provided, scope is optional */
    if( scope ){
        /* check if identifier is used within scope */
        if( ic_scope_get_from_symbol(scope, identifier) ){
            /* identifier was found in scope */
            return 1;
        }
    }

    /* check for existing type */
    if( ic_kludge_get_type_from_symbol(kludge, identifier) ){
        /* identifier was found as a type in kludge */
        return 1;
    }

    /* check for existing func */
    if( ic_kludge_get_fdecl_from_symbol(kludge, identifier) ){
        /* identifier was found as a func in kludge */
        return 1;
    }

    /* FIXME check for operator */

    return 0;
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

