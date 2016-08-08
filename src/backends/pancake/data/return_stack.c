#include <stdio.h>
#include <stdlib.h>

#include "return_stack.h"

/* fixed size stack */
//unsigned int stack[IC_BACKEND_PANCAKE_RETURN_STACK_SIZE];

/* current offset into stack
   * -1 = empty
   * 0 = first useable spot
   * IC_BACKEND_PANCAKE_RETURN_STACK_SIZE -1 = tail
   */
//int head = -1;

/* allocate and initialise a new return stack
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_return_stack *ic_backend_pancake_return_stack_new(void) {
    struct ic_backend_pancake_return_stack *return_stack = 0;

    return_stack = calloc(1, sizeof(struct ic_backend_pancake_return_stack));
    if (!return_stack) {
        puts("ic_backend_pancake_return_stack_new: stack was null");
        return 0;
    }

    if (!ic_backend_pancake_return_stack_init(return_stack)) {
        puts("ic_backend_pancake_return_stack_new: call to ic_backend_pancake_return_stack_init failed");
        return 0;
    }

    return return_stack;
}

/* initialise an existing return stack
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_return_stack_init(struct ic_backend_pancake_return_stack *stack) {
    if (!stack) {
        puts("ic_backend_pancake_return_stack_init: stack was null");
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
unsigned int ic_backend_pancake_return_stack_destroy(struct ic_backend_pancake_return_stack *stack, unsigned int free_stack) {
    if (!stack) {
        puts("ic_backend_pancake_return_stack_destroy: stack was null");
        return 0;
    }

    if (free_stack) {
        free(stack);
    }

    return 1;
}

/* get current return address from stack and pop (removing it)
 *
 * NB: 0 is not a valid address
 *
 * returns address on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_return_stack_pop(struct ic_backend_pancake_return_stack *stack) {
    unsigned int return_address = 0;

    if (!stack) {
        puts("ic_backend_pancake_return_stack_pop: stack was null");
        return 0;
    }

    if (stack->head == -1) {
        puts("ic_backend_pancake_return_stack_pop: stack was empty");
        return 0;
    }

    return_address = stack->stack[stack->head];
    stack->head -= 1;

    return return_address;
}

/* push a new address onto the stack
 *
 * NB: 0 is not a valid address
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_return_stack_push(struct ic_backend_pancake_return_stack *stack, unsigned int return_address) {
    if (!stack) {
        puts("ic_backend_pancake_return_stack_push: stack was null");
        return 0;
    }

    if (return_address == 0) {
        puts("ic_backend_pancake_return_stack_push: address '0' passed in - invalid");
        return 0;
    }

    if (stack->head >= IC_BACKEND_PANCAKE_RETURN_STACK_SIZE) {
        puts("ic_backend_pancake_return_stack_push: stack is full");
        return 0;
    }

    stack->head += 1;
    stack->stack[stack->head] = return_address;

    return 1;
}
