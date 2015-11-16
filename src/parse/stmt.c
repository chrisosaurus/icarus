#include <stdio.h> /* puts */
#include <string.h> /* strncmp */
#include <stdlib.h> /* free */

#include "data/ast.h"
#include "parse.h"
#include "data/stmt.h"
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
static struct ic_stmt * ic_parse_stmt_ret(struct ic_token_list *token_list){
    /* return form
     *      return expr
     */

    /* out eventual return value */
    struct ic_stmt *stmt = 0;
    /* our internal ret value */
    struct ic_stmt_ret *ret = 0;

    /* current token */
    struct ic_token *token = 0;

    /* allocate and initialise */
    stmt = ic_stmt_new(ic_stmt_type_ret);
    if( ! stmt ){
        puts("ic_parse_stmt_ret: call to ic_stmt_new failed");
        return 0;
    }

    /* fetch and store out ret to make things easier */
    ret = ic_stmt_get_ret(stmt);

    /* check for `return` */
    token = ic_token_list_expect_important(token_list, IC_RETURN);
    if( ! token ){
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
    ret->ret = ic_parse_expr(token_list);
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
static struct ic_stmt * ic_parse_stmt_let(struct ic_token_list *token_list){
    /* current let forms
     *      let identifier::type = expression
     *
     * in the future we may expand to support all of:
     *      let identifier::type = expression
     *      let identifier
     *      let identifier::type
     *      let identifier = expression
     *
     * in the case where no initialisers expression is provided,
     * what do we init. to?
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

    /* our permissions */
    unsigned int permissions = 0;

    /* current token */
    struct ic_token *token = 0;

    /* allocate and initialise */
    stmt = ic_stmt_new(ic_stmt_type_let);
    if( ! stmt ){
        puts("ic_parse_stmt_let: call to ic_stmt_new failed");
        return 0;
    }

    /* fetch and store out let to make things easier */
    let = ic_stmt_get_let(stmt);

    /* check for `let` */
    token = ic_token_list_expect_important(token_list, IC_LET);
    if( ! token ){
        puts("ic_parse_stmt_let: Failed to find `let` token");
        free(stmt);
        return 0;
    }

    /* parse permissions, if they are there */
    permissions = ic_parse_permissions(token_list);

    /* consume identifier */
    token = ic_token_list_expect_important(token_list, IC_IDENTIFIER);
    if( ! token ){
        puts("ic_parse_stmt_let: Failed to find let identifier token");
        free(stmt);
        return 0;
    }

    id_start = ic_token_get_string(token);
    id_len = ic_token_get_string_length(token);
    if( ! id_start || ! id_len ){
        puts("ic_parse_stmt_let: Failed to extract let identifier");
        free(stmt);
        return 0;
    }

    /* initialise our let */
    if( ! ic_stmt_let_init(let, id_start, id_len, permissions) ){
        puts("ic_parse_stmt_let: call to ic_stmt_let_init failed");
        free(stmt);
        return 0;
    }

    /* check for optional `::` */
    token = ic_token_list_peek_important(token_list);
    if( token->id == IC_DOUBLECOLON ){
        /* consume `::` */
        token = ic_token_list_expect_important(token_list, IC_DOUBLECOLON);
        if( ! token ){
            puts("ic_parse_stmt_let: Failed to find let doublecolon token");
            free(stmt);
            return 0;
        }

        /* consume type */
        token = ic_token_list_expect_important(token_list, IC_IDENTIFIER);
        if( ! token ){
            puts("ic_parse_stmt_let: Failed to find let identifier token");
            free(stmt);
            return 0;
        }

        type_start = ic_token_get_string(token);
        type_len = ic_token_get_string_length(token);
        if( ! type_start || ! type_len ){
            puts("ic_parse_stmt_let: Failed to extract type identifier");
            free(stmt);
            return 0;
        }

        /* set type */
        if( ! ic_stmt_let_set_declared_type(let, type_start, type_len) ){
            puts("ic_parse_stmt_let: call to ic_stmt_let_set_declared_type failed");
            free(stmt);
            return 0;
        }
    }

    /* check for `=` */
    token = ic_token_list_expect_important(token_list, IC_ASSIGN);
    if( ! token ){
        puts("ic_parse_stmt_let: Failed to find let assign token");
        free(stmt);
        return 0;
    }

    /* consume initialisation expression */
    let->init = ic_parse_expr(token_list);
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
static struct ic_stmt * ic_parse_stmt_if(struct ic_token_list *token_list){
    /* out eventual return value */
    struct ic_stmt *stmt = 0;
    /* our condition expression */
    struct ic_expr *expr = 0;
    /* our body */
    struct ic_body *body = 0;

    /* current token */
    struct ic_token *token = 0;

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
    token = ic_token_list_expect_important(token_list, IC_IF);
    if( ! token ){
        puts("ic_parse_stmt_if: Failed to find `if` token");
        free(stmt);
        return 0;
    }

    /* parse if condition */
    expr = ic_parse_expr(token_list);
    if( ! expr ){
        puts("ic_parse_stmt_if: call to ic_parse_expr failed");
        return 0;
    }

    /* save our expr on the body stmt */
    stmt->u.sif.expr = expr;

    /* FIXME check that our 'expr' has the type Bool */

    /* parse our body */
    body = ic_parse_body(token_list);
    if( ! body ){
        puts("ic_parse_stmt_if: call to ic_parse_body failed");
        return 0;
    }

    /* save our body on the stmt */
    stmt->u.sif.body = body;

    /* ic_parse_body consumes our `end` for us */

    return stmt;
}

/* consume token
 * returns ic_stmt* on success
 * returns 0 on failure
 */
static struct ic_stmt * ic_parse_stmt_for(struct ic_token_list *token_list){
    /* out eventual return value */
    struct ic_stmt *stmt = 0;
    /* our expression */
    struct ic_expr *expr = 0;
    /* our iterator expression */
    struct ic_expr *iterator = 0;
    /* our body */
    struct ic_body *body = 0;

    /* current token */
    struct ic_token *token = 0;

    /* there are a few cases of if we care about at this point
     *
     *  for expr in iterator
     *      body
     *  end
     */

    stmt = ic_stmt_new(ic_stmt_type_for);
    if( ! stmt ){
        puts("ic_parse_stmt_for: call to ic_stmt_new failed");
        return 0;
    }

    /* consume for */
    token = ic_token_list_expect_important(token_list, IC_FOR);
    if( ! token ){
        puts("ic_parse_stmt_for: Failed to find `for` token");
        free(stmt);
        return 0;
    }

    /* parse expression */
    expr = ic_parse_expr(token_list);
    if( ! expr ){
        puts("ic_parse_stmt_for: call to ic_parse_expr failed for expression");
        return 0;
    }

    /* save our expr on the body stmt */
    stmt->u.sfor.expr = expr;

    /* consume in */
    token = ic_token_list_expect_important(token_list, IC_IN);
    if( ! token ){
        puts("ic_parse_stmt_for: Failed to find `in` token");
        free(stmt);
        return 0;
    }


    /* parse iterator */
    iterator = ic_parse_expr(token_list);
    if( ! iterator ){
        puts("ic_parse_stmt_for: call to ic_parse_expr failed for iterator");
        return 0;
    }

    /* save our iterator on the body stmt */
    stmt->u.sfor.iterator = iterator;

    /* FIXME check types for expr and iterator */

    /* parse our body */
    body = ic_parse_body(token_list);
    if( ! body ){
        puts("ic_parse_stmt_for: call to ic_parse_body failed");
        return 0;
    }

    /* save our body on the stmt */
    stmt->u.sfor.body = body;

    /* ic_parse_body consumes our `end` for us */

    return stmt;
}

/* consume token
 * returns ic_stmt* on success
 * returns 0 on failure
 */
static struct ic_stmt * ic_parse_stmt_while(struct ic_token_list *token_list){
    /* out eventual return value */
    struct ic_stmt *stmt = 0;
    /* our condition expression */
    struct ic_expr *expr = 0;
    /* our body */
    struct ic_body *body = 0;

    /* current token */
    struct ic_token *token = 0;

    /* there are a few cases of if we care about at this point
     *
     *  while expr
     *      body
     *  end
     */

    stmt = ic_stmt_new(ic_stmt_type_while);
    if( ! stmt ){
        puts("ic_parse_stmt_while: call to ic_stmt_new failed");
        return 0;
    }

    /* consume while */
    token = ic_token_list_expect_important(token_list, IC_WHILE);
    if( ! token ){
        puts("ic_parse_stmt_while: Failed to find `if` token");
        free(stmt);
        return 0;
    }

    /* parse if condition */
    expr = ic_parse_expr(token_list);
    if( ! expr ){
        puts("ic_parse_stmt_while: call to ic_parse_expr failed");
        return 0;
    }

    /* save our expr on the body stmt */
    stmt->u.swhile.expr = expr;

    /* FIXME check that our 'expr' has the type Bool */

    /* parse our body */
    body = ic_parse_body(token_list);
    if( ! body ){
        puts("ic_parse_stmt_while: call to ic_parse_body failed");
        return 0;
    }

    /* save our body on the stmt */
    stmt->u.swhile.body = body;

    /* ic_parse_body consumes our `end` for us */

    return stmt;
}

/* consume token
 * returns ic_stmt* on success
 * returns 0 on failure
 */
static struct ic_stmt * ic_parse_stmt_expr(struct ic_token_list *token_list){
    /* out eventual return value */
    struct ic_stmt *stmt = 0;
    /* our expr */
    struct ic_expr *expr = 0;
    /* out possible right expr */
    struct ic_expr *right = 0;
    /* our stmt type */
    enum ic_stmt_tag tag = ic_stmt_type_expr;
    /* our next token */
    struct ic_token *token = 0;

    expr = ic_parse_expr(token_list);
    if( ! expr ){
        puts("ic_parse_stmt_expr: call to ic_parse_expr failed");
        return 0;
    }

    /* check for possible assignment */
    token = ic_token_list_peek_important(token_list);
    if( token && token->id == IC_ASSIGN ){
        /* if so, consume */
        token = ic_token_list_expect_important(token_list, IC_ASSIGN);
        if( ! token ){
            puts("ic_parse_stmt_expr: failed to grab assign");
            return 0;
        }

        right = ic_parse_expr(token_list);

        if( ! right ){
            puts("ic_parse_stmt_expr: call to ic_parse_expr failed for right side of assignment");
            return 0;
        }

        tag = ic_stmt_type_assign;
    }

    /* allocate and initialise */
    stmt = ic_stmt_new(tag);
    if( ! stmt ){
        puts("ic_parse_stmt_expr: call to ic_stmt_new failed");
        return 0;
    }

    /* if we have a right then we have 2 exprs, we are an assign */
    if( right ){
        stmt->u.assign.left = expr;
        stmt->u.assign.right = right;
    } else {
        /* here we really just wrap ic_parse_expr */
        stmt->u.expr = expr;
    }

    return stmt;
}

/* an entry in the parse table showing the
 * length and token string that must be match
 * and if a match is found, the function to dispatch to
 */
static struct ic_parse_table_entry {
    enum ic_token_id id;
    struct ic_stmt * (*func)(struct ic_token_list *token_list);
} ic_parse_table [] = {
    /* id          function    */
    {  IC_IF,      ic_parse_stmt_if     },
    {  IC_FOR,     ic_parse_stmt_for    },
    {  IC_WHILE,   ic_parse_stmt_while  },
    {  IC_LET,     ic_parse_stmt_let    },
    {  IC_RETURN,  ic_parse_stmt_ret    }
    /* otherwise we default to ic_parse_stmt_expr */
};

struct ic_stmt * ic_parse_stmt(struct ic_token_list *token_list){
    /* offset into ic_parse_table */
    unsigned int pt_offset = 0;

    /* function to dispatch to */
    struct ic_stmt * (*func)(struct ic_token_list *token_list) = 0;

    /* return from call to func */
    struct ic_stmt *ret = 0;

    /* current token */
    struct ic_token *token = 0;

#ifdef DEBUG_PARSE_STMT
    puts("ic_parse_stmt: entered");
#endif

    token = ic_token_list_peek_important(token_list);
    if( ! token ){
        puts("ic_parse_stmt: call to ic_token_list_peek_important failed");
        return 0;
    }

    for( pt_offset=0; pt_offset < LENGTH(ic_parse_table); ++pt_offset ){

        if( token->id == ic_parse_table[pt_offset].id ){

            func = ic_parse_table[pt_offset].func;
            if( ! func ){
                printf( "ic_parse: Error matched with: ");
                ic_token_id_print_debug(token->id);
                puts("but parse table function was null, bailing");
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
    ret = func(token_list);

    if( ! ret ){
        puts("ic_parse_stmt: call to matched function failed");
        return 0;
    }

#ifdef DEBUG_PARSE_STMT
    puts("ic_parse_stmt: success! returning result");
#endif

    return ret;
}

