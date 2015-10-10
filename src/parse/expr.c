#include <stdio.h> /* puts */
#include <stdlib.h> /* strtol, free */
#include <errno.h> /* errno */
#include <limits.h> /* LONG_MIN, LONG_MAX */

#include "parse.h"

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
 * renurns 0 on failure
 */
static struct ic_expr * ic_parse_expr_fcall(struct ic_token_list *token_list){
    /* our eventual return value */
    struct ic_expr * expr = 0;
    /* temporary used for capturing the argument expression */
    struct ic_expr * arg = 0;
    /* current token */
    struct ic_token *token = 0;

    if( ! token_list ){
        puts("ic_parse_expr_fcall: token_list was null");
        return 0;
    }

    /* build our return expr */
    expr = ic_expr_new(ic_expr_type_func_call);
    if( ! expr ){
        puts("ic_parse_expr_fcall: call to ic_expr_new failed");
        return 0;
    }

    /* find our function name */
    token = ic_token_list_expect_important(token_list, IC_IDENTIFIER);
    if( ! token ){
        puts("ic_parse_expr_fcall: failed to get function name");
        free(expr);
        return 0;
    }

    /* build our function */
    if( ! ic_expr_func_call_init( &(expr->u.fcall), ic_token_get_string(token), ic_token_get_string_length(token)) ){
        puts("ic_parse_expr_fcall: call to ic_expr_func_call_init failed");
        free(expr);
        return 0;
    }

    /* skip over opening ( */
    token = ic_token_list_expect_important(token_list, IC_LRBRACKET);
    if( ! token ){
        puts("ic_parse_expr_fcall: failed to find opening bracket '('");
        free(expr);
        return 0;
    }

    /* keep consuming arguments until we see the closing ) */
    while( (token = ic_token_list_peek_important(token_list)) ){
        /* if closing right bracket then stop */
        if( token->id == IC_RRBRACKET ){
            break;
        }

        /* if comma then skip
         * FIXME: this makes commas optional
         */
        if( token->id == IC_COMMA ){
            token = ic_token_list_next_important(token_list);
            if( ! token ){
            puts("ic_parse_expr_fcall: parsing arg error, call to ic_token_list_next_important failed when trying to consume comma");
            free(expr);
            return 0;
            }
            /* skip */
            continue;
        }

        /* parse this argument */
        arg = ic_parse_expr(token_list);
        if( ! arg ){
            puts("ic_parse_expr_fcall: parsing arg error, call to ic_parse_expr failed");
            free(expr);
            return 0;
        }

        /* store it inside our function */
        if( ! ic_expr_func_call_add_arg( &(expr->u.fcall), arg ) ){
            puts("ic_parse_expr_fcall: call to ic_expr_func_call_add_arg failed");
            free(expr);
            return 0;
        }
    }

    token = ic_token_list_expect_important(token_list, IC_RRBRACKET);
    if( ! token ){
        puts("ic_parse_expr_fcall: failed to get closing bracket");
        free(expr);
        return 0;
    }

    /* victory */
    return expr;
}

/* consume token
 * returns ic_expr* on success
 * returns 0 on failure
 */
static struct ic_expr * ic_parse_expr_identifier(struct ic_token_list *token_list){
    /* our final return expr */
    struct ic_expr * expr = 0;
    /* pointer to our internal id */
    struct ic_expr_identifier *id = 0;

    /* iter into this identifier */
    unsigned int iter = 0;
    /* current char in identifier we are checking */
    char ch = 0;

    /* current token */
    struct ic_token *token = 0;

    if( ! token_list ){
        puts("ic_parse_expr_identifier: token_list was null");
        return 0;
    }

    token = ic_token_list_expect_important(token_list, IC_IDENTIFIER);
    if( ! token ){
        puts("ic_parse_expr_identifier: failed to get identifier");
        free(expr);
        return 0;
    }

    /* build our new expr */
    expr = ic_expr_new(ic_expr_type_identifier);
    if( ! expr ){
        puts("ic_parse_expr_identifier: call to ic_expr_new failed");
        return 0;
    }

    /* fetch our internal id */
    id = ic_expr_get_identifier(expr);
    if( ! id ){
        puts("ic_parse_expr_identifier: call to ic_expr_get_identifier failed");
        free(expr);
        return 0;
    }

    /* initialise our id */
    if( ! ic_expr_identifier_init(id, ic_token_get_string(token), ic_token_get_string_length(token)) ){
        puts("ic_parse_expr_identifier: call to ic_expr_identifier_init failed");
        free(expr);
        return 0;
    }

    /* victory */
    return expr;
}

/* consume token and make a string
 * returns ic_expr* on success
 * returns 0 on failure
 */
static struct ic_expr * ic_parse_expr_constant_string(struct ic_token_list *token_list){
    /* our eventual return value */
    struct ic_expr * expr = 0;

    /* pointer to our constant */
    struct ic_expr_constant *cons = 0;
    /* pointer to our ic_string */
    struct ic_string *string = 0;

    /* current token */
    struct ic_token *token = 0;

    if( ! token_list ){
        puts("ic_parse_expr_constant_string: token_list was null");
        return 0;
    }

    token = ic_token_list_expect_important(token_list, IC_LITERAL_STRING);
    if( ! token ){
        puts("ic_parse_expr_constant_string: failed to get string");
        free(expr);
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
        free(expr);
        return 0;
    }

    /* initialise our constant */
    if( ! ic_expr_constant_init(cons, ic_expr_constant_type_string) ){
        puts("ic_parse_expr_constant_string: call to ic_expr_constant_init failed");
        free(expr);
        return 0;
    }

    /* get out our ic_string */
    string = ic_expr_constant_get_string(cons);
    if( ! string ){
        puts("ic_parse_expr_constant_string: call to ic_expr_constant_get_string failed");
        free(expr);
        return 0;
    }

    /* initialise our ic_string
     * we have the start of the string (start)
     * and the total length (dist_sum)
     */
    if( ! ic_string_init(string, ic_token_get_string(token), ic_token_get_string_length(token)) ){
        puts("ic_parse_expr_constant_string: call to ic_string_init failed");
        free(expr);
        return 0;
    }

    /* victory */
    return expr;
}

/* consume token and make an int
 * returns ic_expr* on success
 * returns 0 on failure
 */
static struct ic_expr * ic_parse_expr_constant_integer(struct ic_token_list *token_list){
    /* our eventual return value */
    struct ic_expr * expr = 0;
    /* our constant */
    struct ic_expr_constant *cons = 0;
    /* pointer to our integer value */
    long int *integer = 0;

    /* current token */
    struct ic_token *token = 0;

    if( ! token_list ){
        puts("ic_parse_expr_constant_integer: token_list was null");
        return 0;
    }

    token = ic_token_list_expect_important(token_list, IC_LITERAL_INTEGER);
    if( ! token ){
        puts("ic_parse_expr_constant_integer: unable to find integer");
        return 0;
    }

    /* build our return expr */
    expr = ic_expr_new(ic_expr_type_constant);
    if( ! expr ){
        puts("ic_parse_expr_constant_integer: call to ic_expr_new failed");
        return 0;
    }

    /* unpack our constant */
    cons = ic_expr_get_constant(expr);
    if( ! cons ){
        puts("ic_parse_expr_constant_integer: call to ic_expr_get_constant failed");
        free(expr);
        return 0;
    }

    /* initialise our constant */
    if( ! ic_expr_constant_init(cons, ic_expr_constant_type_integer) ){
        puts("ic_parse_expr_constant_integer: call to ic_expr_constant_init failed");
        free(expr);
        return 0;
    }

    /* get out our ic_integer */
    integer = ic_expr_constant_get_integer(cons);
    if( ! integer ){
        puts("ic_parse_expr_constant_integer: call to ic_expr_constant_get_integer failed");
        free(expr);
        return 0;
    }

    /* strtol */
    *integer = ic_token_get_integer(token);

    /* victory */
    return expr;
}

/* used to parse an expression that is not made up off more than one token:
 *  number
 *  string
 *  identifier
 *
 * this is useful if we know the next thing is say an operator but we want
 * to constrain parsing up until it
 */
static struct ic_expr * ic_parse_expr_single_token(struct ic_token_list *token_list){
    /* current token */
    struct ic_token *token = 0;

    if( ! token_list ){
        puts("ic_parse_expr_single_token: token_list was null");
        return 0;
    }

    token = ic_token_list_peek_important(token_list);
    if( ! token ){
        puts("ic_parse_expr_single_token: call to ic_token_list_peek failed");
        return 0;
    }

    /* rules:
     *  we see a " token -> string value
     *  we see a number as the first char -> int value
     *  else -> identifier
     */

    if( token->id == IC_LITERAL_STRING ){
        return ic_parse_expr_constant_string(token_list);
    }

    if( token->id == IC_LITERAL_INTEGER ){
        return ic_parse_expr_constant_integer(token_list);
    }
    /* otherwise assume this is just an identifier */
    return ic_parse_expr_identifier(token_list);
}

/* consume token
 * returns ic_expr* on success
 * returns 0 on failure
 */
static struct ic_expr * ic_parse_expr_operator(struct ic_token_list *token_list){
    /* our eventual return value */
    struct ic_expr *expr = 0;
    /* our internal operator */
    struct ic_expr_operator *operator = 0;

    /* an operator is made up of
     *  single-token operator expr
     */
    /* our left child */
    struct ic_expr *left = 0;
    /* our operator char* and len */
    char *op_start = 0;
    unsigned int op_len = 0;
    /* our right child */
    struct ic_expr *right = 0;

    /* operator token */
    struct ic_token *token = 0;

    if( ! token_list ){
        puts("ic_parse_expr_operator: token_list was null");
        return 0;
    }

    /* build our new expr */
    expr = ic_expr_new(ic_expr_type_operator);
    if( ! expr ){
        puts("ic_parse_expr_operator: call to ic_expr_new failed");
        return 0;
    }

    /* fetch our internal operator */
    operator = ic_expr_get_operator(expr);
    if( ! operator ){
        puts("ic_parse_nameexpr_operator: call to ic_expr_get_operator failed");
        free (expr);
        return 0;
    }

    /* grab our left token */
    left = ic_parse_expr_single_token(token_list);
    if( ! left ){
        puts("ic_parse_expr_operator: left call to ic_parse_expr_single_token failed");
        free(expr);
        return 0;
    }

    /* op handling */
    token = ic_token_list_next_important(token_list);
    if( ! token ){
        puts("ic_parse_expr_operator: operator call to token list next important failed");
        return 0;
    }

    /* our right expr can be more complex */
    right = ic_parse_expr(token_list);
    if( ! right ){
        puts("ic_parse_expr_operator: right call to ic_parse_exprfailed");
        free(expr);
        free(left);
        return 0;
    }

    /* initialise our operator */
    if( ! ic_expr_operator_init_binary(operator, left, right, token) ){
        puts("ic_parse_expr_operator: call to ic_expr_operator_init_binary failed");
        free(expr);
        free(left);
        free(right);
        return 0;    /* an operator is made up of
     *  single-token operator expr
     */
    }

    return expr;
}

struct ic_expr * ic_parse_expr(struct ic_token_list *token_list){
    /* current token */
    struct ic_token *token = 0;
    /* next token */
    struct ic_token *next_token = 0;

    if( ! token_list ){
        puts("ic_parse_expr: token_list was null");
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

    /* rules:
     *  peek at next token and check for
     *  ( -> function call
     *  operator -> operator
     *
     * otherwise call single token
     */

    token = ic_token_list_peek_important(token_list);
    if( ! token ){
        /* this in theory could mean out final token is an identifier
         * however this is illegal in practice as all expr/stmt must
         * appears inside a body ending with `end`
         * so this is indeed an error
         */
        puts("ic_parse_expr: call to ic_token_list_peek failed");
        return 0;
    }

    next_token = ic_token_list_peek_ahead_important(token_list);
    if( ! next_token ){
        puts("ic_parse_expr: call to ic_token_list_peek_ahead failed");
        return 0;
    }

    /* otherwise default to next token */
    /* if we peek ahead and see a binary operator
     * then parse a binary operation expression
     */
    if( ic_token_isoperator(next_token) ){
        return ic_parse_expr_operator(token_list);
    }

    /* if we see an open bracket this is a function call */
    if( next_token->id == IC_LRBRACKET ){
        return ic_parse_expr_fcall(token_list);
    }

    return ic_parse_expr_single_token(token_list);
}


