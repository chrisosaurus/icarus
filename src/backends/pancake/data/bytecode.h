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
    icp_push_bool,
    /* push_uint uint */
    icp_push_unsigned,
    /* push_int int */
    icp_push_signed,
    /* push_str string */
    icp_push_str,
    /* push_unit */
    icp_push_unit,
    /* copyarg argn::uint
     * copyarg at offset argn onto stack
     * Note: arguments to functions are pushed in order
     *       a function call pushes the arguments, and then records the stack
     *       position of the stack before the args were inserted
     *       `copyarg 0` will refer to the 0th argument
     *       which is the *first* argument
     *       if you had 3 arguments then,
     *       copyarg 0 - 1st argument
     *       copyarg 1 - 2nd argument
     *       copyarg 2 - 3rd argument
     */
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
    /* return_unit, pushes a unit onto stack and then returns */
    icp_return_unit,
    /* jmp addr::uint */
    icp_jmp,
    /* jif addr::uint */
    icp_jif,
    /* jnif addr::uint */
    icp_jnif,
    /* jmp_label label::string */
    icp_jmp_label,
    /* jif_label label::string */
    icp_jif_label,
    /* jnif_label label::string */
    icp_jnif_label,
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
    /* alloc slots::uint
     * allocate a new object with this many slots
     * a slot is usually 64 bits
     */
    icp_alloc,
    /* store_offset slot::uint
     * let value = pop()
     * let object = peek()
     * store value at offset `slot` within object
     */
    icp_store_offset,
    /* load_offset_bool slot::uint
     * let object = peek()
     * load value at offset `slot` within object and push onto stack as bool
     */
    icp_load_offset_bool,
    /* load_offset_str slot::uint
     * let object = peek()
     * load value at offset `slot` within object and push onto stack as str
     */
    icp_load_offset_str,
    /* load_offset_uint slot::uint
     * let object = peek()
     * load value at offset `slot` within object and push onto stack as uint
     */
    icp_load_offset_uint,
    /* load_offset_sint slot::uint
     * let object = peek()
     * load value at offset `slot` within object and push onto stack as sint
     */
    icp_load_offset_sint,
    /* load_offset_ref slot::uint
     * let object = peek()
     * load value at offset `slot` within object and push onto stack as ref
     */
    icp_load_offset_ref,
    /* load_offset_unit slot::uint
     * let object = peek()
     * load value at offset `slot` within object and push onto stack as unit
     */
    icp_load_offset_unit,
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
        /* TODO FIXME leaking this void * */
        void *v;
        /* TODO FIXME leaking this char * */
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
 * will only free bytecode if `free_bytecode` is truthy
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
unsigned int ic_backend_pancake_bytecode_print(FILE *fd, struct ic_backend_pancake_bytecode *bytecode);

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
