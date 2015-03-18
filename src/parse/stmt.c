#include <stdio.h> /* puts */
#include <string.h> /* strncmp */

#include "../ast.h"
#include "../parse.h"
#include "../statement.h"
#include "../lexer.h"

/* enable by default for now */
#ifndef DEBUG_PARSE_STMT
#define DEBUG_PARSE_STMT
#endif

/* ignore unused parameter warnings */
#pragma GCC diagnostic ignored "-Wunused-parameter"

/* ignore unused functions */
#pragma GCC diagnostic ignored "-Wunused-function"


/* there are currently 3 types of statements
 *  let
 *  if
 *  expr (void context)
 */


/* consume token
 * returns ic_stmt* on success
 * returns 0 on failure
 */
static struct ic_stmt * ic_parse_stmt_let(struct ic_tokens *tokens, unsigned int *i){
    puts("ic_parse_stmt_let: unimplemented");
    return 0;
}

/* consume token
 * returns ic_stmt* on success
 * returns 0 on failure
 */
static struct ic_stmt * ic_parse_stmt_if(struct ic_tokens *tokens, unsigned int *i){
    puts("ic_parse_stmt_if: unimplemented");
    return 0;
}

/* consume token
 * returns ic_stmt* on success
 * returns 0 on failure
 */
static struct ic_stmt * ic_parse_stmt_expr(struct ic_tokens *tokens, unsigned int *i){
    puts("ic_parse_stmt_expr: unimplemented");
    return 0;
}

/* an entry in the parse table showing the
 * length and token string that must be match
 * and if a match is found, the function to dispatch to
 */
static struct ic_parse_table_entry {
    unsigned int len;
    char *token;
    struct ic_stmt * (*func)(struct ic_tokens *tokens, unsigned int *i);
} ic_parse_table [] = {
    /* len    token       function    */
    {  2,     "if",       ic_parse_stmt_if  },
    {  3,     "let",      ic_parse_stmt_let }
    /* otherwise we default to ic_parse_stmt_expr */
};

struct ic_stmt * ic_parse_stmt(struct ic_tokens *tokens, unsigned int *i){
    /* length of current token */
    unsigned int dist = 0;

    /* offset into ic_parse_table */
    unsigned int pt_offset = 0;

    /* function to dispatch to */
    struct ic_stmt * (*func)(struct ic_tokens *tokens, unsigned int *i) = 0;

    /* return from call to func */
    struct ic_stmt *ret = 0;

#ifdef DEBUG_PARSE_STMT
    puts("ic_parse_stmt: entered");
#endif

    /* find length of token */
    dist = ic_parse_token_length(tokens->tokens, *i);

    for( pt_offset=0; pt_offset < LENGTH(ic_parse_table); ++pt_offset ){

#ifdef DEBUG_PARSE_STMT
            printf( "ic_parse_stmt: comparing token token '%.*s' (%u) with parse_table entry '%.*s' (%u)\n",
                    dist,
                    &(tokens->tokens[*i]),
                    dist,
                    ic_parse_table[pt_offset].len,
                    ic_parse_table[pt_offset].token,
                    ic_parse_table[pt_offset].len
                  );
#endif

        if( dist == ic_parse_table[pt_offset].len
            && ! strncmp(
                        &(tokens->tokens[*i]),
                        ic_parse_table[pt_offset].token,
                        dist )
          ){
#ifdef DEBUG_PARSE_STMT

                printf( "ic_parse_stmt: match found! token '%.*s' with parse_table entry '%.*s'\n",
                        dist,
                        &(tokens->tokens[*i]),
                        ic_parse_table[pt_offset].len,
                        ic_parse_table[pt_offset].token );
#endif

            func = ic_parse_table[pt_offset].func;
            if( ! func ){
                printf( "ic_parse: Error matched with '%.*s' but parse table function was null, bailing\n",
                        ic_parse_table[pt_offset].len,
                        ic_parse_table[pt_offset].token );
                return 0;
            }
        }
    }

    /* at this point if we did not find a match then we
     * default to trying to parse it as an expr
     */
    if( ! func ){
        func = ic_parse_stmt_expr;
    }

    /* out return value */
    ret = func(tokens, i);

    if( ! ret ){
        puts("ic_parse_stmt: call to matched function failed");
        return 0;
    }

#ifdef DEBUG_PARSE_STMT
    puts("ic_parse_stmt: success! returning result");
#endif

    return ret;
}
