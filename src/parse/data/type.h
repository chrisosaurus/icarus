#ifndef ICARUS_TYPE_H
#define ICARUS_TYPE_H

#include "decl.h"
#include "../../data/symbol.h"

enum ic_type_type {
    /* a type field had 3 states: */

    /* unknown
     * this type must be inferred during analysis
     */
    ic_type_unknown,

    /* a symbol found during parsing,
     * most likely user declared (`a::Int`)
     * this will need to resolved to a tdecl
     * during analysis
     */
    ic_type_symbol,

    /* a fully fledged type
     */
    ic_type_tdecl
};

struct ic_type {
    enum ic_type_type type; /* heh */
    union {
        struct ic_symbol sym;
        struct ic_type_decl *tdecl;
    } u ;
};

/* allocate and intialise a new type
 * this will set type.type to unknown
 *
 * returns new type on success
 * returns 0 on failure
 */
struct ic_type * ic_type_new(void);

/* intialise a type
 * this will set type.type to unknown
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_type_init(struct ic_type *type);

/* destroy type
 *
 * this will only free type if `free_type` is truthy
 *
 * returns 0 on sucess
 * returns 1 on error
 */
unsigned int ic_type_destroy(struct ic_type *type, unsigned int free_type);

/* set the sym on this type from the provided string
 * this will change type.type to sym
 *
 * Note that this is ONLY allowed if a tdecl hasn't already been set
 *
 * if type.type is tdecl then calling this function is an error
 * as that would be going 'backwards'
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_type_set_symbol(struct ic_type *type, char *type_str, unsigned int type_len);

/* set the *tdecl on this type
 * this will change type.type to tdecl
 *
 * FIXME decide on what happens to sym
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_type_set_tdecl(struct ic_type *type, struct ic_type_decl *tdecl);

#endif
