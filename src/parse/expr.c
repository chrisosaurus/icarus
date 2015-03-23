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

