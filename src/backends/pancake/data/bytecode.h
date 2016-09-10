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
    icp_label,
    /* push_bool bool */
    icp_pushbool,
    /* push_uint uint */
    icp_pushuint,
    /* push_int int */
    icp_pushint,
    /* push_str string */
    icp_pushstr,
    /* copyarg argn::uint */
    icp_copyarg,
    /* call fname::string argn::uint */
    icp_call,
    /* tailcall_void fname::string argn::uint */
    icp_tailcall_void,
    /* tailcall_value fname::string argn::uint */
    icp_tailcall_value,
    /* call_builtin fname::string argn::uint */
    icp_call_builtin,
    /* pop n::uint */
    icp_pop,
    /* return_value */
    icp_return_value,
    /* return_void */
    icp_return_void,
    /* jmp addr::uint */
    icp_jmp,
    /* jif addr::uint */
    icp_jif,
    /* jnif addr::uint */
    icp_jnif,
    /* panic desc::str */
    icp_panic,
    /* exit_success */
    icp_exit,
    /* save current top of stack to restore later
     * NB: save will overwrite any previously saved value
     * NB: saved value not guaranteed to survive past any function calls
     *     as they may themselves call save
     *     only intended to be used immediately before a popn call
     *     and then to call restore
     */
    icp_save,
    /* restore previously saved item to top of stack
     * NB: error if no previous item was saved
     */
    icp_restore,
    /* store key::string
     * stores current top of stack under key
     */
    icp_store,
    /* load key::string
     * loads from key onto under
     */
    icp_load,
    /* clean_stack
     * resets stack-frame back to how it was before caller inserted args
     */
    icp_clean_stack,
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
