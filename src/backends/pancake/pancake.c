#include <stdio.h>

#include "../../analyse/data/kludge.h"
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
    unsigned int offset = 0;
    unsigned int len = 0;
    struct ic_decl_func *fdecl = 0;
    struct ic_string *fdecl_sig_call = 0;
    char *fdecl_sig_call_ch = 0;
    struct ic_transform_body *fdecl_tbody = 0;
    unsigned int fdecl_offset = 0;

    if (!kludge) {
        puts("ic_backend_pancake_compile: kludge was null");
        return 0;
    }

    instructions = ic_backend_pancake_instructions_new();
    if (!instructions) {
        puts("ic_backend_pancake_compile: call to ic_backend_pancake_instructions_new failed");
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
    len = ic_pvector_length(&(kludge->fdecls));
    if (!len) {
        puts("ic_backend_pancake_compile: call to ic_pvector_length failed");
        return instructions;
    }

    for (offset = 0; offset < len; ++offset) {
        fdecl = ic_pvector_get(&(kludge->fdecls), offset);
        if (!fdecl) {
            puts("ic_backend_pancake_compile: call to ic_pvector_get failed");
            return instructions;
        }

        fdecl_sig_call = &(fdecl->sig_call);
        fdecl_sig_call_ch = ic_string_contents(fdecl_sig_call);
        if (!fdecl_sig_call_ch) {
            puts("ic_backend_pancake_compile: call to ic_string_contents failed");
            return instructions;
        }

        fdecl_tbody = fdecl->tbody;
        if (!fdecl_tbody) {
            puts("ic_backend_pancake_compile: no tbody found on fdecl");
            return instructions;
        }

        /* get length - which is offset of next instruction
         * FIXME TODO may want to insert a dummy no-op instructions with a label
         *  for each function
         */
        fdecl_offset = ic_backend_pancake_instructions_length(instructions);

        /* register function at offset */
        if (!ic_backend_pancake_instructions_register_fdecl(instructions, fdecl_sig_call, fdecl_offset)) {
            puts("ic_backend_pancake_compile: call to ic_backend_pancake_instructions_register_fdecl failed");
            return instructions;
        }

        /* FIXME TODO compile fdecl_tbody */
        printf("ic_backend_pancake_compile: UNIMPLEMENTED: skipping compilation for '%s'\n", fdecl_sig_call_ch);
    }

    puts("ic_backend_pancake_compile: implementation pending");
    return instructions;
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
