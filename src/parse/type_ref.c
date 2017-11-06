
#include <stdio.h>  /* puts, printf */
#include <string.h> /* strncmp */

#include "data/type_ref.h"
#include "parse.h"

unsigned int ic_parse_type_ref_embedded(struct ic_type_ref *type_ref, struct ic_token_list *token_list) {
    struct ic_token *token = 0;
    char *token_str = 0;
    unsigned int token_len = 0;
    struct ic_type_ref *type_arg = 0;

    if (!type_ref) {
        puts("ic_parse_type_ref_embedded: token_list was null");
        return 0;
    }

    if (!token_list) {
        puts("ic_parse_type_ref_embedded: token_list was null");
        return 0;
    }

    token = ic_token_list_expect_important(token_list, IC_IDENTIFIER);
    if (!token) {
        puts("ic_parse_type_ref_embedded: no type found");
        return 0;
    }

    token_str = ic_token_get_string(token);
    token_len = ic_token_get_string_length(token);
    if (!token_str || !token_len) {
        puts("ic_parse_type_ref_embedded: failed to get string from token");
        return 0;
        ;
    }

    if (!ic_type_ref_symbol_init(type_ref, token_str, token_len)) {
        puts("ic_parse_type_ref_embedded: call to ic_type_ref_symbol_init failed");
        return 0;
    }

    /* if we see a [ then we have to recurse */
    token = ic_token_list_peek_important(token_list);
    if (!token) {
        puts("ic_parse_type_ref_embedded: call to ic_token_list_peek_important failed");
        return 0;
    }

    /* recurse time */
    if (token->id == IC_LSBRACKET) {
        token = ic_token_list_expect_important(token_list, IC_LSBRACKET);
        if (!token) {
            puts("ic_parse_type_ref_embedded: lost [");
            return 0;
        }

        while ((token = ic_token_list_peek_important(token_list))) {
            /* if closing right square bracket then stop */
            if (token->id == IC_RSBRACKET) {
                break;
            }

            /* if comma then skip
           * FIXME: this makes commas optional
           */
            if (token->id == IC_COMMA) {
                token = ic_token_list_next_important(token_list);
                if (!token) {
                    puts("ic_parse_type_ref_embedded: call to ic_token_list_next_important failed when trying to consume comma");
                    return 0;
                }
                /* skip */
                continue;
            }

            type_arg = ic_parse_type_ref(token_list);
            if (!type_arg) {
                puts("ic_parse_type_ref_embedded: call to ic_parse_type_ref failed");
                return 0;
            }

            if (!ic_type_ref_type_args_add(type_ref, type_arg)) {
                puts("ic_parse_type_ref_embedded: call to ic_type_ref_type_params_add failed");
                return 0;
            }
        }

        token = ic_token_list_expect_important(token_list, IC_RSBRACKET);
        if (!token) {
            puts("ic_parse_type_ref_embedded: couldn't find matching ]");
            return 0;
        }
    }

    return 1;
}

struct ic_type_ref *ic_parse_type_ref(struct ic_token_list *token_list) {
    struct ic_type_ref *tref = 0;

    tref = ic_type_ref_new();
    if (!tref) {
        puts("ic_parse_type_ref: call to ic_type_ref_new failed");
        return 0;
    }

    if (!ic_parse_type_ref_embedded(tref, token_list)) {
        puts("ic_parse_type_ref: call to ic_parse_type_ref_embedded failed");
        return 0;
    }

    return tref;
}
