#include <stdio.h>

#include "lexer.h"

/* takes a character array of the source program
 *
 * returns a token_list on success
 * returns 0 on failure
 */
struct ic_token_list* ic_lex(char *source){
    if( ! source ){
        puts("ic_lex: source was null");
        return 0;
    }

    puts("ic_lex: unimplemented");
    return 0;
}

