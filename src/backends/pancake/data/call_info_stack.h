#ifndef IC_BACKEND_PANCAKE_CALL_INFO_STACK_H
#define IC_BACKEND_PANCAKE_CALL_INFO_STACK_H

#include "call_info.h"

#define IC_BACKEND_PANCAKE_CALL_INFO_STACK_SIZE 2048

struct ic_backend_pancake_call_info_stack {
    struct ic_backend_pancake_call_info stack[IC_BACKEND_PANCAKE_CALL_INFO_STACK_SIZE];

    /* current head
   * -1 = empty
   *  0 = first
   *  IC_BACKEND_PANCAKE_CALL_INFO_STACK_SIZE -1 = max
   */
    int head;
};

/* allocate and initialise a new return stack
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_call_info_stack *ic_backend_pancake_call_info_stack_new(void);

/* initialise an existing return stack
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_call_info_stack_init(struct ic_backend_pancake_call_info_stack *stack);

/* destroy return stack
 *
 * will only free actual stack if `free_stack` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_call_info_stack_destroy(struct ic_backend_pancake_call_info_stack *stack, unsigned int free_stack);

/* get current top of stack
 *
 * returns * on success
 * returns 0 on error
 */
struct ic_backend_pancake_call_info *ic_backend_pancake_call_info_stack_peek(struct ic_backend_pancake_call_info_stack *stack);

/* remove top of stack
 *
 * returns 1 on success
 * returns 0 on error
 */
unsigned int ic_backend_pancake_call_info_stack_pop(struct ic_backend_pancake_call_info_stack *stack);

/* push a new call_info onto the stack and return * to it
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_call_info *ic_backend_pancake_call_info_stack_push(struct ic_backend_pancake_call_info_stack *stack);

#endif /* IC_BACKEND_PANCAKE_CALL_INFO_STACK_H */
