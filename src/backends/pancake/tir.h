#ifndef IC_BACKEND_PANCAKE_TIR_H
#define IC_BACKEND_PANCAKE_TIR_H

/* predeclarations */
struct ic_kludge;
struct ic_transform_ir_stmt;
struct ic_instructions;

/* compile a given statement onto specified instructions
 *
 * generates a pancake-bytecode program
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_stmt(struct ic_kludge *kludge, struct ic_transform_ir_stmt *tstmt, struct ic_instructions *instructions);

#endif /* IC_BACKEND_PANCAKE_TIR_H */
