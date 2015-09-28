#include <stdio.h> /* puts */

#include "data/body.h"
#include "parse.h"

/* ignore unused parameter warnings */
#pragma GCC diagnostic ignored "-Wunused-parameter"

/* parse a body
 * a body ends when an unmatched `end` is found, this will be consumed
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_body * ic_parse_body(struct ic_old_tokens *tokens, unsigned int *i){
    /* the body we eventually return */
    struct ic_body *body = 0;
    /* each intermediate statement that we add to the body */
    struct ic_stmt * stmt = 0;
    /* used for return code of ic_parse_this_is_not_the_end */
    int ret = 0;

    if( ! tokens ){
        puts("ic_parse_body: tokens was null");
        return 0;
    }

    if( ! i ){
        puts("ic_parse_body: i was null");
        return 0;
    }

    /* create our body */
    body = ic_body_new();
    if( ! body ){
        puts("ic_parse_body: call to ic_body_new failed");
        return 0;
    }

    /* keep going until we see an unexpected end */
    while( (ret = ic_parse_this_is_not_the_end(tokens, i)) > 0 ){
        /* parse a single statement */
        stmt = ic_parse_stmt(tokens, i);
        if( ! stmt ){
            puts("ic_parse_body: call to ic_parse_stmt failed");
            return 0;
        }

        /* store this stmt in the body */
        if( -1 == ic_body_append(body, stmt) ){
            puts("ic_parse_body: call to ic_body_append failed");
            return 0;
        }
    }

    /* if ret is 0 then we hit an end token */
    if( ! ret ){
        /* victory */
        return body;
    }

    /* otherwise another error occurred */
    puts("ic_parse_body: call to ic_parse_this_is_not_the_end encountered an error");
    return 0;
}


