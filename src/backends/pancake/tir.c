#include <stdio.h>

#include "tir.h"

/* compile a given statement onto specified instructions
 *
 * generates a pancake-bytecode program
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_stmt(struct ic_kludge *kludge, struct ic_transform_ir_stmt *tstmt, struct ic_instructions *instructions) {
    if (!kludge) {
        puts("ic_backend_pancake_compile_stmt: kludge was null");
        return 0;
    }

    if (!tstmt) {
        puts("ic_backend_pancake_compile_stmt: tstmt was null");
        return 0;
    }

    if (!instructions) {
        puts("ic_backend_pancake_compile_stmt: instructions was null");
        return 0;
    }

    puts("ic_backend_pancake_compile_stmt: implementation pending");
    return 0;
}
