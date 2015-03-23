#include <stdio.h> /* puts */

#include "../parse.h"

/* ignore unused parameter warnings */
#pragma GCC diagnostic ignored "-Wunused-parameter"

/* ignore unused functions */
#pragma GCC diagnostic ignored "-Wunused-function"

/* ignore unused functions */
#pragma GCC diagnostic ignored "-Wunused-variable"

/* current supported expression types:
 *  func call
 *  identifier
 *  constant
 *  operator application
 */


/* consume token
 * returns ic_expr* on success
 * returns 0 on failure
 */
static struct ic_expr * ic_parse_expr_fcall(struct ic_tokens *tokens, unsigned int *i){
    struct ic_expr * expr = 0;

    if( ! tokens ){
        puts("ic_parse_expr_fcall: tokens was null");
        return 0;
    }
    if( ! i ){
        puts("ic_parse_expr_fcall: i was null");
        return 0;
    }

    puts("ic_parse_expr_fcall: unimplemented");
    return 0;
}

/* consume token
 * returns ic_expr* on success
 * returns 0 on failure
 */
static struct ic_expr * ic_parse_expr_identifier(struct ic_tokens *tokens, unsigned int *i){
    struct ic_expr * expr = 0;

    if( ! tokens ){
        puts("ic_parse_expr_identifier: tokens was null");
        return 0;
    }
    if( ! i ){
        puts("ic_parse_expr_identifier: i was null");
        return 0;
    }

    puts("ic_parse_expr_identifier: unimplemented");
    return 0;
}

/* consume token and make a string
 * returns ic_expr* on success
 * returns 0 on failure
 */
static struct ic_expr * ic_parse_expr_constant_string(struct ic_tokens *tokens, unsigned int *i){
    struct ic_expr * expr = 0;

    if( ! tokens ){
        puts("ic_parse_expr_constant_string: tokens was null");
        return 0;
    }
    if( ! i ){
        puts("ic_parse_expr_constant_string: i was null");
        return 0;
    }

    puts("ic_parse_expr_constant_string: unimplemented");
    return 0;
}

/* consume token and make an int
 * returns ic_expr* on success
 * returns 0 on failure
 */
static struct ic_expr * ic_parse_expr_constant_integer(struct ic_tokens *tokens, unsigned int *i){
    struct ic_expr * expr = 0;

    if( ! tokens ){
        puts("ic_parse_expr_constant_integer: tokens was null");
        return 0;
    }
    if( ! i ){
        puts("ic_parse_expr_constant_integer: i was null");
        return 0;
    }

    puts("ic_parse_expr_constant_integer: unimplemented");
    return 0;
}


/* consume token
 * returns ic_expr* on success
 * returns 0 on failure
 */
static struct ic_expr * ic_parse_expr_operator(struct ic_tokens *tokens, unsigned int *i){
    puts("ic_parse_expr_operator: unimplemented");
    return 0;
}

/* check if the current token looks like a string
 * returns 1 if yes
 * returns 0 if no
 */
static int ic_parse_stringish(struct ic_tokens *tokens, unsigned int *i){
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
        case '\'':
            return 1;
            break;

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
static int ic_parse_numberish(struct ic_tokens *tokens, unsigned int *i){
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
static char * ic_parse_peek_next(struct ic_tokens *tokens, unsigned int *i){
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
    offset = dist + 1;

    if( offset > tokens->len ){
        puts("ic_parse_peek_next: no next token, out of tokens");
        return 0;
    }

    return &(tokens->tokens[offset]);
}

struct ic_expr * ic_parse_expr(struct ic_tokens *tokens, unsigned int *i){
    /* pointer used to peek at start of next token */
    char *next = 0;

    if( ! tokens ){
        puts("ic_parse_expr: tokens was null");
        return 0;
    }
    if( ! i ){
        puts("ic_parse_expr: i was null");
        return 0;
    }

    /* rules
     *
     * we see a " token -> string value
     * we see a number as the first char -> int value
     * we see a symbol, need to inspect next token
     *      ( -> func call
     *      {+ - * ...} -> operator - FIXME UNSUPPORTED
     *      else -> identifier
     */
    if( ic_parse_stringish(tokens, i) ){
        return ic_parse_expr_constant_string(tokens, i);
    }

    if( ic_parse_numberish(tokens, i) ){
        return ic_parse_expr_constant_integer(tokens, i);
    }

    next = ic_parse_peek_next(tokens, i);
    if( ! next ){
        /* this in theory could mean out final token is an identifier
         * however this is illegal in practice as all expr/stmt must
         * appears inside a body ending with `end`
         * so this is indeed an error
         */
        puts("ic_parse_expr: call to ic_parse_peek_next failed");
        return 0;
    }

    /* if we see an open bracket this is a function call */
    if( *next == '(' ){
        return ic_parse_expr_fcall(tokens, i);
    }

    /* FIXME no support for operations yet */

    /* otherwise assume this is just an identifier */
    return ic_parse_expr_identifier(tokens, i);
}

