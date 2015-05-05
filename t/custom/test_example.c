#include <stdio.h> /* puts, printf */
#include <stdlib.h> /* exit, free */
#include <string.h>

#include "../../src/read/read.h" /* ic_read_slurp */
#include "../../src/lex/lexer.h" /* ic_lex */
#include "../../src/parse/parse.h" /* ic_parse */
#include "../../src/parse/data/ast.h" /* ic_ast structure */

int main(int argc, char **argv){
    char *filename = 0, *source = 0;
    struct ic_tokens *tokens = 0;
    struct ic_ast *ast = 0;

    filename = "example/simple.ic";

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

    ast = ic_parse(tokens);
    if( ! ast ){
        puts("parsing failed");
        exit(1);
    }

    puts("\nparser output:");
    puts("----------------");
    ic_ast_print(ast);
    puts("----------------\n");

    if( ic_ast_destroy(ast, 1) ){
        puts("text_example: ic_ast_destroy failed");
    }

    free(source);
    free(tokens->tokens);
    free(tokens);

    return 0;
}

