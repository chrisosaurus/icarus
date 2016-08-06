#ifndef IC_BACKEND_PANCAKE_BYTECODE_H
#define IC_BACKEND_PANCAKE_BYTECODE_H

#include <stdbool.h>
#include <stdint.h>

/* NB: the types here are NOT IC_ types
 * 'str' is char *
 * 'uint' in unsigned int *
 * 'bool' is bool *
 *
 * FIXME TODO consider changing address from uint to label
 */
enum ic_backend_pancake_bytecode_type {
    /* fdecl_label fdecl_sig_call */
    ipbp_fdecl_label,
    /* push_bool bool */
    ipbp_pushbool,
    /* push_uint uint */
    ibpb_pushuint,
    /* push_int int */
    ibpb_pushint,
    /* push_str string */
    ibpb_pushstr,
    /* push key::string */
    ibpb_push,
    /* call fname argn::uint */
    ibpb_call,
    /* pop n::uint */
    ibpb_pop,
    /* return */
    ibpb_return,
    /* jmp addr::uint */
    ibpb_jmp,
    /* jif addr::uint */
    ibpb_jif,
    /* jnif addr::uint */
    ibpb_jnif,
    /* panic desc::str */
    ibpb_panic,
    /* exit_success */
    ipbp_exit,
};

enum ic_backend_pancake_bytecode_arg_type {
    ic_backend_pancake_bytecode_arg_type_void,
    ic_backend_pancake_bytecode_arg_type_char,
    ic_backend_pancake_bytecode_arg_type_bool,
    ic_backend_pancake_bytecode_arg_type_sint,
    ic_backend_pancake_bytecode_arg_type_uint,
};

struct ic_backend_pancake_bytecode_arg {
    enum ic_backend_pancake_bytecode_arg_type tag;
    union {
        void *v;
        char *ch;
        uint8_t boolean;
        int32_t sint;
        uint32_t uint;
    } u;
};

struct ic_backend_pancake_bytecode {
    enum ic_backend_pancake_bytecode_type tag;
    struct ic_backend_pancake_bytecode_arg arg1;
    struct ic_backend_pancake_bytecode_arg arg2;
};

/* allocate and initialise a new bytecode
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_bytecode *ic_backend_pancake_bytecode_new(enum ic_backend_pancake_bytecode_type type);

/* initialise an existing bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_bytecode_init(struct ic_backend_pancake_bytecode *bytecode, enum ic_backend_pancake_bytecode_type type);

/* destroy a bytecode
 *
 * will only fre bytecode if `free_bytecode` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_bytecode_destroy(struct ic_backend_pancake_bytecode *bytecode, unsigned int free_bytecode);

/* print bytecode representation to provided FILE
 *
 * will not append a trailing \n
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_bytecode_print(struct ic_backend_pancake_bytecode *bytecode, FILE *file);

/* set arg1 as void* on bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_bytecode_arg1_set_void(struct ic_backend_pancake_bytecode *bytecode, void *arg1);

/* set arg2 as void* on bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_bytecode_arg2_set_void(struct ic_backend_pancake_bytecode *bytecode, void *arg2);

/* get arg1 as void* from bytecode
 *
 * returns * on success
 * returns 0 on failure
 */
void *ic_backend_pancake_bytecode_arg1_get_void(struct ic_backend_pancake_bytecode *bytecode);

/* get arg2 as void* from bytecode
 *
 * returns * on success
 * returns 0 on failure
 */
void *ic_backend_pancake_bytecode_arg2_get_void(struct ic_backend_pancake_bytecode *bytecode);

/* set arg1 as char* on bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_bytecode_arg1_set_char(struct ic_backend_pancake_bytecode *bytecode, char *arg1);

/* set arg2 as char* on bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_bytecode_arg2_set_char(struct ic_backend_pancake_bytecode *bytecode, char *arg2);

/* get arg1 as char* from bytecode
 *
 * returns * on success
 * returns 0 on failure
 */
char *ic_backend_pancake_bytecode_arg1_get_char(struct ic_backend_pancake_bytecode *bytecode);

/* get arg2 as char* from bytecode
 *
 * returns * on success
 * returns 0 on failure
 */
char *ic_backend_pancake_bytecode_arg2_get_char(struct ic_backend_pancake_bytecode *bytecode);

/* set arg1 as bool on bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_bytecode_arg1_set_bool(struct ic_backend_pancake_bytecode *bytecode, uint8_t arg1);

/* set arg2 as bool on bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_bytecode_arg2_set_bool(struct ic_backend_pancake_bytecode *bytecode, uint8_t arg2);

/* get arg1 as bool from bytecode
 *
 * returns * on success
 * returns 0 on failure
 */
uint8_t ic_backend_pancake_bytecode_arg1_get_bool(struct ic_backend_pancake_bytecode *bytecode);

/* get arg2 as bool from bytecode
 *
 * returns * on success
 * returns 0 on failure
 */
uint8_t ic_backend_pancake_bytecode_arg2_get_bool(struct ic_backend_pancake_bytecode *bytecode);

/* set arg1 as sint on bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_bytecode_arg1_set_sint(struct ic_backend_pancake_bytecode *bytecode, int32_t arg1);

/* set arg2 as sint on bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_bytecode_arg2_set_sint(struct ic_backend_pancake_bytecode *bytecode, int32_t arg2);

/* get arg1 as sint from bytecode
 *
 * unable to communicate error
 *
 * returns sintsuccess
 * returns 0 on failure
 */
int32_t ic_backend_pancake_bytecode_arg1_get_sint(struct ic_backend_pancake_bytecode *bytecode);

/* get arg2 as sint from bytecode
 *
 *unable to communicate error

 * returns sintsuccess
 * returns 0 on failure
 */
int32_t ic_backend_pancake_bytecode_arg2_get_sint(struct ic_backend_pancake_bytecode *bytecode);

/* set arg1 as uint on bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_bytecode_arg1_set_uint(struct ic_backend_pancake_bytecode *bytecode, uint32_t arg1);

/* set arg2 as uint on bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_bytecode_arg2_set_uint(struct ic_backend_pancake_bytecode *bytecode, uint32_t arg2);

/* get arg1 as uint from bytecode
 *
 * unable to communicate error
 *
 * returns uint on success
 * returns 0 on failure
 */
uint32_t ic_backend_pancake_bytecode_arg1_get_uint(struct ic_backend_pancake_bytecode *bytecode);

/* get arg2 as uint from bytecode
 *
 * unable to communicate error
 *
 * returns uint on success
 * returns 0 on failure
 */
uint32_t ic_backend_pancake_bytecode_arg2_get_uint(struct ic_backend_pancake_bytecode *bytecode);

#endif /* IC_BACKEND_PANCAKE_BYTECODE_H */
