#include <stdlib.h> /* calloc, free */
#include <stdio.h> /* puts */

#include "token.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"

/* alloc and init a new token
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_token * ic_token_new(enum ic_token_id id, char *line, unsigned int offset, char *file, unsigned int line_num){
    struct ic_token *token = 0;

    token = calloc(1, sizeof(struct ic_token));
    if( ! token ){
        puts("ic_token_new: call to calloc failed");
        return 0;
    }

    if( ! ic_token_init(token, id, line, offset, file, line_num) ){
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
unsigned int ic_token_init(struct ic_token *token, enum ic_token_id id, char *line, unsigned int offset, char *file, unsigned int line_num){
    if( ! token ){
        puts("ic_token_init: token was null");
        return 0;
    }

    if( ! line ){
        puts("ic_token_init: line was null");
        return 0;
    }

    if( ! file ){
        puts("ic_token_init: file was null");
        return 0;
    }

    token->id = id;
    token->line = line;
    token->offset = offset;
    token->file = file;
    token->line_num = line_num;

    return 1;
}

/* set string data on token
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_token_set_string(struct ic_token *token, char *string, unsigned int len){
    if( ! token ){
        puts("ic_token_set_string: token was null");
        return 0;
    }

    /* FIXME consider checking if token->id allows for a string */
    token->u.str.string = string;
    token->u.str.len = len;
    return 1;
}

/* get string data on token
 *
 * returns * on success
 * returns 0 on failure
 */
char * ic_token_get_string(struct ic_token *token){
    if( ! token ){
        puts("ic_token_get_string_length: token was null");
        return 0;
    }

    /* FIXME consider checking if token->id allows for a string */
    return token->u.str.string;
}

/* get string length on token
 *
 * returns length on success
 * returns 0 on failure
 */
unsigned int ic_token_get_string_length(struct ic_token *token){
    if( ! token ){
        puts("ic_token_get_string_length: token was null");
        return 0;
    }

    /* FIXME consider checking if token->id allows for a string */
    return token->u.str.len;
}

/* set integer data on token
 *
 * returns 1 on success
 * returns 0 on failure
 */
int ic_token_set_integer(struct ic_token *token, int integer){
    if( ! token ){
        puts("ic_token_set_integer: token was null");
        return 0;
    }

    /* FIXME consider checking if token->id allows for an integer */
    token->u.integer = integer;
    return 1;
}

/* get integer data on token
 *
 * returns * on success
 * returns 0 on failure
 */
int ic_token_get_integer(struct ic_token *token){
    if( ! token ){
        puts("ic_token_get_integer: token was null");
        return 0;
    }

    /* FIXME consider checking if token->id allows for an integer */
    return token->u.integer;
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

