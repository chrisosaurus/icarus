#ifndef IC_BACKEND_PANCAKE_H
#define IC_BACKEND_PANCAKE_H

#include "../../opts.h"

/* predeclare */
struct ic_kludge;
struct ic_backend_pancake_runtime_data;

/* pancake - stack based interpreter backend
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake(struct ic_kludge *kludge, struct ic_opts *opts);

/* compile a kludge into bytecode
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_instructions *ic_backend_pancake_compile(struct ic_kludge *kludge);

/* interpret bytecode in runtime_data
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_interpret(struct ic_backend_pancake_runtime_data *runtime_data);

/* load bytecode from file (opts->in_bytecode_filename) and then interpret it
 * in a fresh pancake runtime
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_run_bytecode_from_file(struct ic_opts *opts);

#endif /* IC_BACKEND_PANCAKE_H */
