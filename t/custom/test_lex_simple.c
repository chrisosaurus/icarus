#include <stdio.h>  /* puts, printf */
#include <stdlib.h> /* exit, free */
#include <string.h>

#include "../../src/lex/lexer.h" /* ic_lex */
#include "../../src/read/read.h" /* ic_read_slurp */

int main(void) {
    char *filename = 0, *source = 0;
    struct ic_token_list *token_list = 0;

    filename = "example/simple.ic";

    source = ic_read_slurp(filename);
    if (!source) {
        puts("text_lex_example: slurping failed");
        exit(1);
    }

    token_list = ic_lex(filename, source);
    if (!token_list) {
        puts("text_lex_example: lexing failed");
        exit(1);
    }

    puts("\nlexer output:");
    puts("----------------");
    ic_token_list_print(token_list);
    puts("----------------\n");

    /* FIXME may be leaking tokens */
    if (!ic_token_list_destroy(token_list, 1)) {
        puts("text_lex_example: ic_token_list_destroy failed");
    }

    free(source);

    return 0;
}
