#include <stdio.h>  /* puts */
#include <stdlib.h> /* free */
#include <string.h> /* strncmp */

#include "../lex/lexer.h"
#include "data/ast.h"
#include "data/stmt.h"
#include "parse.h"

/* there are currently 3 types of statements
 *  let
 *  if
 *  expr (void context)
 */

/* consume token
 * returns ic_stmt* on success
 * returns 0 on failure
 */
static struct ic_stmt *ic_parse_stmt_ret(struct ic_token_list *token_list) {
    /* return form
     *      return expr
     */

    /* out eventual return value */
    struct ic_stmt *stmt = 0;
    /* our internal ret value */
    struct ic_stmt_ret *ret = 0;

    /* current token */
    struct ic_token *token = 0;

    if (!token_list) {
        puts("ic_parse_stmt_ret: token_list was null");
        return 0;
    }

    /* allocate and initialise */
    stmt = ic_stmt_new(ic_stmt_type_ret);
    if (!stmt) {
        puts("ic_parse_stmt_ret: call to ic_stmt_new failed");
        return 0;
    }

    /* fetch and store out ret to make things easier */
    ret = ic_stmt_get_ret(stmt);

    /* check for `return` */
    token = ic_token_list_expect_important(token_list, IC_RETURN);
    if (!token) {
        puts("ic_parse_stmt_ret: Failed to find `return` token");
        free(stmt);
        return 0;
    }
    /* initialise our ret */
    if (!ic_stmt_ret_init(ret)) {
        puts("ic_parse_stmt_ret: call to ic_stmt_ret_init failed");
        free(stmt);
        return 0;
    }

    /* consume initialisation expression */
    ret->ret = ic_parse_expr(token_list);
    if (!ret->ret) {
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
static struct ic_stmt *ic_parse_stmt_let(struct ic_token_list *token_list) {
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
    unsigned int id_len = 0;

    struct ic_type_ref *type_ref = 0;

    /* our permissions */
    unsigned int permissions = 0;

    /* current token */
    struct ic_token *token = 0;

    if (!token_list) {
        puts("ic_parse_stmt_let: token_list was null");
        return 0;
    }

    /* allocate and initialise */
    stmt = ic_stmt_new(ic_stmt_type_let);
    if (!stmt) {
        puts("ic_parse_stmt_let: call to ic_stmt_new failed");
        return 0;
    }

    /* fetch and store out let to make things easier */
    let = ic_stmt_get_let(stmt);

    /* check for `let` */
    token = ic_token_list_expect_important(token_list, IC_LET);
    if (!token) {
        puts("ic_parse_stmt_let: Failed to find `let` token");
        free(stmt);
        return 0;
    }

    /* parse permissions, if they are there */
    permissions = ic_parse_permissions(token_list);

    /* consume identifier */
    token = ic_token_list_expect_important(token_list, IC_IDENTIFIER);
    if (!token) {
        puts("ic_parse_stmt_let: Failed to find let identifier token");
        free(stmt);
        return 0;
    }

    id_start = ic_token_get_string(token);
    id_len = ic_token_get_string_length(token);
    if (!id_start || !id_len) {
        puts("ic_parse_stmt_let: Failed to extract let identifier");
        free(stmt);
        return 0;
    }

    /* initialise our let */
    if (!ic_stmt_let_init(let, id_start, id_len, permissions)) {
        puts("ic_parse_stmt_let: call to ic_stmt_let_init failed");
        free(stmt);
        return 0;
    }

    /* check for optional `::` */
    token = ic_token_list_peek_important(token_list);
    if (!token) {
        puts("ic_parse_stmt_let: call to ic_token_list_peek failed");
        free(stmt);
        return 0;
    }

    if (token->id == IC_DOUBLECOLON) {
        /* consume `::` */
        token = ic_token_list_expect_important(token_list, IC_DOUBLECOLON);
        if (!token) {
            puts("ic_parse_stmt_let: Failed to find let doublecolon token");
            free(stmt);
            return 0;
        }

        type_ref = ic_parse_type_ref(token_list);
        if (!type_ref) {
            puts("ic_parse_stmt_let: call to ic_parse_type_ref failed");
            free(stmt);
            return 0;
        }

        /* set type */
        if (!ic_stmt_let_set_declared_type(let, type_ref)) {
            puts("ic_parse_stmt_let: call to ic_stmt_let_set_declared_type failed");
            free(stmt);
            return 0;
        }
    }

    /* check for `=` */
    token = ic_token_list_expect_important(token_list, IC_ASSIGN);
    if (!token) {
        puts("ic_parse_stmt_let: Failed to find let assign token");
        free(stmt);
        return 0;
    }

    /* consume initialisation expression */
    let->init = ic_parse_expr(token_list);
    if (!let->init) {
        puts("ic_parse_stmt_let: call to ic_parse_expr failed");
        free(stmt);
        return 0;
    }

    /* victory */
    return stmt;
}

/* consume token
 * returns ic_stmt_case * on success
 * returns 0 on failure
 */
static struct ic_stmt_case *ic_parse_stmt_match_case(struct ic_token_list *token_list) {
    struct ic_token *token = 0;
    struct ic_body *body = 0;
    struct ic_stmt_case *scase = 0;
    char *id_ch = 0;
    unsigned int id_len = 0;
    char *type_ch = 0;
    unsigned int type_len = 0;

    if (!token_list) {
        puts("ic_parse_stmt_match_case: token_list was null");
        return 0;
    }

    token = ic_token_list_expect_important(token_list, IC_CASE);
    if (!token) {
        puts("ic_parse_stmt_match_catch: failed to find `case` token");
        return 0;
    }

    token = ic_token_list_expect_important(token_list, IC_IDENTIFIER);
    if (!token) {
        puts("ic_parse_stmt_match_catch: failed to find `identifier` token");
        return 0;
    }

    id_ch = ic_token_get_string(token);
    if (!id_ch) {
        puts("ic_parse_stmt_match_catch: call to ic_token_get_string failed");
        return 0;
    }
    id_len = ic_token_get_string_length(token);

    token = ic_token_list_expect_important(token_list, IC_DOUBLECOLON);
    if (!token) {
        puts("ic_parse_stmt_match_catch: failed to find `::` token");
        return 0;
    }

    token = ic_token_list_expect_important(token_list, IC_IDENTIFIER);
    if (!token) {
        puts("ic_parse_stmt_match_catch: failed to find `Type token");
        return 0;
    }

    type_ch = ic_token_get_string(token);
    if (!type_ch) {
        puts("ic_parse_stmt_match_catch: call to ic_token_get_string failed");
        return 0;
    }
    type_len = ic_token_get_string_length(token);

    body = ic_parse_body(token_list, 0);
    if (!body) {
        puts("ic_parse_stmt_match_catch: call to ic_parse_body failed");
        return 0;
    }

    scase = ic_stmt_case_new(id_ch, id_len, type_ch, type_len, body);
    if (!scase) {
        puts("ic_parse_stmt_match_case: call to ic_stmt_case_new failed");
        return 0;
    }

    return scase;
}

/* consume token
 * returns ic_body * on success
 * returns 0 on failure
 */
static struct ic_body *ic_parse_stmt_match_else(struct ic_token_list *token_list) {
    struct ic_token *token = 0;
    struct ic_body *body = 0;

    if (!token_list) {
        puts("ic_parse_stmt_match_else: token_list was null");
        return 0;
    }

    token = ic_token_list_expect_important(token_list, IC_ELSE);
    if (!token) {
        puts("ic_parse_stmt_match_else: failed to find `else` token");
        return 0;
    }

    body = ic_parse_body(token_list, 0);
    if (!body) {
        puts("ic_parse_stmt_match_else: call to ic_parse_body failed");
        return 0;
    }

    return body;
}

/* consume token
 * returns ic_stmt* on success
 * returns 0 on failure
 */
static struct ic_stmt *ic_parse_stmt_match(struct ic_token_list *token_list) {
    struct ic_stmt *stmt = 0;
    struct ic_stmt_match *match = 0;
    struct ic_stmt_case *scase = 0;
    struct ic_body *else_body = 0;
    struct ic_token *token = 0;
    unsigned int found_something = 0;

    if (!token_list) {
        puts("ic_parse_stmt_match: token_list was null");
        return 0;
    }

    stmt = ic_stmt_new(ic_stmt_type_match);
    if (!stmt) {
        puts("ic_parse_stmt_match: call to ic_stmt_new failed");
        goto MATCH_ERROR;
    }

    match = ic_stmt_get_match(stmt);
    if (!match) {
        puts("ic_parse_stmt_match: call to ic_stmt_get_match failed");
        goto MATCH_ERROR;
    }

    /* check for `return` */
    token = ic_token_list_expect_important(token_list, IC_MATCH);
    if (!token) {
        puts("ic_parse_stmt_match: failed to find `match` token");
        goto MATCH_ERROR;
    }

    /* initialise our match */
    if (!ic_stmt_match_init(match)) {
        puts("ic_parse_stmt_match: call to ic_stmt_ret_init failed");
        goto MATCH_ERROR;
    }

    /* consume matching expression */
    match->expr = ic_parse_expr(token_list);
    if (!match->expr) {
        puts("ic_parse_stmt_match: call to ic_parse_expr failed");
        goto MATCH_ERROR;
    }

    /* match expr
     *  case name::Type
     *    body
     *  case name::Type
     *    body
     *  else
     *    body
     *  end
     * end
     */

    /* match expr
     *  else
     *    body
     *  end
     * end
     */

    /* match expr
     *  case name::Type
     *    body
     *  case name::Type
     *    body
     *  end
     * end
     */

    for (
        token = ic_token_list_peek_important(token_list);
        token && token->id == IC_CASE;
        token = ic_token_list_peek_important(token_list)) {
        scase = ic_parse_stmt_match_case(token_list);
        if (!scase) {
            puts("ic_parse_stmt_match: call to ic_parse_stmt_match_case failed");
            goto MATCH_ERROR;
        }
        if (-1 == ic_pvector_append(&(match->cases), scase)) {
            puts("ic_parse_stmt_match: call to ic_pvector_append failed");
            goto MATCH_ERROR;
        }

        found_something = 1;
    }

    if (!token) {
        puts("ic_parse_stmt_match: call to ic_token_list_peek failed");
        goto MATCH_ERROR;
    }

    if (token->id == IC_ELSE) {
        else_body = ic_parse_stmt_match_else(token_list);
        if (!else_body) {
            puts("ic_parse_stmt_match: call to ic_parse_stmt_match_else failed");
            goto MATCH_ERROR;
        }
        match->else_body = else_body;
        found_something = 1;
    }

    if (!found_something) {
        puts("ic_parse_stmt_match: failed to find any case/else statements");
        goto MATCH_ERROR;
    }

    /* at this point we need 2 ends, one to end the case/else, one to end the match */

    token = ic_token_list_expect_important(token_list, IC_END);
    if (!token) {
        puts("ic_parse_stmt_match: failed to find first `end` token");
        free(stmt);
        return 0;
    }

    token = ic_token_list_expect_important(token_list, IC_END);
    if (!token) {
        puts("ic_parse_stmt_match: failed to find second `end` token");
        free(stmt);
        return 0;
    }

    /* success */
    return stmt;

MATCH_ERROR:

    /* FIXME TODO clean up rest */

    if (stmt) {
        free(stmt);
    }

    puts("ic_parse_stmt_match: error");
    return 0;
}

/* consume token
 * returns ic_stmt* on success
 * returns 0 on failure
 */
static struct ic_stmt *ic_parse_stmt_if(struct ic_token_list *token_list) {
    /* out eventual return value */
    struct ic_stmt *stmt = 0;
    /* our condition expression */
    struct ic_expr *expr = 0;
    /* our then body */
    struct ic_body *then_body = 0;
    /* our else body */
    struct ic_body *else_body = 0;

    /* current token */
    struct ic_token *token = 0;

    if (!token_list) {
        puts("ic_parse_stmt_if: token_list was null");
        return 0;
    }

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
    if (!stmt) {
        puts("ic_parse_stmt_if: call to ic_stmt_new failed");
        return 0;
    }

    /* consume if */
    token = ic_token_list_expect_important(token_list, IC_IF);
    if (!token) {
        puts("ic_parse_stmt_if: Failed to find `if` token");
        free(stmt);
        return 0;
    }

    /* parse if condition */
    expr = ic_parse_expr(token_list);
    if (!expr) {
        puts("ic_parse_stmt_if: call to ic_parse_expr failed");
        return 0;
    }

    /* save our expr on the body stmt */
    stmt->u.sif.expr = expr;

    /* FIXME check that our 'expr' has the type Bool */

    /* parse our body
     * do not consume end, it could be else or end
     */
    then_body = ic_parse_body(token_list, 0);
    if (!then_body) {
        puts("ic_parse_stmt_if: call to ic_parse_body failed");
        return 0;
    }

    /* save our body on the stmt */
    stmt->u.sif.then_body = then_body;

    token = ic_token_list_peek_important(token_list);
    if (!token) {
        puts("ic_parse_stmt_if: call to ic_token_list_peek_important failed");
        return 0;
    }

    if (token->id == IC_END) {
        /* consume end */
        token = ic_token_list_expect_important(token_list, IC_END);
        if (!token) {
            puts("ic_parse_stmt_if: unable to find end token");
            return 0;
        }
    } else if (token->id == IC_ELSE) {
        /* consume else */
        token = ic_token_list_expect_important(token_list, IC_ELSE);
        if (!token) {
            puts("ic_parse_stmt_if: unable to find else token");
            return 0;
        }
        /* parse else body, consume end */
        else_body = ic_parse_body(token_list, 0);
        if (!else_body) {
            puts("ic_parse_stmt_if: call to ic_parse_body failed");
            return 0;
        }

        stmt->u.sif.else_body = else_body;

        /* consume our end */
        token = ic_token_list_expect_important(token_list, IC_END);
        if (!token) {
            puts("ic_parse_stmt_if: unable to find end token");
            return 0;
        }
    } else {
        puts("ic_parse_stmt_if: failed to deal with end/else");
        return 0;
    }

    return stmt;
}

/* consume token
 * returns ic_stmt* on success
 * returns 0 on failure
 */
static struct ic_stmt *ic_parse_stmt_for(struct ic_token_list *token_list) {
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

    if (!token_list) {
        puts("ic_parse_stmt_for: token_list was null");
        return 0;
    }

    /* there are a few cases of if we care about at this point
     *
     *  for expr in iterator
     *      body
     *  end
     */

    stmt = ic_stmt_new(ic_stmt_type_for);
    if (!stmt) {
        puts("ic_parse_stmt_for: call to ic_stmt_new failed");
        return 0;
    }

    /* consume for */
    token = ic_token_list_expect_important(token_list, IC_FOR);
    if (!token) {
        puts("ic_parse_stmt_for: Failed to find `for` token");
        free(stmt);
        return 0;
    }

    /* parse expression */
    expr = ic_parse_expr(token_list);
    if (!expr) {
        puts("ic_parse_stmt_for: call to ic_parse_expr failed for expression");
        return 0;
    }

    /* save our expr on the body stmt */
    stmt->u.sfor.expr = expr;

    /* consume in */
    token = ic_token_list_expect_important(token_list, IC_IN);
    if (!token) {
        puts("ic_parse_stmt_for: Failed to find `in` token");
        free(stmt);
        return 0;
    }

    /* parse iterator */
    iterator = ic_parse_expr(token_list);
    if (!iterator) {
        puts("ic_parse_stmt_for: call to ic_parse_expr failed for iterator");
        return 0;
    }

    /* save our iterator on the body stmt */
    stmt->u.sfor.iterator = iterator;

    /* FIXME check types for expr and iterator */

    /* parse our body, consume end for us */
    body = ic_parse_body(token_list, 1);
    if (!body) {
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
static struct ic_stmt *ic_parse_stmt_while(struct ic_token_list *token_list) {
    /* out eventual return value */
    struct ic_stmt *stmt = 0;
    /* our condition expression */
    struct ic_expr *expr = 0;
    /* our body */
    struct ic_body *body = 0;

    /* current token */
    struct ic_token *token = 0;

    if (!token_list) {
        puts("ic_parse_stmt_while: token_list was null");
        return 0;
    }

    /* there are a few cases of if we care about at this point
     *
     *  while expr
     *      body
     *  end
     */

    stmt = ic_stmt_new(ic_stmt_type_while);
    if (!stmt) {
        puts("ic_parse_stmt_while: call to ic_stmt_new failed");
        return 0;
    }

    /* consume while */
    token = ic_token_list_expect_important(token_list, IC_WHILE);
    if (!token) {
        puts("ic_parse_stmt_while: Failed to find `if` token");
        free(stmt);
        return 0;
    }

    /* parse if condition */
    expr = ic_parse_expr(token_list);
    if (!expr) {
        puts("ic_parse_stmt_while: call to ic_parse_expr failed");
        return 0;
    }

    /* save our expr on the body stmt */
    stmt->u.swhile.expr = expr;

    /* FIXME check that our 'expr' has the type Bool */

    /* parse our body, consume end for us */
    body = ic_parse_body(token_list, 1);
    if (!body) {
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
static struct ic_stmt *ic_parse_stmt_expr(struct ic_token_list *token_list) {
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

    if (!token_list) {
        puts("ic_parse_stmt_expr: token_list was null");
        return 0;
    }

    expr = ic_parse_expr(token_list);
    if (!expr) {
        puts("ic_parse_stmt_expr: call to ic_parse_expr failed");
        return 0;
    }

    /* check for possible assignment */
    token = ic_token_list_peek_important(token_list);
    if (token && token->id == IC_ASSIGN) {
        /* if so, consume */
        token = ic_token_list_expect_important(token_list, IC_ASSIGN);
        if (!token) {
            puts("ic_parse_stmt_expr: failed to grab assign");
            return 0;
        }

        right = ic_parse_expr(token_list);

        if (!right) {
            puts("ic_parse_stmt_expr: call to ic_parse_expr failed for right side of assignment");
            return 0;
        }

        tag = ic_stmt_type_assign;
    }

    /* allocate and initialise */
    stmt = ic_stmt_new(tag);
    if (!stmt) {
        puts("ic_parse_stmt_expr: call to ic_stmt_new failed");
        return 0;
    }

    /* if we have a right then we have 2 exprs, we are an assign */
    if (right) {
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
    struct ic_stmt *(*func)(struct ic_token_list *token_list);
} ic_parse_table[] = {
    /* id          function    */
    {IC_IF, ic_parse_stmt_if},
    {IC_FOR, ic_parse_stmt_for},
    {IC_WHILE, ic_parse_stmt_while},
    {IC_LET, ic_parse_stmt_let},
    {IC_MATCH, ic_parse_stmt_match},
    {IC_RETURN, ic_parse_stmt_ret}
    /* otherwise we default to ic_parse_stmt_expr */
};

struct ic_stmt *ic_parse_stmt(struct ic_token_list *token_list) {
    /* offset into ic_parse_table */
    unsigned int pt_offset = 0;

    /* function to dispatch to */
    struct ic_stmt *(*func)(struct ic_token_list * token_list) = 0;

    /* return from call to func */
    struct ic_stmt *ret = 0;

    /* current token */
    struct ic_token *token = 0;

#ifdef DEBUG_PARSE_STMT
    puts("ic_parse_stmt: entered");
#endif

    if (!token_list) {
        puts("ic_parse_stmt: token_list was null");
        return 0;
    }

    token = ic_token_list_peek_important(token_list);
    if (!token) {
        puts("ic_parse_stmt: call to ic_token_list_peek_important failed");
        return 0;
    }

    for (pt_offset = 0; pt_offset < LENGTH(ic_parse_table); ++pt_offset) {

        if (token->id == ic_parse_table[pt_offset].id) {

            func = ic_parse_table[pt_offset].func;
            if (!func) {
                printf("ic_parse: Error matched with: ");
                ic_token_id_print_debug(stdout, token->id);
                puts("but parse table function was null, bailing");
                return 0;
            }
        }
    }

    /* at this point if we did not find a match then we
     * default to trying to parse it as an expr
     */
    if (!func) {
        func = ic_parse_stmt_expr;
    }

    /* out return value */
    ret = func(token_list);

    if (!ret) {
        puts("ic_parse_stmt: call to matched function failed");
        return 0;
    }

#ifdef DEBUG_PARSE_STMT
    puts("ic_parse_stmt: success! returning result");
#endif

    return ret;
}
