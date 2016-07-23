#include <stdio.h>
#include <stdlib.h>

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

    instructions->bytecode_array = calloc(instructions->cap, sizeof(struct ic_backend_pancake_bytecode));
    if (!instructions->bytecode_array) {
        puts("ic_backend_pancake_instructions_init: call to calloc failed");
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
