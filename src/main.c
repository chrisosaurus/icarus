#include <stdio.h> /* puts */
#include <stdlib.h> /* exit */

#include "lexer.h"

int main(int argc, char **argv){
    char *filename;

    if( argc < 2 ){
        puts("No source file specified");
        exit(1);
    } else if( argc > 2 ){
        puts("Too many arguments supplied, only source file was expected");
        exit(1);
    }

    filename = argv[1];

    lex(filename);
}

