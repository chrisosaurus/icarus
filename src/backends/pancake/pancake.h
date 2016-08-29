#ifndef IC_BACKEND_PANCAKE_H
#define IC_BACKEND_PANCAKE_H

/* predeclare */
struct ic_kludge;
struct ic_backend_pancake_runtime;

/* pancake - stack based interpreter backend
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake(struct ic_kludge *kludge);

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

#endif /* IC_BACKEND_PANCAKE_H */
