#ifndef ICARUS_KLUDGE_H
#define ICARUS_KLUDGE_H

#include "../../parse/data/ast.h"
#include "../../data/pvector.h"
#include "../../data/dict.h"

/* a Kludge is the output from the analyse stage of icarus
 *
 * a Kludge consists of the following:
 *      dict Type name -> ic_type
 *      dict Func sig  -> Func decl
 *      list Type decls
 *      list Func decls
 *      list of Errors
 *      AAST - annotated AST
 */
struct ic_kludge {
    /* dict Type name (char *) -> ic_type */
    struct ic_dict dict_tname;

    /* dict Func sig (char *)  -> Func decl */
    struct ic_dict dict_fsig;

    /* list of Type decls */
    struct ic_pvector tdecls;

    /* list of Func decls */
    struct ic_pvector fdecls;

    /* list of Errors
     * FIXME no error type yet
     */
    struct ic_pvector errors;

    /* annotated AST */
    struct ic_ast *aast;
};

/* alloc and init a new kludge
 *
 * this call will break apart the ast to populate the
 * fields stored on kludge
 *
 * this will NOT perform any analysis
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_kludge * ic_kludge_new(struct ic_ast *ast);

/* init an existing kludge
 *
 * this call will break apart the ast to populate the
 * fields stored on kludge
 *
 * this will NOT perform any analysis
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_kludge_init(struct ic_kludge *kludge, struct ic_ast *ast);

/* destroy kludge
 *
 * will only free kludge if `free_kludge` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_kludge_destroy(struct ic_kludge *kludge, unsigned int free_kludge);

/* add a new type decl to this kludge
 * this will insert into tdecls
 * it will also construct a new ic_type and insert into dict_tname
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_kludge_add_tdecl(struct ic_kludge *kludge, struct ic_type_decl *tdecl);

/* add a new func decl to this kludge
 * this will insert into dict_fname and also
 * into fsigs
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_kludge_add_fdecl(struct ic_kludge *kludge, struct ic_func_decl *fdecl);

/* retrieve ic_type by string
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_type * ic_kludge_get_type(struct ic_kludge *kludge, char *tdecl_str);

/* retrieve func decl by string
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_func_decl * ic_kludge_get_fdecl(struct ic_kludge *kludge, char *fdecl_str);

/* add a new error to error list
 *
 * FIXME no error type
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_kludge_add_error(struct ic_kludge *kludge, void *error);


#endif
