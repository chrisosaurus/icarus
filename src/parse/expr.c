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
static struct ic_expr * ic_parse_expr_fcall(struct ic_token_list *token_list, struct ic_expr *func_name){
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

    if( ! func_name ){
        puts("ic_parse_expr_fcall: func_name was null");
        return 0;
    }

    if( func_name->tag != ic_expr_type_identifier ){
        puts("ic_parse_expr_fcall: func_name was not an identifier");
        return 0;
    }

    /* build our return expr */
    expr = ic_expr_new(ic_expr_type_func_call);
    if( ! expr ){
        puts("ic_parse_expr_fcall: call to ic_expr_new failed");
        return 0;
    }

    /* build our function */
    if( ! ic_expr_func_call_init( &(expr->u.fcall), func_name) ){
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
    /* permissions */
    unsigned int permissions = 0;

    /* current token */
    struct ic_token *token = 0;

    if( ! token_list ){
        puts("ic_parse_expr_identifier: token_list was null");
        return 0;
    }

    /* parse permissions if present */
    permissions = ic_parse_permissions(token_list);

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
    if( ! ic_expr_identifier_init(id, ic_token_get_string(token), ic_token_get_string_length(token), permissions) ){
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
    struct ic_expr *expr = 0;

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
        expr =  ic_parse_expr_constant_string(token_list);
        if( ! expr ){
            puts("ic_parse_expr_single_token: call to ic_parse_expr_constant_string failed");
            return 0;
        }
        return expr;
    }

    if( token->id == IC_LITERAL_INTEGER ){
        expr =  ic_parse_expr_constant_integer(token_list);
        if( ! expr ){
            puts("ic_parse_expr_single_token: call to ic_parse_expr_constant_integer failed");
            return 0;
        }
        return expr;
    }

    /* otherwise assume this is just an identifier */
    expr = ic_parse_expr_identifier(token_list);
    if( ! expr ){
        puts("ic_parse_expr_single_token: call to ic_parse_expr_identifier failed");
        return 0;
    }

    return expr;
}

/* consume token
 * returns ic_expr* on success
 * returns 0 on failure
 */
static struct ic_expr * ic_parse_expr_operator(struct ic_token_list *token_list, struct ic_expr *left){
    /* our eventual return value */
    struct ic_expr *expr = 0;
    /* our internal operator */
    struct ic_expr_operator *operator = 0;

    /* an operator is made up of
     *  single-token operator expr
     */
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
        return 0;
    }

    /* initialise our operator */
    if( ! ic_expr_operator_init_binary(operator, left, right, token) ){
        puts("ic_parse_expr_operator: call to ic_expr_operator_init_binary failed");
        free(expr);
        free(right);
        return 0;    /* an operator is made up of
     *  single-token operator expr
     */
    }

    return expr;
}

/* consume token
 * returns ic_expr* on success
 * returns 0 on failure
 */
static struct ic_expr * ic_parse_expr_fieldaccess(struct ic_token_list *token_list, struct ic_expr *left){
    /* our eventual return value */
    struct ic_expr *expr = 0;

    /* our internal field access */
    struct ic_expr_faccess *faccess = 0;

    /* a field access is made up of
     * left_expr single-token
     */
    /* our right child, this must be an identifier */
    struct ic_expr *right = 0;

    /* operator token */
    struct ic_token *token = 0;

    if( ! token_list ){
        puts("ic_parse_expr_faccess: token_list was null");
        return 0;
    }

    /* build our new expr */
    expr = ic_expr_new(ic_expr_type_field_access);
    if( ! expr ){
        puts("ic_parse_expr_faccess: call to ic_expr_new failed");
        return 0;
    }

    /* op handling */
    token = ic_token_list_next_important(token_list);
    if( ! token ){
        puts("ic_parse_expr_faccess: operator call to token list next important failed");
        return 0;
    }

    /* our right expr can only be an identifier */
    right = ic_parse_expr_single_token(token_list);
    if( ! right ){
        puts("ic_parse_expr_faccess: right call to ic_parse_expr failed");
        free(expr);
        return 0;
    }

    /* check it is an identifier */
    if( right->tag != ic_expr_type_identifier ){
        puts("ic_parse_expr_faccess: right token was not an identifier");
        return 0;
    }

    /* get our internal faccess */
    faccess = ic_expr_get_faccess(expr);
    if( ! faccess ){
        puts("ic_parse_expr_faccess: call to ic_expr_get_faccess failed");
        return 0;
    }

    /* init our faccess */
    if( ! ic_expr_faccess_init(faccess, left, right) ){
        puts("ic_parse_expr_faccess: call to ic_expr_faccess_init failed");
        free(expr);
        free(left);
        free(right);
        return 0;    /* an operator is made up of
     *  single-token operator expr
     */
    }

    return expr;
}

/* consume token
 * returns ic_expr* on success
 * returns 0 on failure
 */
static struct ic_expr * ic_parse_expr_perm(struct ic_token_list *token_list){
    /* our eventual return value */
    struct ic_expr *expr = 0;

    /* our internal field identifier */
    struct ic_expr_identifier *id = 0;

    /* our permissions */
    unsigned int permissions = 0;

    /* operator token */
    struct ic_token *token = 0;

    if( ! token_list ){
        puts("ic_parse_expr_perm: token_list was null");
        return 0;
    }

    /* build our new expr */
    expr = ic_expr_new(ic_expr_type_identifier);
    if( ! expr ){
        puts("ic_parse_expr_perm: call to ic_expr_new failed");
        return 0;
    }

    permissions = ic_parse_permissions(token_list);

    /* make parse_expr_identifier do the hard work */
    expr = ic_parse_expr_identifier(token_list);
    if( ! expr ){
        puts("ic_parse_expr_perm: call to ic_parse_expr_identifier failed");
        return 0;
    }

    if( expr->tag != ic_expr_type_identifier ){
        puts("ic_parse_expr_perm: call to ic_parse_expr_identifier gave back a non-identifier");
        return 0;
    }

    /* get our internal identifier */
    id = ic_expr_get_identifier(expr);
    if( ! id ){
        puts("ic_parse_expr_perm: call to ic_expr_get_identifier failed");
        return 0;
    }

    id->permissions = permissions;

    return expr;
}

struct ic_expr * ic_parse_expr(struct ic_token_list *token_list){
    /* current token */
    struct ic_token *token = 0;

    /* the expression we have built so far */
    struct ic_expr *current = 0;

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

    /* we need to have a pointer to the last expr parsed, we will call this `current`
     * we will no longer need to peek ahead at next
     *
     * forever :
     *      if token is an operator:
     *           current = operator(current)
     *           continue
     *
     *      if token is a field access:
     *           current = fieldaccess(current)
     *           continue
     *
     *      if token is a l bracket:
     *          current = fcall(current)
     *          continue
     *
     *      if current:
     *          # expression ended
     *          return current
     *
     *      # otherwise a single token
     *      current = single()
     *
     */

    while( 1 ){
        /* fcalls can span across lines
         * field accesses can span across lines
         *
         * operators can only under certain cases
         *
         *      left +
         *      right
         *
         * is allowed
         *
         *
         * however the following is illegal
         *
         *      left
         *      +right
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

        /* if we see a permission */
        if( ic_token_ispermission(token) ){
            /* if we have no current then we are a perm */
            if( ! current ){
                return ic_parse_expr_perm(token_list);
            }

            /* if there is an eol between here and the perm
             * then the perm is on the next line
             * this expression is thus terminated
             */
            if( ic_token_list_peek_iseol(token_list) ){
                return current;
            }
        }

        if( token->id == IC_ASSIGN) {
            /* if we see an assignment then this is as assignment
             *
             * for assignment we return current, and our caller must deal with it,
             * this is because assignment is a stmt and not an expr
             */

            if( ! current ){
                puts("ic_parse_expr: encountered assignment with no left expr");
                return 0;
            }

            return current;

        } else if( token->id == IC_PERIOD ){
            /* field access time */

            if( ! current ){
                puts("ic_parse_expr: encountered field access with no left expr");
                return 0;
            }

            current = ic_parse_expr_fieldaccess(token_list, current);
            if( ! current ){
                puts("ic_parse_expr: call to ic_parse_expr_field_access failed");
                return 0;
            }

            continue;

        } else if( ic_token_isoperator(token) ){
            /* field access time */

            if( ! current ){
                puts("ic_parse_expr: encountered operator with no left expr");
                return 0;
            }

            current = ic_parse_expr_operator(token_list, current);
            if( ! current ){
                puts("ic_parse_expr: call to ic_parse_expr_operator failed");
                return 0;
            }

            continue;

        } else if( token->id == IC_LRBRACKET ){
            /* this is a function call */

            if( ! current ){
                puts("ic_parse_expr: encountered function call with no left expr");
                return 0;
            }

            current = ic_parse_expr_fcall(token_list, current);
            if( ! current ){
                puts("ic_parse_expr: call to ic_parse_expr_fcall failed");
                return 0;
            }

            continue;

        } else if( current ){
            /* we already have an expr in current
             * and we did not see a field access or operator
             * then this expression has ended
             */
            return current;
        }

        /* otherwise parse a single token and continue */
        current = ic_parse_expr_single_token(token_list);
        if( ! current ){
            puts("ic_parse_expr: call to ic_parse_expr_single_token failed");
            return 0;
        }
    }

    puts("ic_parse_expr: impossible case");
    return 0;
}

