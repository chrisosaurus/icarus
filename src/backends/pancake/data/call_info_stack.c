#include <stdio.h>
#include <stdlib.h>

#include "call_info_stack.h"

/* allocate and initialise a new return stack
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_call_info_stack *ic_backend_pancake_call_info_stack_new(void) {
    struct ic_backend_pancake_call_info_stack *call_info_stack = 0;

    call_info_stack = calloc(1, sizeof(struct ic_backend_pancake_call_info_stack));
    if (!call_info_stack) {
        puts("ic_backend_pancake_call_info_stack_new: stack was null");
        return 0;
    }

    if (!ic_backend_pancake_call_info_stack_init(call_info_stack)) {
        puts("ic_backend_pancake_call_info_stack_new: call to ic_backend_pancake_call_info_stack_init failed");
        return 0;
    }

    return call_info_stack;
}

/* initialise an existing return stack
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_call_info_stack_init(struct ic_backend_pancake_call_info_stack *stack) {
    if (!stack) {
        puts("ic_backend_pancake_call_info_stack_init: stack was null");
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
unsigned int ic_backend_pancake_call_info_stack_destroy(struct ic_backend_pancake_call_info_stack *stack, unsigned int free_stack) {
    if (!stack) {
        puts("ic_backend_pancake_call_info_stack_destroy: stack was null");
        return 0;
    }

    if (free_stack) {
        free(stack);
    }

    return 1;
}

/* get current top of stack
 *
 * returns * on success
 * returns 0 on error
 */
struct ic_backend_pancake_call_info *ic_backend_pancake_call_info_stack_peek(struct ic_backend_pancake_call_info_stack *stack) {
    struct ic_backend_pancake_call_info *ret;

    if (!stack) {
        puts("ic_backend_pancake_call_info_stack_peek: stack was null");
        return 0;
    }

    if (stack->head == -1) {
        puts("ic_backend_pancake_call_info_stack_peek: stack was empty");
        return 0;
    }

    ret = &(stack->stack[stack->head]);

    return ret;
}

/* remove top of stack
 *
 * returns 1 on success
 * returns 0 on error
 */
unsigned int ic_backend_pancake_call_info_stack_pop(struct ic_backend_pancake_call_info_stack *stack) {
    if (!stack) {
        puts("ic_backend_pancake_call_info_stack_pop: stack was null");
        return 0;
    }

    if (stack->head == -1) {
        puts("ic_backend_pancake_call_info_stack_pop: stack was empty");
        return 0;
    }

    stack->head -= 1;
    return 1;
}

/* push a new value onto the stack and return * to it
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_call_info *ic_backend_pancake_call_info_stack_push(struct ic_backend_pancake_call_info_stack *stack) {
    struct ic_backend_pancake_call_info *ret;
    if (!stack) {
        puts("ic_backend_pancake_call_info_stack_push: stack was null");
        return 0;
    }

    if (stack->head >= IC_BACKEND_PANCAKE_CALL_INFO_STACK_SIZE) {
        puts("ic_backend_pancake_call_info_stack_push: stack is full");
        return 0;
    }

    stack->head += 1;

    ret = &(stack->stack[stack->head]);

    return ret;
}