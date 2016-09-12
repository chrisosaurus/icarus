#ifndef IC_BACKEND_PANCAKE_INSTRUCTIONS_H
#define IC_BACKEND_PANCAKE_INSTRUCTIONS_H

#include "../../../data/string.h"
#include "bytecode.h"

struct ic_backend_pancake_instructions {
    /* current number of elements inserted */
    unsigned int len;
    /* allocated capacity */
    unsigned int cap;
    /* current instruction we are up to */
    unsigned int offset;
    /* array of all instructions */
    struct ic_backend_pancake_bytecode *bytecode_array;
    /* map from label sig to offset */
    struct ic_dict *label_offset_map;
};

/* alloc and init a new instructions
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_instructions *ic_backend_pancake_instructions_new(void);

/* init an existing instructions
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_instructions_init(struct ic_backend_pancake_instructions *);

/* destroy an instructions
 *
 * will only free instructions if `free_instructions` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_instructions_destroy(struct ic_backend_pancake_instructions *instructions, unsigned int free_instructions);

/* extend the instructions array by one and return a pointer to this
 * new bytecode
 *
 * also initialises bytecode to specified type
 *
 * will resize as needed
 *
 * returns * on success
 * returns 0 on falure
 */
struct ic_backend_pancake_bytecode *ic_backend_pancake_instructions_add(struct ic_backend_pancake_instructions *instructions, enum ic_backend_pancake_bytecode_type type);

/* get current length of instructions
 *
 * returns number on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_instructions_length(struct ic_backend_pancake_instructions *);

/* get a specified instruction
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_bytecode *ic_backend_pancake_instructions_get(struct ic_backend_pancake_instructions *instructions, unsigned int offset);

/* get current offset into instructions
 *
 * returns offset on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_instructions_get_offset(struct ic_backend_pancake_instructions *);

/* get current instruction
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_bytecode *ic_backend_pancake_instructions_get_current(struct ic_backend_pancake_instructions *);

/* advance offset by 1 and return instruction
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_bytecode *ic_backend_pancake_instructions_advance(struct ic_backend_pancake_instructions *);

/* set offset
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_instructions_set_offset(struct ic_backend_pancake_instructions *instructions, unsigned int offset);

/* add new fdecl-sig to offset mapping
 * this registers a function at an offset
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_instructions_register_label(struct ic_backend_pancake_instructions *instructions, char *fdecl_sig_call, unsigned int offset);

/* get an offset for a fdecl-sig
 *
 * FIXME no way to indicate failure
 *
 * returns int on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_instructions_get_label(struct ic_backend_pancake_instructions *instructions, char *fdecl_sig_call);

/* print instructions to provided file
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_instructions_print(struct ic_backend_pancake_instructions *instructions, FILE *file);

/* load bytecode instructions from file
 *
 * allocates new instructions to hold
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_instructions *ic_backend_pancake_instructions_load(FILE *file);

#endif /* IC_BACKEND_PANCAKE_INSTRUCTIONS_H */
