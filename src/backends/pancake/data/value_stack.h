#ifndef IC_BACKEND_PANCAKE_VALUE_STACK_H
#define IC_BACKEND_PANCAKE_VALUE_STACK_H

#include "value.h"

#define IC_BACKEND_PANCAKE_VALUE_STACK_SIZE 2048

struct ic_backend_pancake_value_stack {
    struct ic_backend_pancake_value stack[IC_BACKEND_PANCAKE_VALUE_STACK_SIZE];

    /* current heah
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

/* get current top of stack (and remove it)
 *
 * returned value must be read before another push is called
 *
 * returns * on success
 * returns 0 on error
 */
struct ic_backend_pancake_value *ic_backend_pancake_value_stack_pop(struct ic_backend_pancake_value_stack *stack);

/* push a new value onto the stack
 *
 * will copy from passed in value
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_value_stack_push(struct ic_backend_pancake_value_stack *stack, struct ic_backend_pancake_value *value);

#endif /* IC_BACKEND_PANCAKE_VALUE_STACK_H */
