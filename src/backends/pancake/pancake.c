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
     *    FIXME TODO need fdecl-sig mapping
     *  deal with args ???
     *  for each instruction in fdecl
     *    compile
     *  end
     *  pop args
     * end
     */

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
