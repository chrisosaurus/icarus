#include <stdio.h> /* puts */
#include <string.h> /* strncmp */
#include <stdlib.h> /* free */

#include "data/ast.h"
#include "parse.h"
#include "data/statement.h"
#include "../lex/lexer.h"

/* ignore unused parameter warnings */
#pragma GCC diagnostic ignored "-Wunused-parameter"

/* ignore unused functions */
#pragma GCC diagnostic ignored "-Wunused-function"

/* ignore unused variable */
#pragma GCC diagnostic ignored "-Wunused-variable"

/* there are currently 3 types of statements
 *  let
 *  if
 *  expr (void context)
 */

/* consume token
 * returns ic_stmt* on success
 * returns 0 on failure
 */
static struct ic_stmt * ic_parse_stmt_ret(struct ic_tokens *tokens, unsigned int *i){
    /* return form
     *      return expr
     */

    /* out eventual return value */
    struct ic_stmt *stmt = 0;
    /* our internal ret value */
    struct ic_stmt_ret *ret = 0;

    /* allocate and initialise */
    stmt = ic_stmt_new(ic_stmt_type_ret);
    if( ! stmt ){
        puts("ic_parse_stmt_ret: call to ic_stmt_new failed");
        return 0;
    }

    /* fetch and store out ret to make things easier */
    ret = ic_stmt_get_ret(stmt);

    /* check for `return` */
    if( ic_parse_check_token("return", 6, tokens->tokens, i) ){
        puts("ic_parse_stmt_ret: Failed to find `return` token");
        free(stmt);
        return 0;
    }
    /* initialise our ret */
    if( ! ic_stmt_ret_init(ret) ){
        puts("ic_parse_stmt_ret: call to ic_stmt_ret_init failed");
        free(stmt);
        return 0;
    }

    /* consume initialisation expression */
    ret->ret = ic_parse_expr(tokens, i);
    if( ! ret->ret ){
        puts("ic_parse_stmt_ret: call to ic_parse_expr failed");
        free(stmt);
        return 0;
    }

    /* victory */
    return stmt;
}


/* consume token
 * returns ic_stmt* on success
 * returns 0 on failure
 */
static struct ic_stmt * ic_parse_stmt_let(struct ic_tokens *tokens, unsigned int *i){
    /* current let forms
     *      let identifier::type = expression
     *
     * in the future we may expand to
     *      let identifier
     *      let identifier::type
     *      let identifier = expression
     */

    /* out eventual return value */
    struct ic_stmt *stmt = 0;
    /* our internal let value */
    struct ic_stmt_let *let = 0;

    /* pointer and length for our identifier */
    char *id_start = 0;
    unsigned int id_len= 0;

    /* pointer and length for our type */
    char *type_start = 0;
    unsigned int type_len = 0;

    /* allocate and initialise */
    stmt = ic_stmt_new(ic_stmt_type_let);
    if( ! stmt ){
        puts("ic_parse_stmt_let: call to ic_stmt_new failed");
        return 0;
    }

    /* fetch and store out let to make things easier */
    let = ic_stmt_get_let(stmt);

    /* check for `let` */
    if( ic_parse_check_token("let", 3, tokens->tokens, i) ){
        puts("ic_parse_stmt_let: Failed to find `let` token");
        free(stmt);
        return 0;
    }

    /* consume identifier */
    id_start = &(tokens->tokens[*i]);
    id_len = ic_parse_token_length(tokens->tokens, *i);
    ic_parse_token_advance(i, id_len);

    /* check for `::` */
    if( ic_parse_check_token("::", 2, tokens->tokens, i) ){
        puts("ic_parse_stmt_let: Failed to find `::` token");
        free(stmt);
        return 0;
    }

    /* consume type */
    type_start = &(tokens->tokens[*i]);
    type_len = ic_parse_token_length(tokens->tokens, *i);
    ic_parse_token_advance(i, type_len);

    /* initialise our let */
    if( ! ic_stmt_let_init(let, id_start, id_len, type_start, type_len) ){
        puts("ic_parse_stmt_let: call to ic_stmt_let_init failed");
        free(stmt);
        return 0;
    }

    /* check for `=` */
    if( ic_parse_check_token("=", 1, tokens->tokens, i) ){
        puts("ic_parse_stmt_let: Failed to find `=` token");
        free(stmt);
        return 0;
    }

    /* consume initialisation expression */
    let->init = ic_parse_expr(tokens, i);
    if( ! let->init ){
        puts("ic_parse_stmt_let: call to ic_parse_expr failed");
        free(stmt);
        return 0;
    }

    /* victory */
    return stmt;
}

/* consume token
 * returns ic_stmt* on success
 * returns 0 on failure
 */
static struct ic_stmt * ic_parse_stmt_if(struct ic_tokens *tokens, unsigned int *i){
    /* out eventual return value */
    struct ic_stmt *stmt = 0;
    /* our condition expression */
    struct ic_expr *expr = 0;
    /* our body */
    struct ic_body *body = 0;

    /* there are a few cases of if we care about at this point
     *
     *  if expr then
     *      body
     *  end
     *
     * an expression may contain more operators and expressions
     * which yields a generic form of
     *
     *  if [operator] expr [operator expr] ... then
     *      body
     *  end
     *
     * notice that after lexing these will appear as
     *
     *  if [operator] expr [operator expr] ... then body end
     *
     */

    stmt = ic_stmt_new(ic_stmt_type_if);
    if( ! stmt ){
        puts("ic_parse_stmt_if: call to ic_stmt_new failed");
        return 0;
    }

    /* consume if */
    if( ic_parse_check_token("if", 2, tokens->tokens, i) ){
        puts("ic_parse_stmt_ret: Failed to find `return` token");
        free(stmt);
        return 0;
    }

    /* FIXME parse if condition */

    /* save our expr on the body stmt */
    stmt->u.sif.expr = expr;

    /* parse our body */
    body = ic_parse_body(tokens, i);
    if( ! body ){
        puts("ic_parse_stmt_if: call to ic_parse_body failed");
        return 0;
    }

    /* save our body on the stmt */
    stmt->u.sif.body = body;

    /* FIXME consume end */

    return stmt;
}

/* consume token
 * returns ic_stmt* on success
 * returns 0 on failure
 */
static struct ic_stmt * ic_parse_stmt_expr(struct ic_tokens *tokens, unsigned int *i){
    /* out eventual return value */
    struct ic_stmt *stmt = 0;

    /* allocate and initialise */
    stmt = ic_stmt_new(ic_stmt_type_expr);
    if( ! stmt ){
        puts("ic_parse_stmt_expr: call to ic_stmt_new failed");
        return 0;
    }

    /* here we really just wrap ic_parse_expr */
    stmt->u.expr = ic_parse_expr(tokens, i);
    if( ! stmt->u.expr ){
        puts("ic_parse_stmt_expr: call to ic_parse_expr failed");
        free(stmt);
        return 0;
    }

    return stmt;
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
    {  3,     "let",      ic_parse_stmt_let },
    {  6,     "return",   ic_parse_stmt_ret }
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
            printf( "ic_parse_stmt: comparing token '%.*s' (%u) with parse_table entry '%.*s' (%u)\n",
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
