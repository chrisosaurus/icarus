#ifndef ICARUS_ANALYSE_HELPERS_H
#define ICARUS_ANALYSE_HELPERS_H

#include "../data/pvector.h"
#include "../data/scope.h"
#include "../parse/data/expr.h"
#include "../parse/data/stmt.h"
#include "data/kludge.h"

/* iterate through the field list checking:
 *  a) all field's names are unique within this list
 *  b) all field's types exist in this kludge
 *
 * `unit` and `unit_name` are used for error printing
 * it is always printed as '%s for %s error goes here'
 * e.g. unit of 'function declaration', name of 'Foo'
 *
 * returns 1 on success (all fields are valid as per the 3 rules)
 * returns 0 on failure
 */
unsigned int ic_analyse_field_list(char *unit, char *unit_name, struct ic_kludge *kludge, struct ic_pvector *type_params, struct ic_pvector *fields);

/* perform analysis on body
 * this will iterate through each statement and perform analysis
 *
 * `unit` and `unit_name` are used for error printing
 * it is always printed as '%s for %s error goes here'
 * e.g. unit of 'function declaration', name of 'Foo'
 * returns 1 on success (pass)
 * returns 0 on failure
 */
unsigned int ic_analyse_body(char *unit, char *unit_name, struct ic_kludge *kludge, struct ic_body *body, struct ic_decl_func *fdecl);

/* takes an expr and returns the inferred type
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
 *  infer Foo(1,"hello") -> Foo(Int,String) -> Foo
 *
 * returns ic_type * on success
 * returns 0 on failure
 */
struct ic_decl_type *ic_analyse_infer(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_expr *expr);

struct ic_decl_type *ic_analyse_infer_constant(struct ic_kludge *kludge, struct ic_expr_constant *cons);

struct ic_decl_type *ic_analyse_infer_fcall(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_expr_func_call *fcall);

/* perform analyse of let statement in the provided body
 *
 * returns 1 for success
 * returns 0 on failure
 */
unsigned int ic_analyse_let(char *unit, char *unit_name, struct ic_kludge *kludge, struct ic_body *body, struct ic_stmt_let *let);

/* create a function signature string from a function call
 *
 * this function must be compatible with the one produced
 * by `ic_decl_func_sig_call`
 *      foo(Sint,Sint)
 *
 * and
 *      bar(&Sint,String)
 *
 * for a generic function this code has a special case, if we have a function
 *     fn id[T](t::T) -> return t end
 *
 * and an fcall of the form
 *     id[Sint](6s)
 *
 * we need to check first for this function as
 *     id[Sint](Sint)
 * if that is not found, then we must check for
 *     id[_](_)
 * and proceed with instantiation
 *
 * returns char * on success
 * returns 0 on failure
 */
char *ic_analyse_fcall_str(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_expr_func_call *fcall);

#endif /* ifndef ICARUS_ANALYSE_HELPERS_H */
