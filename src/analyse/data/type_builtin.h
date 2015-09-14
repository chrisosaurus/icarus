#ifndef ICARUS_TYPE_BUILTIN_H
#define ICARUS_TYPE_BUILTIN_H

#include "../../data/symbol.h"

struct ic_type_builtin {
    /* FIXME need to decide on ownership
     * destroy does not free
     */
    struct ic_symbol *name;
};

/* allocate and initialise a new builtin
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_builtin_new(struct ic_symbol *name);

/* initialise an existing builtin
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_builtin_init(struct ic_type_builtin *builtin, struct ic_symbol *name);

/* destroy builtin
 *
 * will only free builtin is free_builtin is truthy
 *
 * FIXME does not free name, need to decide on ownership
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_builtin_destroy(struct ic_type_builtin *builtin, unsigned int free_builtin);

#endif
