#include <stdio.h> /* puts */
#include <stdlib.h>

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
            goto ERROR;
        }

        /* store this stmt in the body */
        if( -1 == ic_body_append(body, stmt) ){
            puts("ic_parse_body: call to ic_body_append failed");
            goto ERROR;
        }
    }

    /* consume end */
    token = ic_token_list_expect_important(token_list, IC_END);
    if( ! token ){
        puts("ic_parse_body: unable to find end token");
        goto ERROR;
    }

    /* if success is 1 then we hit an end token */
    if( success ){
        /* victory */
        return body;
    }

    puts("ic_parse_body: unknown error");

ERROR:
    /* FIXME free here probably isn't enough... */
    free(body);

    return 0;
}


