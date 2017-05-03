#ifndef ICARUS_ANALYSE_H
#define ICARUS_ANALYSE_H

#include "data/kludge.h"

/* takes a kludge and performs analysis on it
 *
 * see kludge->errors for errors
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_analyse(struct ic_kludge *kludge);

/* takes a decl_type and performs analysis
 *
 * FIXME need a way of signalling and passing errors
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_analyse_decl_type(struct ic_kludge *kludge, struct ic_decl_type *tdecl);

/* generate the needed function(s) for this type
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_analyse_decl_type_generate_functions(struct ic_kludge *kludge, struct ic_decl_type *tdecl);

/* takes a decl_type_struct and performs analysis
 *
 * FIXME need a way of signalling and passing errors
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_analyse_decl_type_struct(struct ic_kludge *kludge, struct ic_decl_type_struct *tdecl_struct);

/* generate the needed function(s) for this type
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_analyse_decl_type_struct_generate_functions(struct ic_kludge *kludge, struct ic_decl_type *tdecl, struct ic_decl_type_struct *tdecl_struct);

/* takes a decl_type_union and performs analysis
 *
 * FIXME need a way of signalling and passing errors
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_analyse_decl_type_union(struct ic_kludge *kludge, struct ic_decl_type_union *tdecl_union);

/* generate the needed function(s) for this type
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_analyse_decl_type_union_generate_functions(struct ic_kludge *kludge, struct ic_decl_type *tdecl, struct ic_decl_type_union *tdecl_union);

/* takes a decl_func and performs analysis
 *
 * FIXME need a way of signalling and passing errors
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_analyse_decl_func(struct ic_kludge *kludge, struct ic_decl_func *fdecl);

#endif
