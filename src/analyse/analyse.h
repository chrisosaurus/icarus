#ifndef ICARUS_ANALYSE_H
#define ICARUS_ANALYSE_H

#include "../parse/data/ast.h"
#include "../parse/data/decl.h"
#include "../parse/data/expression.h"
#include "data/kludge.h"

/* ignore unused parameter */
#pragma GCC diagnostic ignored "-Wunused-parameter"
/* ignore unused functions */
#pragma GCC diagnostic ignored "-Wunused-function"

/* takes an ast and performs analysis on it
 * this returns a kludge
 *
 * see kludge->errors for errors
 *
 * returns kludge on success
 * returns 0 on failure
 */
struct ic_kludge * ic_analyse(struct ic_ast *ast);

/* takes a type_decl and performs analysis
 *
 * FIXME need a way of signalling and passing errors
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_analyse_type_decl(struct ic_kludge *kludge, struct ic_type_decl *tdecl);

/* takes a func_decl and performs analysis
 *
 * FIXME need a way of signalling and passing errors
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_analyse_func_decl(struct ic_kludge *kludge, struct ic_func_decl *fdecl);

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
struct ic_symbol * ic_analyse_infer(struct ic_kludge *kludge, struct ic_expr *expr);

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
unsigned int ic_analyse_check(struct ic_kludge *kludge, struct ic_stmt *stmt);

#endif

