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
    struct ic_ast aast;
};

#endif
