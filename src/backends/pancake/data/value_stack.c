#include <stdio.h>
#include <stdlib.h>

#include "value_stack.h"

/* allocate and initialise a new return stack
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_value_stack *ic_backend_pancake_value_stack_new(void) {
    struct ic_backend_pancake_value_stack *value_stack = 0;

    value_stack = calloc(1, sizeof(struct ic_backend_pancake_value_stack));
    if (!value_stack) {
        puts("ic_backend_pancake_value_stack_new: stack was null");
        return 0;
    }

    if (!ic_backend_pancake_value_stack_init(value_stack)) {
        puts("ic_backend_pancake_value_stack_new: call to ic_backend_pancake_value_stack_init failed");
        return 0;
    }

    return value_stack;
}

/* initialise an existing return stack
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_value_stack_init(struct ic_backend_pancake_value_stack *stack) {
    if (!stack) {
        puts("ic_backend_pancake_value_stack_init: stack was null");
        return 0;
    }

    stack->head = -1;

    return 1;
}

/* destroy return stack
 *
 * will only free actual stack if `free_stack` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_value_stack_destroy(struct ic_backend_pancake_value_stack *stack, unsigned int free_stack) {
    if (!stack) {
        puts("ic_backend_pancake_value_stack_destroy: stack was null");
        return 0;
    }

    if (free_stack) {
        free(stack);
    }

    return 1;
}

/* get current top of stack (and remove it)
 *
 * returns address on success
 * returns 0 on failure
 */
void *ic_backend_pancake_value_stack_pop(struct ic_backend_pancake_value_stack *stack) {
    void *ret = 0;

    if (!stack) {
        puts("ic_backend_pancake_value_stack_pop: stack was null");
        return 0;
    }

    if (stack->head == -1) {
        puts("ic_backend_pancake_value_stack_pop: stack was empty");
        return 0;
    }

    ret = stack->stack[stack->head];
    stack->head -= 1;

    return ret;
}

/* push a new item onto the stack
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_value_stack_push(struct ic_backend_pancake_value_stack *stack, void *item) {
    if (!stack) {
        puts("ic_backend_pancake_value_stack_push: stack was null");
        return 0;
    }

    if (item) {
        puts("ic_backend_pancake_value_stack_push: item was null");
        return 0;
    }

    if (stack->head > IC_BACKEND_PANCAKE_VALUE_STACK_SIZE) {
        puts("ic_backend_pancake_value_stack_push: stack is full");
        return 0;
    }

    stack->head += 1;
    stack->stack[stack->head] = item;

    return 1;
}
