#include <stdio.h> /* puts */

#include "../parse.h"

/* ignore unused parameter warnings */
#pragma GCC diagnostic ignored "-Wunused-parameter"

/* ignore unused functions */
#pragma GCC diagnostic ignored "-Wunused-function"

/* ignore unused variables */
#pragma GCC diagnostic ignored "-Wunused-variable"

/* ignore unused variables that are set */
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

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
    /* our eventual return value */
    struct ic_expr * expr = 0;
    /* used for getting the distance of our function name */
    unsigned int dist = 0;
    /* temporary used for capturing the argument expression */
    struct ic_expr * arg = 0;

    if( ! tokens ){
        puts("ic_parse_expr_fcall: tokens was null");
        return 0;
    }
    if( ! i ){
        puts("ic_parse_expr_fcall: i was null");
        return 0;
    }

    /* build our return expr */
    expr = ic_expr_new(ic_expr_type_func_call);
    if( ! expr ){
        puts("ic_parse_expr_fcall: call to ic_expr_new failed");
        return 0;
    }

    /* find our function name */
    dist = ic_parse_token_length(tokens->tokens, *i);
    if( ! dist ){
        puts("ic_parse_expr_fcall: failed to get distance of function name");
        return 0;
    }

    /* build our function */
    if( ic_expr_func_call_init( &(expr->u.fcall), &(tokens->tokens[*i]), dist ) ){
        puts("ic_parse_expr_fcall: call to ic_expr_func_call_init failed");
        return 0;
    }

    /* keep consuming arguments until we see the closing ) */
    while( ! ic_parse_check_token(")", 1, tokens->tokens, i) ){
        /* parse this argument */
        arg = ic_parse_expr(tokens, i);
        if( ! arg ){
            puts("ic_parse_expr_fcall: parsing arg error, call to ic_parse_expr failed");
        }

        /* store it inside our function */
        ic_expr_func_call_add_arg( &(expr->u.fcall), arg );
    }

    /* ic_parse_check_token will skip over the closing ) for us */

    /* victory */
    return expr;
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
    /* our eventual return value */
    struct ic_expr * expr = 0;
    /* the i value that marks the beginning of our string
     * this is the offset *after* the opening " is read
     */
    char *start = 0;
    /* length of string */
    unsigned int length = 0;

    /* pointer to our constant */
    struct ic_expr_constant *cons = 0;
    /* pointer to our ic_string */
    struct ic_string *string = 0;

    if( ! tokens ){
        puts("ic_parse_expr_constant_string: tokens was null");
        return 0;
    }
    if( ! i ){
        puts("ic_parse_expr_constant_string: i was null");
        return 0;
    }

    /* check for opening quote */
    if( tokens->tokens[*i] != '"' ){
        puts("ic_parse_expr_constant_string: failed to find opening quote (\")");
        return 0;
    }

    /* build our return expr */
    expr = ic_expr_new(ic_expr_type_constant);
    if( ! expr ){
        puts("ic_parse_expr_constant_string: call to ic_expr_new failed");
        return 0;
    }

    /* unpack our constant */
    cons = ic_expr_get_constant(expr);
    if( ! cons ){
        puts("ic_parse_expr_constant_string: call to ic_expr_get_constant failed");
        return 0;
    }

    /* initialise our constant */
    if( ic_expr_constant_init(cons, ic_expr_constant_type_string) ){
        puts("ic_parse_expr_constant_string: call to ic_expr_constant_init failed");
        return 0;
    }

    /* get out our ic_string */
    string = ic_expr_constant_get_string(cons);
    if( ! string ){
        puts("ic_parse_expr_constant_string: call to ic_expr_constant_get_string failed");
        return 0;
    }

    /* skip over opening " */
    ++*i;

    /* record our starting value */
    start = &(tokens->tokens[*i]);

    /* find length of string
     * FIXME naive
     */
    for( length=0; tokens->tokens[*i + length] != '"'; ++length ) ;

    /* also skip over closing " */
    ++ *i;

    /* skip over inter-token delim */
    ++ *i;

    /* initialise our ic_string
     * we have the start of the string (start)
     * and the total length (dist_sum)
     */
    if( ic_string_init(string, start, length) ){
        puts("ic_parse_expr_constant_string: call to ic_string_init failed");
        return 0;
    }

    /* victory */
    return expr;
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
     *      {+ - * ...} -> operator
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

    /* basic support for operators
     * this will only support:
     *  identifier operator ...
     * as any numbers or strings are caught above
     *
     * FIXME reconsider this
     */
    if( ic_parse_operatorish(next) ){
        return ic_parse_expr_operator(tokens, i);
    }

    /* if we see an open bracket this is a function call */
    if( *next == '(' ){
        return ic_parse_expr_fcall(tokens, i);
    }

    /* otherwise assume this is just an identifier */
    return ic_parse_expr_identifier(tokens, i);
}

