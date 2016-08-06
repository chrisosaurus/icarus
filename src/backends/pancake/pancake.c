#include <stdio.h>

#include "../../analyse/data/kludge.h"
#include "data/bytecode.h"
#include "data/instructions.h"
#include "data/runtime.h"
#include "pancake.h"

/* compile a kludge into bytecode
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_instructions *ic_backend_pancake_compile(struct ic_kludge *kludge);

/* interpret bytecode in runtime
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_interpret(struct ic_backend_pancake_runtime *runtime);

/* compile an fdecl into bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_fdecl(struct ic_backend_pancake_instructions *instructions, struct ic_kludge *kludge, struct ic_decl_func *fdecl);

/* pancake - stack based interpreter backend
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake(struct ic_kludge *kludge) {
    struct ic_backend_pancake_instructions *instructions = 0;
    struct ic_backend_pancake_runtime *runtime = 0;

    if (!kludge) {
        puts("ic_backend_pancake: kludge was null");
        return 0;
    }

    instructions = ic_backend_pancake_compile(kludge);
    if (!instructions) {
        puts("ic_backend_pancake: call to ic_backend_pancake_compile failed");
        return 0;
    }

    runtime = ic_backend_pancake_runtime_new(instructions);
    if (!instructions) {
        puts("ic_backend_pancake: call to ic_backend_pancake_runtime_new failed");
        return 0;
    }

    if (!ic_backend_pancake_interpret(runtime)) {
        puts("ic_backend_pancake: call to ic_backend_pancake_interpret failed");
        return 0;
    }

    puts("ic_backend_pancake: unimplemented");
    return 1;
}

/* compile a kludge into bytecode
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_instructions *ic_backend_pancake_compile(struct ic_kludge *kludge) {
    struct ic_backend_pancake_instructions *instructions = 0;
    /* offset into kludge fdecls */
    unsigned int offset = 0;
    /* length of kludge fdecls */
    unsigned int len = 0;
    /* current kludge fdecl */
    struct ic_decl_func *fdecl = 0;

    if (!kludge) {
        puts("ic_backend_pancake_compile: kludge was null");
        return 0;
    }

    instructions = ic_backend_pancake_instructions_new();
    if (!instructions) {
        puts("ic_backend_pancake_compile: call to ic_backend_pancake_instructions_new failed");
        return 0;
    }

    len = ic_pvector_length(&(kludge->fdecls));
    if (!len) {
        puts("ic_backend_pancake_compile: call to ic_pvector_length failed");
        return 0;
    }

    /* go through each fdecl, pull it out, call compile_fdecl to do work */
    for (offset = 0; offset < len; ++offset) {
        fdecl = ic_pvector_get(&(kludge->fdecls), offset);
        if (!fdecl) {
            puts("ic_backend_pancake_compile: call to ic_pvector_get failed");
            return 0;
        }

        if (!ic_backend_pancake_compile_fdecl(instructions, kludge, fdecl)) {
            puts("ic_backend_pancake_compile: call to ic_backend_pancake_compile_fdecl failed");
            return 0;
        }
    }

    puts("ic_backend_pancake_compile: implementation pending");
    return instructions;
}

/* compile an fdecl into bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_fdecl(struct ic_backend_pancake_instructions *instructions, struct ic_kludge *kludge, struct ic_decl_func *fdecl) {
    /* sig_call for current fdecl */
    struct ic_string *fdecl_sig_call = 0;
    /* char * to sig_call for current fdec
     * used for bytecode
     */
    char *fdecl_sig_call_ch = 0;
    /* tbody of current fdecl */
    struct ic_transform_body *fdecl_tbody = 0;
    /* offset we created fdecl at */
    unsigned int fdecl_instructions_offset = 0;
    /* dummy bytecode for fdecl */
    struct ic_backend_pancake_bytecode *bc_dummy_fdecl = 0;

    if (!instructions) {
        puts("ic_backend_pancake_compile_fdecl: instructions was null");
        return 0;
    }

    if (!kludge) {
        puts("ic_backend_pancake_compile_fdecl: kludge was null");
        return 0;
    }

    if (!fdecl) {
        puts("ic_backend_pancake_compile_fdecl: fdecl_tbody was null");
        return 0;
    }

    /* FIXME TODO compile kludge into bytecode instructions */

    /* for each fdecl
     *  get current pos, insert into map of fdecl-sig to pos
     *  deal with args ???
     *  for each instruction in fdecl
     *    compile
     *  end
     *  pop args
     * end
     */

    fdecl_sig_call = &(fdecl->sig_call);
    fdecl_sig_call_ch = ic_string_contents(fdecl_sig_call);
    if (!fdecl_sig_call_ch) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_string_contents failed");
        return 0;
    }

    fdecl_tbody = fdecl->tbody;
    if (!fdecl_tbody) {
        puts("ic_backend_pancake_compile_fdecl: no tbody found on fdecl");
        return 0;
    }

    /* get length - which is offset of next instruction */
    fdecl_instructions_offset = ic_backend_pancake_instructions_length(instructions);

    /* register function at offset */
    if (!ic_backend_pancake_instructions_register_fdecl(instructions, fdecl_sig_call, fdecl_instructions_offset)) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_instructions_register_fdecl failed");
        return 0;
    }

    /* insert dummy no-op function label instruction */
    bc_dummy_fdecl = ic_backend_pancake_bytecode_new(ipbp_fdecl_label);
    if (!bc_dummy_fdecl) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_bytecode_new failed");
        return 0;
    }

    if (!ic_backend_pancake_bytecode_arg1_set_char(bc_dummy_fdecl, fdecl_sig_call_ch)) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_bytecode_arg1_set_char failed");
        return 0;
    }

    if (!ic_backend_pancake_instructions_append(instructions, bc_dummy_fdecl)) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_instructions_append failed");
        return 0;
    }

    /* FIXME TODO compile fdecl_tbody */
    printf("ic_backend_pancake_compile_fdecl: UNIMPLEMENTED: skipping compilation for '%s'\n", fdecl_sig_call_ch);

    return 1;
}

/* interpret bytecode in runtime
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_interpret(struct ic_backend_pancake_runtime *runtime) {
    if (!runtime) {
        puts("ic_backend_pancake_interpret: runtime was null");
        return 0;
    }

    puts("ic_backend_pancake_interpret: unimplemented");
    return 0;
}
