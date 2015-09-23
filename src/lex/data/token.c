#include <stdlib.h> /* calloc, free */
#include <stdio.h> /* puts */

#include "token.h"

/* alloc and init a new token
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_token * ic_token_new(void){
    struct ic_token *token = 0;

    token = calloc(1, sizeof(struct ic_token));
    if( ! token ){
        puts("ic_token_new: call to calloc failed");
        return 0;
    }

    if( ! ic_token_init(token) ){
        puts("ic_token_new: call to ic_token_init failed");
        return 0;
    }

    return token;
}

/* init an existing token
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_token_init(struct ic_token *token){
    if( ! token ){
        puts("ic_token_init: token was null");
        return 0;
    }

    return 1;
}

/* destroy
 *
 * will only free token if free_token is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_token_destroy(struct ic_token *token, unsigned int free_token){
    if( ! token ){
        puts("ic_token_destroy: token was null");
        return 0;
    }

    if( free_token ){
        free(token);
    }

    return 1;
}

