#include <stdio.h> /* puts, printf */
#include <stdlib.h> /* exit, free */
#include <string.h>

#include "read.h" /* read_slurp */
#include "lexer.h" /* lex */

int main(int argc, char **argv){
    char *filename=0, *source=0;
    struct ic_tokens *tokens;

    if( argc < 2 ){
        puts("No source file specified");
        exit(1);
    } else if( argc > 2 ){
        puts("Too many arguments supplied, only source file was expected");
        exit(1);
    }

    filename = argv[1];

    source = ic_read_slurp(filename);
    if( ! source ){
        puts("slurping failed");
        exit(1);
    }

    tokens = ic_lex(source);
    if( ! tokens ){
        puts("lexing failed");
        exit(1);
    }

#ifdef DEBUG_LEXER
    printf("len of str '%d', len of tokens '%d', cap of tokens '%d'\n", strlen(source), tokens->len, tokens->cap);
#endif

    printf("lexer output:\n%s\n", tokens->tokens);

    free(source);
    free(tokens->tokens);
    free(tokens);

}

