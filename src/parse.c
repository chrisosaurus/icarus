#include <stdio.h> /* puts, printf */
#include <string.h> /* strchr, strncmp */

#include "lexer.h"
#include "types.h"
#include "ast.h"
#include "parse.h"

#define DEBUG_PARSE

#define LENGTH(x) (sizeof(x) / sizeof(x[0]))

/* ignore unused parameter warnings */
#pragma GCC diagnostic ignored "-Wunused-parameter"

struct ic_decl * ic_parse_type_decl(struct ic_tokens *tokens, unsigned int *i){
    /* FIXME */
    return 0;
}

struct ic_decl * ic_parse_enum_decl(struct ic_tokens *tokens, unsigned int *i){
    /* FIXME */
    return 0;
}

struct ic_decl * ic_parse_union_decl(struct ic_tokens *tokens, unsigned int *i){
    /* FIXME */
    return 0;
}

struct ic_decl * ic_parse_func_decl(struct ic_tokens *tokens, unsigned int *i){
    /* FIXME */
    return 0;
}

/* an entry in the parse table showing the
 * length and token string that must be match
 * and if a match is found, the function to dispatch to
 */
struct ic_parse_table_entry {
    unsigned int len;
    char *token;
    struct ic_decl * (*func)(struct ic_tokens *tokens, unsigned int *i);
} ic_parse_table [] = {
    /* len    token       function    */
    {  4,     "type",     ic_parse_type_decl  },
    {  4,     "enum",     ic_parse_enum_decl  },
    {  5,     "union",    ic_parse_union_decl },
    {  8,     "function", ic_parse_func_decl  }
};

struct ic_ast * ic_parse(struct ic_tokens *tokens){
    /* offset into tokens */
    unsigned int i = 0;
    /* position of next space in tokens */
    char *space = 0;
    /* length of current token */
    unsigned int dist = 0;

    /* offset into ic_parse_table */
    unsigned int pt_offset = 0;

    /* function to dispatch to */
    struct ic_decl * (*func)(struct ic_tokens *tokens, unsigned int *i) = 0;

    /* possible leading tokens:
     * type
     * enum
     * union
     * function
     * #
     */

    /* step through tokens until consumed */
    for( i=0; i < tokens->len; ){
        /* find next space in string */
        space = strchr( &(tokens->tokens[i]), ' ' );
        /* find length of token
         * which is the distance from the start to the space
         */
        dist = space - &(tokens->tokens[i]);

#ifdef DEBUG_PARSE
        printf( "considering token '%.*s' with distance '%u'\n", dist, &(tokens->tokens[i]), dist );
#endif

        for( pt_offset=0; pt_offset < LENGTH(ic_parse_table); ++pt_offset ){
#ifdef DEBUG_PARSE
            printf( "comparing token token '%.*s' (%u) with parse_table entry '%.*s' (%u)\n",
                    dist,
                    &(tokens->tokens[i]),
                    dist,
                    ic_parse_table[pt_offset].len,
                    ic_parse_table[pt_offset].token,
                    ic_parse_table[pt_offset].len
                  );
#endif

            if( dist == ic_parse_table[pt_offset].len
                && ! strncmp( &(tokens->tokens[i]), ic_parse_table[pt_offset].token, dist )
              ){
#ifdef DEBUG_PARSE
                printf( "match found! token '%.*s' with parse_table entry '%.*s'\n", dist, &(tokens->tokens[i]), ic_parse_table[pt_offset].len, ic_parse_table[pt_offset].token );
#endif

                func = ic_parse_table[pt_offset].func;
                if( ! func ){
                    printf( "ic_parse: Error matched with '%.*s' but parse table function was null, bailing\n", ic_parse_table[pt_offset].len, ic_parse_table[pt_offset].token );
                    return 0;
                }

                /* FIXME call function
                 * figure out what to do with result
                 * do that
                 */
                return 0;
            }

            /* FIXME
             * eventually we can rely on the parse entires being in order and bail out early
             * for now we will just keep going
             */
        }

        /* no match found */
        printf("ic_parse: unsupported string '%.*s' found, bailing\n", dist, &(tokens->tokens[i]) );
        return 0;
    }

    /* FIXME */
    puts("ic_parse finished, bailing");
    return 0;
}

