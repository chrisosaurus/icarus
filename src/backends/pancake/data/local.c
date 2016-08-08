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
    puts("ic_backend_pancake_local_new: unimplemented");
    return 0;
}

/* init an existing local
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_local_init(struct ic_backend_pancake_local *local, struct ic_symbol *name, enum ic_backend_pancake_local_type tag) {
    puts("ic_backend_pancake_local_init: unimplemented");
    return 0;
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
    puts("ic_backend_pancake_local_destroy: unimplemented");
    return 0;
}

/* set literal on this local
 *
 * must be a literal
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_local_set_literal(struct ic_backend_pancake_local *local, struct ic_expr_constant *literal) {
    puts("ic_backend_pancake_local_literal: unimplemented");
    return 0;
}

/* set offset on this local
 *
 * must be an offset
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_local_set_offset(struct ic_backend_pancake_local *local, unsigned int offset) {
    puts("ic_backend_pancake_local_set_offset: unimplemented");
    return 0;
}
