#include <stdlib.h> /* FIMXE exit */
#include <stdio.h>
#include <string.h> /* strcmp */

#include "../parse/data/field.h"
#include "../parse/data/statement.h"
#include "../data/set.h"
#include "helpers.h"
#include "analyse.h"

/* ignored unused parameter */
#pragma GCC diagnostic ignored "-Wunused-parameter"
/* ignored unused variable */
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"


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
    /* index into fields */
    unsigned int i = 0;

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
    /* index into body */
    unsigned int i = 0;
    /* len of body */
    unsigned int len = 0;
    /* current statement in body */
    struct ic_stmt *stmt = 0;

    if( ! kludge ){
        puts("ic_analyse_func_decl: kludge was null");
        return 1;
    }

    if( ! fdecl ){
        puts("ic_analyse_func_decl: fdecl was null");
        return 1;
    }

    /* name of this type, useful for error printing */
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


    /* step through body checking each statement
     * FIXME
     */
    len = ic_body_length( &(fdecl->body) );
    for( i=0; i<len; ++i ){
        stmt = ic_body_get( &(fdecl->body), i );
        if( ! stmt ){
            printf("ic_analyse_func_decl: call to ic_body_get failed for i '%d' in fdecl for function '%s'\n",
                    i,
                    this_func);
            goto ERROR;
        }

        /* FIXME in all cases we must do some work:
         *  1) if fcall then check exists, and bind
         *  2) if variable used then check type, and bind
         */
        switch( stmt->type ){
            case ic_stmt_type_ret:
                /* infer type of expression
                 * check returned value matches declared return type
                 */
                puts("ic_analyse_func_decl: unimplemented stmt->type ic_stmt_type_ref");
                goto ERROR;

            case ic_stmt_type_let:
                /* infer type of init. expression
                 * check against declared let type (if declared)
                 * check all mentioned types exist
                 */
                puts("ic_analyse_func_decl: unimplemented stmt->type ic_stmt_type_let");
                goto ERROR;

            case ic_stmt_type_if:
                /* need to validate expression
                 * need to the recurse to validate the body in
                 * each branch
                 *
                 * FIXME no support for recursing
                 */
                puts("ic_analyse_func_decl: unimplemented stmt->type ic_stmt_type_if");
                goto ERROR;

            case ic_stmt_type_expr:
                /* infer expr type
                 * warn if using non-void function in void context
                 */
                puts("ic_analyse_func_decl: unimplemented stmt->type ic_stmt_type_expr");
                goto ERROR;

            default:
                printf("ic_analyse_func_decl: impossible stmt->type '%d'\n", stmt->type);
                goto ERROR;
        }

        /* FIXME */
    }

    puts("ic_analyse_func_decl: implementation pending");
    return 0;

ERROR:

    puts("ic_analyse_func_decl: error");
    return 1;
}

/* takes an expr and returns the inferred type as a symbol
 *
 * FIXME need a way of signalling error and passing errors
 * possible suggestions:
 *  0 -> unrecoverable / internal error
 *  special ic_type_ref * -> syntax error -> ignore this statement, record error, keep going
 *  ic_type_ref * -> actual type inferred
 *
 * examples:
 *  infer 1 -> Int
 *  infer "helo" -> String
 *  infer addone(1) -> addone(Int)->Int -> Int
 *  infer Foo(1 "hello") -> Foo(Int String) -> Foo
 *
 * returns ic_type_ref on success
 * returns 0 on error
 */
struct ic_type_ref * ic_analyse_infer(struct ic_kludge *kludge, struct ic_expr *expr){

    if( ! kludge ){
        puts("ic_analyse_infer: kludge was null");
        return 0;
    }

    if( ! expr ){
        puts("ic_analyse_infer: expr was null");
        return 0;
    }

    switch( expr->type ){
        case ic_expr_type_func_call:
            /*
             *  infer addone(1) -> addone(Int)->Int -> Int
             *  expr->type == func_call
             *  fc = expr->u.fcall
             *  fstr = str(fc) // FIXME ic_func_call_str doesn't exist
             *  fdecl = kludge get fdecl from fstr
             *  tstr = fdecl->ret_type
             *  type = kludge get tdecl from tstr
             *  return type
             */

            /*
             *  infer Foo(1 "hello") -> Foo(Int String) -> Foo
             *  expr->type == func_call
             *  fc = expr->u.fcall
             *  fstr = str(fc) // FIXME ic_func_call_str doesn't exist
             *  fdecl = kludge get fdecl from fstr
             *  tstr = fdecl->ret_type
             *  type = kludge get tdecl from tstr
             *  return type
             */

            puts("ic_analyse_infer: ic_expr_type_func_call unimplemented");
            return 0;

        case ic_expr_type_identifier:
            /*
             *  infer f -> typeof contents of f
             *  expr->type = identifier
             *  id = expr->u.id
             *  name = str(id)
             *  variable = get variable within scope name // FIXME no mechanism
             *  type = get type of variable // FIXME no mechanism
             *  return type
             */

            puts("ic_analyse_infer: ic_expr_type_identifier unimplemented");
            return 0;

        case ic_expr_type_constant:
            /*
             *  infer 1 -> Int
             *  expr->type == constant
             *  cons = expr->u.cons
             *  cons->type == integer
             *  return integer
             */

            /*
             *  infer "hello" -> String
             *  expr->type == constant
             *  cons = expr->u.cons
             *  cons->type == string
             *  return string
             */

            puts("ic_analyse_infer: ic_expr_type_constant unimplemented");
            return 0;

        case ic_expr_type_operator:
            puts("ic_analyse_infer: ic_expr_type_operator unimplemented");
            return 0;

        default:
            printf("ic_analyse_infer: unknown expr->type '%d'\n", expr->type);
            return 0;
    }

    puts("ic_analyse_infer: unimplemented");
    return 0;
}

/* check a statement for validity
 *
 * FIXME need a way of signalling and passing errors
 *
 * examples:
 *  check let f::Foo = Foo(addone(1) "hello")
 *  check d(f)
 *  check print(s)
 *
 * returns 0 for success
 * returns 1 for error
 */
unsigned int ic_analyse_check(struct ic_kludge *kludge, struct ic_stmt *stmt){
    puts("ic_analyse_check: unimplemented");
    exit(1);
}


