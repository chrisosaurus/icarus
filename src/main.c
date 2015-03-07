#include <stdio.h> /* puts */
#include <stdlib.h> /* exit, free */

#include "read.h" /* read_slurp */
#include "lexer.h" /* lex */

int main(int argc, char **argv){
    char *filename=0, *source=0;
    icarus_token *tokens;

    if( argc < 2 ){
        puts("No source file specified");
        exit(1);
    } else if( argc > 2 ){
        puts("Too many arguments supplied, only source file was expected");
        exit(1);
    }

    filename = argv[1];

    source = read_slurp(filename);
    if( ! source ){
        puts("slurping failed");
        exit(1);
    }

    tokens = lex(source);
    if( ! tokens ){
        puts("lexing failed");
        exit(1);
    }

    free(source);
    free(tokens);

}

