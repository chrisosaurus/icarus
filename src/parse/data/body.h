#ifndef ICARU_BODY_H
#define ICARU_BODY_H

#include "../../data/pvector.h"
#include "../../data/scope.h"

/* predeclare ic_stmt to allow use of *ic_stmt
 * as type
 */
struct ic_stmt;

/* the body of a function or statement
 * (such as the body of an if-statement)
 * a body is collection of statements
 */
struct ic_body {
    /* pvector containing statements */
    struct ic_pvector contents;
    /* the scope of this body
     * char* -> ic_slot
     */
    struct ic_scope *scope;
};

/* allocate and initialise a new ic_body
 *
 * returns pointer to new ic_body on success
 * returns 0 on failure
 */
struct ic_body *ic_body_new(void);

/* initialise an existing ic_body
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_body_init(struct ic_body *body);

/* destroy body
 *
 * will only free body if `free_body` is truthy
 *
 * this will NOT free the scope
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_body_destroy(struct ic_body *body, unsigned int free_body);

/* perform a deep copy of a body
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_body *ic_body_deep_copy(struct ic_body *body);

/* perform a deep copy of a body embedded within an object
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_body_deep_copy_embedded(struct ic_body *from, struct ic_body *to);

/* returns item at offset i on success
 * returns 0 on failure
 */
struct ic_stmt *ic_body_get(struct ic_body *body, unsigned int i);

/* append data to body
 *
 * returns index of item on success
 * returns -1 on failure
 */
int ic_body_append(struct ic_body *body, struct ic_stmt *data);

/* returns length on success
 * returns 0 on failure
 */
unsigned int ic_body_length(struct ic_body *body);

/* print this body */
void ic_body_print(FILE *fd, struct ic_body *body, unsigned int *indent_level);

#endif
