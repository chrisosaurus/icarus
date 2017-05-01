#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../../data/dict.h"
#include "../../../data/strdup.h"
#include "../../../data/string.h"
#include "../../../read/read.h"
#include "instructions.h"

#define IC_BACKEND_PANCAKE_INSTRUCTIONS_START_SIZE 1024

/* alloc and init a new instructions
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_instructions *ic_backend_pancake_instructions_new(void) {
    struct ic_backend_pancake_instructions *instructions = 0;

    instructions = calloc(1, sizeof(struct ic_backend_pancake_instructions));
    if (!instructions) {
        puts("ic_backend_pancake_instructions_new: call to calloc failed");
        return 0;
    }

    if (!ic_backend_pancake_instructions_init(instructions)) {
        puts("ic_backend_pancake_instructions_new: call to ic_backend_pancake_instructions_init failed");
        free(instructions);
        return 0;
    }

    return instructions;
}

/* init an existing instructions
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_instructions_init(struct ic_backend_pancake_instructions *instructions) {
    if (!instructions) {
        puts("ic_backend_pancake_instructions_init: instructions was null");
        return 0;
    }

    instructions->offset = 0;

    instructions->len = 0;

    instructions->cap = IC_BACKEND_PANCAKE_INSTRUCTIONS_START_SIZE;

    /* using a c-array and realloc below
     * does not use pvector
     * no init call needed
     */
    instructions->bytecode_array = calloc(instructions->cap, sizeof(struct ic_backend_pancake_bytecode));
    if (!instructions->bytecode_array) {
        puts("ic_backend_pancake_instructions_init: call to calloc failed bytecode_array");
        return 0;
    }

    instructions->label_offset_map = calloc(1, sizeof(struct ic_dict));
    if (!instructions->label_offset_map) {
        puts("ic_backend_pancake_instructions_init: call to calloc failed for label_offset_map");
        return 0;
    }

    if (!ic_dict_init(instructions->label_offset_map)) {
        puts("ic_backend_pancake_instructions_init: call to ic_dict_init");
        return 0;
    }

    return 1;
}

/* destroy an instructions
 *
 * will only free instructions if `free_instructions` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_instructions_destroy(struct ic_backend_pancake_instructions *instructions, unsigned int free_instructions) {
    if (!instructions) {
        puts("ic_backend_pancake_instructions_destroy: instructions was null");
        return 0;
    }

    free(instructions->bytecode_array);
    instructions->bytecode_array = 0;

    /* dict_destroy(dict, free_dict, free_data)
     * free-ing data as it is alloced in register
     */
    if (!ic_dict_destroy(instructions->label_offset_map, 1, 1)) {
        puts("ic_backend_pancake_instructions_destroy: call to ic_dict_destroy failed");
        return 0;
    }

    if (free_instructions) {
        free(instructions);
    }

    return 1;
}

/* extend the instructions array by one and return a pointer to this
 * new bytecode
 *
 * also initialises bytecode to specified type
 *
 * will resize as needed
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_bytecode *ic_backend_pancake_instructions_add(struct ic_backend_pancake_instructions *instructions, enum ic_backend_pancake_bytecode_type type) {
    struct ic_backend_pancake_bytecode *bytecode = 0;

    if (!instructions) {
        puts("ic_backend_pancake_instructions_add: instructions was null");
        return 0;
    }

    /* resize if needed */
    if (instructions->len >= instructions->cap) {
        /* double capacity */
        instructions->cap *= 2;
        instructions->bytecode_array = realloc(instructions->bytecode_array, instructions->cap * sizeof(struct ic_backend_pancake_bytecode));
        if (!instructions->bytecode_array) {
            puts("ic_backend_pancake_instructions_add: call to realloc failed");
            return 0;
        }
    }

    /* get address of current head*/
    bytecode = &(instructions->bytecode_array[instructions->len]);

    if (!ic_backend_pancake_bytecode_init(bytecode, type)) {
        puts("ic_backend_pancake_instructions_add: call to ic_backend_pancake_bytecode_init failed");
        return 0;
    }

    /* increment len counter */
    instructions->len += 1;

    return bytecode;
}

/* return last instruction inserted
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_bytecode *ic_backend_pancake_instructions_get_last(struct ic_backend_pancake_instructions *instructions) {
    unsigned int len = 0;
    struct ic_backend_pancake_bytecode *bytecode = 0;

    if (!instructions) {
        puts("ic_backend_pancake_instructions_get_last: instructions was null");
        return 0;
    }

    len = instructions->len;

    if (len == 0) {
        puts("ic_backend_pancake_instructions_get_last: instructions was empty");
        return 0;
    }

    bytecode = ic_backend_pancake_instructions_get(instructions, len - 1);
    if (!bytecode) {
        puts("ic_backend_pancake_instructions_get_last: call to ic_backend_pancake_instructions_get failed");
        return 0;
    }

    return bytecode;
}

/* get current length of instructions
 *
 * returns number on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_instructions_length(struct ic_backend_pancake_instructions *instructions) {
    if (!instructions) {
        puts("ic_backend_pancake_instructions_length: instructions was null");
        return 0;
    }

    return instructions->len;
}

/* get a specified instruction
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_bytecode *ic_backend_pancake_instructions_get(struct ic_backend_pancake_instructions *instructions, unsigned int offset) {

    if (!instructions) {
        puts("ic_backend_pancake_instructions_get: instructions was null");
        return 0;
    }

    if (offset >= instructions->len) {
        puts("ic_backend_pancake_instructions_get: offset >= len");
        return 0;
    }

    return &(instructions->bytecode_array[offset]);
}

/* get current offset into instructions
 *
 * returns offset on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_instructions_get_offset(struct ic_backend_pancake_instructions *instructions) {

    if (!instructions) {
        puts("ic_backend_pancake_instructions_get_offset: instructions was null");
        return 0;
    }

    return instructions->offset;
}

/* get current instruction
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_bytecode *ic_backend_pancake_instructions_get_current(struct ic_backend_pancake_instructions *instructions) {

    if (!instructions) {
        puts("ic_backend_pancake_instructions_get_current: instructions was null");
        return 0;
    }

    return &(instructions->bytecode_array[instructions->offset]);
}

/* advance offset by 1 and return instruction
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_bytecode *ic_backend_pancake_instructions_advance(struct ic_backend_pancake_instructions *instructions) {

    if (!instructions) {
        puts("ic_backend_pancake_instructions_advance: instructions was null");
        return 0;
    }

    /* check we have room to advance into
   * instructions->len is an impossible offset
   * instructions->len - 1 is the highest possible offset
   * therefore in order to advance we must be < instructions->len - 1
   * so if instructions->offset >= instructions->len - 1
   * we are too far
   */
    if (instructions->offset >= (instructions->len - 1)) {
        puts("ic_backend_pancake_instructions_advance: cannot advance, at end of tape");
        return 0;
    }

    instructions->offset += 1;

    return &(instructions->bytecode_array[instructions->offset]);
}

/* set offset
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_instructions_set_offset(struct ic_backend_pancake_instructions *instructions, unsigned int offset) {
    if (!instructions) {
        puts("ic_backend_pancake_instructions_set_offset: instructions was null");
        return 0;
    }

    if (offset >= instructions->len) {
        puts("ic_backend_pancake_instructions_set_offset: offset >= len");
        return 0;
    }

    instructions->offset = offset;

    return 1;
}

/* add new fdecl-sig to offset mapping
 * this registers a function at an offset
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_instructions_register_label(struct ic_backend_pancake_instructions *instructions, char *fdecl_sig_call, unsigned int offset) {
    unsigned int *offset_alloc = 0;

    if (!instructions) {
        puts("ic_backend_pancake_instructions_register_label: instructions was null");
        return 0;
    }

    if (!fdecl_sig_call) {
        puts("ic_backend_pancake_instructions_register_label: fdecl_sig_call was null");
        return 0;
    }

    /* have to allocate int to get pointer to it
   * bit gross
   * TODO FIXME think about this alloc */
    offset_alloc = calloc(1, sizeof(unsigned int));
    if (!offset_alloc) {
        puts("ic_backend_pancake_instructions_register_label: call to calloc failed");
        return 0;
    }

    *offset_alloc = offset;

    if (!ic_dict_insert(instructions->label_offset_map, fdecl_sig_call, offset_alloc)) {
        puts("ic_backend_pancake_instructions_register_label: call to ic_dict_insert failed");
        return 0;
    }

    return 1;
}

/* get an offset for a fdecl-sig
 *
 * FIXME no way to indicate failure
 *
 * returns int on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_instructions_get_label(struct ic_backend_pancake_instructions *instructions, char *fdecl_sig_call) {
    unsigned int offset = 0;
    unsigned int *offset_alloc = 0;

    if (!instructions) {
        puts("ic_backend_pancake_instructions_get_label: instructions was null");
        return 0;
    }

    if (!fdecl_sig_call) {
        puts("ic_backend_pancake_instructions_get_label: fdecl_sig_call was null");
        return 0;
    }

    offset_alloc = ic_dict_get(instructions->label_offset_map, fdecl_sig_call);
    if (!offset_alloc) {
        puts("ic_backend_pancake_instructions_get_label: call to ic_dict_get failed");
        return 0;
    }

    offset = *offset_alloc;

    return offset;
}

/* print instructions to provided file
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_instructions_print(FILE *fd, struct ic_backend_pancake_instructions *instructions) {
    unsigned int i = 0;
    unsigned int len = 0;
    struct ic_backend_pancake_bytecode *current_bytecode = 0;

    if (!instructions) {
        puts("ic_backend_pancake_instructions_print: instructions was null");
        return 0;
    }

    len = instructions->len;
    for (i = 0; i < len; ++i) {
        if (i > 0) {
            /* \n sep. between lines */
            fputs("\n", fd);
        }

        current_bytecode = &(instructions->bytecode_array[i]);
        if (!ic_backend_pancake_bytecode_print(fd, current_bytecode)) {
            puts("ic_backend_pancake_instructions_print: call to ic_backend_pancake_bytecode_print failed");
            return 0;
        }
    }

    /* trailing \n */
    fputs("\n", fd);

    return 1;
}

/* load bytecode instructions from file
 *
 * allocates new instructions to hold
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_instructions *ic_backend_pancake_instructions_load(FILE *file) {
    struct ic_backend_pancake_instructions *instructions = 0;
    struct ic_backend_pancake_bytecode *instruction = 0;

    char op[256];
    char str_arg1[256];
    char *nstr = 0;
    unsigned int uint_arg1;
    unsigned int uint_arg2;
    int sint_arg1;

    int ret = 0;

    /* offset of instruction we just added */
    unsigned int cur_offset = 0;

    if (!file) {
        puts("ic_backend_pancake_instructions_load: file was null");
        return 0;
    }

    instructions = ic_backend_pancake_instructions_new();
    if (!instructions) {
        puts("ic_backend_pancake_instructions_load: call to ic_backend_pancake_instruction_new failed");
        return 0;
    }

    /* FIXME TODO
     * parse through file
     * create bytecode
     * insert into instructions
     */

    while (1) {
        cur_offset = ic_backend_pancake_instructions_length(instructions);

        ret = fscanf(file, "%s", op);

        if (ret == EOF) {
            /* success */
            break;
        }

        if (ret != 1) {
            /* failure */
            puts("ic_backend_pancake_instructions_load: failed to read");
            return 0;
        }

        if (!strcmp("pushuint", op)) {
            instruction = ic_backend_pancake_instructions_add(instructions, icp_pushuint);
        } else if (!strcmp("pushint", op)) {
            instruction = ic_backend_pancake_instructions_add(instructions, icp_pushint);
        } else if (!strcmp("pushstr", op)) {
            instruction = ic_backend_pancake_instructions_add(instructions, icp_pushstr);
        } else if (!strcmp("pushbool", op)) {
            instruction = ic_backend_pancake_instructions_add(instructions, icp_pushbool);
        } else if (!strcmp("call_builtin", op)) {
            instruction = ic_backend_pancake_instructions_add(instructions, icp_call_builtin);
        } else if (!strcmp("exit", op)) {
            instruction = ic_backend_pancake_instructions_add(instructions, icp_exit);
        } else if (!strcmp("label", op)) {
            instruction = ic_backend_pancake_instructions_add(instructions, icp_label);
        } else if (!strcmp("save", op)) {
            instruction = ic_backend_pancake_instructions_add(instructions, icp_save);
        } else if (!strcmp("restore", op)) {
            instruction = ic_backend_pancake_instructions_add(instructions, icp_restore);
        } else if (!strcmp("clean_stack", op)) {
            instruction = ic_backend_pancake_instructions_add(instructions, icp_clean_stack);
        } else if (!strcmp("call", op)) {
            instruction = ic_backend_pancake_instructions_add(instructions, icp_call);
        } else if (!strcmp("return_value", op)) {
            instruction = ic_backend_pancake_instructions_add(instructions, icp_return_value);
        } else if (!strcmp("return_void", op)) {
            instruction = ic_backend_pancake_instructions_add(instructions, icp_return_void);
        } else if (!strcmp("copyarg", op)) {
            instruction = ic_backend_pancake_instructions_add(instructions, icp_copyarg);
        } else if (!strcmp("store", op)) {
            instruction = ic_backend_pancake_instructions_add(instructions, icp_store);
        } else if (!strcmp("load", op)) {
            instruction = ic_backend_pancake_instructions_add(instructions, icp_load);
        } else if (!strcmp("jmp_label", op)) {
            instruction = ic_backend_pancake_instructions_add(instructions, icp_jmp_label);
        } else if (!strcmp("jif_label", op)) {
            instruction = ic_backend_pancake_instructions_add(instructions, icp_jif_label);
        } else if (!strcmp("jnif_label", op)) {
            instruction = ic_backend_pancake_instructions_add(instructions, icp_jnif_label);
        } else {
            printf("ic_backend_pancake_instructions_load: unsupported instruction '%s'\n", op);
            return 0;
        }

        if (!instruction) {
            puts("ic_backend_pancake_instructions_load: call to ic_backend_pancake_instructions_add failed");
            return 0;
        }

        switch (instruction->tag) {
            case icp_copyarg:
                /* consume uint */
                ret = fscanf(file, "%u", &uint_arg1);
                if (ret == EOF || ret == 0) {
                    puts("ic_backend_pancake_instructions_load: read failed for copyarg");
                    return 0;
                }
                if (!ic_backend_pancake_bytecode_arg1_set_uint(instruction, uint_arg1)) {
                    puts("ic_backend_pancake_instructions_load: call to backend_pancake_bytecode_arg1_set_uint failed");
                    return 0;
                }
                break;

            case icp_pushuint:
                /* consume uint */
                ret = fscanf(file, "%u", &uint_arg1);
                if (ret == EOF || ret == 0) {
                    puts("ic_backend_pancake_instructions_load: read failed for pushuint");
                    return 0;
                }
                if (!ic_backend_pancake_bytecode_arg1_set_uint(instruction, uint_arg1)) {
                    puts("ic_backend_pancake_instructions_load: call to backend_pancake_bytecode_arg1_set_uint failed");
                    return 0;
                }
                break;

            case icp_pushint:
                /* consume uint */
                ret = fscanf(file, "%d", &sint_arg1);
                if (ret == EOF || ret == 0) {
                    puts("ic_backend_pancake_instructions_load: read failed for pushint");
                    return 0;
                }
                if (!ic_backend_pancake_bytecode_arg1_set_sint(instruction, sint_arg1)) {
                    puts("ic_backend_pancake_instructions_load: call to backend_pancake_bytecode_arg1_set_sint failed");
                    return 0;
                }
                break;

            case icp_pushbool:
                /* consume bool */
                ret = fscanf(file, "%u", &uint_arg1);
                if (ret == EOF || ret == 0) {
                    puts("ic_backend_pancake_instructions_load: read failed for pushuint");
                    return 0;
                }
                if (!ic_backend_pancake_bytecode_arg1_set_bool(instruction, uint_arg1)) {
                    puts("ic_backend_pancake_instructions_load: call to backend_pancake_bytecode_arg1_set_uint failed");
                    return 0;
                }
                break;

            case icp_jmp_label:
            case icp_jif_label:
            case icp_jnif_label:
            case icp_store:
            case icp_load:
                /* consume str */
                ret = fscanf(file, "%s", str_arg1);
                if (ret == EOF || ret == 0) {
                    puts("ic_backend_pancake_instructions_load: read failed for store/load/j*_label");
                    return 0;
                }

                nstr = ic_strdup(str_arg1);
                if (!nstr) {
                    puts("ic_backend_pancake_instructions_load: call to ic_strdup failed");
                    return 0;
                }

                if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, nstr)) {
                    puts("ic_backend_pancake_instructions_load: call to backend_pancake_bytecode_arg1_set_char failed");
                    return 0;
                }

                break;

            case icp_pushstr:
                /* consume str */
                ret = fscanf(file, " \"%[^\"]\"", str_arg1);
                if (ret == EOF || ret == 0) {
                    puts("ic_backend_pancake_instructions_load: read failed for pushstr");
                    return 0;
                }

                nstr = ic_strdup(str_arg1);
                if (!nstr) {
                    puts("ic_backend_pancake_instructions_load: call to ic_strdup failed");
                    return 0;
                }

                if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, nstr)) {
                    puts("ic_backend_pancake_instructions_load: call to backend_pancake_bytecode_arg1_set_char failed");
                    return 0;
                }

                break;

            case icp_label:
                /* consume string */
                ret = fscanf(file, "%s", str_arg1);
                if (ret == EOF || ret != 1) {
                    puts("ic_backend_pancake_instructions_load: read failed for icp_label");
                    return 0;
                }

                nstr = ic_strdup(str_arg1);
                if (!nstr) {
                    puts("ic_backend_pancake_instructions_load: call to ic_strdup failed");
                    return 0;
                }

                if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, nstr)) {
                    puts("ic_backend_pancake_instructions_load: call to backend_pancake_bytecode_arg1_set_char failed");
                    return 0;
                }

                /* register label as function */
                if (!ic_backend_pancake_instructions_register_label(instructions, nstr, cur_offset)) {
                    puts("ic_backend_pancake_instructions_load: call to ic_backend_pancake_instructions_register_label failed");
                    return 0;
                }

                break;

            case icp_call:
            case icp_call_builtin:
                /* consume str and arg count */
                ret = fscanf(file, "%s", str_arg1);
                if (ret == EOF || ret != 1) {
                    puts("ic_backend_pancake_instructions_load: read 1 failed for call or call_builtin");
                    return 0;
                }

                ret = fscanf(file, "%u", &uint_arg2);
                if (ret == EOF || ret != 1) {
                    puts("ic_backend_pancake_instructions_load: read 2 failed for call or call_builtin");
                    return 0;
                }

                nstr = ic_strdup(str_arg1);

                if (!nstr) {
                    puts("ic_backend_pancake_instructions_load: call to ic_strdup failed");
                    return 0;
                }

                if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, nstr)) {
                    puts("ic_backend_pancake_instructions_load: call to backend_pancake_bytecode_arg1_set_char failed");
                    return 0;
                }

                if (!ic_backend_pancake_bytecode_arg2_set_uint(instruction, uint_arg2)) {
                    puts("ic_backend_pancake_instructions_load: call to backend_pancake_bytecode_arg2_set_uint failed");
                    return 0;
                }

                break;

            case icp_save:
            case icp_restore:
            case icp_clean_stack:
            case icp_return_void:
            case icp_return_value:
            case icp_exit:
                /* nothing more to do */
                break;

            default:
                puts("ic_backend_pancake_instructions_load: unsupported instruction->tag");
                return 0;
                break;
        }
    }

    return instructions;
}
