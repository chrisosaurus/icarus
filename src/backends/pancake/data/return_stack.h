#ifndef IC_BACKEND_PANCAKE_RETURN_STACK_H
#define IC_BACKEND_PANCAKE_RETURN_STACK_H

/* default stack size */
#define IC_BACKEND_PANCAKE_RETURN_STACK_SIZE 1024

struct ic_backend_pancake_return_stack {
  /* fixed size stack */
  unsigned int stack[IC_BACKEND_PANCAKE_RETURN_STACK_SIZE];

  /* current offset into stack
   * -1 = empty
   * 0 = first useable spot
   * IC_BACKEND_PANCAKE_RETURN_STACK_SIZE -1 = tail
   */
  int head;
};

/* allocate and initialise a new return stack
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_return_stack * ic_backend_pancake_return_stack_new(void);

/* initialise an existing return stack
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_return_stack_init(struct ic_backend_pancake_return_stack * stack);

/* destroy return stack
 *
 * will only free actual stack if `free_stack` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_return_stack_destroy(struct ic_backend_pancake_return_stack * stack, unsigned int free_stack);

/* get current return address from stack and pop (removing it)
 *
 * NB: 0 is not a valid address
 *
 * returns address on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_return_stack_pop(struct ic_backend_pancake_return_stack * stack);

/* push a new address onto the stack
 *
 * NB: 0 is not a valid address
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_return_stack_push(struct ic_backend_pancake_return_stack * stack, unsigned int return_address);

#endif /* IC_BACKEND_PANCAKE_RETURN_STACK_H */
