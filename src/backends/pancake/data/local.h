#ifndef IC_BACKEND_PANCAKE_LOCAL_H
#define IC_BACKEND_PANCAKE_LOCAL_H

#include <stdbool.h>

enum ic_backend_pancake_local_type {
    icpl_literal,
    icpl_runtime,
};

/* a local 'variable' used during bytecode compilation */
struct ic_backend_pancake_local {
    /* whether or not this local variable was accessed
     * used to throw warnings for unused variables
     */
    bool accessed;

    /* local variable name */
    struct ic_symbol *name;

    enum ic_backend_pancake_local_type tag;

    /* from here we can have one of two cases
     *  literal - value
     *  runtime - offset into this function
     */
    union {
        struct ic_expr_constant *literal;
        unsigned int function_offset;
    } u;
};

/* allocate and init a new local
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_local *ic_backend_pancake_local_new(struct ic_symbol *name, enum ic_backend_pancake_local_type tag);

/* init an existing local
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_local_init(struct ic_backend_pancake_local *local, struct ic_symbol *name, enum ic_backend_pancake_local_type tag);

/* destroy a local
 *
 * will only free local if `free_local` is truthy
 *
 * FIXME doesn't free any elements within
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_local_destroy(struct ic_backend_pancake_local *local, unsigned int free_local);

/* set literal on this local
 *
 * must be a literal
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_local_set_literal(struct ic_backend_pancake_local *local, struct ic_expr_constant *literal);

/* set offset on this local
 *
 * must be an offset
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_local_set_offset(struct ic_backend_pancake_local *local, unsigned int offset);

#endif /* IC_BACKEND_PANCAKE_LOCAL_H */
