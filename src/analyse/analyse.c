#include <stdlib.h> /* FIMXE exit */
#include <stdio.h>

#include "analyse.h"

/* takes an ast and performs analysis on it
 * this returns a kludge
 *
 * see kludge->errors for errors
 */
struct ic_kludge * analyse(struct ic_ast *ast){
    puts("analyse: unimplemented");
    exit(1);
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


