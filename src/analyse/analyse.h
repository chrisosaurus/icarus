#ifndef ICARUS_ANALYSE_H
#define ICARUS_ANALYSE_H

#include "data/kludge.h"

/* takes an ast and performs analysis on it
 * this returns a kludge
 *
 * see kludge->errors for errors
 *
 * returns kludge on success
 * returns 0 on failure
 */
struct ic_kludge * ic_analyse(struct ic_ast *ast);

/* takes a decl_type and performs analysis
 *
 * FIXME need a way of signalling and passing errors
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_analyse_decl_type(struct ic_kludge *kludge, struct ic_decl_type *tdecl);

/* takes a decl_func and performs analysis
 *
 * FIXME need a way of signalling and passing errors
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_analyse_decl_func(struct ic_kludge *kludge, struct ic_decl_func *fdecl);

#endif

