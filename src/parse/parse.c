#include <stdio.h> /* puts, printf */
#include <string.h> /* strchr, strncmp */
#include <stdlib.h> /* free */

#include "../lex/lexer.h"
#include "data/ast.h"
#include "parse.h"

/* indent level defined as 4 spaces */
#define INDENT_LEVEL "    "

/* please note that only a small subset of ic_parse_* is implemented
 * here, many of the ic_parse_* functions are not implemented in parse.c
 * and have instead been moved into src/parse/ source files
 * based on their return types
 */

#ifndef DEBUG_PARSE
#define DEBUG_PARSE
#endif

/* an entry in the parse table showing the
 * length and token string that must be match
 * and if a match is found, the function to dispatch to
 */
static struct ic_parse_table_entry {
    unsigned int len;
    char *token;
    struct ic_decl * (*func)(struct ic_tokens *tokens, unsigned int *i);
} ic_parse_table [] = {
    /* len    token       function    */
    {  2,     "fn",       ic_parse_func_decl  },
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

    /* our eventual return value */
    struct ic_ast *ast = 0;

    /* allocate and initialise our ast */
    ast = ic_ast_new();
    if( ! ast ){
        puts("ic_parse: call to ic_ast_new failed");
        return 0;
    }

    /* possible leading tokens:
     * type
     * enum
     * union
     * function
     */

    /* step through tokens until consumed */
    for( i=0; i < tokens->len; ){
        /* find length of token */
        dist = ic_parse_token_length(tokens->tokens, i);

#ifdef DEBUG_PARSE
        printf( "considering token '%.*s' with distance '%u'\n",
                dist,
                &(tokens->tokens[i]),
                dist );
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
                && ! strncmp(
                            &(tokens->tokens[i]),
                            ic_parse_table[pt_offset].token,
                            dist )
              ){
#ifdef DEBUG_PARSE
                printf( "match found! token '%.*s' with parse_table entry '%.*s'\n",
                        dist,
                        &(tokens->tokens[i]),
                        ic_parse_table[pt_offset].len,
                        ic_parse_table[pt_offset].token );
#endif

                func = ic_parse_table[pt_offset].func;
                if( ! func ){
                    printf( "ic_parse: Error matched with '%.*s' but parse table function was null, bailing\n",
                            ic_parse_table[pt_offset].len,
                            ic_parse_table[pt_offset].token );
                    /* FIXME this is leaking the items stored in ast */
                    free(ast);
                    return 0;
                }

                /* call found function and store result to save */
                ret = func(tokens, &i);
                if( ! ret ){
                    /* presume parsing failed */
                    puts("ic_parse: error when calling parsing function");
                    /* FIXME this is leaking the items stored in ast */
                    free(ast);
                    return 0;
                }

                /* store ret in our ast */
                if( ic_ast_append(ast, ret) == -1 ){
                    puts("ic_parse: call to ic_ast_append failed");
                    /* FIXME this is leaking the items stored in ast */
                    free(ast);
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
        printf("ic_parse: unsupported string '%.*s' found, bailing\n",
                dist,
                &(tokens->tokens[i]) );

        /* FIXME this is leaking the items stored in ast */
        free(ast);
        return 0;
    }

#ifdef DEBUG_PARSE
    puts("ic_parse finished");
#endif

    return ast;
}

/* returns length of token starting at source[i]
 * returns 0 on error
 */
unsigned int ic_parse_token_length(char *source, unsigned int i){
    /* position of next space in tokens */
    char *space = 0;

    /* length of current token */
    unsigned int dist = 0;

    if( ! source ){
        return 0;
    }

    /* find next space */
    space = strchr( &(source[i]), ' ' );

    if( ! space ){
        /* character not found */
        return 0;
    }

    /* find length of token
     * which is the distance from the start to the space
     */
    dist = space - &(source[i]);

    return dist;
}

/* advance i by one token with token length dist */
void ic_parse_token_advance(unsigned int *i, unsigned int dist){
    if( ! i ){
        return;
    }

    /* dist + 1 to account for space */
    *i += (dist + 1);
}

/* check if we are looking at an `end` token
 *
 *
 * useful for continually iterating through tokens until we find `end
 *
 * this will also advance i past the token
 *
 * example:
 *
 *      int ret = 0;
 *
 *      while( (ret = ic_parse_this_is_not_the_end(tokens, i)) > 0 ){
 *          ...
 *      }
 *
 *      if( ! ret )
 *          return true; // success, end token found
 *      return false; // other error occurred in this_is_not_the_end
 *
 * returns 0 if this is an `end` token
 * returns 1 if this is NOT an `end` token
 * returns -1 if an error occurs (if \0 is hit for example)
 */
int ic_parse_this_is_not_the_end(struct ic_tokens *tokens, unsigned int *i){
    unsigned int dist = 0;

    if( ! tokens ){
        puts("ic_parse_is_this_the_end: tokens was null");
        return -1;
    }

    if( ! i ){
        puts("ic_parse_is_this_the_end: i was null");
        return -1;
    }

    /* get the distance of the token */
    dist = ic_parse_token_length(tokens->tokens, *i);

    /* if our token is 3 long and looks like end
     * then this is the end
     */
    if( dist == 3 && ! strncmp("end", &(tokens->tokens[*i]), 3) ){
        /* step over the token */
        ic_parse_token_advance(i, dist);

        /* signal the end of days */
        return 0;
    }

    /* if our dist is '0' that means there are no tokens left
     * so this also qualifies as the end/
     */
    if( ! dist ){
        return -1;
    }

    /* if we are looking at the null terminator then
     * this is also the end
     * adding an explicit check JUST in case
     */
    if( tokens->tokens[*i] == '\0' ){
        return -1;
    }

    /* and finally if we are off the end of tokens
     * then this is also the end
     */
    if( *i >= tokens->len ){
        return -1;
    }

    /* otherwise this is not the end */
    return 1;
}

/* compare and consume un-important token
 *
 * this function will take an expected token-string
 * and compare it to the current token
 *
 * if they are equal then this function will consume that token
 * and return 0
 *
 * if thy are non-equal then this function will NOT consume that token
 * and just return 1
 *
 * returns 0 if token is as expected
 * returns 1 if token is not as expected
 */
unsigned int ic_parse_check_token(char *expected, unsigned int exp_len, char *source, unsigned int *i){
    unsigned int dist = 0;

    /* get arg checking out of the way */
    if( ! expected ){
        puts("ic_parse_check_token: null expected");
        return 1;
    }

    if( ! source ){
        puts("ic_parse_check_token: null source");
        return 1;
    }

    if( ! i ){
        puts("ic_parse_check_token: null i");
        return 1;
    }

    /* get our distance */
    dist = ic_parse_token_length(source, *i);
    if( ! dist ){
        return 1;
    }

    /* compare len */
    if( dist != exp_len ){
        return 1;
    }

    /* compare string */
    if( strncmp(expected, &(source[*i]), exp_len) ){
        return 1;
    }

    /* otherwise they are equal
     * consume token
     */
    ic_parse_token_advance(i, dist);

    /* signal success */
    return 0;
}

/* check if the current token looks like a string
 * returns 1 if yes
 * returns 0 if no
 */
int ic_parse_stringish(struct ic_tokens *tokens, unsigned int *i){
    if( ! tokens ){
        puts("ic_parse_stringish: tokens was null");
        return 0;
    }
    if( ! i ){
        puts("ic_parse_stringish: i was null");
        return 0;
    }

    /* peek at first character and check
     * for quotes
     */
    switch( tokens->tokens[*i] ){
        case '"':
            return 1;
            break;

        /* FIXME not yet supporting
         * single quote (') as string delim
         *
        case '\'':
            return 1;
            break;
         */

        default:
            return 0;
            break;
    }

    return 0;
}

/* check if the current token looks like a number
 * returns 1 if yes
 * returns 0 if no
 */
int ic_parse_numberish(struct ic_tokens *tokens, unsigned int *i){
    if( ! tokens ){
        puts("ic_parse_numberish: tokens was null");
        return 0;
    }
    if( ! i ){
        puts("ic_parse_numberish: i was null");
        return 0;
    }

    /* peek at first character
     * if this is a digit then this is a number
     * otherwise it is not
     */
    switch( tokens->tokens[*i] ){
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return 1;
            break;

        default:
            return 0;
            break;
    }

    return 0;
}

/* peek at token after current one
 * returns a char* pointing to the start of the next
 * token
 *
 * returns 0 on error
 */
char * ic_parse_peek_next(struct ic_tokens *tokens, unsigned int *i){
    /* dist of current token */
    unsigned int dist = 0;
    /* offset of next token */
    unsigned int offset = 0;

    if( ! tokens ){
        puts("ic_parse_peek_next: tokens was null");
        return 0;
    }
    if( ! i ){
        puts("ic_parse_peek_next: i was null");
        return 0;
    }

    dist = ic_parse_token_length(tokens->tokens, *i);
    if( ! dist ){
        puts("ic_parse_peek_next: call to ic_parse_token_length failed");
        return 0;
    }

    /* plus 1 to get past space */
    offset = *i + dist + 1;

    /* check we are still within bounds */
    if( offset >= tokens->len ){
        puts("ic_parse_peek_next: no next token, out of tokens");
        return 0;
    }

    /* return pointer to start of next token */
    return &(tokens->tokens[offset]);
}

/* check if the current token looks like an operator
 * this take a char* and will currently only dereference it
 *
 * returns 1 for true
 * returns 0 for false
 */
int ic_parse_operatorish(char *cursor){
    if( ! cursor ){
        puts("ic_parse_operatorish: cursor was null");
        return 0;
    }

    /* check if the current character is one of the recognised
     * operators
     */
    switch( *cursor ){
        case '+':
        case '-':
        case '/':
        case '*':
            return 1;
            break;

        default:
            return 0;
            break;
    }

    return 0;
}

/* print levels worth of indent levels
 * an indent level is defined in parse.c
 */
void ic_parse_print_indent(unsigned int levels){
    unsigned int i = 0;
    for( i = 0; i<levels; ++i ){
        fputs(INDENT_LEVEL, stdout);
    }
}


