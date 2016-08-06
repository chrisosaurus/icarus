#include <stdio.h>
#include <stdlib.h>

#include "runtime.h"

/* alloc and init a new runtime
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_runtime *ic_backend_pancake_runtime_new(struct ic_backend_pancake_instructions *instructions) {
    struct ic_backend_pancake_runtime *runtime = 0;

    if (!instructions) {
        puts("ic_backend_panecake_runtime_new: instructions was null");
        return 0;
    }

    runtime = calloc(1, sizeof(struct ic_backend_pancake_runtime));
    if (!runtime) {
        puts("ic_backend_panecake_runtime_new: call to calloc failed");
        return 0;
    }

    if (!ic_backend_pancake_runtime_init(runtime, instructions)) {
        puts("ic_backend_panecake_runtime_new: call to ic_backend_pancake_runtme failed");
        free(runtime);
        return 0;
    }

    return runtime;
}

/* init an existing runtime
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_runtime_init(struct ic_backend_pancake_runtime *runtime, struct ic_backend_pancake_instructions *instructions) {
    if (!runtime) {
        puts("ic_backend_pancake_runtime_init: runtime was null");
        return 0;
    }

    if (!instructions) {
        puts("ic_backend_pancake_runtime_init: instructions was null");
        return 0;
    }

    /* FIXME TODO decide on instructions ownership */
    runtime->instructions = instructions;

    runtime->return_stack = ic_backend_pancake_return_stack_new();
    if (!(runtime->return_stack)) {
        puts("ic_backend_pancake_runtime_init: call to ic_backend_pancake_return_stack_new failed");
        return 0;
    }

    runtime->value_stack = ic_backend_pancake_value_stack_new();
    if (!(runtime->value_stack)) {
        puts("ic_backend_pancake_runtime_init: call to ic_backend_pancake_value_stack_new failed");
        return 0;
    }

    return 1;
}

/* destroy runtime
 *
 * will only free runtime if `free_runtime` is truthy
 *
 * returns 1 on success
 * returns on failure
 */
unsigned int ic_backend_pancake_runtime_destroy(struct ic_backend_pancake_runtime *runtime, unsigned int free_runtime) {
    if (!runtime) {
        puts("ic_backend_pancake_runtime_init: runtime was null");
        return 0;
    }

    if (!ic_backend_pancake_return_stack_destroy(runtime->return_stack, 1)) {
        puts("ic_backend_pancake_runtime_init: call to ic_backend_pancake_return_stack_destroy failed");
        return 0;
    }

    if (!ic_backend_pancake_value_stack_destroy(runtime->value_stack, 1)) {
        puts("ic_backend_pancake_runtime_init: call to ic_backend_pancake_value_stack_destroy failed");
        return 0;
    }

    /* FIXME TODO decide on instructions ownership */

    runtime->return_stack = 0;
    runtime->value_stack = 0;
    runtime->instructions = 0;

    if (free_runtime) {
        free(runtime);
    }

    return 1;
}
