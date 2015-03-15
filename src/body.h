#ifndef ICARU_BODY_H
#define ICARU_BODY_H

#include "pvector.h"

/* predeclare ic_statement to allow use of ic_statement *
 * as type
 */
struct ic_statement;

/* the body of a function or statement
 * (such as the body of an if-statement)
 * a body is collection of statements
 */
struct ic_body {
    struct ic_pvector contents;
};

/* allocate and initialise a new ic_body
 *
 * returns pointer to new ic_body on sucess
 * returns 0 on failure
 */
struct ic_body * ic_body_new(void);

/* initialise an existing ic_body
 *
 * returns 0 on sucess
 * returns 1 on failure
 */
int ic_body_init(struct ic_body *body);

/* returns item at offset i on sucess
 * returns 0 on failure
 */
struct ic_statement * ic_body_get(struct ic_body *body, unsigned int i);

/* append data to body
 *
 * returns index of item on success
 * returns -1 on failure
 */
int ic_body_append(struct ic_body *body, struct ic_statement * data);

/* returns length on success
 * returns 0 on failure
 */
unsigned int ic_body_length(struct ic_body *body);

#endif
