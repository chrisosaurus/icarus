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

/* get current top of stack
 *
 * returns * on success
 * returns 0 on error
 */
struct ic_backend_pancake_value *ic_backend_pancake_value_stack_peek(struct ic_backend_pancake_value_stack *stack) {
    struct ic_backend_pancake_value *ret;

    if (!stack) {
        puts("ic_backend_pancake_value_stack_peek: stack was null");
        return 0;
    }

    if (stack->head == -1) {
        puts("ic_backend_pancake_value_stack_peek: stack was empty");
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
unsigned int ic_backend_pancake_value_stack_pop(struct ic_backend_pancake_value_stack *stack) {
    if (!stack) {
        puts("ic_backend_pancake_value_stack_pop: stack was null");
        return 0;
    }

    if (stack->head == -1) {
        puts("ic_backend_pancake_value_stack_pop: stack was empty");
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
struct ic_backend_pancake_value *ic_backend_pancake_value_stack_push(struct ic_backend_pancake_value_stack *stack) {
    struct ic_backend_pancake_value *ret;
    if (!stack) {
        puts("ic_backend_pancake_value_stack_push: stack was null");
        return 0;
    }

    if (stack->head >= IC_BACKEND_PANCAKE_VALUE_STACK_SIZE) {
        puts("ic_backend_pancake_value_stack_push: stack is full");
        return 0;
    }

    stack->head += 1;

    ret = &(stack->stack[stack->head]);

    return ret;
}

/* current 'height' of value_stack
 *
 * returns n on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_value_stack_height(struct ic_backend_pancake_value_stack *stack) {
    if (!stack) {
        puts("ic_backend_pancake_value_stack_height: stack was null");
        return 0;
    }

    if (stack->head < 0) {
        return 0;
    }

    return stack->head;
}

/* fetch arbitrary offset within value_stack
 *
 * returns n on success
 * returns 0 on failure
 */
struct ic_backend_pancake_value *ic_backend_pancake_value_stack_get_offset(struct ic_backend_pancake_value_stack *stack, unsigned int offset) {
    struct ic_backend_pancake_value *ret;
    if (!stack) {
        puts("ic_backend_pancake_value_stack_get_offset: stack was null");
        return 0;
    }

    ret = &(stack->stack[offset]);

    return ret;
}

/* reset value stack back to this offset
 *
 * this will drop all values about it
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_value_stack_reset(struct ic_backend_pancake_value_stack *stack, unsigned int offset) {
    unsigned int stack_head = 0;
    if (!stack) {
        puts("ic_backend_pancake_value_stack_reset: stack was null");
        return 0;
    }

    if (stack->head < 0) {
        /* stack is empty, no need to reset */
        return 1;
    }

    stack_head = stack->head;

    if (stack_head < offset) {
        puts("ic_backend_pancake_value_stack_reset: stack was already above offset");
        return 0;
    }

    stack->head = offset;
    return 1;
}

/* print value stack to provided file
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_value_stack_print(FILE *fd, struct ic_backend_pancake_value_stack *value_stack) {
    int i = 0;
    int len = 0;
    struct ic_backend_pancake_value *value = 0;

    if (!value_stack) {
        puts("ic_backend_pancake_value_stack_print: value_stack was null");
        return 0;
    }

    if (!fd) {
        puts("ic_backend_pancake_value_stack_print: fd was null");
        return 0;
    }

    len = value_stack->head;

    /* NB: has to be >= */
    for (i = len; i >= 0; --i) {
        value = ic_backend_pancake_value_stack_get_offset(value_stack, i);
        if (!value) {
            puts("ic_backend_pancake_value_stack_print: call to ic_backend_pancake_value_stack_get_offset failed");
            return 0;
        }

        if (!ic_backend_pancake_value_print(fd, value)) {
            puts("ic_backend_pancake_value_stack_print: call to ic_backend_pancake_value_print failed");
            return 0;
        }
    }

    return 1;
}
