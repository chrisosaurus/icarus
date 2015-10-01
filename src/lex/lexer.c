#include <stdio.h>

#include "data/token.h"
#include "data/token_list.h"
#include "table.h"
#include "lexer.h"

/* takes a character array of the source program
 *
 * returns a token_list on success
 * returns 0 on failure
 */
struct ic_token_list * ic_lex(char *source){
    struct ic_token_list *tokens = 0;

    if( ! source ){
        puts("ic_lex: source was null");
        return 0;
    }

    tokens = ic_token_list_new();
    if( ! tokens ){
        puts("ic_lex: call to ic_token_list_new failed");
        return 0;
    }

    puts("ic_lex: unimplemented");
    return 0;
}

