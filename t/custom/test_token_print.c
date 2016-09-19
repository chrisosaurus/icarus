#include <stdio.h>  /* puts, printf */
#include <stdlib.h> /* exit, free */
#include <string.h>

#include "../../src/lex/data/token.h" /* ic_token */
#include "../../src/lex/lexer.h"      /* ic_lex */
#include "../../src/read/read.h"      /* ic_read_slurp */

int main(void) {
    /* token iterator */
    unsigned int i = 0;

    /* token */
    struct ic_token *token = 0;

    /* char to pass to ic_token_new to keep happy */
    char *ch = "hello";

    token = ic_token_new(0, ch, 0, ch, 0);
    if (!token) {
        puts("test_token_print: token construction failed");
        exit(1);
    }

    puts("print testing");
    puts("---------");
    /* iterate through all tokens that do not store data */
    for (i = 0; i < IC_TOKEN_LEN; ++i) {
        printf("%d : ", i);
        token->id = i;

        /* make sure to attach payloads before printing */
        switch (i) {
            case IC_IDENTIFIER:
            case IC_LITERAL_STRING:
            case IC_COMMENT:
                if (!ic_token_set_string(token, ch, 5)) {
                    puts("test_token_print: failed to set string");
                    exit(1);
                }
                break;
            case IC_LITERAL_INTEGER:
                if (!ic_token_set_integer(token, 14)) {
                    puts("test_token_print: failed to set integer");
                    exit(1);
                }
                break;
        }

        ic_token_print(stdout, token);
        puts("");
    }
    puts("---------");

    puts("\nprint debug testing");
    puts("---------");
    /* iterate through all token
     * do NOT iterate over marker IC_TOKEN_LEN
     */
    for (i = 0; i < IC_TOKEN_LEN; ++i) {
        printf("%d : ", i);
        token->id = i;
        ic_token_print_debug(stdout, token);
        puts("");
    }
    puts("---------");

    if (!ic_token_destroy(token, 1)) {
        puts("test_token_print: token destruction failed");
        exit(1);
    }

    return 0;
}
