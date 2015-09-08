#include <stdio.h> /* puts, printf */
#include <stdlib.h> /* exit, free */
#include <string.h>

#include "read/read.h" /* ic_read_slurp */
#include "lex/lexer.h" /* ic_lex */
#include "parse/parse.h" /* ic_parse */
#include "parse/data/ast.h" /* ic_ast structure */
#include "analyse/analyse.h" /* ic_kludge */

int main(int argc, char **argv){
    char *filename = 0, *source = 0;
    struct ic_tokens *tokens = 0;
    struct ic_ast *ast = 0;
    struct ic_kludge *kludge = 0;

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

    puts("\nlexer output:");
    puts("----------------");
    printf("%s\n", tokens->tokens);
    puts("----------------\n");


    ast = ic_parse(tokens);
    if( ! ast ){
        puts("parsing failed");
        exit(1);
    }

    puts("\nparser output:");
    puts("----------------");
    ic_ast_print(ast);
    puts("----------------\n");


#if 0
    /* kludge section currently commented out as it is incomplete
     * and doesn't yet pass testing
     */
    kludge = ic_analyse(ast);
    if( ! kludge ){
        puts("analysis failed");
        exit(1);
    }


    /* clean up time */
    if( ! ic_kludge_destroy(kludge, 1) ){
        puts("main: ic_kludge_destroy call failed");
    }
#endif

    /* FIXME ic_kludge_destroy will destroy ast
     * so no need to call both once the above is active
     */
    if( ! ic_ast_destroy(ast, 1) ){
        puts("main: ic_ast_destroy call failed");
    }

    free(source);
    free(tokens->tokens);
    free(tokens);

    return 0;
}

