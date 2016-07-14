#ifndef IC_BACKEND_PANCAKE_VALUE_STACK_H
#define IC_BACKEND_PANCAKE_VALUE_STACK_H

#define IC_BACKEND_PANCAKE_VALUE_STACK_SIZE 2048

struct ic_backend_pancake_value_stack {
    void *stack[IC_BACKEND_PANCAKE_VALUE_STACK_SIZE];

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
 * returns * on success
 * returns 0 on failure
 */
void *ic_backend_pancake_value_stack_pop(struct ic_backend_pancake_value_stack *stack);

/* push a new item onto the stack
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_value_stack_push(struct ic_backend_pancake_value_stack *stack, void *item);

#endif /* IC_BACKEND_PANCAKE_VALUE_STACK_H */
