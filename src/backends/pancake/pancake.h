#ifndef IC_BACKEND_PANCAKE_H
#define IC_BACKEND_PANCAKE_H

/* predeclare */
struct ic_kludge;

/* pancake - stack based vm backend
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake(struct ic_kludge *kludge);

#endif /* IC_BACKEND_PANCAKE_H */
