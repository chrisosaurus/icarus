#ifndef IC_BACKEND_PANCAKE_VALUE_STACK_H
#define IC_BACKEND_PANCAKE_VALUE_STACK_H

#include "value.h"

#define IC_BACKEND_PANCAKE_VALUE_STACK_SIZE 2048

struct ic_backend_pancake_value_stack {
    struct ic_backend_pancake_value stack[IC_BACKEND_PANCAKE_VALUE_STACK_SIZE];

    /* current head
   * -1 = empty
   *  0 = first
   *  IC_BACKEND_PANCAKE_VALUE_STACK_SIZE -1 = max
   */
    int head;
};

/* allocate and initialise a new return stack
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_value_stack *ic_backend_pancake_value_stack_new(void);

/* initialise an existing return stack
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_value_stack_init(struct ic_backend_pancake_value_stack *stack);

/* destroy return stack
 *
 * will only free actual stack if `free_stack` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_value_stack_destroy(struct ic_backend_pancake_value_stack *stack, unsigned int free_stack);

/* get current top of stack
 *
 * returns * on success
 * returns 0 on error
 */
struct ic_backend_pancake_value *ic_backend_pancake_value_stack_peek(struct ic_backend_pancake_value_stack *stack);

/* remove top of stack
 *
 * returns 1 on success
 * returns 0 on error
 */
unsigned int ic_backend_pancake_value_stack_pop(struct ic_backend_pancake_value_stack *stack);

/* push a new value onto the stack and return * to it
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_value *ic_backend_pancake_value_stack_push(struct ic_backend_pancake_value_stack *stack);

/* current 'height' of value_stack
 *
 * returns n on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_value_stack_height(struct ic_backend_pancake_value_stack *stack);

/* fetch arbitrary offset within value_stack
 *
 * returns n on success
 * returns 0 on failure
 */
struct ic_backend_pancake_value *ic_backend_pancake_value_stack_get_offset(struct ic_backend_pancake_value_stack *stack, unsigned int offset);

/* reset value stack back to this offset
 *
 * this will drop all values about it
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_value_stack_reset(struct ic_backend_pancake_value_stack *stack, unsigned int offset);

/* print value stack to provided file
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_value_stack_print(FILE *fd, struct ic_backend_pancake_value_stack *value_stack);

#endif /* IC_BACKEND_PANCAKE_VALUE_STACK_H */
