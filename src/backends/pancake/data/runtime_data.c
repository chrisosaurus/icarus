#include <stdio.h>
#include <stdlib.h>

#include "runtime_data.h"

/* alloc and init a new runtime_data
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_runtime_data *ic_backend_pancake_runtime_data_new(struct ic_backend_pancake_instructions *instructions) {
    struct ic_backend_pancake_runtime_data *runtime_data = 0;

    if (!instructions) {
        puts("ic_backend_panecake_runtime_data_new: instructions was null");
        return 0;
    }

    runtime_data = calloc(1, sizeof(struct ic_backend_pancake_runtime_data));
    if (!runtime_data) {
        puts("ic_backend_panecake_runtime_data_new: call to calloc failed");
        return 0;
    }

    if (!ic_backend_pancake_runtime_data_init(runtime_data, instructions)) {
        puts("ic_backend_panecake_runtime_data_new: call to ic_backend_pancake_runtme failed");
        free(runtime_data);
        return 0;
    }

    return runtime_data;
}

/* init an existing runtime_data
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_runtime_data_init(struct ic_backend_pancake_runtime_data *runtime_data, struct ic_backend_pancake_instructions *instructions) {
    if (!runtime_data) {
        puts("ic_backend_pancake_runtime_data_init: runtime_data was null");
        return 0;
    }

    if (!instructions) {
        puts("ic_backend_pancake_runtime_data_init: instructions was null");
        return 0;
    }

    /* FIXME TODO decide on instructions ownership */
    runtime_data->instructions = instructions;

    runtime_data->call_info_stack = ic_backend_pancake_call_info_stack_new();
    if (!(runtime_data->call_info_stack)) {
        puts("ic_backend_pancake_runtime_data_init: call to ic_backend_pancake_call_info_stack_new failed");
        return 0;
    }

    runtime_data->value_stack = ic_backend_pancake_value_stack_new();
    if (!(runtime_data->value_stack)) {
        puts("ic_backend_pancake_runtime_data_init: call to ic_backend_pancake_value_stack_new failed");
        return 0;
    }

    return 1;
}

/* destroy runtime_data
 *
 * will only free runtime_data if `free_runtime_data` is truthy
 *
 * returns 1 on success
 * returns on failure
 */
unsigned int ic_backend_pancake_runtime_data_destroy(struct ic_backend_pancake_runtime_data *runtime_data, unsigned int free_runtime_data) {
    if (!runtime_data) {
        puts("ic_backend_pancake_runtime_data_init: runtime_data was null");
        return 0;
    }

    if (!ic_backend_pancake_call_info_stack_destroy(runtime_data->call_info_stack, 1)) {
        puts("ic_backend_pancake_runtime_data_init: call to ic_backend_pancake_call_info_stack_destroy failed");
        return 0;
    }

    if (!ic_backend_pancake_value_stack_destroy(runtime_data->value_stack, 1)) {
        puts("ic_backend_pancake_runtime_data_init: call to ic_backend_pancake_value_stack_destroy failed");
        return 0;
    }

    /* FIXME TODO decide on instructions ownership */

    runtime_data->call_info_stack = 0;
    runtime_data->value_stack = 0;
    runtime_data->instructions = 0;

    if (free_runtime_data) {
        free(runtime_data);
    }

    return 1;
}
