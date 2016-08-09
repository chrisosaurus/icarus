#include <stdio.h>
#include <stdlib.h>

#include "local.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"

/* allocate and init a new local
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_local *ic_backend_pancake_local_new(struct ic_symbol *name, enum ic_backend_pancake_local_type tag) {
    struct ic_backend_pancake_local *local = 0;

    if (!name) {
        puts("ic_backend_pancake_local_new: name was null");
        return 0;
    }

    local = calloc(1, sizeof(struct ic_backend_pancake_local));
    if (!local) {
        puts("ic_backend_pancake_local_new: call to calloc failed");
        return 0;
    }

    if (!ic_backend_pancake_local_init(local, name, tag)) {
        puts("ic_backend_pancake_local_new: call to ic_backend_pancake_local_init failed");
        return 0;
    }

    return local;
}

/* init an existing local
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_local_init(struct ic_backend_pancake_local *local, struct ic_symbol *name, enum ic_backend_pancake_local_type tag) {
    if (!local) {
        puts("ic_backend_pancake_local_init: local was null");
        return 0;
    }

    if (!name) {
        puts("ic_backend_pancake_local_init: name was null");
        return 0;
    }

    local->accessed = 0;
    local->name = name;
    local->tag = tag;

    switch (tag) {
        case icpl_literal:
            local->u.literal = 0;
            break;

        case icpl_offset:
            local->u.offset = 0;
            break;

        default:
            puts("ic_backend_pancake_local_init: impossible tag");
            return 0;
            break;
    }

    return 1;
}

/* destroy a local
 *
 * will only free local if `free_local` is truthy
 *
 * FIXME doesn't free any elements within
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_local_destroy(struct ic_backend_pancake_local *local, unsigned int free_local) {
    if (!local) {
        puts("ic_backend_pancake_local_destroy: local was null");
        return 0;
    }

    /* FIXME doesn't free any elements within */

    if (free_local) {
        free(local);
    }

    return 1;
}

/* set literal on this local
 *
 * must be a literal
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_local_set_literal(struct ic_backend_pancake_local *local, struct ic_expr_constant *literal) {
    if (!local) {
        puts("ic_backend_pancake_local_set_literal: local was null");
        return 0;
    }

    if (local->tag != icpl_literal) {
        puts("ic_backend_pancake_local_set_literal: was not literal");
        return 0;
    }

    local->u.literal = literal;

    return 1;
}

/* set offset on this local
 *
 * must be an offset
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_local_set_offset(struct ic_backend_pancake_local *local, unsigned int offset) {
    if (!local) {
        puts("ic_backend_pancake_local_set_offset: local was null");
        return 0;
    }

    if (local->tag != icpl_offset) {
        puts("ic_backend_pancake_local_set_offset: was not offset");
        return 0;
    }

    local->u.offset = offset;

    return 1;
}

/* get literal from this local
 *
 * must be a literal
 *
 * returns 1 on success
 * returns 0 on failure
 */
struct ic_expr_constant *ic_backend_pancake_local_get_literal(struct ic_backend_pancake_local *local) {
    if (!local) {
        puts("ic_backend_pancake_local_get_literal: local was null");
        return 0;
    }

    if (local->tag != icpl_literal) {
        puts("ic_backend_pancake_local_get_literal: was not literal");
        return 0;
    }

    return local->u.literal;
}

/* get offset from this local
 *
 * must be an offset
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_local_get_offset(struct ic_backend_pancake_local *local) {
    if (!local) {
        puts("ic_backend_pancake_local_get_offset: local was null");
        return 0;
    }

    if (local->tag != icpl_offset) {
        puts("ic_backend_pancake_local_get_offset: was not offset");
        return 0;
    }

    return local->u.offset;
}

/* get the accessed field on this local
 *
 * has no way of signalling error
 *
 * returns boolean on success
 * returns 0 on failure
 */
bool ic_backend_pancake_local_accessed(struct ic_backend_pancake_local *local) {
    if (!local) {
        puts("ic_backend_pancake_local_accessed: local was null");
        return 0;
    }

    return local->accessed;
}
