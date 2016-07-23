#ifndef IC_BACKEND_PANCAKE_RUNTIME
#define IC_BACKEND_PANCAKE_RUNTIME

#include "instructions.h"
#include "return_stack.h"
#include "value_stack.h"

struct ic_backend_pancake_runtime {
    struct ic_backend_pancake_instructions *instructions;
    struct ic_backend_pancake_return_stack *return_stack;
    struct ic_backend_pancake_value_stack *value_stack;
    /* FIXME TODO eventually this should also have a heap for allocation */
};

/* alloc and init a new runtime
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_runtime *ic_backend_pancake_runtime_new(struct ic_backend_pancake_instructions *instructions);

/* init an existing runtime
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_runtime_init(struct ic_backend_pancake_runtime *runtime, struct ic_backend_pancake_instructions *instructions);

/* destroy runtime
 *
 * will only free runtime if `free_runtime` is truthy
 *
 * returns 1 on success
 * returns on failure
 */
unsigned int ic_backend_pancake_runtime_destroy(struct ic_backend_pancake_runtime *runtime, unsigned int free_runtime);

#endif /* IC_BACKEND_PANCAKE_RUNTIME */
