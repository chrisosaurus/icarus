#include <stdio.h>
#include <string.h>

#include "../analyse/data/kludge.h"
#include "../analyse/data/slot.h"
#include "../analyse/helpers.h"
#include "../data/pvector.h"
#include "../parse/data/body.h"
#include "../parse/data/decl.h"
#include "../parse/data/stmt.h"
#include "data/tbody.h"
#include "transform.h"

#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-parameter"

/* perform translation of all fdecls on kludge
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_fdecls(struct ic_kludge *kludge);

/* perform translation of a single fdecl on kludge
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_fdecl(struct ic_kludge *kludge, struct ic_decl_func *func);

/* perform translation of a single body
 *
 * appends tir stmts to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_body(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_transform_body *tbody, struct ic_body *body);

/* perform translation of a single stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt *stmt);

/* perform translation of a single `ret` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_ret(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_ret *ret);

/* perform translation of a single `let` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_let(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_let *let);

/* perform translation of a single `assign` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_assign(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_assign *assign);

/* perform translation of a single `if` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_if(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_if *sif);

/* perform translation of a single `for` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_for(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_for *sfor);

/* perform translation of a single `while` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_while(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_while *swhile);

/* perform translation of a single `expr` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_expr(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_transform_body *tbody, struct ic_body *body, struct ic_expr *expr);

/* register and append a new temporary expr as a let
 * generates a new symbol name and returns it
 *
 * returns * on success
 * returns 0 on failure
 */
static struct ic_symbol *ic_transform_new_temp(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_transform_body *tbody, struct ic_expr *expr);

/* transform an fcall to tir_fcall
 *
 * returns * on success
 * returns 0 on failure
 */
static struct ic_transform_ir_fcall *ic_transform_fcall(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_transform_body *tbody, struct ic_expr_func_call *fcall);

/* transform an fcall argument to an symbol
 * if the arg is already a symbol, just return it
 * otherwise create a new let binding the result of expr
 *
 * returns * on success
 * returns 0 on failure
 */
static struct ic_symbol *ic_transform_fcall_arg(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_transform_body *tbody, struct ic_expr *arg);

/* perform translation to TIR from kludge
 *
 * modifies kludge in place
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform(struct ic_kludge *kludge) {
    if (!kludge) {
        puts("ic_transform: kludge was null");
        return 0;
    }

    if (!ic_transform_fdecls(kludge)) {
        puts("ic_transform: call to ic_transform_fdecls failed");
        return 0;
    }

    return 1;
}

/* print out all transformed items within kludge
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_print(FILE *fd, struct ic_kludge *kludge) {
    /* i for kludge fdecls */
    unsigned int i = 0;
    /* len for kludge fdecls */
    unsigned int len = 0;
    /* current fdecl we are considering */
    struct ic_decl_func *fdecl = 0;
    /* pointer to tbody within */
    struct ic_transform_body *tbody = 0;
    /* indent level */
    unsigned int indent = 1;
    /* fake indent */
    unsigned int fake_indent = 0;

    if (!kludge) {
        puts("ic_transform_print: kludge was null");
        return 0;
    }

    len = ic_pvector_length(&(kludge->fdecls));

    for (i = 0; i < len; ++i) {
        fdecl = ic_pvector_get(&(kludge->fdecls), i);
        if (!fdecl) {
            puts("ic_transform_print: call to ic_pvector_get failed");
            return 0;
        }

        /* skip printing of builtins */
        if (ic_decl_func_isbuiltin(fdecl)) {
            continue;
        }

        /* header for fdecl */
        ic_decl_func_print_header(fd, fdecl, &fake_indent);

        /* get transformed body */
        tbody = fdecl->tbody;
        if (!tbody) {
            puts("ic_transform_print: fdecl lacked tbody");
            return 0;
        }

        /* print transformed body */
        if (!ic_transform_body_print(fd, tbody, &indent)) {
            puts("ic_transform_print: call to ic_transform_body_print failed");
            return 0;
        }

        /* of fdecl body */
        fputs("end\n", fd);
    }

    return 1;
}

/* perform translation of all fdecls on kludge
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_fdecls(struct ic_kludge *kludge) {
    /* i for kludge fdecls */
    unsigned int kf_i = 0;
    /* len for kludge fdecls */
    unsigned int kf_len = 0;
    /* current tdecl we are considering */
    struct ic_decl_func *fdecl = 0;

    if (!kludge) {
        puts("ic_transform_fdecls: kludge was null");
        return 0;
    }

    kf_len = ic_pvector_length(&(kludge->fdecls));

    for (kf_i = 0; kf_i < kf_len; ++kf_i) {
        fdecl = ic_pvector_get(&(kludge->fdecls), kf_i);
        if (!fdecl) {
            puts("ic_transform_fdecls: call to ic_pvector_get failed");
            return 0;
        }

        if (!ic_transform_fdecl(kludge, fdecl)) {
            puts("ic_transform_fdecls: call to ic_transform_fdecl failed");
            return 0;
        }
    }

    return 1;
}

/* perform translation of a single fdecl on kludge
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_fdecl(struct ic_kludge *kludge, struct ic_decl_func *func) {
    if (!kludge) {
        puts("ic_transform_fdecl: kludge was null");
        return 0;
    }

    if (!func) {
        puts("ic_transform_fdecl: func was null");
        return 0;
    }

    /* check transform_body is not populated */
    if (func->tbody) {
        puts("ic_transform_fdecl: func->tbody was already set");
        return 0;
    }

    /* populate tbody */
    func->tbody = ic_transform_body_new();
    if (!func->tbody) {
        puts("ic_transform_fdecl: call to ic_transform_body_new failed");
        return 0;
    }

    /* dispatch to transform_body for work */
    if (!ic_transform_body(kludge, func->body.scope, func->tbody, &(func->body))) {
        puts("ic_transform_fdecl: call to ic_transform_body failed");
        return 0;
    }

    return 1;
}

static unsigned int ic_transform_body(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_transform_body *tbody, struct ic_body *body) {
    /* length of body */
    unsigned int len = 0;
    /* offset into body */
    unsigned int i = 0;
    /* current stmt within body */
    struct ic_stmt *stmt = 0;

    if (!kludge) {
        puts("ic_transform_body: kludge was null");
        return 0;
    }

    if (!scope) {
        puts("ic_transform_body: scope was null");
        return 0;
    }

    if (!tbody) {
        puts("ic_transform_body: tbody was null");
        return 0;
    }

    if (!body) {
        puts("ic_transform_body: body was null");
        return 0;
    }

    len = ic_body_length(body);

    /* step through body stmts */
    for (i = 0; i < len; ++i) {
        /* for each stmt */
        stmt = ic_body_get(body, i);
        if (!stmt) {
            puts("ic_transform_body: call to ic_body_get failed");
            return 0;
        }

        /* transform - dispatch to function */
        if (!ic_transform_stmt(kludge, scope, tbody, body, stmt)) {
            puts("ic_transform_body: call to ic_transform_stmt failed");
            return 0;
        }
    }

    return 1;
}

static unsigned int ic_transform_stmt(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt *stmt) {
    if (!kludge) {
        puts("ic_transform_stmt: kludge was null");
        return 0;
    }

    if (!scope) {
        puts("ic_transform_stmt: scope was null");
        return 0;
    }

    if (!tbody) {
        puts("ic_transform_stmt: tbody was null");
        return 0;
    }

    if (!body) {
        puts("ic_transform_stmt: body was null");
        return 0;
    }

    if (!stmt) {
        puts("ic_transform_stmt: stmt was null");
        return 0;
    }

    /* dispatch to functions for each stmt type */
    switch (stmt->tag) {
        case ic_stmt_type_ret:
            if (!ic_transform_stmt_ret(kludge, scope, tbody, body, &(stmt->u.ret))) {
                puts("ic_transform_stmt: call to ic_transform_stmt_ret failed");
                return 0;
            }
            break;

        case ic_stmt_type_let:
            if (!ic_transform_stmt_let(kludge, scope, tbody, body, &(stmt->u.let))) {
                puts("ic_transform_stmt: call to ic_transform_stmt_let failed");
                return 0;
            }
            break;

        case ic_stmt_type_assign:
            if (!ic_transform_stmt_assign(kludge, scope, tbody, body, &(stmt->u.assign))) {
                puts("ic_transform_stmt: call to ic_transform_stmt_assign failed");
                return 0;
            }
            break;

        case ic_stmt_type_if:
            if (!ic_transform_stmt_if(kludge, scope, tbody, body, &(stmt->u.sif))) {
                puts("ic_transform_stmt: call to ic_transform_stmt_if failed");
                return 0;
            }
            break;

        case ic_stmt_type_for:
            if (!ic_transform_stmt_for(kludge, scope, tbody, body, &(stmt->u.sfor))) {
                puts("ic_transform_stmt: call to ic_transform_stmt_for failed");
                return 0;
            }
            break;

        case ic_stmt_type_while:
            if (!ic_transform_stmt_while(kludge, scope, tbody, body, &(stmt->u.swhile))) {
                puts("ic_transform_stmt: call to ic_transform_stmt_while failed");
                return 0;
            }
            break;

        case ic_stmt_type_expr:
            if (!ic_transform_stmt_expr(kludge, scope, tbody, body, stmt->u.expr)) {
                puts("ic_transform_stmt: call to ic_transform_stmt_expr failed");
                return 0;
            }
            break;

        default:
            puts("ic_transform_stmt: impossible stmt->tag");
            return 0;
            break;
    }

    return 1;
}

/* perform translation of a single `ret` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_ret(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_ret *ret) {
    struct ic_expr *expr = 0;
    struct ic_transform_ir_stmt *stmt = 0;
    struct ic_symbol *name = 0;

    if (!kludge) {
        puts("ic_transform_stmt_ret: kludge was null");
        return 0;
    }

    if (!scope) {
        puts("ic_transform_stmt_ret: scope was null");
        return 0;
    }

    if (!tbody) {
        puts("ic_transform_stmt_ret: tbody was null");
        return 0;
    }

    if (!body) {
        puts("ic_transform_stmt_ret: body was null");
        return 0;
    }

    if (!ret) {
        puts("ic_transform_stmt_ret: ret was null");
        return 0;
    }

    expr = ret->ret;

    /* if expr is already an identifier then we don't need a temp */
    if (expr->tag == ic_expr_type_identifier) {
        name = &expr->u.id.identifier;
    } else {
        name = ic_transform_new_temp(kludge, scope, tbody, expr);
        if (!name) {
            puts("ic_transform_stmt_ret: call to ic_transform_new_temp failed");
            return 0;
        }
    }

    stmt = ic_transform_ir_stmt_ret_new(name);
    if (!stmt) {
        puts("ic_transform_stmt_ret: call to ic_transform_ir_stmt_ret_new failed");
        return 0;
    }

    if (!ic_transform_body_append(tbody, stmt)) {
        puts("ic_transform_stmt_ret: call to ic_transform_body_append failed");
        return 0;
    }

    return 1;
}

/* perform translation of a single `let` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_let(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_let *let) {
    struct ic_expr_constant *cons = 0;
    struct ic_expr_func_call *fcall = 0;
    struct ic_transform_ir_expr *expr = 0;
    struct ic_transform_ir_stmt *stmt = 0;
    struct ic_transform_ir_fcall *tir_fcall = 0;
    struct ic_transform_ir_let *tlet = 0;

    unsigned int fake_indent = 1;

    if (!kludge) {
        puts("ic_transform_stmt_let: kludge was null");
        return 0;
    }

    if (!scope) {
        puts("ic_transform_stmt_let: scope was null");
        return 0;
    }

    if (!tbody) {
        puts("ic_transform_stmt_let: tbody was null");
        return 0;
    }

    if (!body) {
        puts("ic_transform_stmt_let: body was null");
        return 0;
    }

    if (!let) {
        puts("ic_transform_stmt_let: let was null");
        return 0;
    }

    fcall = 0;

    switch (let->init->tag) {
        case ic_expr_type_operator:
            if (!let->init->u.op.fcall) {
                /* all operators should have had their fcall set
                 * during analysis
                 */
                puts("ic_transform_stmt_let: let->init->u.op.fcall was null");
                return 0;
            }
            fcall = let->init->u.op.fcall;

        /* share body with func_call */

        case ic_expr_type_func_call:
            /* fcall may be set by _operator */
            if (!fcall) {
                fcall = &(let->init->u.fcall);
            }

            tir_fcall = ic_transform_fcall(kludge, scope, tbody, fcall);
            if (!tir_fcall) {
                puts("ic_transform_stmt_let: call to ic_transform_ir_fcall failed");
                return 0;
            }

            expr = ic_transform_ir_expr_new();
            if (!expr) {
                puts("ic_transform_stmt_let: call to ic_transform_ir_expr_new failed");
                return 0;
            }

            /* FIXME TODO ownership ... */
            expr->fcall = tir_fcall;

            stmt = ic_transform_ir_stmt_let_expr_new(&(let->identifier), let->inferred_type, expr);
            if (!stmt) {
                puts("ic_transform_stmt_let: call to ic_transform_ir_stmt_let_expr_new failed");
                return 0;
            }
            if (!ic_transform_body_append(tbody, stmt)) {
                puts("ic_transform_stmt_let: call to ic_transform_body_append failed");
                return 0;
            }

            tlet = ic_transform_ir_stmt_get_let(stmt);
            if (!tlet) {
                puts("ic_transform_stmt_let: call to ic_transform_ir_stmt_get_let failed");
                return 0;
            }

            break;

        case ic_expr_type_identifier:
            puts("ic_transform_stmt_let: unsupported let->init->tag: identifier in statement:");
            ic_stmt_let_print(stdout, let, &fake_indent);
            return 0;
            break;

        case ic_expr_type_constant:
            cons = &(let->init->u.cons);
            stmt = ic_transform_ir_stmt_let_literal_new(&(let->identifier), let->inferred_type, cons);
            if (!stmt) {
                puts("ic_transform_stmt_let: call to ic_transform_ir_stmt_let_literal_new failed");
                return 0;
            }
            if (!ic_transform_body_append(tbody, stmt)) {
                puts("ic_transform_stmt_let: call to ic_transform_body_append failed");
                return 0;
            }

            tlet = ic_transform_ir_stmt_get_let(stmt);
            if (!tlet) {
                puts("ic_transform_stmt_let: call to ic_transform_ir_stmt_get_let failed");
                return 0;
            }

            break;

        default:
            puts("ic_transform_stmt_let: unsupported let->init->tag");
            return 0;
            break;
    }

    if (!tlet) {
        puts("ic_transform_stmt_let: tlet was null");
        return 0;
    }

    /* copy over assigned_t0 field */
    tlet->assigned_to = let->assigned_to;

    return 1;
}

/* perform translation of a single `assign` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_assign(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_assign *assign) {
    struct ic_expr_identifier *id = 0;
    struct ic_expr_func_call *fcall = 0;
    struct ic_transform_ir_assign *tassign = 0;
    struct ic_transform_ir_stmt *tstmt = 0;
    char *ch = 0;
    struct ic_slot *slot = 0;

    if (!kludge) {
        puts("ic_transform_stmt_assign: kludge was null");
        return 0;
    }

    if (!scope) {
        puts("ic_transform_stmt_assign: scope was null");
        return 0;
    }

    if (!tbody) {
        puts("ic_transform_stmt_assign: tbody was null");
        return 0;
    }

    if (!body) {
        puts("ic_transform_stmt_assign: body was null");
        return 0;
    }

    if (!assign) {
        puts("ic_transform_stmt_assign: assign was null");
        return 0;
    }

    if (!assign->left) {
        puts("ic_transform_stmt_assign: assign->left was null");
        return 0;
    }

    if (!assign->right) {
        puts("ic_transform_stmt_assign: assign->right was null");
        return 0;
    }

    switch (assign->left->tag) {
        case ic_expr_type_identifier:
            id = &(assign->left->u.id);
            break;

        default:
            puts("ic_transform_stmt_assign: assign->left was not an identifier");
            return 0;
            break;
    }

    switch (assign->right->tag) {
        case ic_expr_type_identifier:
            puts("ic_transform_stmt_assign: assign->right identifier unsupported");
            return 0;
            break;

        case ic_expr_type_constant:
            puts("ic_transform_stmt_assign: assign->right constant unsupported");
            return 0;
            break;

        case ic_expr_type_func_call:
            fcall = &(assign->right->u.fcall);
            break;

        case ic_expr_type_operator:
            if (!assign->right->u.op.fcall) {
                puts("ic_transform_stmt_assign: operator didn't have fcall set");
                return 0;
            }
            fcall = assign->right->u.op.fcall;
            break;

        default:
            puts("ic_transform_stmt_assign: unsupported case");
            return 0;
            break;
    }

    tstmt = ic_transform_ir_stmt_new(ic_transform_ir_stmt_type_assign);
    if (!tstmt) {
        puts("ic_transform_stmt_assign: call to ic_transform_ir_stmt_new failed");
        return 0;
    }

    tassign = &(tstmt->u.assign);

    /* from
     *  struct ic_stmt_assign {
     *      struct ic_expr *left;
     *      struct ic_expr *right;
     *  };
     */

    /* to
     *  struct ic_transform_ir_assign {
     *      struct ic_symbol *left;
     *      struct ic_transform_ir_expr *right;
     *  };
     */

    tassign->left = &(id->identifier);

    /* mark left as requiring runtime storage */
    ch = ic_symbol_contents(tassign->left);
    if (!ch) {
        puts("ic_transform_stmt_assign: call to ic_symbol_contents failed");
        return 0;
    }

    slot = ic_scope_get(scope, ch);
    if (!slot) {
        puts("ic_transform_stmt_assign: call to ic_scope_get failed");
        return 0;
    }

    tassign->right = ic_transform_ir_expr_new();
    if (!tassign->right) {
        puts("ic_transform_stmt_assign: call to ic_transform_ir_expr_new failed");
        return 0;
    }

    tassign->right->fcall = ic_transform_fcall(kludge, scope, tbody, fcall);
    if (!tassign->right->fcall) {
        puts("ic_transform_stmt_assign: call to ic_transform_fcall failed");
        return 0;
    }

    if (!ic_transform_body_append(tbody, tstmt)) {
        puts("ic_transform_stmt_assign: call to ic_transform_body_append failed");
        return 0;
    }

    return 1;
}

/* perform translation of a single `if` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_if(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_if *sif) {
    struct ic_transform_ir_stmt *tstmt = 0;
    struct ic_transform_ir_if *tif = 0;
    struct ic_symbol *cond_sym = 0;
    if (!kludge) {
        puts("ic_transform_stmt_if: kludge was null");
        return 0;
    }

    if (!scope) {
        puts("ic_transform_stmt_if: scope was null");
        return 0;
    }

    if (!tbody) {
        puts("ic_transform_stmt_if: tbody was null");
        return 0;
    }

    if (!body) {
        puts("ic_transform_stmt_if: body was null");
        return 0;
    }

    if (!sif) {
        puts("ic_transform_stmt_if: sif was null");
        return 0;
    }

    /* process if-expr */
    cond_sym = ic_transform_new_temp(kludge, scope, tbody, sif->expr);
    if (!cond_sym) {
        puts("ic_transform_stmt_if: call to ic_transform_new_temp failed");
        return 0;
    }

    /* make our new tif */
    tstmt = ic_transform_ir_stmt_if_new(cond_sym);
    if (!tstmt) {
        puts("ic_transform_stmt_if: call to ic_transform_ir_stmt_if_new failed");
        return 0;
    }

    /* get our tif out */
    tif = ic_transform_ir_stmt_get_if(tstmt);
    if (!tif) {
        puts("ic_transform_stmt_if: call to ic_transform_ir_stmt_get_if failed");
        return 0;
    }

    /* create new nested body */
    tif->then_tbody = ic_transform_body_new_child(tbody);
    if (!tif->then_tbody) {
        puts("ic_transform_fdecl: call to ic_transform_body_new_child failed");
        return 0;
    }

    /* dispatch to transform_body for work */
    if (!ic_transform_body(kludge, scope, tif->then_tbody, sif->then_body)) {
        puts("ic_transform_fdecl: call to ic_transform_body failed");
        return 0;
    }

    /* FIXME TODO
     * deal with optional else clause
     */
    if (sif->else_body) {
        /* create new nested body */
        tif->else_tbody = ic_transform_body_new_child(tbody);
        if (!tif->else_tbody) {
            puts("ic_transform_fdecl: call to ic_transform_body_new failed");
            return 0;
        }

        /* dispatch to transform_body for work */
        if (!ic_transform_body(kludge, scope, tif->else_tbody, sif->else_body)) {
            puts("ic_transform_fdecl: call to ic_transform_body failed");
            return 0;
        }
    }

    if (!ic_transform_body_append(tbody, tstmt)) {
        puts("ic_transform_stmt_ret: call to ic_transform_body_append failed");
        return 0;
    }

    return 1;
}

/* perform translation of a single `for` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_for(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_for *sfor) {
    if (!kludge) {
        puts("ic_transform_stmt_for: kludge was null");
        return 0;
    }

    if (!tbody) {
        puts("ic_transform_stmt_for: tbody was null");
        return 0;
    }

    if (!body) {
        puts("ic_transform_stmt_for: body was null");
        return 0;
    }

    if (!sfor) {
        puts("ic_transform_stmt_for: sfor was null");
        return 0;
    }

    puts("ic_transform_stmt_for: unimplemented");
    return 0;
}

/* perform translation of a single `while` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_while(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_while *swhile) {
    if (!kludge) {
        puts("ic_transform_stmt_while: kludge was null");
        return 0;
    }

    if (!scope) {
        puts("ic_transform_stmt_while: scope was null");
        return 0;
    }

    if (!tbody) {
        puts("ic_transform_stmt_while: tbody was null");
        return 0;
    }

    if (!body) {
        puts("ic_transform_stmt_while: body was null");
        return 0;
    }

    if (!swhile) {
        puts("ic_transform_stmt_while: swhile was null");
        return 0;
    }

    puts("ic_transform_stmt_while: unimplemented");
    return 0;
}

/* perform translation of a single `expr` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_expr(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_transform_body *tbody, struct ic_body *body, struct ic_expr *expr) {
    struct ic_transform_ir_stmt *tir_stmt = 0;
    struct ic_transform_ir_expr *tir_expr = 0;
    struct ic_expr_func_call *fcall = 0;

    if (!kludge) {
        puts("ic_transform_stmt_expr: kludge was null");
        return 0;
    }

    if (!scope) {
        puts("ic_transform_stmt_expr: scope was null");
        return 0;
    }

    if (!tbody) {
        puts("ic_transform_stmt_expr: tbody was null");
        return 0;
    }

    if (!body) {
        puts("ic_transform_stmt_expr: body was null");
        return 0;
    }

    if (!expr) {
        puts("ic_transform_stmt_expr: expr was null");
        return 0;
    }

    switch (expr->tag) {
        case ic_expr_type_func_call:
            tir_stmt = ic_transform_ir_stmt_new(ic_transform_ir_stmt_type_expr);
            if (!tir_stmt) {
                puts("ic_transform_stmt_expr: call to ic_transform_ir_stmt_new failed");
                return 0;
            }
            fcall = &(expr->u.fcall);
            tir_expr = &(tir_stmt->u.expr);
            tir_expr->fcall = ic_transform_fcall(kludge, scope, tbody, fcall);
            if (!tir_expr->fcall) {
                puts("ic_transform_stmt_expr: call to ic_transform_fcall failed");
                return 0;
            }
            if (!ic_transform_body_append(tbody, tir_stmt)) {
                puts("ic_transform_stmt_expr: call to ic_transform_body_append failed");
                return 0;
            }
            /* success */
            return 1;
            break;

        case ic_expr_type_identifier:
            puts("ic_transform_stmt_expr: unimplemented expr->tag");
            return 0;
            break;

        case ic_expr_type_constant:
            puts("ic_transform_stmt_expr: unimplemented expr->tag");
            return 0;
            break;

        case ic_expr_type_operator:
            puts("ic_transform_stmt_expr: unimplemented expr->tag");
            return 0;
            break;

        case ic_expr_type_field_access:
            puts("ic_transform_stmt_expr: unimplemented expr->tag");
            return 0;
            break;

        default:
            puts("ic_transform_stmt_expr: impossible expr->tag");
            return 0;
            break;
    }

    return 1;
}

/* register and append a new temporary expr as a let
 * generates a new symbol name and returns it
 *
 * returns * on success
 * returns 0 on failure
 */
static struct ic_symbol *ic_transform_new_temp(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_transform_body *tbody, struct ic_expr *expr) {
    /* the tir stmt we generate */
    struct ic_transform_ir_stmt *tir_stmt = 0;

    /* symbol */
    struct ic_symbol *sym = 0;
    /* type */
    struct ic_type *type = 0;

    /* constant used in literal case */
    struct ic_expr_constant *constant = 0;

    /* fcall used in func call case */
    struct ic_expr_func_call *fcall = 0;
    /* tir fcall used in func_call case */
    struct ic_transform_ir_fcall *tir_fcall = 0;
    /* tir expr wrapping fcall used in func_call case */
    struct ic_transform_ir_expr *tir_expr = 0;
    /* left symbol for faccess */
    struct ic_symbol *left_sym = 0;
    /* right symbol for faccess */
    struct ic_symbol *right_sym = 0;

    if (!kludge) {
        puts("ic_transform_new_temp: kludge was null");
        return 0;
    }

    if (!scope) {
        puts("ic_transform_new_temp: scope was null");
        return 0;
    }

    if (!tbody) {
        puts("ic_transform_new_temp: tbody was null");
        return 0;
    }

    if (!expr) {
        puts("ic_transform_new_temp: expr was null");
        return 0;
    }

    /* FIXME TODO */
    /* register on tbody */
    /* generate new unique temporary symbol */
    /* create a new let for this expr */
    /* append new let to tbody */
    /* return sym name*/

    switch (expr->tag) {
        case ic_expr_type_func_call:
            /* generate name */
            sym = ic_transform_body_register_temporary(tbody);
            if (!sym) {
                puts("ic_transform_new_temp: call to ic_transform_body_register_temporary failed");
                return 0;
            }

            /* get return type of function call */
            type = ic_analyse_infer(kludge, scope, expr);
            if (!type) {
                puts("ic_transform_new_temp: call to ic_analyse_infer failed");
                return 0;
            }

            /* unwrap fcall */
            fcall = ic_expr_get_fcall(expr);
            if (!fcall) {
                puts("ic_transform_new_temp: call to ic_expr_get_fcall failed");
                return 0;
            }

            /* transform fcall */
            tir_fcall = ic_transform_fcall(kludge, scope, tbody, fcall);
            if (!tir_fcall) {
                puts("ic_transform_new_temp: call to ic_transform_fcall failed");
                return 0;
            }

            /* wrap in tir_expr */
            tir_expr = ic_transform_ir_expr_new();
            if (!tir_expr) {
                puts("ic_transform_new_temp: call to ic_transform_ir_expr_new failed");
                return 0;
            }
            /* wrap tir_fcall in tir_expr */
            tir_expr->fcall = tir_fcall;

            /* generate new statement */
            tir_stmt = ic_transform_ir_stmt_let_expr_new(sym, type, tir_expr);
            if (!tir_stmt) {
                puts("ic_transform_new_temp: call to ic_transform_ir_stmt_let_expr_new failed");
                return 0;
            }

            if (!ic_transform_body_append(tbody, tir_stmt)) {
                puts("ic_transform_new_temp: call to ic_transform_body_append failed");
                return 0;
            }

            /* success */
            return sym;
            break;

        case ic_expr_type_identifier:
            /* should never try to do this */
            puts("ic_transform_new_temp: caller passed in expr->tag identifier - doesn't make sense");
            return 0;
            break;

        case ic_expr_type_constant:
            /* generate name */
            sym = ic_transform_body_register_literal(tbody);
            if (!sym) {
                puts("ic_transform_new_temp: call to ic_transform_body_register_literal failed");
                return 0;
            }

            /* unpack literal */
            constant = ic_expr_get_constant(expr);
            if (!constant) {
                puts("ic_transform_new_temp: call to ic_expr_get_constant failed");
                return 0;
            }

            type = ic_analyse_infer_constant(kludge, constant);
            if (!type) {
                puts("ic_transform_new_temp: call to ic_anlyse_infer_constant failed");
                return 0;
            }

            /* generate new statement */
            tir_stmt = ic_transform_ir_stmt_let_literal_new(sym, type, constant);
            if (!tir_stmt) {
                puts("ic_transform_new_temp: call to ic_transform_ir_stmt_let_literal_new failed");
                return 0;
            }

            if (!ic_transform_body_append(tbody, tir_stmt)) {
                puts("ic_transform_new_temp: call to ic_transform_body_append failed");
                return 0;
            }

            /* success */
            return sym;
            break;

        case ic_expr_type_operator:
            if (!expr->u.op.fcall) {
                /* all operators should have had their fcall set
                 * during analysis
                 */
                puts("ic_transform_new_temp: caller passed in expr->tag operator without fcall set");
                return 0;
            }

            /* generate name */
            sym = ic_transform_body_register_temporary(tbody);
            if (!sym) {
                puts("ic_transform_new_temp: call to ic_transform_body_register_temporary failed");
                return 0;
            }

            /* unwrap fcall */
            fcall = expr->u.op.fcall;

            /* get return type of function call */
            type = ic_analyse_infer_fcall(kludge, scope, fcall);
            if (!type) {
                puts("ic_transform_new_temp: call to ic_analyse_infer_fcall failed");
                return 0;
            }

            /* transform fcall */
            tir_fcall = ic_transform_fcall(kludge, scope, tbody, fcall);
            if (!tir_fcall) {
                puts("ic_transform_new_temp: call to ic_transform_fcall failed");
                return 0;
            }

            /* wrap in tir_expr */
            tir_expr = ic_transform_ir_expr_new();
            if (!tir_expr) {
                puts("ic_transform_new_temp: call to ic_transform_ir_expr_new failed");
                return 0;
            }
            /* wrap tir_fcall in tir_expr */
            tir_expr->fcall = tir_fcall;

            /* generate new statement */
            tir_stmt = ic_transform_ir_stmt_let_expr_new(sym, type, tir_expr);
            if (!tir_stmt) {
                puts("ic_transform_new_temp: call to ic_transform_ir_stmt_let_expr_new failed");
                return 0;
            }

            if (!ic_transform_body_append(tbody, tir_stmt)) {
                puts("ic_transform_new_temp: call to ic_transform_body_append failed");
                return 0;
            }

            /* success */
            return sym;
            break;

        case ic_expr_type_field_access:
            /* generate name */
            sym = ic_transform_body_register_temporary(tbody);
            if (!sym) {
                puts("ic_transform_new_temp: call to ic_transform_body_register_temporary failed");
                return 0;
            }

            /* get return type of inner struct field */
            type = ic_analyse_infer(kludge, scope, expr);
            if (!type) {
                puts("ic_transform_new_temp: call to ic_analyse_infer failed");
                return 0;
            }

            if (!expr->u.faccess.right) {
                puts("ic_transform_new_temp: faccess right was null");
                return 0;
            }

            /* right must always be an id */
            if (expr->u.faccess.right->tag != ic_expr_type_identifier) {
                puts("ic_transform_new_temp: faccess right was not id");
                return 0;
            }
            right_sym = &(expr->u.faccess.right->u.id.identifier);

            if (!expr->u.faccess.left) {
                puts("ic_transform_new_temp: faccess left was null");
                return 0;
            }

            /* left can either be an id or a compound expression */
            if (expr->u.faccess.left->tag == ic_expr_type_identifier) {
                left_sym = &(expr->u.faccess.left->u.id.identifier);
            } else {
                left_sym = ic_transform_new_temp(kludge, scope, tbody, expr->u.faccess.left);
                if (!left_sym) {
                    puts("ic_transform_new_temp: call to ic_transform_new_temp failed");
                    return 0;
                }
            }

            /* generate new statement */
            tir_stmt = ic_transform_ir_stmt_let_faccess_new(sym, type, left_sym, right_sym);
            if (!tir_stmt) {
                puts("ic_transform_new_temp: call to ic_transform_ir_stmt_let_expr_new failed");
                return 0;
            }

            if (!ic_transform_body_append(tbody, tir_stmt)) {
                puts("ic_transform_new_temp: call to ic_transform_body_append failed");
                return 0;
            }

            /* success */
            return sym;
            break;

        default:
            puts("ic_transform_new_temp: caller passed in imposible/unknown expr->tag");
            return 0;
            break;
    }

    puts("ic_transform_new_temp: impossible! fell off end of switch");
    return 0;
}

/* transform an fcall to tir_fcall
 *
 * returns 1 on success
 * returns 0 on failure
 */
static struct ic_transform_ir_fcall *ic_transform_fcall(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_transform_body *tbody, struct ic_expr_func_call *fcall) {

    /* offset into fcall->args */
    unsigned int i = 0;
    /* length of fcall->args */
    unsigned int len = 0;
    /* current fcall->arg */
    struct ic_expr *arg = 0;
    /* the symbol representing this argument */
    struct ic_symbol *sym = 0;

    /* our new pvector of symbols
     * each representing the name for a let expr
     * holding an argument for this fcall
     * this is owned by the tir_fcall
     */
    struct ic_pvector *new_args = 0;

    /* our eventual output tir_fcall */
    struct ic_transform_ir_fcall *tir_fcall = 0;

    if (!kludge) {
        puts("ic_transform_fcall: kludge was null");
        return 0;
    }

    if (!scope) {
        puts("ic_transform_fcall: scope was null");
        return 0;
    }

    if (!tbody) {
        puts("ic_transform_fcall: tbody was null");
        return 0;
    }

    if (!fcall) {
        puts("ic_transform_fcall: fcall was null");
        return 0;
    }

    new_args = ic_pvector_new(0);
    if (!new_args) {
        puts("ic_transform_fcall: call to ic_pvector_new failed");
        return 0;
    }

    /* for each args */
    len = ic_expr_func_call_length(fcall);
    for (i = 0; i < len; ++i) {
        arg = ic_expr_func_call_get_arg(fcall, i);
        if (!arg) {
            puts("ic_transform_fcall: call to ic_expr_func_call_get_arg failed");
            return 0;
        }

        /* build up new_args */
        sym = ic_transform_fcall_arg(kludge, scope, tbody, arg);
        if (!sym) {
            puts("ic_transform_fcall: call to ic_transform_fcall_arg failed");
            return 0;
        }

        if (-1 == ic_pvector_append(new_args, sym)) {
            puts("ic_transform_fcall: call to ic_pvector_append failed");
            return 0;
        }
    }

    /* FIXME TODO ownership ... */
    tir_fcall = ic_transform_ir_fcall_new(fcall, new_args);
    if (!tir_fcall) {
        puts("ic_transform_fcall: call to ic_transform_ir_fcall_new failed");
        return 0;
    }

    return tir_fcall;
}

/* transform an fcall argument to an symbol
 * if the arg is already a symbol, just return it
 * otherwise create a new let binding the result of expr
 *
 * returns * on success
 * returns 0 on failure
 */
static struct ic_symbol *ic_transform_fcall_arg(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_transform_body *tbody, struct ic_expr *arg) {
    /* FIXME ownership */
    struct ic_symbol *sym = 0;

    if (!kludge) {
        puts("ic_transform_fcall_arg: kludge was null");
        return 0;
    }

    if (!scope) {
        puts("ic_transform_fcall_arg: scope was null");
        return 0;
    }

    if (!tbody) {
        puts("ic_transform_fcall_arg: tbody was null");
        return 0;
    }

    if (!arg) {
        puts("ic_transform_fcall_arg: arg was null");
        return 0;
    }

    switch (arg->tag) {
        case ic_expr_type_func_call:
            sym = ic_transform_new_temp(kludge, scope, tbody, arg);
            if (!sym) {
                puts("ic_transform_fcall_arg: call to ic_transform_new_temp (for func_call) failed");
                return 0;
            }
            return sym;
            break;

        case ic_expr_type_identifier:
            /* FIXME ownership */
            sym = &(arg->u.id.identifier);
            /* FIXME ownership */
            return sym;
            break;

        case ic_expr_type_constant:
            sym = ic_transform_new_temp(kludge, scope, tbody, arg);
            if (!sym) {
                puts("ic_transform_fcall_arg: call to ic_transform_new_temp (for constant) failed");
                return 0;
            }
            return sym;
            break;

        case ic_expr_type_operator:
            sym = ic_transform_new_temp(kludge, scope, tbody, arg);
            if (!sym) {
                puts("ic_transform_fcall_arg: call to ic_transform_new_temp (for operator) failed");
                return 0;
            }
            return sym;
            break;

        case ic_expr_type_field_access:
            sym = ic_transform_new_temp(kludge, scope, tbody, arg);
            if (!sym) {
                puts("ic_transform_fcall_arg: call to ic_transform_new_temp (for field access) failed");
                return 0;
            }
            return sym;
            break;

        default:
            puts("ic_transform_fcall_arg: impossible/unknown arg->tag");
            return 0;
            break;
    }

    return 0;
}
