#include <stdio.h>  /* fopen, puts */
#include <stdlib.h> /* exit */
#include <string.h> /* strcmp */

#include "../../../src/lex/data/token.h"
#include "../../../src/lex/lexer.h"

struct test_lex_tests {
    char *input;
    unsigned int n_expected;
    enum ic_token_id expected[100];
};

#define TABLE_LEN() (sizeof tests / sizeof tests[0])

static struct test_lex_tests tests[] = {
    {"-", 1, {IC_MINUS}},
    {"hello", 1, {IC_IDENTIFIER}},
    {"\"world\"", 1, {IC_LITERAL_STRING}},
    {"fn name(i::Int) end", 9, {IC_FUNC, IC_WHITESPACE, IC_IDENTIFIER, IC_LRBRACKET, IC_IDENTIFIER, IC_DOUBLECOLON, IC_IDENTIFIER, IC_RRBRACKET, IC_WHITESPACE, IC_END}}};

int main(void) {
    /* offset into table */
    unsigned int i = 0;
    /* cached table len */
    unsigned int len = TABLE_LEN();

    /* current token_list output */
    struct ic_token_list *token_list = 0;
    /* current token in token_list */
    struct ic_token *token = 0;

    /* current table input */
    char *input = 0;
    /* current table n_expected */
    unsigned int n_expected = 0;
    /* current table expected */
    enum ic_token_id *expected = 0;
    /* offset into expected */
    unsigned int expected_offset = 0;

    for (i = 0; i < len; ++i) {
        input = tests[i].input;
        n_expected = tests[i].n_expected;
        expected = tests[i].expected;

        /* lex */
        token_list = ic_lex("test_lex fake", input);
        if (!token_list) {
            printf("test_lex: call to ic_lex for i '%d' failed\n", i);
            return 1;
        }

        /* check lex output */
        for (expected_offset = 0; expected_offset < n_expected; ++expected_offset) {
            token = ic_token_list_next(token_list);
            if (!token) {
                printf("test_lex: call to ic_token_list_next for i '%d' and expected_offset '%d' failed\n", i, expected_offset);
                return 1;
            }

            if (token->id != expected[expected_offset]) {
                printf("test_lex: token didnt match expected for i '%d'\n", i);
                printf("for input '%s'\n", input);
                printf("lexed as:");
                ic_token_list_print_debug(token_list);
                printf("\nfor expected_offset '%d' I got '%d' and expected '%d'\n", expected_offset, token->id, expected[expected_offset]);
                printf("got:");
                ic_token_id_print_debug(token->id);
                printf("\nexpected:");
                ic_token_id_print_debug(expected[expected_offset]);
                puts("");
                return 1;
            }
        }
    }

    return 0;
}
