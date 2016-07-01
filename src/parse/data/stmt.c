#include <stdio.h>  /* puts */
#include <stdlib.h> /* calloc */

#include "../../data/symbol.h"
#include "../parse.h"
#include "body.h"
#include "stmt.h"

/* allocate and initialise a new return
 * does not touch init ic_expr
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_stmt_ret *ic_stmt_ret_new(void) {
    struct ic_stmt_ret *ret = 0;

    /* alloc */
    ret = calloc(1, sizeof(struct ic_stmt_ret));
    if (!ret) {
        puts("ic_stmt_ret: call to calloc failed");
        return 0;
    }

    /* init */
    if (!ic_stmt_ret_init(ret)) {
        puts("ic_stmt_ret: call to ic_stmt_ret_init failed");
        return 0;
    }

    /* success */
    return ret;
}

/* initialise an existing return
 * does not touch the init expression
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_ret_init(struct ic_stmt_ret *ret) {
    if (!ret) {
        puts("ic_stmt_ret_init: ret was null ");
        return 0;
    }

    /* only work to do is to ensure our ic_expr is zerod */
    ret->ret = 0;

    return 1;
}

/* destroy ret
 *
 * will only free ret if `free_ret` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_ret_destroy(struct ic_stmt_ret *ret, unsigned int free_ret) {
    if (!ret) {
        puts("ic_stmt_ret_destroy: ret was null ");
        return 0;
    }

    if (ret->ret) {
        /* dispatch to expr destroy
         * here we set free_expr as ret->ret is
         * a pointer membe
         */
        if (!ic_expr_destroy(ret->ret, 1)) {
            puts("ic_stmt_ret_destroy: call ot ic_expr_destroy failed");
            return 0;
        }
    }

    /* if asked nicely */
    if (free_ret) {
        free(ret);
    }

    return 1;
}

/* get the ic_expr * contained within
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr *ic_stmt_ret_get_expr(struct ic_stmt_ret *ret) {
    if (!ret) {
        puts("ic_stmt_ret_get_expr: ret was null");
        return 0;
    }

    /* give the caller what they want */
    return ret->ret;
}

/* print this return */
void ic_stmt_ret_print(struct ic_stmt_ret *ret, unsigned int *indent_level) {
    /* our fake indent for our expr */
    unsigned int fake_indent = 0;

    if (!ret) {
        return;
    }
    if (!indent_level) {
        return;
    }

    /* indent */
    ic_parse_print_indent(*indent_level);

    /* print out 'return ' */
    fputs("return ", stdout);

    /* dispatch to expr print */
    ic_expr_print(ret->ret, &fake_indent);

    /* print out trailing '\n' */
    puts("");
}

/* allocate and initialise a new let
 * does not touch init ic_expr
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_stmt_let *ic_stmt_let_new(char *id_src, unsigned int id_len, unsigned int permissions) {
    struct ic_stmt_let *let = 0;

    /* alloc */
    let = calloc(1, sizeof(struct ic_stmt_let));
    if (!let) {
        puts("ic_stmt_let_new: calloc failed");
        return 0;
    }

    /* hand over for init
     * NB: we leave arg checking up to init
     */
    if (!ic_stmt_let_init(let, id_src, id_len, permissions)) {
        puts("ic_stmt_let_new: call to ic_stmt_let_init failed");
        free(let);
        return 0;
    }

    return let;
}

/* initialise an existing let
 * does not touch the init expression
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_let_init(struct ic_stmt_let *let, char *id_src, unsigned int id_len, unsigned int permissions) {
    if (!let) {
        puts("ic_stmt_let_init: let was null");
        return 0;
    }

    if (!id_src) {
        puts("ic_stmt_let_init: id_src was null");
        return 0;
    }

    /* dispatch to symbol init for id */
    if (!ic_symbol_init(&(let->identifier), id_src, id_len)) {
        puts("ic_smtm_let_init: call to ic_symbol_init for id failed");
        return 0;
    }

    /* zero out init */
    let->init = 0;

    let->declared_type = 0;
    let->inferred_type = 0;

    let->permissions = permissions;

    return 1;
}

/* destroy let
 *
 * will only free let if `free_let` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_let_destroy(struct ic_stmt_let *let, unsigned int free_let) {
    if (!let) {
        puts("ic_stmt_let_destroy: let was null");
        return 0;
    }

    /* free = 0 as member */
    if (!ic_symbol_destroy(&(let->identifier), 0)) {
        puts("ic_stmt_let_destroy: identifier called to ic_symbol_destroy failed");
        return 0;
    }

    /* free = 1 as pointer */
    if (let->declared_type) {
        if (!ic_symbol_destroy(let->declared_type, 1)) {
            puts("ic_stmt_let_destroy: type called to ic_symbol_destroy failed");
            return 0;
        }
    }

    if (let->init) {
        /* free = 1 as pointer member */
        if (!ic_expr_destroy(let->init, 0)) {
            puts("ic_stmt_let_destroy: called to ic_expr_destroy failed");
            return 0;
        }
    }

    /* if asked nicely */
    if (free_let) {
        free(let);
    }

    return 1;
}

/* set declared type on this let
 *
 * this sets the symbol `declared_type`
 *
 * this is used for types declared in source,
 * this is set at parse time
 *
 * this is an error if either types have already been set
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_let_set_declared_type(struct ic_stmt_let *let, char *type_src, unsigned int type_len) {
    if (!let) {
        puts("ic_stmt_let_set_declared_type: let was null");
        return 0;
    }

    if (!type_src) {
        puts("ic_stmt_let_set_declared_type: type_src was null");
        return 0;
    }

    if (!type_len) {
        puts("ic_stmt_let_set_declared_type: type_len was 0");
        return 0;
    }

    if (let->declared_type) {
        puts("ic_stmt_let_set_declared_type: let already had declared type");
        return 0;
    }

    if (let->inferred_type) {
        puts("ic_stmt_let_set_declared_type: let already had declared type");
        return 0;
    }

    let->declared_type = ic_symbol_new(type_src, type_len);
    if (!let->declared_type) {
        puts("ic_smtm_let_declared_init: call to ic_symbol_new for type failed");
        return 0;
    }

    return 1;
}

/* set inferred type on this let
 *
 * this sets the type ref `inferred_type`
 *
 * this is used for the type we know of
 * at analsyis time, this could be from
 * the declared type in source or via
 * the type of the init. expr.
 *
 * this is set at analysis time
 *
 * this is an error if inferred type is already set
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_let_set_inferred_type(struct ic_stmt_let *let, struct ic_type *type) {
    if (!let) {
        puts("ic_stmt_let_set_inferred_type: let was null");
        return 0;
    }

    if (!type) {
        puts("ic_stmt_let_set_inferred_type: type was null");
        return 0;
    }

    if (let->inferred_type) {
        puts("ic_stmt_let_set_inferred_type: let already had inferred type");
        return 0;
    }

    let->inferred_type = type;

    return 1;
}

/* get the ic_expr * contained within
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr *ic_stmt_let_get_expr(struct ic_stmt_let *let) {
    if (!let) {
        puts("ic_stmt_let_get_expr: let was null");
        return 0;
    }

    /* return what they want */
    return let->init;
}

/* print this let */
void ic_stmt_let_print(struct ic_stmt_let *let, unsigned int *indent_level) {
    /* our fake indent for our subexpr */
    unsigned int fake_indent = 0;

    if (!let) {
        puts("ic_stmt_let_print: let was null");
        return;
    }
    if (!indent_level) {
        puts("ic_stmt_let_print: indent_level was null");
        return;
    }

    /* output indent */
    ic_parse_print_indent(*indent_level);

    /* want to output
     * let identifier::type = init
     */

    fputs("let ", stdout);

    ic_symbol_print(&(let->identifier));

    /* declared type on let is optional */
    if (let->declared_type) {
        fputs("::", stdout);
        ic_symbol_print(let->declared_type);
    }

    fputs(" = ", stdout);

    ic_expr_print(let->init, &fake_indent);

    /* statements are displayed on their own line */
    puts("");
}

/* allocate and initialise a new assign
 * does not touch init ic_expr
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_stmt_assign *ic_stmt_assign_new(struct ic_expr *left, struct ic_expr *right) {
    struct ic_stmt_assign *assign = 0;

    if (!left) {
        puts("ic_stmt_assign_new: left was null");
        return 0;
    }

    if (!right) {
        puts("ic_stmt_assign_new: right was null");
        return 0;
    }

    /* alloc */
    assign = calloc(1, sizeof(struct ic_stmt_assign));
    if (!assign) {
        puts("ic_stmt_assign_new: calloc failed");
        return 0;
    }

    /* hand over for init
     * NB: we leave arg checking up to init
     */
    if (!ic_stmt_assign_init(assign, left, right)) {
        puts("ic_stmt_assign_new: call to ic_stmt_assign_init failed");
        free(assign);
        return 0;
    }

    return assign;
}

/* initialise an existing assign
 * does not touch the init expression
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_assign_init(struct ic_stmt_assign *assign, struct ic_expr *left, struct ic_expr *right) {
    if (!assign) {
        puts("ic_stmt_assign_init: assign was null");
        return 0;
    }

    if (!left) {
        puts("ic_stmt_assign_init: left was null");
        return 0;
    }

    if (!right) {
        puts("ic_stmt_assign_init: right was null");
        return 0;
    }

    assign->left = left;
    assign->right = right;

    return 1;
}

/* destroy assign
 *
 * will only free assign if `free_assign` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_assign_destroy(struct ic_stmt_assign *assign, unsigned int free_assign) {
    if (!assign) {
        puts("ic_stmt_assign_destroy: assign was null");
        return 0;
    }

    /* FIXME who frees left and right ? */

    if (free_assign) {
        free(assign);
    }

    return 1;
}

/* get the left ic_expr * contained within
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr *ic_stmt_assign_get_left(struct ic_stmt_assign *assign) {
    if (!assign) {
        puts("ic_stmt_assign_left: assign was null");
        return 0;
    }

    return assign->left;
}

/* get the right ic_expr * contained within
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr *ic_stmt_assign_get_right(struct ic_stmt_assign *assign) {
    if (!assign) {
        puts("ic_stmt_assign_get_right: assign was null");
        return 0;
    }

    return assign->right;
}

/* print this assign */
void ic_stmt_assign_print(struct ic_stmt_assign *assign, unsigned int *indent_level) {
    /* our fake indent for our subexpr */
    unsigned int fake_indent = 0;

    if (!assign) {
        puts("ic_stmt_assign_init: assign was null");
        return;
    }

    if (!indent_level) {
        puts("ic_stmt_assign_print: indent_level was null");
        return;
    }

    /* output indent */
    ic_parse_print_indent(*indent_level);

    /* want to output
     * left = right
     */

    fake_indent = 0;
    ic_expr_print(assign->left, &fake_indent);

    fputs(" = ", stdout);

    fake_indent = 0;
    ic_expr_print(assign->right, &fake_indent);

    /* statements are displayed on their own line */
    puts("");
}

/* allocate and initialise a new ic_stmtm_if
 * this will initialise the body
 * but will NOT initialise the expression
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_stmt_if *ic_stmt_if_new(void) {
    struct ic_stmt_if *sif = 0;

    /* alloc */
    sif = calloc(1, sizeof(struct ic_stmt_if));
    if (!sif) {
        puts("ic_stmt_if_new: calloc failed");
        return 0;
    }

    /* init */
    if (!ic_stmt_if_init(sif)) {
        puts("ic_stmt_if_new: call to ic_stmt_if_init failed");
        free(sif);
        return 0;
    }

    return sif;
}

/* initialise an existing new ic_stmtm_if
 * this will initialise the body
 * but will NOT initialise the expression
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_if_init(struct ic_stmt_if *sif) {
    if (!sif) {
        puts("ic_stmt_if_init: sif was null");
        return 0;
    }

    /* just zero out expr and body*/
    sif->expr = 0;
    sif->body = 0;

    /* return success */
    return 1;
}

/* destroy if
 *
 * only frees stmt_if if `free_if` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_if_destroy(struct ic_stmt_if *sif, unsigned int free_if) {
    if (!sif) {
        puts("ic_stmt_if_destroy: sif was null");
        return 0;
    }

    if (sif->expr) {
        /* free_expr = 1 as pointer member */
        if (!ic_expr_destroy(sif->expr, 1)) {
            puts("ic_stmt_if_destroy: call to ic_expr_destroy failed");
            return 0;
        }
    }

    /* free_body = 0 as member */
    if (!ic_body_destroy(sif->body, 0)) {
        puts("ic_stmt_if_destroy: call to ic_body_destroy failed");
        return 0;
    }

    if (free_if) {
        free(sif);
    }

    return 1;
}

/* returns pointer on success
 * returns 0 on failure
 */
struct ic_expr *ic_stmt_if_get_expr(struct ic_stmt_if *sif) {
    if (!sif) {
        puts("ic_stmt_if_get_expr: sif was null");
        return 0;
    }

    /* return our expr innards */
    return sif->expr;
}

/* get statement of offset i within the body
 *
 * returns pointer to element on success
 * returns 0 on failure
 */
struct ic_stmt *ic_stmt_if_get_stmt(struct ic_stmt_if *sif, unsigned int i) {
    if (!sif) {
        puts("ic_stmt_if_get_stmt: sif was null");
        return 0;
    }

    /* let body do the lifting */
    return ic_body_get(sif->body, i);
}

/* get length of body
 *
 * returns length on success
 * returns 0 on failure
 */
unsigned int ic_stmt_if_length(struct ic_stmt_if *sif) {
    if (!sif) {
        puts("ic_stmt_if_length: sif was null");
        return 0;
    }

    /* let body do the lifting */
    return ic_body_length(sif->body);
}

/* print this if */
void ic_stmt_if_print(struct ic_stmt_if *sif, unsigned int *indent_level) {
    /* our fake indent for our subexpr */
    unsigned int fake_indent = 0;

    if (!sif) {
        puts("ic_stmt_if_print: sif was null");
        return;
    }
    if (!indent_level) {
        puts("ic_stmt_if_print: indent_level was null");
        return;
    }

    /* print indent */
    ic_parse_print_indent(*indent_level);

    /* we want to print
     *  if expr
     *      body
     *  end
     */
    fputs("if ", stdout);
    ic_expr_print(sif->expr, &fake_indent);
    puts("");

    /* print body
     * body will handle incr and decr of the indent level
     */
    ic_body_print(sif->body, indent_level);

    /* statements are displayed on their own line */
    /* print indent */
    ic_parse_print_indent(*indent_level);
    puts("end");
}

/* allocate and initialise a new ic_stmt_for
 * this will initialise the body
 * but will NOT initialise the expression OR the iterator
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_stmt_for *ic_stmt_for_new(void) {
    struct ic_stmt_for *sfor = 0;

    /* alloc */
    sfor = calloc(1, sizeof(struct ic_stmt_for));
    if (!sfor) {
        puts("ic_stmt_for_new: calloc failed");
        return 0;
    }

    /* init */
    if (!ic_stmt_for_init(sfor)) {
        puts("ic_stmt_for_new: call to ic_stmt_for_init failed");
        free(sfor);
        return 0;
    }

    return sfor;
}

/* initialise an existing new ic_stmt_for
 * this will initialise the body
 * but will NOT initialise the expression
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_for_init(struct ic_stmt_for *sfor) {
    if (!sfor) {
        puts("ic_stmt_for_init: sfor was null");
        return 0;
    }

    /* just zero out expr, iterator and body*/
    sfor->expr = 0;
    sfor->iterator = 0;
    sfor->body = 0;

    /* return success */
    return 1;
}

/* destroy for
 *
 * only frees stmt_for if `free_for` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_for_destroy(struct ic_stmt_for *sfor, unsigned int free_for) {
    if (!sfor) {
        puts("ic_stmt_for_destroy: sfor was null");
        return 0;
    }

    if (sfor->expr) {
        /* free_expr = 1 as pointer member */
        if (!ic_expr_destroy(sfor->expr, 1)) {
            puts("ic_stmt_for_destroy: call to ic_expr_destroy failed for expr");
            return 0;
        }
    }

    if (sfor->iterator) {
        /* free_expr = 1 as pointer member */
        if (!ic_expr_destroy(sfor->iterator, 1)) {
            puts("ic_stmt_for_destroy: call to ic_expr_destroy failed for iterator");
            return 0;
        }
    }

    /* free_body = 0 as member */
    if (!ic_body_destroy(sfor->body, 0)) {
        puts("ic_stmt_for_destroy: call to ic_body_destroy failed");
        return 0;
    }

    if (free_for) {
        free(sfor);
    }

    return 1;
}

/* returns pointer on success
 * returns 0 on failure
 */
struct ic_expr *ic_stmt_for_get_expr(struct ic_stmt_for *sfor) {
    if (!sfor) {
        puts("ic_stmt_for_get_expr: sfor was null");
        return 0;
    }

    /* return our expr innards */
    return sfor->expr;
}

/* returns pointer on success
 * returns 0 on failure
 */
struct ic_expr *ic_stmt_for_get_iterator(struct ic_stmt_for *sfor) {
    if (!sfor) {
        puts("ic_stmt_for_get_iterator: sfor was null");
        return 0;
    }

    /* return our iterator innards */
    return sfor->iterator;
}

/* get statement of offset i within the body
 *
 * returns pointer to element on success
 * returns 0 on failure
 */
struct ic_stmt *ic_stmt_for_get_stmt(struct ic_stmt_for *sfor, unsigned int i) {
    if (!sfor) {
        puts("ic_stmt_for_get_stmt: sfor was null");
        return 0;
    }

    /* let body do the lifting */
    return ic_body_get(sfor->body, i);
}

/* get length of body
 *
 * returns length on success
 * returns 0 on failure
 */
unsigned int ic_stmt_for_length(struct ic_stmt_for *sfor) {
    if (!sfor) {
        puts("ic_stmt_for_length: sfor was null");
        return 0;
    }

    /* let body do the lifting */
    return ic_body_length(sfor->body);
}

/* print this for */
void ic_stmt_for_print(struct ic_stmt_for *sfor, unsigned int *indent_level) {
    /* our fake indent for our subexpr */
    unsigned int fake_indent = 0;

    if (!sfor) {
        puts("ic_stmt_for_print: sfor was null");
        return;
    }
    if (!indent_level) {
        puts("ic_stmt_for_print: indent_level was null");
        return;
    }

    /* print indent */
    ic_parse_print_indent(*indent_level);

    /* we want to print
     *  for expr in iterator
     *      body
     *  end
     */
    fputs("for ", stdout);
    ic_expr_print(sfor->expr, &fake_indent);
    fputs(" in ", stdout);
    ic_expr_print(sfor->iterator, &fake_indent);
    puts("");

    /* print body
     * body will handle incr and decr of the indent level
     */
    ic_body_print(sfor->body, indent_level);

    /* statements are displayed on their own line */
    /* print indent */
    ic_parse_print_indent(*indent_level);
    puts("end");
}

/* allocate and initialise a new ic_stmt_while
 * this will initialise the body
 * but will NOT initialise the expression
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_stmt_while *ic_stmt_while_new(void) {
    struct ic_stmt_while *swhile = 0;

    /* alloc */
    swhile = calloc(1, sizeof(struct ic_stmt_while));
    if (!swhile) {
        puts("ic_stmt_while_new: calloc failed");
        return 0;
    }

    /* init */
    if (!ic_stmt_while_init(swhile)) {
        puts("ic_stmt_while_new: call to ic_stmt_while_init failed");
        free(swhile);
        return 0;
    }

    return swhile;
}

/* initialise an existing new ic_stmt_while
 * this will initialise the body
 * but will NOT initialise the expression
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_while_init(struct ic_stmt_while *swhile) {
    if (!swhile) {
        puts("ic_stmt_while_init: swhile was null");
        return 0;
    }

    /* just zero out expr and body*/
    swhile->expr = 0;
    swhile->body = 0;

    /* return success */
    return 1;
}

/* destroy while
 *
 * only frees stmt_while if `free_while` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_while_destroy(struct ic_stmt_while *swhile, unsigned int free_while) {
    if (!swhile) {
        puts("ic_stmt_while_destroy: swhile was null");
        return 0;
    }

    if (swhile->expr) {
        /* free_expr = 1 as pointer member */
        if (!ic_expr_destroy(swhile->expr, 1)) {
            puts("ic_stmt_while_destroy: call to ic_expr_destroy failed");
            return 0;
        }
    }

    /* free_body = 0 as member */
    if (!ic_body_destroy(swhile->body, 0)) {
        puts("ic_stmt_while_destroy: call to ic_body_destroy failed");
        return 0;
    }

    if (free_while) {
        free(swhile);
    }

    return 1;
}

/* returns pointer on success
 * returns 0 on failure
 */
struct ic_expr *ic_stmt_while_get_expr(struct ic_stmt_while *swhile) {
    if (!swhile) {
        puts("ic_stmt_while_get_expr: swhile was null");
        return 0;
    }

    /* return our expr innards */
    return swhile->expr;
}

/* get statement of offset i within the body
 *
 * returns pointer to element on success
 * returns 0 on failure
 */
struct ic_stmt *ic_stmt_while_get_stmt(struct ic_stmt_while *swhile, unsigned int i) {
    if (!swhile) {
        puts("ic_stmt_while_get_stmt: swhile was null");
        return 0;
    }

    /* let body do the lifting */
    return ic_body_get(swhile->body, i);
}

/* get length of body
 *
 * returns length on success
 * returns 0 on failure
 */
unsigned int ic_stmt_while_length(struct ic_stmt_while *swhile) {
    if (!swhile) {
        puts("ic_stmt_while_length: swhile was null");
        return 0;
    }

    /* let body do the lifting */
    return ic_body_length(swhile->body);
}

/* print this if */
void ic_stmt_while_print(struct ic_stmt_while *swhile, unsigned int *indent_level) {
    /* our fake indent for our subexpr */
    unsigned int fake_indent = 0;

    if (!swhile) {
        puts("ic_stmt_while_print: swhile was null");
        return;
    }
    if (!indent_level) {
        puts("ic_stmt_while_print: indent_level was null");
        return;
    }

    /* print indent */
    ic_parse_print_indent(*indent_level);

    /* we want to print
     *  while expr
     *      body
     *  end
     */
    fputs("while ", stdout);
    ic_expr_print(swhile->expr, &fake_indent);
    puts("");

    /* print body
     * body will handle incr and decr of the indent level
     */
    ic_body_print(swhile->body, indent_level);

    /* statements are displayed on their own line */
    /* print indent */
    ic_parse_print_indent(*indent_level);
    puts("end");
}

/* allocate and initialise a new ic_stmt
 * will not initialise union members
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_stmt *ic_stmt_new(enum ic_stmt_tag tag) {
    struct ic_stmt *stmt = 0;

    /* alloc */
    stmt = calloc(1, sizeof(struct ic_stmt));
    if (!stmt) {
        puts("ic_stmt_new: calloc failed");
        return 0;
    }

    /* init */
    if (!ic_stmt_init(stmt, tag)) {
        puts("ic_stmt_new: call to ic_stmt_init failed");
        return 0;
    }

    return stmt;
}

/* initialise an existing ic_stmt
 *
 * returns 1 on success
 * returns 0 on failure
 */
int ic_stmt_init(struct ic_stmt *stmt, enum ic_stmt_tag tag) {
    if (!stmt) {
        puts("ic_stmt_init: stmt was null");
        return 0;
    }

    /* only initialise type */
    stmt->tag = tag;

    /* do not touch union */

    return 1;
}

/* destroy stmt
 *
 * will only free stmt if `free_stmt` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_destroy(struct ic_stmt *stmt, unsigned int free_stmt) {
    if (!stmt) {
        puts("ic_stmt_destroy: stmt was null");
        return 0;
    }

    /* dispatch for appropritte subtype */
    switch (stmt->tag) {
        case ic_stmt_type_ret:
            /* do not free as member */
            if (!ic_stmt_ret_destroy(&(stmt->u.ret), 0)) {
                puts("ic_stmt_destroy: call to ic_stmt_ret_destroy failed");
                return 0;
            }
            break;

        case ic_stmt_type_let:
            /* do not free as member */
            if (!ic_stmt_let_destroy(&(stmt->u.let), 0)) {
                puts("ic_stmt_destroy: call to ic_stmt_let_destroy failed");
                return 0;
            }
            break;

        case ic_stmt_type_if:
            /* do not free as member */
            if (!ic_stmt_if_destroy(&(stmt->u.sif), 0)) {
                puts("ic_stmt_destroy: call to ic_stmt_if_destroy failed");
                return 0;
            }
            break;

        case ic_stmt_type_for:
            /* do not free as member */
            if (!ic_stmt_for_destroy(&(stmt->u.sfor), 0)) {
                puts("ic_stmt_destroy: call to ic_stmt_for_destroy failed");
                return 0;
            }
            break;

        case ic_stmt_type_while:
            /* do not free as member */
            if (!ic_stmt_while_destroy(&(stmt->u.swhile), 0)) {
                puts("ic_stmt_destroy: call to ic_stmt_while_destroy failed");
                return 0;
            }
            break;

        case ic_stmt_type_expr:
            if (stmt->u.expr) {
                /* free_expr as pointer member */
                if (!ic_expr_destroy(stmt->u.expr, 1)) {
                    puts("ic_stmt_destroy: call to ic_expr_destroy failed");
                    return 0;
                }
            }

            break;

        case ic_stmt_type_assign:
            /* do not free as member */
            if (!ic_stmt_assign_destroy(&(stmt->u.assign), 0)) {
                puts("ic_stmt_destroy: call to ic_stmt_assign_destroy failed");
                return 0;
            }

            break;

        default:
            puts("ic_stmt_destroy: impossible stmt type");
            return 0;
            break;
    }

    /* free if asked */
    if (free_stmt) {
        free(stmt);
    }

    return 1;
}

/* get a pointer to the return within
 * will only succeed if ic_stmt is of the correct type
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_stmt_ret *ic_stmt_get_ret(struct ic_stmt *stmt) {
    if (!stmt) {
        puts("ic_stmt_get_ret: stmt was null");
        return 0;
    }

    /* check type before giving out */
    if (stmt->tag != ic_stmt_type_ret) {
        puts("ic_stmt_get_ret: not of the correct type");
        return 0;
    }

    /* otherwise give them what they asked for */
    return &(stmt->u.ret);
}

/* get a pointer to the let within
 * will only succeed if ic_stmt is of the correct type
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_stmt_let *ic_stmt_get_let(struct ic_stmt *stmt) {
    if (!stmt) {
        puts("ic_stmt_get_let: stmt was null");
        return 0;
    }

    /* check type before giving out */
    if (stmt->tag != ic_stmt_type_let) {
        puts("ic_stmt_get_let: not of the correct type");
        return 0;
    }

    /* otherwise give them what they asked for */
    return &(stmt->u.let);
}

/* get a pointer to the assign within
 * will only succeed if ic_stmt is of the correct type
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_stmt_assign *ic_stmt_get_assign(struct ic_stmt *stmt) {
    if (!stmt) {
        puts("ic_stmt_get_assign: stmt was null");
        return 0;
    }

    /* check type before giving out */
    if (stmt->tag != ic_stmt_type_assign) {
        puts("ic_stmt_get_assign: not of the correct type");
        return 0;
    }

    /* otherwise give them what they asked for */
    return &(stmt->u.assign);
}

/* get a pointer to the sif within
 * will only succeed if ic_stmt is of the correct type
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_stmt_if *ic_stmt_get_sif(struct ic_stmt *stmt) {
    if (!stmt) {
        puts("ic_stmt_get_sif: stmt was null");
        return 0;
    }

    /* check type before giving out */
    if (stmt->tag != ic_stmt_type_if) {
        puts("ic_stmt_get_sif: not of the correct type");
        return 0;
    }

    /* otherwise give them what they asked for */
    return &(stmt->u.sif);
}

/* get a pointer to the sfor within
 * will only succeed if ic_stmt is of the correct type
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_stmt_for *ic_stmt_get_sfor(struct ic_stmt *stmt) {
    if (!stmt) {
        puts("ic_stmt_get_sfor: stmt was null");
        return 0;
    }

    /* check type before giving out */
    if (stmt->tag != ic_stmt_type_for) {
        puts("ic_stmt_get_sfor: not of the correct type");
        return 0;
    }

    /* otherwise give them what they asked for */
    return &(stmt->u.sfor);
}

/* get a pointer to the swhile within
 * will only succeed if ic_stmt is of the correct type
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_stmt_while *ic_stmt_get_swhile(struct ic_stmt *stmt) {
    if (!stmt) {
        puts("ic_stmt_get_swhile: stmt was null");
        return 0;
    }

    /* check type before giving out */
    if (stmt->tag != ic_stmt_type_while) {
        puts("ic_stmt_get_swhile: not of the correct type");
        return 0;
    }

    /* otherwise give them what they asked for */
    return &(stmt->u.swhile);
}

/* get a pointer to the expr within
 * will only succeed if ic_stmt is of the correct type
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr *ic_stmt_get_expr(struct ic_stmt *stmt) {
    if (!stmt) {
        puts("ic_stmt_get_expr: stmt was null");
        return 0;
    }

    /* check type before giving out */
    if (stmt->tag != ic_stmt_type_expr) {
        puts("ic_stmt_get_expr: not of the correct type");
        return 0;
    }

    /* otherwise give them what they asked for */
    return stmt->u.expr;
}

/* print this stmt */
void ic_stmt_print(struct ic_stmt *stmt, unsigned int *indent_level) {
    if (!stmt) {
        puts("ic_stmt_print: stmt was null");
        return;
    }
    if (!indent_level) {
        puts("ic_stmt_print: indent_level was null");
        return;
    }

    switch (stmt->tag) {
        case ic_stmt_type_ret:
            ic_stmt_ret_print(&(stmt->u.ret), indent_level);
            break;

        case ic_stmt_type_let:
            ic_stmt_let_print(&(stmt->u.let), indent_level);
            break;

        case ic_stmt_type_assign:
            ic_stmt_assign_print(&(stmt->u.assign), indent_level);
            break;

        case ic_stmt_type_if:
            ic_stmt_if_print(&(stmt->u.sif), indent_level);
            break;

        case ic_stmt_type_for:
            ic_stmt_for_print(&(stmt->u.sfor), indent_level);
            break;

        case ic_stmt_type_while:
            ic_stmt_while_print(&(stmt->u.swhile), indent_level);
            break;

        case ic_stmt_type_expr:
            ic_expr_print(stmt->u.expr, indent_level);
            /* statements are displayed on their own line */
            puts("");
            break;

        default:
            puts("ic_stmt_print: unknown type");
            return;
    }
}
