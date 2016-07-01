#include <stdio.h>  /* puts, printf */
#include <stdlib.h> /* exit, free */
#include <string.h>

#include "../../src/lex/lexer.h"      /* ic_lex */
#include "../../src/parse/data/ast.h" /* ic_ast structure */
#include "../../src/parse/parse.h"    /* ic_parse */
#include "../../src/read/read.h"      /* ic_read_slurp */

int main(void) {
    char *filename = 0, *source = 0;
    struct ic_token_list *token_list = 0;
    struct ic_ast *ast = 0;

    filename = "example/simple.ic";

    source = ic_read_slurp(filename);
    if (!source) {
        puts("test_parse_example: slurping failed");
        exit(1);
    }

    token_list = ic_lex(filename, source);
    if (!token_list) {
        puts("test_parse_example: lexing failed");
        exit(1);
    }

    ast = ic_parse(token_list);
    if (!ast) {
        puts("test_parse_example: parsing failed");
        exit(1);
    }

    puts("\nparser output:");
    puts("----------------");
    ic_ast_print(ast);
    puts("----------------\n");

    if (!ic_ast_destroy(ast, 1)) {
        puts("test_parse_example: ic_ast_destroy failed");
    }

    if (!ic_token_list_destroy(token_list, 1)) {
        puts("test_parse_example: ic_token_list_destroy failed");
    }

    free(source);

    return 0;
}
