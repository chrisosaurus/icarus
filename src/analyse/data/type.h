#ifndef IC_TYPE_H
#define IC_TYPE_H

#include "../../parse/data/decl.h"

/* an instance of a type
 * ic_type has a single instance for each type within a program (FIXME consider type scoping)
 *
 * an ic_type can either be:
 *  - a builtin - FIXME no current support for builtins
 *  - a user defined type - ic_type_decl
 */

enum ic_type_type {
    ic_type_builtin,
    ic_type_user
};

struct ic_type {
    enum ic_type_type type;
    union {
        /* FIXME what do we store for a builtin ? */
        struct ic_type_decl *decl;
    } u;
};

/* alloc and init a new type
 *
 * FIXME unknown params
 *
 * returns new type on success
 * returns 0 on failure
 */
struct ic_type * ic_type_new(void);

/* init an existing type
 *
 * FIXME unknown params
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_init(struct ic_type *type);

/* destroy a type
 *
 * will only free the type if `free_type` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_destroy(struct ic_type *type, unsigned int free_tytpe);

#endif
