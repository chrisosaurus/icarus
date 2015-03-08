#include <stdlib.h> /* calloc, realloc */
#include <stdio.h> /* puts */
#include <string.h> /* strlen, memset */

#include "lexer.h"

/* expand supplied tokens to the specified new capacity
 * makes sure the new area is memset to 0
 *
 * if 0 is passed in as *tokens then it behaves like realloc
 * in that it will allocated *tokens and return it
 *
 * returns pointer to tokens on success (which may have a different value)
 * returns 0 on error
 */
static icarus_tokens * expand_icarus_tokens(icarus_tokens *tokens, int new_cap);

icarus_tokens * lex(char *source){
    int i = 0;
    int len = 0;
    icarus_tokens *tokens = 0;

    len = strlen(source);

    /* set initial capacity to strlen of source */
    tokens = expand_icarus_tokens(tokens, len);
    if( ! tokens ){
        puts("lex icarus token expansion failed");
        return 0;
    }

    for( i=0; i<len; ++i ){
        /* FIXME */
    }

    return tokens;
}

/* expand supplied tokens to the specified new capacity
 * makes sure the new area is memset to 0
 *
 * if 0 is passed in as *tokens then it behaves like realloc
 * in that it will allocated *tokens and return it
 *
 * returns pointer to tokens on success (which may have a different value)
 * returns 0 on error
 */
static icarus_tokens * expand_icarus_tokens(icarus_tokens *tokens, int new_cap){;
    /* if null was provided we must behave like realloc
     * and correctly allocate the new icarus_tokens
     */
    if( ! tokens ){
        tokens = calloc(1, sizeof(icarus_tokens));
        if( ! tokens ){
            puts("expand_icarus_tokens: alloc failed");
            return 0;
        }
    }

    if( new_cap <= tokens->cap ){
        /* no work to be done */
        return tokens;
    }

    tokens->cap = new_cap;

    /* resize tokens char[] */
    tokens->tokens = realloc(tokens->tokens, sizeof(char) * new_cap);
    if( ! tokens->tokens ){
        puts("expand_icarus_tokens: realloc failed");
        return 0;
    }

    /* zero out new bytes */
    memset( &(tokens->tokens[tokens->len]), 0, (new_cap - tokens->len) );

    return tokens;
}


