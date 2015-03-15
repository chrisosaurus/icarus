#include <stdio.h> /* puts, printf */
#include <stdlib.h> /* calloc */

#include "body.h"
#include "pvector.h"
#include "statement.h"

/* allocate and initialise a new ic_body
 *
 * returns pointer to new ic_body on sucess
 * returns 0 on failure
 */
struct ic_body * ic_body_new(void){
    struct ic_body *body = 0;

    /* alloc */
    body = calloc(1, sizeof(struct ic_body));
    if( ! body ){
        puts("ic_body_new: calloc failed");
        return 0;
    }

    /* init */
    if( ic_body_init(body) ){
        puts("ic_body_new: call to ic_body_init failed");
        return 0;
    }

    return body;
}

/* initialise an existing ic_body
 *
 * returns 0 on sucess
 * returns 1 on failure
 */
int ic_body_init(struct ic_body *body){
    if( ! body ){
        puts("ic_body_init: body was null");
        return 1;
    };

    /* simply dispatch to pvector init */
    if( ic_pvector_init( &(body->contents), 0 ) ){
        puts("ic_body_init: call to ic_pvector_init failed");
        return 1;
    }

    /* success */
    return 0;
}

/* returns item at offset i on sucess
 * returns 0 on failure
 */
struct ic_statement * ic_body_get(struct ic_body *body, unsigned int i){
    if( ! body ){
        puts("ic_body_get: body was null");
        return 0;
    };

    /* let pvector do all the work */
    return ic_pvector_get( &(body->contents), i );
}

/* append data to body
 *
 * returns index of item on success
 * returns -1 on failure
 */
int ic_body_append(struct ic_body *body, struct ic_statement * data){
    if( ! body ){
        puts("ic_body_append: body was null");
        return -1;
    };

    /* let pvector do all the work */
    return ic_pvector_append( &(body->contents), data );
}

/* returns length on success
 * returns 0 on failure
 */
unsigned int ic_body_length(struct ic_body *body){
    if( ! body ){
        puts("ic_body_length: body was null");
        return 0;
    };

    /* let pvector do all the work */
    return ic_pvector_length( &(body->contents) );
}

