#include <stdio.h> /* puts */

#include "../body.h"
#include "../parse.h"

/* ignore unused parameter warnings */
#pragma GCC diagnostic ignored "-Wunused-parameter"

struct ic_body * ic_parse_body(struct ic_tokens *tokens, unsigned int *i){
    /* the body we eventually return */
    struct ic_body *body = 0;
    /* each intermediate statement that we add to the body */
    struct ic_stmt * stmt = 0;

    if( ! tokens ){
        puts("ic_parse_body: tokens was null");
        return 0;
    }

    if( ! i ){
        puts("ic_parse_body: i was null");
        return 0;
    }

    /* initialise our body */
    if( ic_body_init(body) ){
        puts("ic_parse_body: call to ic_body_init failed");
        return 0;
    }

    /* keep going until we see an unexpected end */
    while( ic_parse_this_is_not_the_end(tokens, i) ){
        /* parse a single statement */
        stmt = ic_parse_stmt(tokens, i);
        if( ! stmt ){
            puts("ic_parse_body: call to ic_parse_stmt failed");
            return 0;
        }

        /* store this stmt in the body */
        if( ic_body_append(body, stmt) == -1 ){
            puts("ic_parse_body: call to ic_body_append failed");
            return 0;
        }
    }

    return body;
}


