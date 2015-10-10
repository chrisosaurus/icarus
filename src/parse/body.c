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
struct ic_body * ic_parse_body(struct ic_token_list *token_list){
    /* the body we eventually return */
    struct ic_body *body = 0;
    /* each intermediate statement that we add to the body */
    struct ic_stmt * stmt = 0;
    /* used to mark success for while loop */
    int success = 0;
    /* current token */
    struct ic_token *token = 0;

    if( ! token_list ){
        puts("ic_parse_body: toke_list  was null");
        return 0;
    }

    /* create our body */
    body = ic_body_new();
    if( ! body ){
        puts("ic_parse_body: call to ic_body_new failed");
        return 0;
    }

    /* keep going until we see an unexpected end */
    while( (token = ic_token_list_peek_important(token_list)) ){
        if( token->id == IC_END ){
            success = 1;
            break;
        }

        /* parse a single statement */
        stmt = ic_parse_stmt(token_list);
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
    /* consume end */
    token = ic_token_list_expect_important(token_list, IC_END);
    if( ! token ){
        puts("ic_parse_body: unable to find end token");
        return 0;
    }

    /* if success is 1 then we hit an end token */
    if( success ){
            /* victory */
        return body;
    }

    /* otherwise another error occurred */
    puts("ic_parse_body: call to ic_parse_this_is_not_the_end encountered an error");
    return 0;
}


