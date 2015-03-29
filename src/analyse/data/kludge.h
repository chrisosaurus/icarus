#ifndef ICARUS_KLUDGE_H
#define ICARUS_KLUDGE_H

#include "../../parse/data/ast.h"
#include "../../data/pvector.h"

/* a Kludge is the output from the analyse stage of icarus
 *
 * a Kludge consists of the following:
 *      dict Type name -> Type decl
 *      dict Func sig  -> Func decl
 *      list Type decls
 *      list Func decls
 *      list of Errors
 *      AAST - annotated AST
 */
struct ic_kludge {
    /* dict Type name -> Type decl
     * FIXME no dict type
     * FIXME no type name type
     */
    void * dict_tname;

    /* dict Func sig  -> Func decl
     * FIXME no dict type
     * FIXME no func sig type
     */
    void * dict_fsig;

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
 * returns pointer on success
 * returns 0 on error
 */
struct ic_kludge * ic_kludge_new(struct ic_ast *ast);

/* init an existing kludge
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_kludge_init(struct ic_kludge *kludge, struct ic_ast *ast);

/* add a new type decl to this kludge
 * this will insert into dict_tname and also
 * into tdecls
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_klude_add_tdecl(struct ic_kludge *kludge, struct ic_type_decl *tdecl);

/* add a new func decl to this kludge
 * this will insert into dict_fname and also
 * into fsigs
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_klude_add_fdecl(struct ic_kludge *kludge, struct ic_func_decl *fdecl);

/* add a new error to error list
 *
 * FIXME no error type
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_klude_add_error(struct ic_kludge *kludge, void *error);


#endif
