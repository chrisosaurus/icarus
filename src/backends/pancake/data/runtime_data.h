#ifndef IC_BACKEND_PANCAKE_RUNTIME_DATA_H
#define IC_BACKEND_PANCAKE_RUNTIME_DATA_H

#include "call_info_stack.h"
#include "instructions.h"
#include "value_stack.h"

struct ic_backend_pancake_runtime_data {
    struct ic_backend_pancake_instructions *instructions;
    struct ic_backend_pancake_call_info_stack *call_info_stack;
    struct ic_backend_pancake_value_stack *value_stack;
    /* FIXME TODO eventually this should also have a heap for allocation */
};

/* alloc and init a new runtime_data
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_runtime_data *ic_backend_pancake_runtime_data_new(struct ic_backend_pancake_instructions *instructions);

/* init an existing runtime_data
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_runtime_data_init(struct ic_backend_pancake_runtime_data *runtime_data, struct ic_backend_pancake_instructions *instructions);

/* destroy runtime_data
 *
 * will only free runtime_data if `free_runtime_data` is truthy
 *
 * returns 1 on success
 * returns on failure
 */
unsigned int ic_backend_pancake_runtime_data_destroy(struct ic_backend_pancake_runtime_data *runtime_data, unsigned int free_runtime_data);

#endif /* IC_BACKEND_PANCAKE_RUNTIME_DATA_H */
