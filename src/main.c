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
    struct ic_token_list *token_list = 0;
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

    token_list = ic_lex(filename, source);
    if( ! token_list ){
        puts("lexing failed");
        exit(1);
    }

    puts("\nlexer output:");
    puts("----------------");
    ic_token_list_print(token_list);
    puts("----------------\n");

    ast = ic_parse(token_list);
    if( ! ast ){
        puts("parsing failed");
        exit(1);
    }

    puts("\nparser output:");
    puts("----------------");
    ic_ast_print(ast);
    puts("----------------\n");


    /* kludge section currently commented out as it is incomplete
     * and doesn't yet pass testing
     */
    kludge = ic_analyse(ast);
    if( ! kludge ){
        puts("analysis failed");
        exit(1);
    }

    puts("warning: main implementation pending, icarus is currently only partially functional");
    puts("analysis complete");

    /* clean up time
     * this will destroy both the kludge and the ast (via aast)
     */
    if( ! ic_kludge_destroy(kludge, 1) ){
        puts("main: ic_kludge_destroy call failed");
    }

#if 0
    /* FIXME ic_kludge_destroy will destroy ast
     * so no need to call both once the above is active
     */
    if( ! ic_ast_destroy(ast, 1) ){
        puts("main: ic_ast_destroy call failed");
    }
#endif


    free(source);

    return 0;
}

