#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../../data/string.h"
#include "bytecode.h"

/* allocate and initialise a new bytecode
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_bytecode *ic_backend_pancake_bytecode_new(enum ic_backend_pancake_bytecode_type type) {
    struct ic_backend_pancake_bytecode *bc = 0;

    bc = calloc(1, sizeof(struct ic_backend_pancake_bytecode));
    if (!bc) {
        puts("ic_backend_pancake_bytecode_new: call to calloc failed");
        return 0;
    }

    if (!ic_backend_pancake_bytecode_init(bc, type)) {
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
unsigned int ic_backend_pancake_bytecode_init(struct ic_backend_pancake_bytecode *bytecode, enum ic_backend_pancake_bytecode_type type) {
    if (!bytecode) {
        puts("ic_backend_pancake_bytecode_init: bytecode was null");
        return 0;
    }

    bytecode->tag = type;

    bytecode->arg1.tag = ic_backend_pancake_bytecode_arg_type_void;
    bytecode->arg1.u.v = 0;

    bytecode->arg2.tag = ic_backend_pancake_bytecode_arg_type_void;
    bytecode->arg2.u.v = 0;

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
unsigned int ic_backend_pancake_bytecode_print(FILE *fd, struct ic_backend_pancake_bytecode *bytecode) {
    /* types used for possible args */
    char *ch = 0;
    uint32_t uint = 0;
    int32_t sint = 0;
    bool boolean = 0;

    if (!bytecode) {
        puts("ic_backend_pancake_bytecode_print: bytecode was null");
        return 0;
    }

    switch (bytecode->tag) {
        /* label string */
        case icp_label:
            ch = ic_backend_pancake_bytecode_arg1_get_char(bytecode);
            if (!ch) {
                puts("ic_backend_pancake_bytecode_print: call to ic_backend_pancake_bytecode_arg1_get_char failed");
                return 0;
            }

            fprintf(fd, "label %s", ch);

            return 1;
            break;

        /* push_bool bool */
        case icp_pushbool:
            boolean = ic_backend_pancake_bytecode_arg1_get_bool(bytecode);

            fprintf(fd, "pushbool %" PRId8, boolean);

            return 1;
            break;

        /* push_uint uint */
        case icp_pushuint:
            uint = ic_backend_pancake_bytecode_arg1_get_uint(bytecode);

            fprintf(fd, "pushuint %" PRId32, uint);

            return 1;
            break;

        /* push_int int */
        case icp_pushint:
            sint = ic_backend_pancake_bytecode_arg1_get_sint(bytecode);

            fprintf(fd, "pushint %" PRId32, sint);

            return 1;
            break;

        /* push_str string */
        case icp_pushstr:
            ch = ic_backend_pancake_bytecode_arg1_get_char(bytecode);
            if (!ch) {
                puts("ic_backend_pancake_bytecode_print: call to ic_backend_pancake_bytecode_arg1_get_char failed");
                return 0;
            }
            fprintf(fd, "pushstr \"%s\"", ch);

            return 1;
            break;

        /* copyarg argn::uint */
        case icp_copyarg:
            uint = ic_backend_pancake_bytecode_arg1_get_uint(bytecode);

            fprintf(fd, "copyarg %" PRId32, uint);

            return 1;
            break;

        /* call fname::stringargn */
        case icp_call:
            ch = ic_backend_pancake_bytecode_arg1_get_char(bytecode);
            if (!ch) {
                puts("ic_backend_pancake_bytecode_print: call to ic_backend_pancake_bytecode_arg1_get_char failed");
                return 0;
            }

            uint = ic_backend_pancake_bytecode_arg2_get_uint(bytecode);

            fprintf(fd, "call %s %" PRId32, ch, uint);

            return 1;
            break;

        /* tailcall_void fname::stringargn */
        case icp_tailcall_void:
            ch = ic_backend_pancake_bytecode_arg1_get_char(bytecode);
            if (!ch) {
                puts("ic_backend_pancake_bytecode_print: call to ic_backend_pancake_bytecode_arg1_get_char failed");
                return 0;
            }

            uint = ic_backend_pancake_bytecode_arg2_get_uint(bytecode);

            fprintf(fd, "tailcall_void %s %" PRId32, ch, uint);

            return 1;
            break;

        /* tailcall_value fname::stringargn */
        case icp_tailcall_value:
            ch = ic_backend_pancake_bytecode_arg1_get_char(bytecode);
            if (!ch) {
                puts("ic_backend_pancake_bytecode_print: call to ic_backend_pancake_bytecode_arg1_get_char failed");
                return 0;
            }

            uint = ic_backend_pancake_bytecode_arg2_get_uint(bytecode);

            fprintf(fd, "tailcall_value %s %" PRId32, ch, uint);

            return 1;
            break;

        /* call_builtin fname::stringargn */
        case icp_call_builtin:
            ch = ic_backend_pancake_bytecode_arg1_get_char(bytecode);
            if (!ch) {
                puts("ic_backend_pancake_bytecode_print: call to ic_backend_pancake_bytecode_arg1_get_char failed");
                return 0;
            }

            uint = ic_backend_pancake_bytecode_arg2_get_uint(bytecode);

            fprintf(fd, "call_builtin %s %" PRId32, ch, uint);

            return 1;
            break;

        /* pop n */
        case icp_pop:
            uint = ic_backend_pancake_bytecode_arg1_get_uint(bytecode);

            fprintf(fd, "pop %" PRId32, uint);

            return 1;
            break;

        /* return_value */
        case icp_return_value:
            fputs("return_value", fd);
            return 1;
            break;

        /* return_void */
        case icp_return_void:
            fputs("return_void", fd);
            return 1;
            break;

        /* jmp addr::uint */
        case icp_jmp:
            uint = ic_backend_pancake_bytecode_arg1_get_uint(bytecode);

            fprintf(fd, "jmp %" PRId32, uint);

            return 1;
            break;

        /* jif addr */
        case icp_jif:
            uint = ic_backend_pancake_bytecode_arg1_get_uint(bytecode);

            fprintf(fd, "jif %" PRId32, uint);

            return 1;
            break;

        /* jnif addr */
        case icp_jnif:
            uint = ic_backend_pancake_bytecode_arg1_get_uint(bytecode);

            fprintf(fd, "jnif %" PRId32, uint);

            return 1;
            break;

        /* jmp_label label::string */
        case icp_jmp_label:
            ch = ic_backend_pancake_bytecode_arg1_get_char(bytecode);
            if (!ch) {
                puts("ic_backend_pancake_bytecode_print: call to ic_backend_pancake_bytecode_arg1_get_char failed");
                return 0;
            }
            fprintf(fd, "jmp_label %s", ch);

            return 1;
            break;

        /* jif_label label::string */
        case icp_jif_label:
            ch = ic_backend_pancake_bytecode_arg1_get_char(bytecode);
            if (!ch) {
                puts("ic_backend_pancake_bytecode_print: call to ic_backend_pancake_bytecode_arg1_get_char failed");
                return 0;
            }
            fprintf(fd, "jif_label %s", ch);

            return 1;
            break;

        /* jnif_label label::string */
        case icp_jnif_label:
            ch = ic_backend_pancake_bytecode_arg1_get_char(bytecode);
            if (!ch) {
                puts("ic_backend_pancake_bytecode_print: call to ic_backend_pancake_bytecode_arg1_get_char failed");
                return 0;
            }
            fprintf(fd, "jnif_label %s", ch);

            return 1;
            break;

        /* panic str */
        case icp_panic:
            ch = ic_backend_pancake_bytecode_arg1_get_char(bytecode);
            if (!ch) {
                puts("ic_backend_pancake_bytecode_print: call to ic_backend_pancake_bytecode_arg1_get_char failed");
                return 0;
            }

            fprintf(fd, "panic %s", ch);

            return 1;
            break;

        /* exit_success */
        case icp_exit:
            fputs("exit", fd);

            return 1;
            break;

        /* save current top of stack to restore later */
        case icp_save:
            fputs("save", fd);

            return 1;
            break;

        /* restore previously saved item to top of stack */
        case icp_restore:
            fputs("restore", fd);

            return 1;
            break;

        case icp_store:
            ch = ic_backend_pancake_bytecode_arg1_get_char(bytecode);
            if (!ch) {
                puts("ic_backend_pancake_bytecode_print: call to ic_backend_pancake_bytecode_arg1_get_char failed");
                return 0;
            }

            fprintf(fd, "store %s", ch);

            return 1;
            break;

        case icp_load:
            ch = ic_backend_pancake_bytecode_arg1_get_char(bytecode);
            if (!ch) {
                puts("ic_backend_pancake_bytecode_print: call to ic_backend_pancake_bytecode_arg1_get_char failed");
                return 0;
            }

            fprintf(fd, "load %s", ch);

            return 1;
            break;

        case icp_clean_stack:
            fputs("clean_stack", fd);
            return 1;
            break;

        case icp_alloc:
            uint = ic_backend_pancake_bytecode_arg1_get_uint(bytecode);
            fprintf(fd, "alloc %u", uint);
            return 1;
            break;

        case icp_store_offset:
            uint = ic_backend_pancake_bytecode_arg1_get_uint(bytecode);
            fprintf(fd, "store_offset %u", uint);
            return 1;
            break;

        case icp_load_offset:
            uint = ic_backend_pancake_bytecode_arg1_get_uint(bytecode);
            fprintf(fd, "load_offset %u", uint);
            return 1;
            break;

        default:
            puts("ic_backend_pancake_bytecode_print: impossible case");
            return 0;
            break;
    }

    return 1;
}

/* set arg1 as void* on bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_bytecode_arg1_set_void(struct ic_backend_pancake_bytecode *bytecode, void *arg1) {
    if (!bytecode) {
        puts("ic_backend_pancake_bytecode_arg1_set_void: bytecode was null");
        return 0;
    }

    /* only allow arg to be set where it is correct */
    switch (bytecode->tag) {
        default:
            puts("ic_backend_pancake_bytecode_arg1_set_void: invalid void arg set on bytecode that doesn't support");
            return 0;
            break;
    }

    bytecode->arg1.tag = ic_backend_pancake_bytecode_arg_type_void;
    bytecode->arg1.u.v = arg1;

    return 1;
}

/* set arg2 as void* on bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_bytecode_arg2_set_void(struct ic_backend_pancake_bytecode *bytecode, void *arg2) {
    if (!bytecode) {
        puts("ic_backend_pancake_bytecode_arg2_set_uint: bytecode was null");
        return 0;
    }

    /* only allow arg to be set where it is correct */
    switch (bytecode->tag) {
        default:
            puts("ic_backend_pancake_bytecode_arg2_set_void: invalid void arg set on bytecode that doesn't support");
            return 0;
            break;
    }

    bytecode->arg2.tag = ic_backend_pancake_bytecode_arg_type_void;
    bytecode->arg2.u.v = arg2;

    return 1;
}

/* get arg1 as void* from bytecode
 *
 * returns * on success
 * returns 0 on failure
 */
void *ic_backend_pancake_bytecode_arg1_get_void(struct ic_backend_pancake_bytecode *bytecode) {
    if (!bytecode) {
        puts("ic_backend_pancake_bytecode_arg1_get_void: bytecode was null");
        return 0;
    }

    if (bytecode->arg1.tag != ic_backend_pancake_bytecode_arg_type_void) {
        puts("ic_backend_pancake_bytecode_arg1_get_void: bytecode arg1 was not voic");
        return 0;
    }

    return bytecode->arg1.u.v;
}

/* get arg2 as void* from bytecode
 *
 * returns * on success
 * returns 0 on failure
 */
void *ic_backend_pancake_bytecode_arg2_get_void(struct ic_backend_pancake_bytecode *bytecode) {
    if (!bytecode) {
        puts("ic_backend_pancake_bytecode_arg2_get_void: bytecode was null");
        return 0;
    }

    if (bytecode->arg2.tag != ic_backend_pancake_bytecode_arg_type_void) {
        puts("ic_backend_pancake_bytecode_arg2_get_void: bytecode arg2 was not void");
        return 0;
    }

    return bytecode->arg2.u.v;
}

/* set arg1 as char* on bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_bytecode_arg1_set_char(struct ic_backend_pancake_bytecode *bytecode, char *arg1) {
    if (!bytecode) {
        puts("ic_backend_pancake_bytecode_arg1_set_char: bytecode was null");
        return 0;
    }
    /* only allow arg to be set where it is correct */
    switch (bytecode->tag) {
        /* fdecl_label fdecl_sig_call */
        case icp_label:
        /* push_str string */
        case icp_pushstr:
        /* call fname::string argn::uint */
        case icp_call:
        /* tailcall_void fname::string argn::uint */
        case icp_tailcall_void:
        /* tailcall_value fname::string argn::uint */
        case icp_tailcall_value:
        /* call_builtin fname::string argn::uint */
        case icp_call_builtin:
        /* panic desc::str */
        case icp_panic:
        /* store key::string
         * stores current top of stack under key
         */
        case icp_store:
        /* load key::string
         * loads from key onto under
         */
        case icp_load:
        /* jmp_label label::string */
        case icp_jmp_label:
        /* jif_label label::string */
        case icp_jif_label:
        /* jnif_label label::string */
        case icp_jnif_label:
            break;

        default:
            puts("ic_backend_pancake_bytecode_arg1_set_char: invalid char arg set on bytecode that doesn't support");
            return 0;
            break;
    }

    bytecode->arg1.tag = ic_backend_pancake_bytecode_arg_type_char;
    bytecode->arg1.u.ch = arg1;

    return 1;
}

/* set arg2 as char* on bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_bytecode_arg2_set_char(struct ic_backend_pancake_bytecode *bytecode, char *arg2) {
    if (!bytecode) {
        puts("ic_backend_pancake_bytecode_arg2_set_char: bytecode was null");
        return 0;
    }

    bytecode->arg2.tag = ic_backend_pancake_bytecode_arg_type_char;
    bytecode->arg2.u.ch = arg2;

    /* only allow arg to be set where it is correct */
    switch (bytecode->tag) {
        default:
            puts("ic_backend_pancake_bytecode_arg2_set_char: invalid char arg set on bytecode that doesn't support");
            return 0;
            break;
    }

    return 1;
}

/* get arg1 as char* from bytecode
 *
 * returns * on success
 * returns 0 on failure
 */
char *ic_backend_pancake_bytecode_arg1_get_char(struct ic_backend_pancake_bytecode *bytecode) {
    if (!bytecode) {
        puts("ic_backend_pancake_bytecode_arg1_get_char: bytecode was null");
        return 0;
    }

    if (bytecode->arg1.tag != ic_backend_pancake_bytecode_arg_type_char) {
        puts("ic_backend_pancake_bytecode_arg1_get_char: bytecode arg1 was not char");
        return 0;
    }

    return bytecode->arg1.u.ch;
}

/* get arg2 as char* from bytecode
 *
 * returns * on success
 * returns 0 on failure
 */
char *ic_backend_pancake_bytecode_arg2_get_char(struct ic_backend_pancake_bytecode *bytecode) {
    if (!bytecode) {
        puts("ic_backend_pancake_bytecode_arg2_get_char: bytecode was null");
        return 0;
    }

    if (bytecode->arg2.tag != ic_backend_pancake_bytecode_arg_type_char) {
        puts("ic_backend_pancake_bytecode_arg2_get_char: bytecode arg2 was not char");
        return 0;
    }

    return bytecode->arg2.u.ch;
}

/* set arg1 as bool on bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_bytecode_arg1_set_bool(struct ic_backend_pancake_bytecode *bytecode, uint8_t arg1) {
    if (!bytecode) {
        puts("ic_backend_pancake_bytecode_arg1_set_bool: bytecode was null");
        return 0;
    }
    /* only allow arg to be set where it is correct */
    switch (bytecode->tag) {
        /* push_bool bool */
        case icp_pushbool:
            break;

        default:
            puts("ic_backend_pancake_bytecode_arg1_set_bool: invalid bool arg set on bytecode that doesn't support");
            return 0;
            break;
    }

    bytecode->arg1.tag = ic_backend_pancake_bytecode_arg_type_bool;
    bytecode->arg1.u.boolean = arg1;

    return 1;
}

/* set arg2 as bool on bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_bytecode_arg2_set_bool(struct ic_backend_pancake_bytecode *bytecode, uint8_t arg2) {
    if (!bytecode) {
        puts("ic_backend_pancake_bytecode_arg2_set_bool: bytecode was null");
        return 0;
    }

    /* only allow arg to be set where it is correct */
    switch (bytecode->tag) {
        default:
            puts("ic_backend_pancake_bytecode_arg2_set_bool: invalid bool arg set on bytecode that doesn't support");
            return 0;
            break;
    }

    bytecode->arg2.tag = ic_backend_pancake_bytecode_arg_type_bool;
    bytecode->arg2.u.boolean = arg2;

    return 1;
}

/* get arg1 as bool from bytecode
 *
 * returns * on success
 * returns 0 on failure
 */
uint8_t ic_backend_pancake_bytecode_arg1_get_bool(struct ic_backend_pancake_bytecode *bytecode) {
    if (!bytecode) {
        puts("ic_backend_pancake_bytecode_arg1_get_bool: bytecode was null");
        return 0;
    }

    if (bytecode->arg1.tag != ic_backend_pancake_bytecode_arg_type_bool) {
        puts("ic_backend_pancake_bytecode_arg1_get_bool: bytecode arg1 was not bool");
        return 0;
    }

    return bytecode->arg1.u.boolean;
}

/* get arg2 as bool from bytecode
 *
 * returns * on success
 * returns 0 on failure
 */
uint8_t ic_backend_pancake_bytecode_arg2_get_bool(struct ic_backend_pancake_bytecode *bytecode) {
    if (!bytecode) {
        puts("ic_backend_pancake_bytecode_arg2_get_bool: bytecode was null");
        return 0;
    }

    if (bytecode->arg2.tag != ic_backend_pancake_bytecode_arg_type_bool) {
        puts("ic_backend_pancake_bytecode_arg2_get_bool: bytecode arg2 was not bool");
        return 0;
    }

    return bytecode->arg2.u.boolean;
}

/* set arg1 as sint on bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_bytecode_arg1_set_sint(struct ic_backend_pancake_bytecode *bytecode, int32_t arg1) {
    if (!bytecode) {
        puts("ic_backend_pancake_bytecode_arg1_set_sint: bytecode was null");
        return 0;
    }
    /* only allow arg to be set where it is correct */
    switch (bytecode->tag) {
        /* push_int int */
        case icp_pushint:
            break;

        default:
            puts("ic_backend_pancake_bytecode_arg1_set_sint: invalid sint arg set on bytecode that doesn't support");
            return 0;
            break;
    }

    bytecode->arg1.tag = ic_backend_pancake_bytecode_arg_type_sint;
    bytecode->arg1.u.sint = arg1;

    return 1;
}

/* set arg2 as sint on bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_bytecode_arg2_set_sint(struct ic_backend_pancake_bytecode *bytecode, int32_t arg2) {
    if (!bytecode) {
        puts("ic_backend_pancake_bytecode_arg2_set_sint: bytecode was null");
        return 0;
    }
    /* only allow arg to be set where it is correct */
    switch (bytecode->tag) {
        default:
            puts("ic_backend_pancake_bytecode_arg2_set_sint: invalid sint arg set on bytecode that doesn't support");
            return 0;
            break;
    }

    bytecode->arg2.tag = ic_backend_pancake_bytecode_arg_type_sint;
    bytecode->arg2.u.sint = arg2;

    return 1;
}

/* get arg1 as sint from bytecode
 *
 * unable to communicate error
 *
 * returns sintsuccess
 * returns 0 on failure
 */
int32_t ic_backend_pancake_bytecode_arg1_get_sint(struct ic_backend_pancake_bytecode *bytecode) {
    if (!bytecode) {
        puts("ic_backend_pancake_bytecode_arg1_get_sint: bytecode was null");
        return 0;
    }

    if (bytecode->arg1.tag != ic_backend_pancake_bytecode_arg_type_sint) {
        puts("ic_backend_pancake_bytecode_arg1_get_sint: bytecode arg2 was not sint");
        return 0;
    }

    return bytecode->arg1.u.sint;
}

/* get arg2 as sint from bytecode
 *
 *unable to communicate error

 * returns sintsuccess
 * returns 0 on failure
 */
int32_t ic_backend_pancake_bytecode_arg2_get_sint(struct ic_backend_pancake_bytecode *bytecode) {
    if (!bytecode) {
        puts("ic_backend_pancake_bytecode_arg2_get_sint: bytecode was null");
        return 0;
    }

    if (bytecode->arg2.tag != ic_backend_pancake_bytecode_arg_type_sint) {
        puts("ic_backend_pancake_bytecode_arg1_get_sint: bytecode arg2 was not sint");
        return 0;
    }

    return bytecode->arg1.u.sint;
}

/* set arg1 as uint on bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_bytecode_arg1_set_uint(struct ic_backend_pancake_bytecode *bytecode, uint32_t arg1) {
    if (!bytecode) {
        puts("ic_backend_pancake_bytecode_arg1_set_uint: bytecode was null");
        return 0;
    }
    /* only allow arg to be set where it is correct */
    switch (bytecode->tag) {
        /* push_uint uint */
        case icp_pushuint:
        /* copyarg argn::uint */
        case icp_copyarg:
        /* pop n::uint */
        case icp_pop:
        /* jmp addr::uint */
        case icp_jmp:
        /* jif addr::uint */
        case icp_jif:
        /* jnif addr::uint */
        case icp_jnif:
        /* alloc slots::uint */
        case icp_alloc:
        /* store_offset slot::uint */
        case icp_store_offset:
        /* load_offset slot::uint */
        case icp_load_offset:
            break;

        default:
            puts("ic_backend_pancake_bytecode_arg1_set_uint: invalid uint arg set on bytecode that doesn't support");
            return 0;
            break;
    }

    bytecode->arg1.tag = ic_backend_pancake_bytecode_arg_type_uint;
    bytecode->arg1.u.uint = arg1;

    return 1;
}

/* set arg2 as uint on bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_bytecode_arg2_set_uint(struct ic_backend_pancake_bytecode *bytecode, uint32_t arg2) {
    if (!bytecode) {
        puts("ic_backend_pancake_bytecode_arg2_set_uint: bytecode was null");
        return 0;
    }

    /* only allow arg to be set where it is correct */
    switch (bytecode->tag) {
        /* call fname::stringargn::uint */
        case icp_call:
        /* tailcall_void fname::stringargn::uint */
        case icp_tailcall_void:
        /* tailcall_value fname::stringargn::uint */
        case icp_tailcall_value:
        /* call_builtin fname::stringargn::uint */
        case icp_call_builtin:
            break;

        default:
            puts("ic_backend_pancake_bytecode_arg2_set_uint: invalid uint arg set on bytecode that doesn't support");
            return 0;
            break;
    }

    bytecode->arg2.tag = ic_backend_pancake_bytecode_arg_type_uint;
    bytecode->arg2.u.uint = arg2;

    return 1;
}

/* get arg1 as uint from bytecode
 *
 * unable to communicate error
 *
 * returns uint on success
 * returns 0 on failure
 */
uint32_t ic_backend_pancake_bytecode_arg1_get_uint(struct ic_backend_pancake_bytecode *bytecode) {
    if (!bytecode) {
        puts("ic_backend_pancake_bytecode_arg1_get_uint: bytecode was null");
        return 0;
    }

    if (bytecode->arg1.tag != ic_backend_pancake_bytecode_arg_type_uint) {
        puts("ic_backend_pancake_bytecode_arg1_get_uint: bytecode arg1 was not uint");
        return 0;
    }

    return bytecode->arg1.u.uint;
}

/* get arg2 as uint from bytecode
 *
 * unable to communicate error
 *
 * returns uint on success
 * returns 0 on failure
 */
uint32_t ic_backend_pancake_bytecode_arg2_get_uint(struct ic_backend_pancake_bytecode *bytecode) {
    if (!bytecode) {
        puts("ic_backend_pancake_bytecode_arg2_get_uint: bytecode was null");
        return 0;
    }

    if (bytecode->arg2.tag != ic_backend_pancake_bytecode_arg_type_uint) {
        puts("ic_backend_pancake_bytecode_arg2_get_uint: bytecode arg2 was not uint");
        return 0;
    }

    return bytecode->arg2.u.uint;
}
