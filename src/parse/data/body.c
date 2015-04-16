#include <stdio.h> /* puts, printf */
#include <stdlib.h> /* calloc, free */

#include "body.h"
#include "../../data/pvector.h"
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
        free(body);
        return 0;
    }

    return body;
}

/* initialise an existing ic_body
 *
 * returns 0 on sucess
 * returns 1 on failure
 */
unsigned int ic_body_init(struct ic_body *body){
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
struct ic_stmt * ic_body_get(struct ic_body *body, unsigned int i){
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
int ic_body_append(struct ic_body *body, struct ic_stmt * data){
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

/* print this body */
void ic_body_print(struct ic_body *body, unsigned int *indent_level){
    struct ic_stmt *stmt = 0;
    unsigned int i = 0;
    unsigned int len = 0;

    if( ! body ){
        puts("ic_body_print: body was null");
        return;
    }
    if( ! indent_level ){
        puts("ic_body_print: indent_level was null");
        return;
    }

    /* incr indent level before body */
    ++ *indent_level;

    len = ic_pvector_length(&(body->contents));
    for( i=0; i<len; ++i ){
        stmt = ic_pvector_get( &(body->contents), i );

        if( ! stmt ){
            puts("ic_body_print: ic_pvector_get call failed");
            continue;
        }

        ic_stmt_print(stmt, indent_level);
    }

    /* decr indent level after body */
    -- *indent_level;
}


