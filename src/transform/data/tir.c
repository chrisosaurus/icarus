#include <stdio.h>
#include <stdlib.h>

#include "../../parse/parse.h"
#include "tir.h"

/* allocate and initialise a new let_literal
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_let_literal *ic_transform_ir_let_literal_new(void) {
    struct ic_transform_ir_let_literal *let = 0;

    let = calloc(1, sizeof(struct ic_transform_ir_let_literal));
    if (!let) {
        puts("ir_transform_ir_let_literal_new: call to calloc failed");
        return 0;
    }

    if (!ic_transform_ir_let_literal_init(let)) {
        puts("ir_transform_ir_let_literal_new: call to ic_transform_let_literal_init failed");
        return 0;
    }

    return let;
}

/* initialise an existing let_literal
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_let_literal_init(struct ic_transform_ir_let_literal *let) {
    if (!let) {
        puts("ic_transform_ir_let_literal_init: let was null");
        return 0;
    }

    let->name = 0;
    let->type = 0;
    let->literal = 0;

    return 1;
}

/* destroy let_literal
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free let if `free_let` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_let_literal_destroy(struct ic_transform_ir_let_literal *let, unsigned int free_let) {
    if (!let) {
        puts("ic_transform_ir_let_let_literal_destroy: let was null");
        return 0;
    }

    if (free_let) {
        free(let);
    }

    return 1;
}

/* print let_literal
 *
 * returns 1 on success
 * return 0 on failure
 */
unsigned int ic_transform_ir_let_literal_print(struct ic_transform_ir_let_literal *let, unsigned int *indent) {
    unsigned int fake_indent = 0;
    if (!let) {
        puts("ic_transform_ir_let_literal_print: let was null");
        return 0;
    }

    if (!indent) {
        puts("ic_transform_ir_let_literal_print: indent was null");
        return 0;
    }

    if (!let->name || !let->type || !let->literal) {
        puts("ic_transform_ir_let_literal_print: this let is not ready for printing - has null fields");
        return 0;
    }

    ic_parse_print_indent(*indent);

    fputs("let ", stdout);

    /* identifier name */
    ic_symbol_print(let->name);

    fputs("::", stdout);

    /* type */
    ic_type_print(let->type);

    fputs(" = ", stdout);

    /* literal */
    ic_expr_constant_print(let->literal, &fake_indent);

    /* trailing \n */
    puts("");

    return 1;
}

/* allocate and initialise a new let_expr
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_let_expr *ic_transform_ir_let_expr_new(void) {
    struct ic_transform_ir_let_expr *expr = 0;

    expr = calloc(1, sizeof(struct ic_transform_ir_let_expr));
    if (!expr) {
        puts("ir_transform_ir_let_expr_new: call to calloc failed");
        return 0;
    }

    if (!ic_transform_ir_let_expr_init(expr)) {
        puts("ir_transform_ir_let_expr_new: call to ic_transform_let_expr_init failed");
        return 0;
    }

    return expr;
}

/* initialise an existing let_expr
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_let_expr_init(struct ic_transform_ir_let_expr *let) {
    if (!let) {
        puts("ic_transform_ir_let_expr_init: let was null");
        return 0;
    }

    let->name = 0;
    let->type = 0;
    let->expr = 0;

    return 1;
}

/* destroy let_expr
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free let if `free_let` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_let_expr_destroy(struct ic_transform_ir_let_expr *let, unsigned int free_let) {
    if (!let) {
        puts("ic_transform_ir_let_expr_destroy: let was null");
        return 0;
    }

    if (free_let) {
        free(let);
    }

    return 1;
}

/* print let_expr
 *
 * returns 1 on success
 * return 0 on failure
 */
unsigned int ic_transform_ir_let_expr_print(struct ic_transform_ir_let_expr *let, unsigned int *indent) {
    unsigned int fake_indent = 0;
    if (!let) {
        puts("ic_transform_ir_let_expr_print: let was null");
        return 0;
    }

    if (!indent) {
        puts("ic_transform_ir_let_expr_print: indent was null");
        return 0;
    }

    if (!let->name || !let->type || !let->expr) {
        puts("ic_transform_ir_let_expr_print: this let is not ready for printing - has null fields");
        return 0;
    }

    ic_parse_print_indent(*indent);

    fputs("let ", stdout);

    /* identifier name */
    ic_symbol_print(let->name);

    fputs("::", stdout);

    /* type */
    ic_type_print(let->type);

    fputs(" = ", stdout);

    /* expr */
    if (!ic_transform_ir_expr_print(let->expr, &fake_indent)) {
        puts("ic_transform_ir_let_expr_print: call to ic_transform_ir_expr_print failed");
        return 0;
    }

    /* trailing \n */
    puts("");

    return 1;
}

/* allocate and initialise a new let
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_let *ic_transform_ir_let_new(enum ic_transform_ir_let_tag tag) {
    struct ic_transform_ir_let *let = 0;

    let = calloc(1, sizeof(struct ic_transform_ir_let));
    if (!let) {
        puts("ir_transform_ir_let_new: call to calloc failed");
        return 0;
    }

    if (!ic_transform_ir_let_init(let, tag)) {
        puts("ir_transform_ir_let_new: call to ic_transform_let_init failed");
        return 0;
    }

    return let;
}

/* initialise an existing let
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_let_init(struct ic_transform_ir_let *let, enum ic_transform_ir_let_tag tag) {
    if (!let) {
        puts("ic_transform_ir_let_init: let was null");
        return 0;
    }

    let->tag = tag;

    return 1;
}

/* destroy let
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free let if `free_let` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_let_destroy(struct ic_transform_ir_let *let, unsigned int free_let) {
    if (!let) {
        puts("ic_transform_ir_let_destroy: let was null");
        return 0;
    }

    if (free_let) {
        free(let);
    }

    return 1;
}

/* print let
 *
 * returns 1 on success
 * return 0 on failure
 */
unsigned int ic_transform_ir_let_print(struct ic_transform_ir_let *let, unsigned int *indent) {
    if (!let) {
        puts("ic_transform_ir_let_print: let was null");
        return 0;
    }

    if (!indent) {
        puts("ic_transform_ir_let_print: indent was null");
        return 0;
    }

    switch (let->tag) {
        case ic_transform_ir_let_type_literal:
            if (!ic_transform_ir_let_literal_print(&(let->u.lit), indent)) {
                puts("ic_transform_ir_let_print: call to ic_transform_let_literal_print failed");
                return 0;
            }
            break;

        case ic_transform_ir_let_type_expr:
            if (!ic_transform_ir_let_expr_print(&(let->u.expr), indent)) {
                puts("ic_transform_ir_let_print: call to ic_transform_let_expr_print failed");
                return 0;
            }
            break;

        default:
            puts("ic_transform_ir_let_print: impossible let type");
            return 0;
            break;
    }

    return 1;
}

/* get pointer to internal let_literal
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_let_literal *ic_transform_ir_let_get_literal(struct ic_transform_ir_let *let) {
    if (!let) {
        puts("ic_transform_ir_let_get_literal: let was null");
        return 0;
    }

    if (let->tag != ic_transform_ir_let_type_literal) {
        puts("ic_transform_ir_let_get_literal: let was not of type literal");
        return 0;
    }

    return &(let->u.lit);
}

/* get pointer to internal let_expr
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_let_expr *ic_transform_ir_let_get_expr(struct ic_transform_ir_let *let) {
    if (!let) {
        puts("ic_transform_ir_let_get_expr: let was null");
        return 0;
    }

    if (let->tag != ic_transform_ir_let_type_expr) {
        puts("ic_transform_ir_let_get_expr: let was not of type expr");
        return 0;
    }

    return &(let->u.expr);
}

/* allocate and initialise a new assign
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_assign *ic_transform_ir_assign_new(void) {
    struct ic_transform_ir_assign *assign = 0;

    assign = calloc(1, sizeof(struct ic_transform_ir_assign));
    if (!assign) {
        puts("ir_transform_ir_assign_new: call to calloc failed");
        return 0;
    }

    if (!ic_transform_ir_assign_init(assign)) {
        puts("ir_transform_ir_assign_new: call to ic_transform_assign_init failed");
        return 0;
    }

    return assign;
}

/* initialise an existing assign
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_assign_init(struct ic_transform_ir_assign *assign) {
    if (!assign) {
        puts("ic_transform_ir_let_assign_init: assign was null");
        return 0;
    }

    assign->left = 0;
    assign->right = 0;

    return 1;
}

/* destroy assign
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free assign if `free_assign` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_assign_destroy(struct ic_transform_ir_assign *assign, unsigned int free_assign) {
    if (!assign) {
        puts("ic_transform_ir_let_assign_destroy: assign was null");
        return 0;
    }

    if (free_assign) {
        free(assign);
    }

    return 1;
}

/* print assign
 *
 * returns 1 on success
 * return 0 on failure
 */
unsigned int ic_transform_ir_assign_print(struct ic_transform_ir_assign *assign, unsigned int *indent) {
    unsigned int fake_indent = 0;

    if (!assign) {
        puts("ic_transform_ir_assign_print: assign was null");
        return 0;
    }

    if (!indent) {
        puts("ic_transform_ir_assign_print: indent was null");
        return 0;
    }

    if (!assign->left || !assign->right) {
        puts("ic_transform_ir_assign_print: this assign is not ready for printing - has null fields");
        return 0;
    }

    ic_parse_print_indent(*indent);

    /* left symbol */
    ic_symbol_print(assign->left);

    fputs(" = ", stdout);

    /* right symbol */
    if (!ic_transform_ir_expr_print(assign->right, &fake_indent)) {
        puts("ic_transform_ir_assign_print: call to ic_transform_ir_expr_print failed");
        return 0;
    }

    /* trailing \n */
    puts("");

    return 0;
}

/* allocate and initialise a new expr
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_expr *ic_transform_ir_expr_new(void) {
    struct ic_transform_ir_expr *expr = 0;

    expr = calloc(1, sizeof(struct ic_transform_ir_expr));
    if (!expr) {
        puts("ir_transform_ir_expr_new: call to calloc failed");
        return 0;
    }

    if (!ic_transform_ir_expr_init(expr)) {
        puts("ir_transform_ir_expr_new: call to ic_transform_expr_init failed");
        return 0;
    }

    return expr;
}

/* initialise an existing expr
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_expr_init(struct ic_transform_ir_expr *expr) {
    if (!expr) {
        puts("ic_transform_ir_expr_init: expr was null");
        return 0;
    }

    expr->fcall = 0;

    return 1;
}

/* destroy expr
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free expr if `free_expr` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_expr_destroy(struct ic_transform_ir_expr *expr, unsigned int free_expr) {
    if (!expr) {
        puts("ic_transform_ir_expr_destroy: expr was null");
        return 0;
    }

    if (free_expr) {
        free(expr);
    }

    return 1;
}

/* print expr
 *
 * returns 1 on success
 * return 0 on failure
 */
unsigned int ic_transform_ir_expr_print(struct ic_transform_ir_expr *expr, unsigned int *indent) {
    if (!expr) {
        puts("ic_transform_ir_expr_print: expr was null");
        return 0;
    }

    if (!indent) {
        puts("ic_transform_ir_expr_print: indent was null");
        return 0;
    }

    if (!expr->fcall) {
        puts("ic_transform_ir_expr_print: expr->fcall was null");
        return 0;
    }

    if (!ic_transform_ir_fcall_print(expr->fcall, indent)) {
        puts("ic_transform_ir_expr_print: call to ic_transform_ir_fcall_print failed");
        return 0;
    }

    return 1;
}

/* allocate and initialise a new ret
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_ret *ic_transform_ir_ret_new(void) {
    struct ic_transform_ir_ret *ret = 0;

    ret = calloc(1, sizeof(struct ic_transform_ir_ret));
    if (!ret) {
        puts("ir_transform_ir_ret_new: call to calloc failed");
        return 0;
    }

    if (!ic_transform_ir_ret_init(ret)) {
        puts("ir_transform_ir_ret_new: call to ic_transform_ret_init failed");
        return 0;
    }

    return ret;
}

/* initialise an existing ret
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_ret_init(struct ic_transform_ir_ret *ret) {
    if (!ret) {
        puts("ic_transform_ir_ret_init: ret was null");
        return 0;
    }

    ret->var = 0;

    return 1;
}

/* destroy ret
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free ret if `free_ret` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_ret_destroy(struct ic_transform_ir_ret *ret, unsigned int free_ret) {
    if (!ret) {
        puts("ic_transform_ir_ret_destroy: ret was null");
        return 0;
    }

    if (free_ret) {
        free(ret);
    }

    return 1;
}

/* print ret
 *
 * returns 1 on success
 * return 0 on failure
 */
unsigned int ic_transform_ir_ret_print(struct ic_transform_ir_ret *ret, unsigned int *indent) {
    if (!ret) {
        puts("ic_transform_ir_ret_print: ret was null");
        return 0;
    }

    if (!indent) {
        puts("ic_transform_ir_ret_print: indent was null");
        return 0;
    }

    ic_parse_print_indent(*indent);

    fputs("return", stdout);

    /* print symbol if it's set */
    if (ret->var) {
        fputs(" ", stdout);
        ic_symbol_print(ret->var);
    }

    /* trailing \n */
    puts("");

    return 1;
}

/* allocate and initialise a new fcall
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_fcall *ic_transform_ir_fcall_new(struct ic_expr_func_call *fcall, struct ic_pvector *args) {
    struct ic_transform_ir_fcall *tir_fcall = 0;

    if (!fcall) {
        puts("ir_transform_ir_fcall_new: fcall was null");
        return 0;
    }

    if (!args) {
        puts("ir_transform_ir_fcall_new: args was null");
        return 0;
    }

    tir_fcall = calloc(1, sizeof(struct ic_transform_ir_fcall));
    if (!tir_fcall) {
        puts("ir_transform_ir_fcall_new: call to calloc failed");
        return 0;
    }

    if (!ic_transform_ir_fcall_init(tir_fcall, fcall, args)) {
        puts("ir_transform_ir_fcall_new: call to ic_transform_fcall_init failed");
        return 0;
    }

    return tir_fcall;
}

/* initialise an existing fcall
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_fcall_init(struct ic_transform_ir_fcall *tir_fcall, struct ic_expr_func_call *fcall, struct ic_pvector *args) {
    if (!tir_fcall) {
        puts("ic_transform_ir_fcall_init: tir_fcall was null");
        return 0;
    }

    if (!fcall) {
        puts("ic_transform_ir_fcall_init: fcall was null");
        return 0;
    }

    if (!args) {
        puts("ic_transform_ir_fcall_init: args was null");
        return 0;
    }

    tir_fcall->fcall = fcall;
    tir_fcall->args = args;

    return 1;
}

/* destroy fcall
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free fcall if `free_fcall` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_fcall_destroy(struct ic_transform_ir_fcall *fcall, unsigned int free_fcall) {
    if (!fcall) {
        puts("ic_transform_ir_fcall_destroy: fcall was null");
        return 0;
    }

    if (free_fcall) {
        free(fcall);
    }

    return 1;
}

/* print fcall
 *
 * fcallurns 1 on success
 * fcallurn 0 on failure
 */
unsigned int ic_transform_ir_fcall_print(struct ic_transform_ir_fcall *fcall, unsigned int *indent) {
    /* offset into fcall args */
    unsigned int i = 0;
    /* length of fcall args */
    unsigned int len = 0;
    /* current arg */
    struct ic_symbol *arg = 0;

    if (!fcall) {
        puts("ic_transform_ir_fcall_print: fcall was null");
        return 0;
    }

    if (!indent) {
        puts("ic_transform_ir_fcall_print: indent was null");
        return 0;
    }

    /* function name and indent */
    ic_expr_print(fcall->fcall->fname, indent);

    /* opening ( */
    fputs("(", stdout);

    /* args */
    len = ic_transform_ir_fcall_length(fcall);

    for (i = 0; i < len; ++i) {
        arg = ic_transform_ir_fcall_get_arg(fcall, i);
        if (!arg) {
            puts("ic_transform_ir_fcall_print: call to ic_transform_ir_fcall_get_arg failed");
            return 0;
        }

        if (i > 0) {
            /* comma and space to sep. args */
            fputs(", ", stdout);
        }

        /* print arg */
        ic_symbol_print(arg);
    }

    /* closing ) */
    fputs(")", stdout);

    return 1;
}

/* get number of args
 *
 * returns number on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_fcall_length(struct ic_transform_ir_fcall *fcall) {
    unsigned len = 0;

    if (!fcall) {
        puts("ic_transform_ir_fcall_length: fcall was null");
        return 0;
    }

    len = ic_pvector_length(fcall->args);

    return len;
}

/* get arg at offset i
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_symbol *ic_transform_ir_fcall_get_arg(struct ic_transform_ir_fcall *fcall, unsigned int i) {
    struct ic_symbol *sym = 0;

    if (!fcall) {
        puts("ic_transform_ir_fcall_get_arg: fcall was null");
        return 0;
    }

    sym = ic_pvector_get(fcall->args, i);
    if (!sym) {
        puts("ic_transform_ir_fcall_get_arg: call to ic_pvector_get failed");
        return 0;
    }

    return sym;
}

/* allocate and initialise a new stmt
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_stmt *ic_transform_ir_stmt_new(enum ic_transform_ir_stmt_tag tag) {
    struct ic_transform_ir_stmt *stmt = 0;

    stmt = calloc(1, sizeof(struct ic_transform_ir_stmt));
    if (!stmt) {
        puts("ir_transform_ir_stmt_new: call to calloc failed");
        return 0;
    }

    if (!ic_transform_ir_stmt_init(stmt, tag)) {
        puts("ir_transform_ir_stmt_new: call to ic_transform_stmt_init failed");
        return 0;
    }

    return stmt;
}

/* initialise an existing stmt
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_stmt_init(struct ic_transform_ir_stmt *stmt, enum ic_transform_ir_stmt_tag tag) {
    if (!stmt) {
        puts("ic_transform_ir_stmt_init: stmt was null");
        return 0;
    }

    stmt->tag = tag;

    return 1;
}

/* destroy stmt
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free stmt if `free_stmt` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_stmt_destroy(struct ic_transform_ir_stmt *stmt, unsigned int free_stmt) {
    if (!stmt) {
        puts("ic_transform_ir_stmt_destroy: stmt was null");
        return 0;
    }

    if (free_stmt) {
        free(stmt);
    }

    return 1;
}

/* print stmt
 *
 * returns 1 on success
 * return 0 on failure
 */
unsigned int ic_transform_ir_stmt_print(struct ic_transform_ir_stmt *stmt, unsigned int *indent) {
    if (!stmt) {
        puts("ic_transform_ir_stmt_print: stmt was null");
        return 0;
    }

    if (!indent) {
        puts("ic_transform_ir_stmt_print: indent was null");
        return 0;
    }

    switch (stmt->tag) {
        case ic_transform_ir_stmt_type_expr:
            if (!ic_transform_ir_expr_print(&(stmt->u.expr), indent)) {
                puts("ic_transform_ir_stmt_print: call to ic_transform_ir_expr_print failed");
                return 0;
            }
            /* trailing \n after expr */
            puts("");
            break;

        case ic_transform_ir_stmt_type_let:
            if (!ic_transform_ir_let_print(&(stmt->u.let), indent)) {
                puts("ic_transform_ir_stmt_print: call to ic_transform_ir_let_print failed");
                return 0;
            }
            break;

        case ic_transform_ir_stmt_type_ret:
            if (!ic_transform_ir_ret_print(&(stmt->u.ret), indent)) {
                puts("ic_transform_ir_stmt_print: call to ic_transform_ir_ret_print failed");
                return 0;
            }
            break;

        default:
            puts("ic_transform_ir_stmt_print: impossible stmt->tag");
            return 0;
            break;
    }

    return 1;
}

/* get pointer to internal expr
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_expr *ic_transform_ir_stmt_get_expr(struct ic_transform_ir_stmt *stmt) {
    if (!stmt) {
        puts("ic_transform_ir_stmt_get_expr: stmt was null");
        return 0;
    }

    if (stmt->tag != ic_transform_ir_stmt_type_expr) {
        puts("ic_transform_ir_stmt_get_expr: stmt was not of type expr");
        return 0;
    }

    return &(stmt->u.expr);
}

/* get pointer to internal let
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_let *ic_transform_ir_stmt_get_let(struct ic_transform_ir_stmt *stmt) {
    if (!stmt) {
        puts("ic_transform_ir_stmt_get_let: stmt was null");
        return 0;
    }

    if (stmt->tag != ic_transform_ir_stmt_type_let) {
        puts("ic_transform_ir_stmt_get_let: stmt was not of type let");
        return 0;
    }

    return &(stmt->u.let);
}

/* get pointer to internal ret
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_ret *ic_transform_ir_stmt_get_ret(struct ic_transform_ir_stmt *stmt) {
    if (!stmt) {
        puts("ic_transform_ir_stmt_get_ret: stmt was null");
        return 0;
    }

    if (stmt->tag != ic_transform_ir_stmt_type_ret) {
        puts("ic_transform_ir_stmt_get_ret: stmt was not of type ret");
        return 0;
    }

    return &(stmt->u.ret);
}

/* get pointer to internal assign
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_assign *ic_transform_ir_stmt_get_assign(struct ic_transform_ir_stmt *stmt) {
    if (!stmt) {
        puts("ic_transform_ir_stmt_get_assign: stmt was null");
        return 0;
    }

    if (stmt->tag != ic_transform_ir_stmt_type_assign) {
        puts("ic_transform_ir_stmt_get_assign: stmt was not of type assign");
        return 0;
    }

    return &(stmt->u.assign);
}

/* allocate and initialise a new stmt->let->literal
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_stmt *ic_transform_ir_stmt_let_literal_new(struct ic_symbol *name, struct ic_type *type, struct ic_expr_constant *literal) {
    struct ic_transform_ir_stmt *stmt = 0;

    if (!name) {
        puts("ic_transform_ir_stmt_let_literal_new: name was null");
        return 0;
    }

    if (!type) {
        puts("ic_transform_ir_stmt_let_literal_new: type was null");
        return 0;
    }

    if (!literal) {
        puts("ic_transform_ir_stmt_let_literal_new: literal was null");
        return 0;
    }

    stmt = ic_transform_ir_stmt_new(ic_transform_ir_stmt_type_let);
    if (!stmt) {
        puts("ic_transform_ir_stmt_let_literal_new: call to ic_transform_ir_stmt_new failed");
        return 0;
    }

    if (!ic_transform_ir_let_init(&(stmt->u.let), ic_transform_ir_let_type_literal)) {
        puts("ic_transform_ir_stmt_let_literal_new: call to ic_transform_ir_let_init failed");
        return 0;
    }

    if (!ic_transform_ir_let_literal_init(&(stmt->u.let.u.lit))) {
        puts("ic_transform_ir_stmt_let_literal_new: call to ic_transform_ir_let_literal_init failed");
        return 0;
    }

    stmt->u.let.u.lit.name = name;
    stmt->u.let.u.lit.type = type;
    stmt->u.let.u.lit.literal = literal;

    return stmt;
}

/* allocate and initialise a new stmt->let->expr
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_stmt *ic_transform_ir_stmt_let_expr_new(struct ic_symbol *name, struct ic_type *type, struct ic_transform_ir_expr *expr) {
    struct ic_transform_ir_stmt *stmt = 0;

    if (!name) {
        puts("ic_transform_ir_stmt_let_expr_new: name was null");
        return 0;
    }

    if (!type) {
        puts("ic_transform_ir_stmt_let_expr_new: type was null");
        return 0;
    }

    if (!expr) {
        puts("ic_transform_ir_stmt_let_expr_new: expr was null");
        return 0;
    }

    stmt = ic_transform_ir_stmt_new(ic_transform_ir_stmt_type_let);
    if (!stmt) {
        puts("ic_transform_ir_stmt_let_expr_new: call to ic_transform_ir_stmt_new failed");
        return 0;
    }

    if (!ic_transform_ir_let_init(&(stmt->u.let), ic_transform_ir_let_type_expr)) {
        puts("ic_transform_ir_stmt_let_expr_new: call to ic_transform_ir_let_init failed");
        return 0;
    }

    if (!ic_transform_ir_let_expr_init(&(stmt->u.let.u.expr))) {
        puts("ic_transform_ir_stmt_let_expr_new: call to ic_transform_ir_let_expr_init failed");
        return 0;
    }

    stmt->u.let.u.expr.name = name;
    stmt->u.let.u.expr.type = type;
    stmt->u.let.u.expr.expr = expr;

    return stmt;
}

/* allocate and initialise a new stmt->ret
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_stmt *ic_transform_ir_stmt_ret_new(struct ic_symbol *var) {
    struct ic_transform_ir_stmt *stmt = 0;

    if (!var) {
        puts("ic_transform_ir_stmt_ret_new: var was null");
        return 0;
    }

    stmt = ic_transform_ir_stmt_new(ic_transform_ir_stmt_type_ret);
    if (!stmt) {
        puts("ic_transform_ir_stmt_ret_new: call to ic_transform_ir_stmt_new failed");
        return 0;
    }

    if (!ic_transform_ir_ret_init(&(stmt->u.ret))) {
        puts("ic_transform_ir_stmt_let_expr_new: call to ic_transform_ir_ret_init failed");
        return 0;
    }

    stmt->u.ret.var = var;

    return stmt;
}
