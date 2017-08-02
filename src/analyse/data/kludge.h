#ifndef ICARUS_KLUDGE_H
#define ICARUS_KLUDGE_H

#include "../../data/dict.h"
#include "../../data/pvector.h"
#include "../../data/symbol.h"
#include "../../parse/data/ast.h"
#include "../../parse/data/type_ref.h"
#include "generate.h"

/* a Kludge is the output from the analyse stage of icarus
 *
 * a Kludge consists of the following:
 *      dict Type name -> ic_decl_type
 *      dict Func sig  -> Func decl
 *      dict op name -> func name (symbol)
 *      list Type decls
 *      list Type builtins
 *      list Func decls
 *      list of Errors
 *      AAST - annotated AST
 */
struct ic_kludge {
    /* dict Type name (char *) -> ic_decl_type */
    struct ic_dict dict_tname;

    /* dict Func sig (char *)  -> ic_decl_func */
    struct ic_dict dict_fsig;

    /* dict Op name (char*) -> Op name (symbol *)
     * e.g.
     * + -> symbol(Plus)
     */
    struct ic_dict dict_op;

    /* list of Type decls */
    struct ic_pvector tdecls;

    /* list of Type builtins
     * this is populated as part of kludge_init
     * FIXME consider builtin population
     * FIXME builtins currently leaked
     */
    struct ic_pvector tbuiltins;

    /* list of Func decls */
    struct ic_pvector fdecls;

    /* list of ic_generate */
    struct ic_pvector generates;

    /* list of Errors
     * FIXME no error type yet
     */
    struct ic_pvector errors;
};

/* alloc and init a new kludge
 *
 * this will NOT perform any analysis
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_kludge *ic_kludge_new(void);

/* init an existing kludge
 *
 * this will NOT perform any analysis
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_kludge_init(struct ic_kludge *kludge);

/* destroy kludge
 *
 * will only free kludge if `free_kludge` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_kludge_destroy(struct ic_kludge *kludge, unsigned int free_kludge);

/* populate the kludge from the provided ast
 * this call will break apart the ast to populate the
 * fields stored on kludge
 *
 * this will NOT perform any analysis
 */
unsigned int ic_kludge_populate(struct ic_kludge *kludge, struct ic_ast *ast);

/* add a new type decl to this kludge
 * this will insert into tdecls and dict_tname
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_kludge_add_tdecl(struct ic_kludge *kludge, struct ic_decl_type *tdecl);

/* add a new decl_func to this kludge
 * this will insert into dict_fname and fsigs
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_kludge_add_fdecl(struct ic_kludge *kludge, struct ic_decl_func *fdecl);

/* add a new decl_op to this kludge
 * this will insert into dict_op
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_kludge_add_op(struct ic_kludge *kludge, struct ic_decl_op *op);

/* retrieve ic_decl_type by string
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_decl_type *ic_kludge_get_decl_type(struct ic_kludge *kludge, char *tdecl_str);

/* retrieve ic_decl_type by symbol
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_decl_type *ic_kludge_get_decl_type_from_symbol(struct ic_kludge *kludge, struct ic_symbol *type);

/* retrieve ic_decl_type by string
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_decl_type *ic_kludge_get_decl_type_from_string(struct ic_kludge *kludge, struct ic_string *type);

/* retrieve ic_decl_type by type_ref
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_decl_type *ic_kludge_get_decl_type_from_typeref(struct ic_kludge *kludge, struct ic_type_ref *type_ref);

/* retrieve ic_decl_func by string
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_decl_func *ic_kludge_get_fdecl(struct ic_kludge *kludge, char *fdecl_str);

/* retrieve ic_decl_func by symbol
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_decl_func *ic_kludge_get_fdecl_from_symbol(struct ic_kludge *kludge, struct ic_symbol *fdecl);

/* retrieve ic_decl_func by string
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_decl_func *ic_kludge_get_fdecl_from_string(struct ic_kludge *kludge, struct ic_string *fdecl);

/* retrieve the function name that this operator maps to
 *
 * '+' -> symbol('plus')
 *
 * returns * on success
 * returns 0 on error
 */
struct ic_symbol *ic_kludge_get_operator(struct ic_kludge *kludge, char *sym_str);

/* retrieve the function name that this operator maps to
 *
 * symbol('+') -> symbol('plus')
 *
 * returns * on success
 * returns 0 on error
 */
struct ic_symbol *ic_kludge_get_operator_from_symbol(struct ic_kludge *kludge, struct ic_symbol *symbol);

/* add a new generate representing a function to generate
 * this will insert into dict_fname and generates
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_kludge_generates_add(struct ic_kludge *kludge, struct ic_generate *gen);

/* get length of generates
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_kludge_generates_length(struct ic_kludge *kludge);

/* get generate at offset
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_generate *ic_kludge_generates_get(struct ic_kludge *kludge, unsigned int i);

/* check if an existing identifier is taken either within the kludge or the provided sope
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
int ic_kludge_identifier_exists(struct ic_kludge *kludge, struct ic_scope *scope, char *identifier);

/* check if an existing identifier is taken either within the kludge or the provided sope
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
int ic_kludge_identifier_exists_symbol(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_symbol *identifier);

/* add a new error to error list
 *
 * FIXME no error type
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_kludge_add_error(struct ic_kludge *kludge, void *error);

#endif
