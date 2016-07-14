#ifndef IC_BACKEND_PANCAKE_BYTECODE_H
#define IC_BACKEND_PANCAKE_BYTECODE_H

enum ic_backend_pancake_bytecode_type {
  /* push_int int */
  ibpb_pushint,
  /* push_str string */
  ibpb_pushstr,
  /* push key */
  ibpb_push,
  /* call fname argn */
  ibpb_call,
  /* pop n */
  ibpb_pop,
  /* return */
  ibpb_return,
  /* jmp addr */
  ibpb_jmp,
  /* jif addr */
  ibpb_jif,
  /* jnif addr */
  ibpb_jnif,
  /* panic str */
  ibpb_panic,
  /* exit_success */
  ipbp_exit,
};

struct ic_backend_pancake_bytecode {
  enum ic_backend_pancake_bytecode_type tag;
  void *arg1;
  void *arg2;
};

/* allocate and initialise a new bytecode
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_bytecode * ic_backend_pancake_bytecode_new(enum ic_backend_pancake_bytecode_type type, void *arg1, void *arg2);

/* initialise an existing bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_bytecode_init(struct ic_backend_pancake_bytecode *bytecode, enum ic_backend_pancake_bytecode_type type, void *arg1, void *arg2);

/* destroy a bytecode
 *
 * will only fre bytecode if `free_bytecode` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_bytecode_destroy(struct ic_backend_pancake_bytecode *bytecode, unsigned int free_bytecode);

#endif /* IC_BACKEND_PANCAKE_BYTECODE_H */
