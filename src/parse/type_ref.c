
#include <stdio.h>  /* puts, printf */
#include <string.h> /* strncmp */

#include "data/type_ref.h"
#include "parse.h"

struct ic_type_ref *ic_parse_type_ref(struct ic_token_list *token_list) {
    struct ic_type_ref *tref = 0;
    struct ic_token *token = 0;
    char *token_str = 0;
    unsigned int token_len = 0;

    if (!token_list) {
        puts("ic_parse_type_ref: token_list was null");
        return 0;
    }

    token = ic_token_list_expect_important(token_list, IC_IDENTIFIER);
    if (!token) {
        puts("ic_parse_type_ref: no type found");
        return 0;
    }

    token_str = ic_token_get_string(token);
    token_len = ic_token_get_string_length(token);
    if (!token_str || !token_len) {
        puts("ic_parse_type_ref: failed to get string from token");
        return 0;
        ;
    }

    tref = ic_type_ref_symbol_new(token_str, token_len);
    if (!tref) {
        puts("ic_parse_type_ref: call to ic_type_ref_symbol_new failed");
        return 0;
        ;
    }

    return tref;
}
