#include <stdlib.h> /* FIMXE exit */
#include <stdio.h>

#include "analyse.h"

/* ignored unused parameter */
#pragma GCC diagnostic ignored "-Wunused-parameter"
/* ignored unused variable */
#pragma GCC diagnostic ignored "-Wunused-variable"


/* takes an ast and performs analysis on it
 * this returns a kludge
 *
 * see kludge->errors for errors
 *
 * returns kludge on success
 * returns 0 on failure
 */
struct ic_kludge * analyse(struct ic_ast *ast){
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
     *      for each type call analyse_type_decl
     *      for each func call analyst_func_decl
     */

    if( ! ast ){
        puts("analyse: ast null");
        return 0;
    }

    /* create kludge from ast */
    kludge = ic_kludge_new(ast);
    if( ! kludge ){
        puts("analyse: call to ic_kludge_new failed");
        return 0;
    }
    /* for each type call analyse_type_decl */
    len = ic_pvector_length(&(kludge->tdecls));
    for( i=0; i<len; ++i ){
        tdecl = ic_pvector_get(&(kludge->tdecls), i);
        if( ! tdecl ){
            puts("analyse: call to ic_pvector_get failed for tdecl");
            return 0;
        }

        if( analyse_type_decl(kludge, tdecl) ){
            puts("analyse: call to analyse_type_decl failed");
            return 0;
        }
    }

    /* for each func call analyst_func_decl */
    len = ic_pvector_length(&(kludge->fdecls));
    for( i=0; i<len; ++i ){
        fdecl = ic_pvector_get(&(kludge->fdecls), i);
        if( ! fdecl ){
            puts("analyse: call to ic_pvector_get failed for fdecl");
            return 0;
        }

        if( analyse_func_decl(kludge, fdecl) ){
            puts("analyse: call to analyse_type_decl failed");
            return 0;
        }
    }

    return kludge;
}

/* takes a type_decl and performs analysis
 *
 * FIXME need a way of signalling and passing errors
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int analyse_type_decl(struct ic_kludge *kludge, struct ic_type_decl *tdecl){
    puts("analyse_type_decl: unimplemented");
    exit(1);
}

/* takes a func_decl and performs analysis
 *
 * FIXME need a way of signalling and passing errors
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int analyse_func_decl(struct ic_kludge *kludge, struct ic_func_decl *fdecl){
    puts("analyse_func_decl: unimplemented");
    exit(1);
}

/* takes an expr and returns the inferred type as a symbol
 *
 * FIXME need a way of signalling error and passing errors
 *
 * examples:
 *  infer 1 -> Int
 *  infer "helo" -> String
 *  infer addone(1) -> addone(Int)->Int -> Int
 *  infer Foo(1 "hello") -> Foo(Int String) -> Foo
 *
 * returns symbol on success
 * returns 0 on error
 */
struct ic_symbol * analyse_infer(struct ic_kludge *kludge, struct ic_expr *expr){
    puts("analyse_infer: unimplemented");
    exit(1);
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
unsigned int analyse_check(struct ic_kludge *kludge, struct ic_stmt *stmt){
    puts("analyse_check: unimplemented");
    exit(1);
}


