#include <stdio.h>
#include <stdlib.h>

#include "../../../data/dict.h"
#include "../../../data/string.h"
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

    instructions->fdecl_offset_map = calloc(1, sizeof(struct ic_dict));
    if (!instructions->fdecl_offset_map) {
        puts("ic_backend_pancake_instructions_init: call to calloc failed for fdecl_offset_map");
        return 0;
    }

    if (!ic_dict_init(instructions->fdecl_offset_map)) {
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
    if (!ic_dict_destroy(instructions->fdecl_offset_map, 1, 1)) {
        puts("ic_backend_pancake_instructions_destroy: call to ic_dict_destroy failed");
        return 0;
    }

    if (free_instructions) {
        free(instructions);
    }

    return 1;
}

/* append given bytecode instructions
 *
 * will resize as needed
 *
 * returns 1 on success
 * returns 0 on falure
 */
unsigned int ic_backend_pancake_instructions_append(struct ic_backend_pancake_instructions *instructions, struct ic_backend_pancake_bytecode *bytecode) {

    if (!instructions) {
        puts("ic_backend_pancake_instructions_append: instructions was null");
        return 0;
    }

    /* resize if needed */
    if (instructions->len >= instructions->cap) {
        /* double capacity */
        instructions->cap *= 2;
        instructions->bytecode_array = realloc(instructions->bytecode_array, instructions->cap * sizeof(struct ic_backend_pancake_bytecode));
        if (!instructions->bytecode_array) {
            puts("ic_backend_pancake_instructions_append: call to realloc failed");
            return 0;
        }
    }

    /* insert via copy */
    instructions->bytecode_array[instructions->len] = *bytecode;

    /* increment len counter */
    instructions->len += 1;

    return 1;
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
unsigned int ic_backend_pancake_instructions_register_fdecl(struct ic_backend_pancake_instructions *instructions, char *fdecl_sig_call, unsigned int offset) {
    unsigned int *offset_alloc = 0;

    if (!instructions) {
        puts("ic_backend_pancake_instructions_register_fdecl: instructions was null");
        return 0;
    }

    if (!fdecl_sig_call) {
        puts("ic_backend_pancake_instructions_register_fdecl: fdecl_sig_call was null");
        return 0;
    }

    /* have to allocate int to get pointer to it
   * bit gross
   * TODO FIXME think about this alloc */
    offset_alloc = calloc(1, sizeof(unsigned int));
    if (!offset_alloc) {
        puts("ic_backend_pancake_instructions_register_fdecl: call to calloc failed");
        return 0;
    }

    *offset_alloc = offset;

    if (!ic_dict_insert(instructions->fdecl_offset_map, fdecl_sig_call, offset_alloc)) {
        puts("ic_backend_pancake_instructions_register_fdecl: call to ic_dict_insert failed");
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
unsigned int ic_backend_pancake_instructions_get_fdecl(struct ic_backend_pancake_instructions *instructions, char *fdecl_sig_call) {
    unsigned int offset = 0;
    unsigned int *offset_alloc = 0;

    if (!instructions) {
        puts("ic_backend_pancake_instructions_get_fdecl: instructions was null");
        return 0;
    }

    if (!fdecl_sig_call) {
        puts("ic_backend_pancake_instructions_get_fdecl: fdecl_sig_call was null");
        return 0;
    }

    offset_alloc = ic_dict_get(instructions->fdecl_offset_map, fdecl_sig_call);
    if (!offset_alloc) {
        puts("ic_backend_pancake_instructions_get_fdecl: call to ic_dict_get failed");
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
unsigned int ic_backend_pancake_instructions_print(struct ic_backend_pancake_instructions *instructions, FILE *file) {
    unsigned int i = 0;
    unsigned int len = 0;
    struct ic_backend_pancake_bytecode *current_bytecode = 0;

    if (!instructions) {
        puts("ic_backend_pancake_instructions_print: instructions was null");
        return 0;
    }

    if (!file) {
        puts("ic_backend_pancake_instructions_print: file was null");
        return 0;
    }

    len = instructions->len;
    for (i = 0; i < len; ++i) {
        if (i > 0) {
            /* \n sep. between lines */
            fputs("\n", file);
        }

        current_bytecode = &(instructions->bytecode_array[i]);
        if (!ic_backend_pancake_bytecode_print(current_bytecode, file)) {
            puts("ic_backend_pancake_instructions_print: call to ic_backend_pancake_bytecode_print failed");
            return 0;
        }
    }

    return 1;
}
