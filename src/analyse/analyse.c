#include <stdlib.h> /* FIMXE exit */
#include <stdio.h>
#include <string.h> /* strcmp */

#include "helpers.h"
#include "analyse.h"

/* takes an ast and performs analysis on it
 * this returns a kludge
 *
 * see kludge->errors for errors
 *
 * returns kludge on success
 * returns 0 on failure
 */
struct ic_kludge * ic_analyse(struct ic_ast *ast){
    /* our offset into various lists */
    unsigned int i = 0;
    /* cached len of various lists */
    unsigned int len = 0;

    /* the current type we are analysing */
    struct ic_type_decl *tdecl = 0;
    /* the current func we are analysing */
    struct ic_func_decl *fdecl = 0;

    /* our mighty kludge */
    struct ic_kludge *kludge = 0;

    /* steps:
     *      create kludge from ast
     *      for each type call ic_analyse_type_decl
     *      for each func call ic_analyse_func_decl
     */

    if( ! ast ){
        puts("ic_analyse: ast null");
        return 0;
    }

    /* create kludge from ast */
    kludge = ic_kludge_new(ast);
    if( ! kludge ){
        puts("ic_analyse: call to ic_kludge_new failed");
        return 0;
    }

    /* for each type call ic_analyse_type_decl */
    len = ic_pvector_length(&(kludge->tdecls));
    for( i=0; i<len; ++i ){
        tdecl = ic_pvector_get(&(kludge->tdecls), i);
        if( ! tdecl ){
            puts("ic_analyse: call to ic_pvector_get failed for tdecl");
            goto ERROR;
        }

        if( ic_analyse_type_decl(kludge, tdecl) ){
            puts("ic_analyse: call to ic_analyse_type_decl failed");
            goto ERROR;
        }
    }

    /* for each func call analyst_func_decl */
    len = ic_pvector_length(&(kludge->fdecls));
    for( i=0; i<len; ++i ){
        fdecl = ic_pvector_get(&(kludge->fdecls), i);
        if( ! fdecl ){
            puts("ic_analyse: call to ic_pvector_get failed for fdecl");
            goto ERROR;
        }

        if( ic_analyse_func_decl(kludge, fdecl) ){
            puts("ic_analyse: call to ic_analyse_func_decl failed");
            goto ERROR;
        }
    }

    return kludge;

ERROR:
    /* destroy kludge
     * free_kludge as allocated with new
     */
    if( ic_kludge_destroy(kludge, 1) ){
        puts("ic_analyse: call to ic_kludge_destroy failed in error case");
    }

    return 0;
}

/* takes a type_decl and performs analysis
 *
 * FIXME need a way of signalling and passing errors
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_analyse_type_decl(struct ic_kludge *kludge, struct ic_type_decl *tdecl){
    /* name of current type we are trying to declare */
    char *this_type = 0;

    if( ! kludge ){
        puts("ic_analyse_type_decl: kludge was null");
        return 1;
    }

    if( ! tdecl ){
        puts("ic_analyse_type_decl: tdecl was null");
        return 1;
    }

    this_type = ic_type_decl_str(tdecl);
    if( ! this_type ){
        puts("ic_analyse_type_decl: for this_type: call to ic_type_decl_str failed");
        return 1;
    }

    /* check fields */
    if( ic_analyse_field_list( "type declaration", this_type, kludge, &(tdecl->fields), this_type ) ){
        puts("ic_analyse_type_decl: call to ic_analyse_field_list for field validation failed");
        goto ERROR;
    }

    return 0;

ERROR:

    return 1;
}

/* takes a func_decl and performs analysis
 *
 * FIXME need a way of signalling and passing errors
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_analyse_func_decl(struct ic_kludge *kludge, struct ic_func_decl *fdecl){
    /* name of current func we are trying to declare */
    char *this_func = 0;

    if( ! kludge ){
        puts("ic_analyse_func_decl: kludge was null");
        return 1;
    }

    if( ! fdecl ){
        puts("ic_analyse_func_decl: fdecl was null");
        return 1;
    }

    /* name of this func, useful for error printing */
    this_func = ic_func_decl_str(fdecl);
    if( ! this_func ){
        puts("ic_analyse_func_decl: for this_type: call to ic_func_decl_str failed");
        return 1;
    }

    /* check arg list */
    if( ic_analyse_field_list( "func declaration", this_func, kludge, &(fdecl->args), 0 ) ){
        puts("ic_analyse_func_decl: call to ic_analyse_field_list for validating argument list failed");
        goto ERROR;
    }

    /* check body */
    if( ic_analyse_body( "func declaration", this_func, kludge, &(fdecl->body) ) ){
        puts("ic_analyse_func_decl: call to ic_analyse_bofy for validating body failed");
        goto ERROR;
    }

    puts("ic_analyse_func_decl: implementation pending");
    return 0;

ERROR:

    puts("ic_analyse_func_decl: error");
    return 1;
}

