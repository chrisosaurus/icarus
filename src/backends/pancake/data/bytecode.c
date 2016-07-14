#include <stdio.h>
#include <stdlib.h>

#include "bytecode.h"

/* allocate and initialise a new bytecode
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_bytecode *ic_backend_pancake_bytecode_new(enum ic_backend_pancake_bytecode_type type, void *arg1, void *arg2) {
    struct ic_backend_pancake_bytecode *bc = 0;

    bc = calloc(1, sizeof(struct ic_backend_pancake_bytecode));
    if (!bc) {
        puts("ic_backend_pancake_bytecode_new: call to calloc failed");
        return 0;
    }

    if (!ic_backend_pancake_bytecode_init(bc, type, arg1, arg2)) {
        puts("ic_backend_pancake_bytecode_new: call to ic_backend_pancake_bytecode_init failed");
        return 0;
    }

    return bc;
}

/* initialise an existing bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_bytecode_init(struct ic_backend_pancake_bytecode *bytecode, enum ic_backend_pancake_bytecode_type type, void *arg1, void *arg2) {
    if (!bytecode) {
        puts("ic_backend_pancake_bytecode_init: bytecode was null");
        return 0;
    }

    bytecode->tag = type;
    bytecode->arg1 = arg1;
    bytecode->arg2 = arg2;

    return 1;
}

/* destroy a bytecode
 *
 * will only fre bytecode if `free_bytecode` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_bytecode_destroy(struct ic_backend_pancake_bytecode *bytecode, unsigned int free_bytecode) {
    if (!bytecode) {
        puts("ic_backend_pancake_bytecode_destroy: bytecode was null");
        return 0;
    }

    if (free_bytecode) {
        free(bytecode);
    }

    return 1;
}
