#include <stdio.h>  /* puts */
#include <stdlib.h> /* calloc */

#include "../../data/symbol.h"
#include "../parse.h"
#include "../permissions.h"
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
void ic_stmt_ret_print(FILE *fd, struct ic_stmt_ret *ret, unsigned int *indent_level) {
    /* our fake indent for our expr */
    unsigned int fake_indent = 0;

    if (!ret) {
        return;
    }
    if (!indent_level) {
        return;
    }

    /* indent */
    ic_parse_print_indent(fd, *indent_level);

    /* print out 'return ' */
    fputs("return ", fd);

    /* dispatch to expr print */
    ic_expr_print(fd, ret->ret, &fake_indent);

    /* print out trailing '\n' */
    fputs("\n", fd);
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

    let->tref = ic_type_ref_new();
    if (!let->tref) {
        puts("ic_smtm_let_init: call to ic_type_ref_new for id failed");
        return 0;
    }

    /* zero out init */
    let->init = 0;

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

    if (!ic_type_ref_destroy(let->tref, 1)) {
        puts("ic_stmt_let_destroy: type called to ic_type_ref_destroy failed");
        return 0;
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
 * this is an error if the type has already been set
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_let_set_declared_type(struct ic_stmt_let *let, struct ic_type_ref *type_ref) {
    if (!let) {
        puts("ic_stmt_let_set_declared_type: let was null");
        return 0;
    }

    if (!type_ref) {
        puts("ic_stmt_let_set_declared_type: type_ref was null");
        return 0;
    }

    if (let->tref->tag != ic_type_ref_unknown) {
        puts("ic_stmt_let_set_declared_type: let tref as already set");
        return 0;
    }

    if (!ic_type_ref_destroy(let->tref, 1)) {
        /* TODO FIXME this seems wasteful */
        puts("ic_stmt_let_set_declared_type: call to ic_type_ref_destroy failed");
        return 0;
    }

    let->tref = type_ref;

    return 1;
}

/* set inferred type on this let
 *
 * this is an error if tye type is already set
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_let_set_inferred_type(struct ic_stmt_let *let, struct ic_decl_type *type) {
    if (!let) {
        puts("ic_stmt_let_set_inferred_type: let was null");
        return 0;
    }

    if (!type) {
        puts("ic_stmt_let_set_inferred_type: type was null");
        return 0;
    }

    if (!let->tref) {
        let->tref = ic_type_ref_new();
        if (!let->tref) {
            puts("ic_stmt_let_set_inferred_type: call to ic_type_ref_new failed");
            return 0;
        }
    }

    if (!ic_type_ref_set_type_decl(let->tref, type)) {
        puts("ic_stmt_let_set_declared_type: call to ic_type_ref_set_type_decl failed");
        return 0;
    }

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
void ic_stmt_let_print(FILE *fd, struct ic_stmt_let *let, unsigned int *indent_level) {
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
    ic_parse_print_indent(fd, *indent_level);

    /* want to output
     * let identifier::type = init
     */

    fputs("let ", fd);

    ic_symbol_print(fd, &(let->identifier));

    /* declared type on let is optional */
    if (let->tref->tag != ic_type_ref_unknown) {
        fputs("::", fd);
        ic_type_ref_print(fd, let->tref);
    }

    fputs(" = ", fd);

    ic_expr_print(fd, let->init, &fake_indent);

    /* statements are displayed on their own line */
    fputs("\n", fd);
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
void ic_stmt_assign_print(FILE *fd, struct ic_stmt_assign *assign, unsigned int *indent_level) {
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
    ic_parse_print_indent(fd, *indent_level);

    /* want to output
     * left = right
     */

    fake_indent = 0;
    ic_expr_print(fd, assign->left, &fake_indent);

    fputs(" = ", fd);

    fake_indent = 0;
    ic_expr_print(fd, assign->right, &fake_indent);

    /* statements are displayed on their own line */
    fputs("\n", fd);
}

/* allocate and initialise a new ic_stmt_begin
 * this will initialise the body
 * but will NOT initialise the expression
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_stmt_begin *ic_stmt_begin_new(void) {
    struct ic_stmt_begin *begin = 0;

    begin = calloc(1, sizeof(struct ic_stmt_begin));
    if (!begin) {
        puts("ic_stmt_begin_new: call to calloc failed");
        return 0;
    }

    return begin;
}

/* initialise an existing new ic_stmt_begin
 * this will initialise the body
 * but will NOT initialise the expression
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_begin_init(struct ic_stmt_begin *begin) {
    if (!begin) {
        puts("ic_stmt_begin_init: begin was null");
        return 0;
    }

    begin->body = 0;

    return 1;
}

/* destroy begin
 *
 * only frees begin if `free_begin` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_begin_destroy(struct ic_stmt_begin *begin, unsigned int free_begin) {
    if (!begin) {
        puts("ic_stmt_begin_destroy:begin was null");
        return 0;
    }

    if (begin->body) {
        if (!ic_body_destroy(begin->body, 1)) {
            puts("ic_stmt_begin_destroy:call to ic_body_destroy failed");
            return 0;
        }
    }

    if (free_begin) {
        free(begin);
    }

    return 1;
}

/* print this begin */
void ic_stmt_begin_print(FILE *fd, struct ic_stmt_begin *begin, unsigned int *indent_level) {
    if (!begin) {
        puts("ic_stmt_begin_print: begin was null");
        return;
    }

    ic_parse_print_indent(fd, *indent_level);
    fputs("begin\n ", fd);

    /* print body
     * body will handle incr and decr of the indent level
     */
    ic_body_print(fd, begin->body, indent_level);

    ic_parse_print_indent(fd, *indent_level);
    fputs("end\n ", fd);
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
    sif->then_body = 0;
    sif->else_body = 0;

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

    if (sif->then_body) {
        /* free_body = 1 as pointer member */
        if (!ic_body_destroy(sif->then_body, 1)) {
            puts("ic_stmt_if_destroy: call to ic_body_destroy failed");
            return 0;
        }
    }

    if (sif->else_body) {
        /* free_body = 1 as pointer member */
        if (!ic_body_destroy(sif->else_body, 1)) {
            puts("ic_stmt_if_destroy: call to ic_body_destroy failed");
            return 0;
        }
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

/* print this if */
void ic_stmt_if_print(FILE *fd, struct ic_stmt_if *sif, unsigned int *indent_level) {
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
    ic_parse_print_indent(fd, *indent_level);

    /* we want to print
     *  if expr
     *      body
     *  end
     */
    fputs("if ", fd);
    ic_expr_print(fd, sif->expr, &fake_indent);
    fputs("\n", fd);

    /* print body
     * body will handle incr and decr of the indent level
     */
    ic_body_print(fd, sif->then_body, indent_level);

    if (sif->else_body) {
        ic_parse_print_indent(fd, *indent_level);
        fputs("else\n", fd);
        ic_body_print(fd, sif->else_body, indent_level);
    }

    /* statements are displayed on their own line */
    /* print indent */
    ic_parse_print_indent(fd, *indent_level);
    fputs("end\n", fd);
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
void ic_stmt_for_print(FILE *fd, struct ic_stmt_for *sfor, unsigned int *indent_level) {
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
    ic_parse_print_indent(fd, *indent_level);

    /* we want to print
     *  for expr in iterator
     *      body
     *  end
     */
    fputs("for ", fd);
    ic_expr_print(fd, sfor->expr, &fake_indent);
    fputs(" in ", fd);
    ic_expr_print(fd, sfor->iterator, &fake_indent);
    fputs("\n", fd);

    /* print body
     * body will handle incr and decr of the indent level
     */
    ic_body_print(fd, sfor->body, indent_level);

    /* statements are displayed on their own line */
    /* print indent */
    ic_parse_print_indent(fd, *indent_level);
    fputs("end\n", fd);
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

    if (!swhile->expr) {
        puts("ic_stmt_while_get_expr: swhile->expr was null");
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
void ic_stmt_while_print(FILE *fd, struct ic_stmt_while *swhile, unsigned int *indent_level) {
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
    ic_parse_print_indent(fd, *indent_level);

    /* we want to print
     *  while expr
     *      body
     *  end
     */
    fputs("while ", fd);
    ic_expr_print(fd, swhile->expr, &fake_indent);
    fputs("\n", fd);

    /* print body
     * body will handle incr and decr of the indent level
     */
    ic_body_print(fd, swhile->body, indent_level);

    /* statements are displayed on their own line */
    /* print indent */
    ic_parse_print_indent(fd, *indent_level);
    fputs("end\n", fd);
}

/* allocate and initialise a new ic_stmt_match
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_stmt_match *ic_stmt_match_new(void) {
    struct ic_stmt_match *match = 0;

    match = calloc(1, sizeof(struct ic_stmt_match));
    if (!match) {
        puts("ic_stmt_match_new: call to calloc failed");
        return 0;
    }

    if (!ic_stmt_match_init(match)) {
        puts("ic_stmt_match_new: call to ic_stmt_match_init failed");
        return 0;
    }

    return match;
}

/* initialise an existing new ic_stmt_match
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_match_init(struct ic_stmt_match *match) {
    if (!match) {
        puts("ic_stmt_match_init: match was null");
        return 0;
    }

    match->expr = 0;
    match->else_body = 0;

    if (!ic_pvector_init(&(match->cases), 0)) {
        puts("ic_stmt_match_init: call to ic_pvector_init failed");
        return 0;
    }

    return 1;
}

/* destroy match
 *
 * only frees stmt_match if `free_match` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_match_destroy(struct ic_stmt_match *match, unsigned int free_match) {
    int i = 0;
    int len = 0;
    struct ic_stmt_case *scase = 0;

    if (!match) {
        puts("ic_stmt_match_destroy: match was null");
        return 0;
    }

    if (match->expr) {
        if (!ic_expr_destroy(match->expr, 1)) {
            puts("ic_stmt_match_destroy: call to ic_expr_destroy failed");
            return 0;
        }
    }

    if (match->else_body) {
        if (!ic_body_destroy(match->else_body, 1)) {
            puts("ic_stmt_match_destroy: call to ic_body_destroy failed");
            return 0;
        }
    }

    len = ic_pvector_length(&(match->cases));
    if (len > 0) {
        for (i = 0; i < len; ++i) {
            scase = ic_pvector_get(&(match->cases), i);
            if (!scase) {
                puts("ic_stmt_match_destroy: call to ic_pvector_get failed");
                return 0;
            }

            if (!ic_stmt_case_destroy(scase, 1)) {
                puts("ic_stmt_match_destroy: call to ic_stmt_case_destroy failed");
                return 0;
            }
        }
    }

    if (!ic_pvector_destroy(&(match->cases), 0, 0)) {
        puts("ic_stmt_match_destroy: call to ic_pvector_destroy failed");
        return 0;
    }

    if (free_match) {
        free(match);
    }

    return 1;
}

/* returns pointer on success
 * returns 0 on failure
 */
struct ic_expr *ic_stmt_match_get_expr(struct ic_stmt_match *match) {
    if (!match) {
        puts("ic_stmt_match_get_expr: match was null");
        return 0;
    }

    if (!match->expr) {
        puts("ic_stmt_match_get_expr: match->expr was null");
        return 0;
    }

    return match->expr;
}

/* returns pointer on success
 * returns 0 on failure
 */
struct ic_body *ic_stmt_match_get_else_body(struct ic_stmt_match *match) {
    if (!match) {
        puts("ic_stmt_match_get_else_body: match was null");
        return 0;
    }

    if (!match->else_body) {
        puts("ic_stmt_match_get_else_body: match->else_body was null");
        return 0;
    }

    return match->else_body;
}

/* get stmt_case of offset i within cases
 *
 * returns pointer to element on success
 * returns 0 on failure
 */
struct ic_stmt_case *ic_stmt_match_cases_get(struct ic_stmt_match *match, unsigned int i) {
    struct ic_stmt_case *scase = 0;

    if (!match) {
        puts("ic_stmt_match_cases_get: match was null");
        return 0;
    }

    scase = ic_pvector_get(&(match->cases), i);
    if (!scase) {
        puts("ic_stmt_match_cases_get: call to ic_pvector_get failed");
        return 0;
    }

    return scase;
}

/* get length of body
 *
 * returns length on success
 * returns 0 on failure
 */
unsigned int ic_stmt_match_cases_length(struct ic_stmt_match *match) {
    unsigned int len = 0;

    if (!match) {
        puts("ic_stmt_match_cases_length: match was null");
        return 0;
    }

    len = ic_pvector_length(&(match->cases));
    return len;
}

/* print this if */
void ic_stmt_match_print(FILE *fd, struct ic_stmt_match *match, unsigned int *indent_level) {
    unsigned int fake_indent = 0;
    unsigned int i = 0;
    unsigned int len = 0;
    struct ic_stmt_case *scase = 0;

    if (!fd) {
        puts("ic_stmt_match_print: fd was null");
        return;
    }

    if (!match) {
        puts("ic_stmt_match_print: match was null");
        return;
    }

    if (!indent_level) {
        puts("ic_stmt_match_print: indent_level was null");
        return;
    }

    /* print indent */
    ic_parse_print_indent(fd, *indent_level);

    /* we want to print
     *  match expr
     *    cases...
     *    else...
     *    end
     *  end
     */
    fputs("match ", fd);
    ic_expr_print(fd, match->expr, &fake_indent);
    fputs("\n", fd);

    len = ic_stmt_match_cases_length(match);

    ++*indent_level;
    for (i = 0; i < len; ++i) {
        scase = ic_stmt_match_cases_get(match, i);
        if (!scase) {
            puts("ic_stmt_match_print: call to ic_stmt_match_cases_get");
            return;
        }

        ic_parse_print_indent(fd, *indent_level);
        fputs("case ", fd);
        ic_field_print(fd, &(scase->field));
        fputs("\n", fd);

        ic_body_print(fd, scase->body, indent_level);
    }

    if (match->else_body) {
        ic_parse_print_indent(fd, *indent_level);
        fputs("else\n", fd);
        ic_body_print(fd, match->else_body, indent_level);
    }

    ic_parse_print_indent(fd, *indent_level);
    fputs("end\n", fd);

    --*indent_level;

    ic_parse_print_indent(fd, *indent_level);
    fputs("end\n", fd);
}

/* allocate and initialise a new ic_stmt_case
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_stmt_case *ic_stmt_case_new(char *id_ch, unsigned int id_len, char *type_ch, unsigned int type_len, struct ic_body *body) {
    struct ic_stmt_case *scase = 0;

    if (!id_ch) {
        puts("ic_stmt_case_new: id_ch was null");
        return 0;
    }

    if (!id_len) {
        puts("ic_stmt_case_new: id_len was 0");
        return 0;
    }

    if (!type_ch) {
        puts("ic_stmt_case_new: type_ch was null");
        return 0;
    }

    if (!type_len) {
        puts("ic_stmt_case_new: type_len was 0");
        return 0;
    }

    if (!body) {
        puts("ic_stmt_case_new: body was null");
        return 0;
    }

    /* alloc */
    scase = calloc(1, sizeof(struct ic_stmt_case));
    if (!scase) {
        puts("ic_stmt_case_new: calloc failed");
        return 0;
    }

    /* init */
    if (!ic_stmt_case_init(scase, id_ch, id_len, type_ch, type_len, body)) {
        puts("ic_stmt_case_new: call to ic_stmt_case_init failed");
        free(scase);
        return 0;
    }

    return scase;
}

/* initialise an existing new ic_stmt_case
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_case_init(struct ic_stmt_case *scase, char *id_ch, unsigned int id_len, char *type_ch, unsigned int type_len, struct ic_body *body) {
    struct ic_type_ref *type_ref = 0;

    if (!scase) {
        puts("ic_stmt_case_init: scase was null");
        return 0;
    }

    if (!id_ch) {
        puts("ic_stmt_case_init: id_ch was null");
        return 0;
    }

    if (!id_len) {
        puts("ic_stmt_case_init: id_len was 0");
        return 0;
    }

    if (!type_ch) {
        puts("ic_stmt_case_init: type_ch was null");
        return 0;
    }

    if (!type_len) {
        puts("ic_stmt_case_init: type_len was 0");
        return 0;
    }

    if (!body) {
        puts("ic_stmt_case_init: body was null");
        return 0;
    }

    scase->body = body;

    type_ref = ic_type_ref_symbol_new(type_ch, type_len);
    if (!type_ref) {
        puts("ic_stmt_case_init: call to ic_type_ref_symbol_new failed");
        return 0;
    }

    if (!ic_field_init(&(scase->field), id_ch, id_len, type_ref, ic_parse_perm_default())) {
        puts("ic_stmt_case_init: call to ic_field_init failed");
        return 0;
    }

    /* return success */
    return 1;
}

/* destroy case
 *
 * only frees stmt_case if `free_case` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_case_destroy(struct ic_stmt_case *scase, unsigned int free_case) {
    if (!scase) {
        puts("ic_stmt_case_destroy: scase was null");
        return 0;
    }

    /* free_field = 0 as member */
    if (!ic_field_destroy(&(scase->field), 0)) {
        puts("ic_stmt_case_destroy: call to ic_field_destroy failed");
        return 0;
    }

    /* free_body = 0 as member */
    if (!ic_body_destroy(scase->body, 0)) {
        puts("ic_stmt_case_destroy: call to ic_body_destroy failed");
        return 0;
    }

    if (free_case) {
        free(scase);
    }

    return 1;
}

/* returns pointer on success
 * returns 0 on failure
 */
struct ic_field *ic_stmt_case_get_field(struct ic_stmt_case *scase) {
    if (!scase) {
        puts("ic_stmt_case_get_field: scase was null");
        return 0;
    }

    /* return our expr innards */
    return &(scase->field);
}

/* get statement of offset i within the body
 *
 * returns pointer to element on success
 * returns 0 on failure
 */
struct ic_stmt *ic_stmt_case_get_stmt(struct ic_stmt_case *scase, unsigned int i) {
    if (!scase) {
        puts("ic_stmt_case_get_stmt: scase was null");
        return 0;
    }

    /* let body do the lifting */
    return ic_body_get(scase->body, i);
}

/* get length of body
 *
 * returns length on success
 * returns 0 on failure
 */
unsigned int ic_stmt_case_length(struct ic_stmt_case *scase) {
    if (!scase) {
        puts("ic_stmt_case_length: scase was null");
        return 0;
    }

    /* let body do the lifting */
    return ic_body_length(scase->body);
}

/* print this if */
void ic_stmt_case_print(FILE *fd, struct ic_stmt_case *scase, unsigned int *indent_level) {
    if (!scase) {
        puts("ic_stmt_case_print: scase was null");
        return;
    }
    if (!indent_level) {
        puts("ic_stmt_case_print: indent_level was null");
        return;
    }

    /* print indent */
    ic_parse_print_indent(fd, *indent_level);

    /* we want to print
     *  case expr
     *      body
     *  end
     */
    fputs("case ", fd);
    ic_field_print(fd, &(scase->field));
    fputs("\n", fd);

    /* print body
     * body will handle incr and decr of the indent level
     */
    ic_body_print(fd, scase->body, indent_level);

    /* statements are displayed on their own line */
    /* print indent */
    ic_parse_print_indent(fd, *indent_level);
    fputs("end\n", fd);
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

        case ic_stmt_type_begin:
            /* do not free as member */
            if (!ic_stmt_begin_destroy(&(stmt->u.begin), 0)) {
                puts("ic_stmt_destroy: call to ic_stmt_begin_destroy failed");
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

        case ic_stmt_type_match:
            /* do not free as member */
            if (!ic_stmt_match_destroy(&(stmt->u.match), 0)) {
                puts("ic_stmt_destroy: call to ic_stmt_match_destroy failed");
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

/* perform a deep copy of a stmt
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_stmt *ic_stmt_deep_copy(struct ic_stmt *stmt) {
    struct ic_stmt *new_stmt = 0;

    if (!stmt) {
        puts("ic_stmt_deep_copy: stmt was null");
        return 0;
    }

    new_stmt = calloc(1, sizeof(struct ic_stmt));
    if (!new_stmt) {
        puts("ic_stmt_deep_copy: call to calloc failed");
        return 0;
    }

    if (!ic_stmt_deep_copy_embedded(stmt, new_stmt)) {
        puts("ic_stmt_deep_copy: call to ic_stmt_deep_copy_embedded failed");
        return 0;
    }

    return new_stmt;
}

/* perform a deep copy of a stmt embedded within an object
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_deep_copy_embedded(struct ic_stmt *from, struct ic_stmt *to) {
    if (!from) {
        puts("ic_stmt_deep_copy_embedded: from was null");
        return 0;
    }

    if (!to) {
        puts("ic_stmt_deep_copy_embedded: to was null");
        return 0;
    }

    to->tag = from->tag;

    switch (from->tag) {
        case ic_stmt_type_ret:
            puts("ic_stmt_deep_copy_embedded: ret: unimplemented");
            return 0;
            break;

        case ic_stmt_type_let:
            puts("ic_stmt_deep_copy_embedded: let: unimplemented");
            return 0;
            break;

        case ic_stmt_type_begin:
            puts("ic_stmt_deep_copy_embedded: begin: unimplemented");
            return 0;
            break;

        case ic_stmt_type_if:
            puts("ic_stmt_deep_copy_embedded: if: unimplemented");
            return 0;
            break;

        case ic_stmt_type_for:
            puts("ic_stmt_deep_copy_embedded: for: unimplemented");
            return 0;
            break;

        case ic_stmt_type_while:
            puts("ic_stmt_deep_copy_embedded: for: unimplemented");
            return 0;
            break;

        case ic_stmt_type_match:
            puts("ic_stmt_deep_copy_embedded: match: unimplemented");
            return 0;
            break;

        case ic_stmt_type_expr:
            to->u.expr = ic_expr_deep_copy(from->u.expr);
            if (!to->u.expr) {
                puts("ic_stmt_deep_copy_embedded: expr: call to ic_expr_deep_copy failed");
                return 0;
            }
            break;

        case ic_stmt_type_assign:
            puts("ic_stmt_deep_copy_embedded: assign: unimplemented");
            return 0;
            break;

        default:
            puts("ic_stmt_deep_copy_embedded: impossible stmt type");
            return 0;
            break;
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

/* get a pointer to the begin within
 * will only succeed if ic_stmt is of the correct type
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_stmt_begin *ic_stmt_get_begin(struct ic_stmt *stmt) {
    if (!stmt) {
        puts("ic_stmt_get_begin: stmt was null");
        return 0;
    }

    if (stmt->tag != ic_stmt_type_begin) {
        puts("ic_stmt_get_begin: not of correct type");
        return 0;
    }

    return &(stmt->u.begin);
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

/* get a pointer to the match within
 * will only succeed if ic_stmt is of the correct type
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_stmt_match *ic_stmt_get_match(struct ic_stmt *stmt) {
    if (!stmt) {
        puts("ic_stmt_get_match: stmt was null");
        return 0;
    }

    /* check type before giving out */
    if (stmt->tag != ic_stmt_type_match) {
        puts("ic_stmt_get_match: not of the correct type");
        return 0;
    }

    /* otherwise give them what they asked for */
    return &(stmt->u.match);
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
void ic_stmt_print(FILE *fd, struct ic_stmt *stmt, unsigned int *indent_level) {
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
            ic_stmt_ret_print(fd, &(stmt->u.ret), indent_level);
            break;

        case ic_stmt_type_let:
            ic_stmt_let_print(fd, &(stmt->u.let), indent_level);
            break;

        case ic_stmt_type_assign:
            ic_stmt_assign_print(fd, &(stmt->u.assign), indent_level);
            break;

        case ic_stmt_type_begin:
            ic_stmt_begin_print(fd, &(stmt->u.begin), indent_level);
            break;

        case ic_stmt_type_if:
            ic_stmt_if_print(fd, &(stmt->u.sif), indent_level);
            break;

        case ic_stmt_type_for:
            ic_stmt_for_print(fd, &(stmt->u.sfor), indent_level);
            break;

        case ic_stmt_type_while:
            ic_stmt_while_print(fd, &(stmt->u.swhile), indent_level);
            break;

        case ic_stmt_type_match:
            ic_stmt_match_print(fd, &(stmt->u.match), indent_level);
            break;

        case ic_stmt_type_expr:
            ic_expr_print(fd, stmt->u.expr, indent_level);
            /* statements are displayed on their own line */
            puts("");
            break;

        default:
            puts("ic_stmt_print: unknown type");
            return;
    }
}
