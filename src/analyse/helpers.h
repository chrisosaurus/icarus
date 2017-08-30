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

/* resolve a type_refs to type_decls
 *
 * if type_ref to a type_param then this link is used
 * if type_ref of a symbol, then it will be looked up in type_params
 * if both fail, it is an error
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_resolve_type_ref(char *unit, char *unit_name, struct ic_pvector *type_params, struct ic_type_ref *type_ref);

/* iterate through the field list resolving any type_refs to type_decls
 *
 * if a field is a type_ref to a type_param then this link is used
 * if a field is a type_ref of a symbol, then it will be looked up in type_params
 * if both fail, it is an error
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_resolve_field_list(char *unit, char *unit_name, struct ic_pvector *type_params, struct ic_pvector *fields);

/* iterate through a type_ref list resolving each type_ref to a decl_type
 *
 * returns 1 on success
 * returns 0 on error
 */
unsigned int ic_analyse_type_ref_list(struct ic_kludge *kludge, struct ic_pvector *type_refs);

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
 * for an fcall of the form
 *     id[Sint](6s)
 *
 * this will generate
 *     id[Sint](Sint)
 *
 * in order to generate
 *     id[_](_)
 * please see ic_analyse_fcall_str_generic
 *
 * returns ic_string * on success
 * returns 0 on failure
 */
struct ic_string *ic_analyse_fcall_str(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_expr_func_call *fcall);

/* create a generic function signature string from a function call
 *
 * for an fcall of the form
 *     id[Sint](6s)
 *
 * we will generate
 *     id[_](_)
 *
 * returns ic_string * on success
 * returns 0 on failure
 */
struct ic_string *ic_analyse_fcall_str_generic(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_expr_func_call *fcall);

/* trigger instantiate of this generic function decl to a concrete function decl
 *
 * creates new fdecl, inserts into kludge, returns * to new fdecl
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_decl_func *ic_analyse_func_decl_instantiate_generic(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_expr_func_call *fcall);

#endif /* ifndef ICARUS_ANALYSE_HELPERS_H */
