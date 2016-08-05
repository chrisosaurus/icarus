#include <stdio.h>
#include <stdlib.h>

#include "../../../data/string.h"
#include "bytecode.h"

/* allocate and initialise a new bytecode
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_bytecode *ic_backend_pancake_bytecode_new(enum ic_backend_pancake_bytecode_type type, void *arg1, void *arg2) {
    struct ic_backend_pancake_bytecode *bc = 0;

    bc = calloc(1, sizeof(struct ic_backend_pancake_bytecode));
    if (!bc) {
        puts("ic_backend_pancake_bytecode_new: call to calloc failed");
        return 0;
    }

    if (!ic_backend_pancake_bytecode_init(bc, type, arg1, arg2)) {
        puts("ic_backend_pancake_bytecode_new: call to ic_backend_pancake_bytecode_init failed");
        return 0;
    }

    return bc;
}

/* initialise an existing bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_bytecode_init(struct ic_backend_pancake_bytecode *bytecode, enum ic_backend_pancake_bytecode_type type, void *arg1, void *arg2) {
    if (!bytecode) {
        puts("ic_backend_pancake_bytecode_init: bytecode was null");
        return 0;
    }

    bytecode->tag = type;
    bytecode->arg1 = arg1;
    bytecode->arg2 = arg2;

    return 1;
}

/* destroy a bytecode
 *
 * will only fre bytecode if `free_bytecode` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_bytecode_destroy(struct ic_backend_pancake_bytecode *bytecode, unsigned int free_bytecode) {
    if (!bytecode) {
        puts("ic_backend_pancake_bytecode_destroy: bytecode was null");
        return 0;
    }

    if (free_bytecode) {
        free(bytecode);
    }

    return 1;
}

/* print bytecode representation to provided FILE
 *
 * will not append a trailing \n
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_bytecode_print(struct ic_backend_pancake_bytecode *bytecode, FILE *file) {
    /* types used for possible args */
    char *ch = 0;
    unsigned int uint = 0;
    int integer = 0;
    unsigned int boolean = 0;

    if (!bytecode) {
        puts("ic_backend_pancake_bytecode_print: bytecode was null");
        return 0;
    }

    if (!file) {
        puts("ic_backend_pancake_bytecode_print: file was null");
        return 0;
    }

    switch (bytecode->tag) {
        /* fdecl_label fdecl_sig_call */
        case ipbp_fdecl_label:
            ch = bytecode->arg1;
            if (!ch) {
                puts("ic_backend_pancake_bytecode_print: ipbp_fdecl_label: arg1 was null");
                return 0;
            }

            fprintf(file, "label %s", ch);

            return 1;
            break;

        /* push_bool bool */
        case ipbp_pushbool:
            if (!bytecode->arg1) {
                puts("ic_backend_pancake_bytecode_print: ipbp_push_bool: arg1 was null");
                return 0;
            }
            boolean = *((unsigned int *)bytecode->arg1);

            fprintf(file, "pushbool %ud", boolean);

            return 1;
            break;

        /* push_uint uint */
        case ibpb_pushuint:
            if (!bytecode->arg1) {
                puts("ic_backend_pancake_bytecode_print: ipbp_push_uint: arg1 was null");
                return 0;
            }
            uint = *((unsigned int *)bytecode->arg1);

            fprintf(file, "pushuint %ud", uint);

            return 1;
            break;

        /* push_int int */
        case ibpb_pushint:
            if (!bytecode->arg1) {
                puts("ic_backend_pancake_bytecode_print: ipbp_push_int: arg1 was null");
                return 0;
            }
            integer = *((int *)bytecode->arg1);

            fprintf(file, "pushint %d", integer);

            return 1;
            break;

        /* push_str string */
        case ibpb_pushstr:
            ch = bytecode->arg1;
            if (!ch) {
                puts("ic_backend_pancake_bytecode_print: ipbp_push_str: arg1 was null");
                return 0;
            }
            fprintf(file, "pushstr %s", ch);

            return 1;
            break;

        /* push key */
        case ibpb_push:
            ch = bytecode->arg1;
            if (!ch) {
                puts("ic_backend_pancake_bytecode_print: ipbp_push: arg1 was null");
                return 0;
            }

            fprintf(file, "push %s", ch);

            return 1;
            break;

        /* call fname argn */
        case ibpb_call:
            ch = bytecode->arg1;
            if (!ch) {
                puts("ic_backend_pancake_bytecode_print: ipbp_call: arg1 was null");
                return 0;
            }

            if (!(bytecode->arg2)) {
                puts("ic_backend_pancake_bytecode_print: ipbp_call: arg2 was null");
                return 0;
            }
            uint = *((unsigned int *)bytecode->arg2);

            fprintf(file, "call %s %ud", ch, uint);

            return 1;
            break;

        /* pop n */
        case ibpb_pop:
            if (!(bytecode->arg1)) {
                puts("ic_backend_pancake_bytecode_print: ipbp_pop: arg1 was null");
                return 0;
            }
            uint = *((unsigned int *)bytecode->arg1);

            fprintf(file, "pop %ud", uint);

            return 1;
            break;

        /* return */
        case ibpb_return:
            fputs("return", file);
            return 1;
            break;

        /* jmp addr */
        case ibpb_jmp:
            if (!(bytecode->arg1)) {
                puts("ic_backend_pancake_bytecode_print: ipbp_jmp: arg1 was null");
                return 0;
            }
            uint = *((unsigned int *)bytecode->arg1);

            fprintf(file, "jmp %ud", uint);

            return 1;
            break;

        /* jif addr */
        case ibpb_jif:
            if (!(bytecode->arg1)) {
                puts("ic_backend_pancake_bytecode_print: ipbp_jif: arg1 was null");
                return 0;
            }
            uint = *((unsigned int *)bytecode->arg1);

            fprintf(file, "jif %ud", uint);

            return 1;
            break;

        /* jnif addr */
        case ibpb_jnif:
            if (!(bytecode->arg1)) {
                puts("ic_backend_pancake_bytecode_print: ipbp_jnif: arg1 was null");
                return 0;
            }
            uint = *((unsigned int *)bytecode->arg1);

            fprintf(file, "jnif %ud", uint);

            return 1;
            break;

        /* panic str */
        case ibpb_panic:
            ch = bytecode->arg1;
            if (!ch) {
                puts("ic_backend_pancake_bytecode_print: ipbp_panic: arg1 was null");
                return 0;
            }

            fprintf(file, "panic %s", ch);

            return 1;
            break;

        /* exit_success */
        case ipbp_exit:
            fputs("exit", file);

            return 1;
            break;

        default:
            puts("ic_backend_pancake_bytecode_print: impossible case");
            return 0;
            break;
    }

    return 1;
}
