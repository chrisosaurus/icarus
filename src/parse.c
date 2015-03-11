#include <stdio.h> /* puts, printf */
#include <string.h> /* strchr, strncmp */

#include "lexer.h"
#include "types.h"
#include "ast.h"
#include "parse.h"

#ifndef DEBUG_PARSE
#define DEBUG_PARSE
#endif

#define LENGTH(x) (sizeof(x) / sizeof(x[0]))

/* ignore unused parameter warnings */
#pragma GCC diagnostic ignored "-Wunused-parameter"

/* return length of token starting at source[i] */
static unsigned int ic_parse_token_len(char *source, unsigned int i);

/* advance i by one token with token length dist */
static void ic_parse_token_advance(unsigned int *i, unsigned int dist);

struct ic_decl * ic_parse_type_decl(struct ic_tokens *tokens, unsigned int *i){
    /* use j as internal iterator to find the end token */
    unsigned int j = 0;
    unsigned int dist = 0;

    if( ! tokens ){
        return 0;
    }

#ifdef DEBUG_PARSE
    puts("ic_parse_type_decl called");
#endif

    for( j=*i; tokens->tokens[j] != '\0' && j < tokens->len ; ){
        dist = ic_parse_token_len(tokens->tokens, j);
        printf("ic_parse_token_type_decl: inspecting token '%.*s'\n", dist, &(tokens->tokens[j]) );

        if( dist == 3 &&
            ! strncmp( &(tokens->tokens[j]), "end", 3) ){
            printf("ic_parse_token_type_decl: found end of string token '%.*s'\n", dist, &(tokens->tokens[j]) );

            ic_parse_token_advance(&j, dist);
            *i = j;
            return 0;
        }

        ic_parse_token_advance(&j, dist);
    }

    /* this means we ran out of tokens */
    return 0;
}

struct ic_decl * ic_parse_enum_decl(struct ic_tokens *tokens, unsigned int *i){
#ifdef DEBUG_PARSE
    puts("ic_parse_enum_decl called");
#endif

    /* FIXME */
    return 0;
}

struct ic_decl * ic_parse_union_decl(struct ic_tokens *tokens, unsigned int *i){
#ifdef DEBUG_PARSE
    puts("ic_parse_union_decl called");
#endif

    /* FIXME */
    return 0;
}

struct ic_decl * ic_parse_func_decl(struct ic_tokens *tokens, unsigned int *i){
#ifdef DEBUG_PARSE
    puts("ic_parse_func_decl called");
#endif

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
    /* length of current token */
    unsigned int dist = 0;

    /* offset into ic_parse_table */
    unsigned int pt_offset = 0;

    /* function to dispatch to */
    struct ic_decl * (*func)(struct ic_tokens *tokens, unsigned int *i) = 0;

    /* return from call to func */
    struct ic_decl *ret = 0;

    /* possible leading tokens:
     * type
     * enum
     * union
     * function
     * #
     */

    /* step through tokens until consumed */
    for( i=0; i < tokens->len; ){
        /* find length of token */
        dist = ic_parse_token_len(tokens->tokens, i);

#ifdef DEBUG_PARSE
        printf( "considering token '%.*s' with distance '%u'\n", dist, &(tokens->tokens[i]), dist );
#endif

        /* we clear func each time so at loop exit we can
         * check if func is set, and if so we know we made a match
         * if func is still 0 at end of loop we know we did not
         * match the token and should signal an error
         */
        func = 0;

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
                ret = func(tokens, &i);
                if( ! ret ){
                    /* presume parsing failed */
                    puts("ic_parse: error when calling parsing function");
                    return 0;
                }
                /* exit inner for loop as this token is done */
                break;
            }

            /* FIXME
             * eventually we can rely on the parse entires being in order and bail out early
             * for now we will just keep going
             */
        }

        /* if func is set then it means we had a match */
        if( func ){
            continue;
        }

        /* no match found */
        printf("ic_parse: unsupported string '%.*s' found, bailing\n", dist, &(tokens->tokens[i]) );
        return 0;
    }

    /* FIXME */
    puts("ic_parse finished, bailing");
    return 0;
}

/* return length of token starting at source[i] */
static unsigned int ic_parse_token_len(char *source, unsigned int i){
    /* position of next space in tokens */
    char *space = 0;

    /* length of current token */
    unsigned int dist = 0;

    if( ! source ){
        return 0;
    }

    /* find next space */
    space = strchr( &(source[i]), ' ' );

    /* find length of token
     * which is the distance from the start to the space
     */
    dist = space - &(source[i]);

    return dist;
}

/* advance i by one token with token length dist */
static void ic_parse_token_advance(unsigned int *i, unsigned int dist){
    if( ! i ){
        return;
    }

    /* dist + 1 to account for space */
    *i += (dist + 1);
}


