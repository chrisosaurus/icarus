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

/* FIXME TODO ic_transform_stmt_for scope unused as unimplemented */
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
static unsigned int ic_transform_body(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *tbody, struct ic_body *body);

/* perform translation of a single stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt *stmt);

/* perform translation of a single `ret` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_ret(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_ret *ret);

/* perform translation of a single `let` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_let(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_let *let);

/* perform translation of a single `assign` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_assign(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_assign *assign);

/* perform translation of a single `begin` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_begin(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_begin *begin);

/* perform translation of a single `if` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_if(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_if *sif);

/* perform translation of a single `for` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_for(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_for *sfor);

/* perform translation of a single `while` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_while(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_while *swhile);

/* perform translation of a single `expr` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_expr(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *tbody, struct ic_body *body, struct ic_expr *expr);

/* perform translation of a single `match` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_match(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_match *match);

/* register and append a new temporary expr as a let
 * generates a new symbol name and returns it
 *
 * returns * on success
 * returns 0 on failure
 */
static struct ic_symbol *ic_transform_new_temp(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *tbody, struct ic_body *body, struct ic_expr *expr);

/* transform an expr to tir_expr
 *
 * returns * on success
 * returns 0 on failure
 */
static struct ic_transform_ir_expr *ic_transform_expr(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *tbody, struct ic_body *body, struct ic_expr *expr);

/* transform an fcall to tir_expr_fcall
 *
 * returns * on success
 * returns 0 on failure
 */
static struct ic_transform_ir_expr_fcall *ic_transform_fcall(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *tbody, struct ic_body *body, struct ic_expr_func_call *fcall);

/* transform an fcall argument to an symbol
 * if the arg is already a symbol, just return it
 * otherwise create a new let binding the result of expr
 *
 * returns * on success
 * returns 0 on failure
 */
static struct ic_symbol *ic_transform_fcall_arg(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *tbody, struct ic_body *body, struct ic_expr *arg);

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
    /* i for kludge fdecls / tdecls */
    unsigned int i = 0;
    /* len for kludge fdecls / tdecls */
    unsigned int len = 0;
    /* current fdecl we are considering */
    struct ic_decl_func *fdecl = 0;
    /* current tdecl we are considering */
    struct ic_decl_type *tdecl = 0;
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

    /* go over tdecls */
    len = ic_pvector_length(&(kludge->tdecls));

    for (i = 0; i < len; ++i) {
        tdecl = ic_pvector_get(&(kludge->tdecls), i);
        if (!tdecl) {
            puts("ic_transform_print: call to ic_pvector_get failed");
            return 0;
        }

        /* skip printing of builtins */
        if (ic_decl_type_isbuiltin(tdecl)) {
            continue;
        }

        /* skip uninstantiated generics */
        if (!ic_decl_type_is_instantiated(tdecl)) {
            continue;
        }

        /* print tdecl */
        ic_decl_type_print(fd, tdecl, &fake_indent);
    }

    /* go over fdecls */
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

        /* skip uninstantiated generics */
        if (!ic_decl_func_is_instantiated(fdecl)) {
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

        /* skip if not instantiated */
        if (!ic_decl_func_is_instantiated(fdecl)) {
            continue;
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
    if (!ic_transform_body(kludge, func, func->tbody, &(func->body))) {
        puts("ic_transform_fdecl: call to ic_transform_body failed");
        return 0;
    }

    return 1;
}

static unsigned int ic_transform_body(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *tbody, struct ic_body *body) {
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

    if (!fdecl) {
        puts("ic_transform_body: fdecl was null");
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
        if (!ic_transform_stmt(kludge, fdecl, tbody, body, stmt)) {
            puts("ic_transform_body: call to ic_transform_stmt failed");
            return 0;
        }
    }

    return 1;
}

static unsigned int ic_transform_stmt(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt *stmt) {
    if (!kludge) {
        puts("ic_transform_stmt: kludge was null");
        return 0;
    }

    if (!fdecl) {
        puts("ic_transform_stmt: fdecl was null");
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
            if (!ic_transform_stmt_ret(kludge, fdecl, tbody, body, &(stmt->u.ret))) {
                puts("ic_transform_stmt: call to ic_transform_stmt_ret failed");
                return 0;
            }
            break;

        case ic_stmt_type_let:
            if (!ic_transform_stmt_let(kludge, fdecl, tbody, body, &(stmt->u.let))) {
                puts("ic_transform_stmt: call to ic_transform_stmt_let failed");
                return 0;
            }
            break;

        case ic_stmt_type_assign:
            if (!ic_transform_stmt_assign(kludge, fdecl, tbody, body, &(stmt->u.assign))) {
                puts("ic_transform_stmt: call to ic_transform_stmt_assign failed");
                return 0;
            }
            break;

        case ic_stmt_type_begin:
            if (!ic_transform_stmt_begin(kludge, fdecl, tbody, body, &(stmt->u.begin))) {
                puts("ic_transform_stmt: call to ic_transform_stmt_begin failed");
                return 0;
            }
            break;

        case ic_stmt_type_if:
            if (!ic_transform_stmt_if(kludge, fdecl, tbody, body, &(stmt->u.sif))) {
                puts("ic_transform_stmt: call to ic_transform_stmt_if failed");
                return 0;
            }
            break;

        case ic_stmt_type_for:
            if (!ic_transform_stmt_for(kludge, fdecl, tbody, body, &(stmt->u.sfor))) {
                puts("ic_transform_stmt: call to ic_transform_stmt_for failed");
                return 0;
            }
            break;

        case ic_stmt_type_while:
            if (!ic_transform_stmt_while(kludge, fdecl, tbody, body, &(stmt->u.swhile))) {
                puts("ic_transform_stmt: call to ic_transform_stmt_while failed");
                return 0;
            }
            break;

        case ic_stmt_type_expr:
            if (!ic_transform_stmt_expr(kludge, fdecl, tbody, body, stmt->u.expr)) {
                puts("ic_transform_stmt: call to ic_transform_stmt_expr failed");
                return 0;
            }
            break;

        case ic_stmt_type_match:
            if (!ic_transform_stmt_match(kludge, fdecl, tbody, body, &(stmt->u.match))) {
                puts("ic_transform_stmt: call to ic_transform_stmt_match failed");
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
static unsigned int ic_transform_stmt_ret(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_ret *ret) {
    struct ic_expr *expr = 0;
    struct ic_transform_ir_stmt *stmt = 0;
    struct ic_symbol *name = 0;

    if (!kludge) {
        puts("ic_transform_stmt_ret: kludge was null");
        return 0;
    }

    if (!fdecl) {
        puts("ic_transform_stmt_ret: tbody was null");
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
        name = ic_transform_new_temp(kludge, fdecl, tbody, body, expr);
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
static unsigned int ic_transform_stmt_let(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_let *let) {
    struct ic_expr_constant *cons = 0;
    struct ic_expr_func_call *fcall = 0;
    struct ic_transform_ir_expr *expr = 0;
    struct ic_transform_ir_stmt *stmt = 0;
    struct ic_transform_ir_expr_fcall *tir_expr_fcall = 0;
    struct ic_transform_ir_let *tlet = 0;
    struct ic_symbol *left_sym = 0;
    struct ic_symbol *right_sym = 0;
    struct ic_decl_type *left_tdecl = 0;

    unsigned int fake_indent = 1;

    struct ic_decl_type *tdecl = 0;

    if (!kludge) {
        puts("ic_transform_stmt_let: kludge was null");
        return 0;
    }

    if (!fdecl) {
        puts("ic_transform_stmt_let: fdecl was null");
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

    expr = ic_transform_expr(kludge, fdecl, tbody, body, let->init);
    if (!expr) {
        puts("ic_transform_stmt_let: call to ic_transform_expr failed");
        return 0;
    }

    tdecl = ic_type_ref_get_type_decl(let->tref);
    if (!tdecl) {
        puts("ic_transform_stmt_let: call to ic_type_ref_get_type_decl failed");
        return 0;
    }

    stmt = ic_transform_ir_stmt_let_expr_new(&(let->identifier), tdecl, expr);
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

    if (!tlet) {
        puts("ic_transform_stmt_let: tlet was null");
        return 0;
    }

    /* copy over assigned_to field */
    tlet->assigned_to = let->assigned_to;

    return 1;

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

            tir_expr_fcall = ic_transform_fcall(kludge, fdecl, tbody, body, fcall);
            if (!tir_expr_fcall) {
                puts("ic_transform_stmt_let: call to ic_transform_ir_expr_fcall failed");
                return 0;
            }

            expr = ic_transform_ir_expr_new(ic_transform_ir_expr_type_fcall);
            if (!expr) {
                puts("ic_transform_stmt_let: call to ic_transform_ir_expr_new failed");
                return 0;
            }

            /* FIXME TODO ownership ... */
            expr->u.fcall = tir_expr_fcall;

            tdecl = ic_type_ref_get_type_decl(let->tref);
            if (!tdecl) {
                puts("ic_transform_stmt_let: call to ic_type_ref_get_type_decl failed");
                return 0;
            }

            stmt = ic_transform_ir_stmt_let_expr_new(&(let->identifier), tdecl, expr);
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

            tdecl = ic_type_ref_get_type_decl(let->tref);
            if (!tdecl) {
                puts("ic_transform_stmt_let: call to ic_type_ref_get_type_decl failed");
                return 0;
            }

            stmt = ic_transform_ir_stmt_let_literal_new(&(let->identifier), tdecl, cons);
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

        case ic_expr_type_field_access:
            /* left can either be an id or a compound expression */
            if (let->init->u.faccess.left->tag == ic_expr_type_identifier) {
                left_sym = &(let->init->u.faccess.left->u.id.identifier);
            } else {
                left_sym = ic_transform_new_temp(kludge, fdecl, tbody, body, let->init->u.faccess.left);
                if (!left_sym) {
                    puts("ic_transform_stmt_let: call to ic_transform_new_temp failed");
                    return 0;
                }
            }

            left_tdecl = ic_analyse_infer(kludge, fdecl, body->scope, let->init->u.faccess.left);
            if (!left_tdecl) {
                puts("ic_transform_stmt_let: call to ic_analyse_infer failed for let->init->u.faccess.left");
                return 0;
            }

            /* get return type of inner struct field */
            tdecl = ic_analyse_infer(kludge, fdecl, body->scope, let->init);
            if (!tdecl) {
                puts("ic_transform_new_temp: call to ic_analyse_infer failed");
                return 0;
            }

            if (!let->init->u.faccess.right) {
                puts("ic_transform_stmt_let: faccess right was null");
                return 0;
            }

            /* right must always be an id */
            if (let->init->u.faccess.right->tag != ic_expr_type_identifier) {
                puts("ic_transform_stmt_let: faccess right was not id");
                return 0;
            }
            right_sym = &(let->init->u.faccess.right->u.id.identifier);

            stmt = ic_transform_ir_stmt_let_faccess_new(&(let->identifier), tdecl, left_sym, left_tdecl, right_sym);
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

    /* copy over assigned_to field */
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
static unsigned int ic_transform_stmt_assign(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_assign *assign) {
    struct ic_expr_identifier *id = 0;
    struct ic_transform_ir_assign *tassign = 0;
    struct ic_transform_ir_stmt *tstmt = 0;
    char *ch = 0;
    struct ic_slot *slot = 0;
    enum ic_slot_assign_result assign_result = 0;

    if (!kludge) {
        puts("ic_transform_stmt_assign: kludge was null");
        return 0;
    }

    if (!fdecl) {
        puts("ic_transform_stmt_assign: fdecl was null");
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

    tstmt = ic_transform_ir_stmt_new(ic_transform_ir_stmt_type_assign);
    if (!tstmt) {
        puts("ic_transform_stmt_assign: call to ic_transform_ir_stmt_new failed");
        return 0;
    }

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

    tassign = &(tstmt->u.assign);

    tassign->left = &(id->identifier);

    /* mark left as requiring runtime storage */
    ch = ic_symbol_contents(tassign->left);
    if (!ch) {
        puts("ic_transform_stmt_assign: call to ic_symbol_contents failed");
        return 0;
    }

    slot = ic_scope_get(body->scope, ch);
    if (!slot) {
        puts("ic_transform_stmt_assign: call to ic_scope_get failed");
        return 0;
    }

    assign_result = ic_slot_assign(slot);
    switch (assign_result) {
        case ic_slot_assign_result_success:
            /* success */
            break;

        default:
            /* ERROR */
            puts("ic_trasnform_stmt_assign: call to ic_slot_assign failed");
            printf("failed to assign to `%s`\n", ch);
            return 0;
            break;
    }

    tassign->right = ic_transform_expr(kludge, fdecl, tbody, body, assign->right);
    if (!tassign->right) {
        puts("ic_transform_stmt_assign: call to ic_transform_expr failed");
        return 0;
    }

    if (!ic_transform_body_append(tbody, tstmt)) {
        puts("ic_transform_stmt_assign: call to ic_transform_body_append failed");
        return 0;
    }

    return 1;
}

/* perform translation of a single `begin` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_begin(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_begin *begin) {
    struct ic_transform_ir_stmt *tstmt = 0;
    struct ic_transform_ir_begin *tbegin = 0;

    if (!kludge) {
        puts("ic_transform_stmt_begin: kludge was null");
        return 0;
    }

    if (!fdecl) {
        puts("ic_transform_stmt_begin: fdecl was null");
        return 0;
    }

    if (!tbody) {
        puts("ic_transform_stmt_begin: tbody was null");
        return 0;
    }

    if (!body) {
        puts("ic_transform_stmt_begin: body was null");
        return 0;
    }

    if (!begin) {
        puts("ic_transform_stmt_begin: begin was null");
        return 0;
    }

    /* make our new begin */
    tstmt = ic_transform_ir_stmt_begin_new();
    if (!tstmt) {
        puts("ic_transform_stmt_begin: call to ic_transform_ir_stmt_begin_new failed");
        return 0;
    }

    /* get our begin out */
    tbegin = ic_transform_ir_stmt_get_begin(tstmt);
    if (!tbegin) {
        puts("ic_transform_stmt_begin: call to ic_transform_ir_stmt_get_begin failed");
        return 0;
    }

    /* create new nested body */
    tbegin->tbody = ic_transform_body_new_child(tbody);
    if (!tbegin->tbody) {
        puts("ic_transform_stmt_begin: call to ic_transform_body_new_child failed");
        return 0;
    }

    /* dispatch to transform_body for work */
    if (!ic_transform_body(kludge, fdecl, tbegin->tbody, begin->body)) {
        puts("ic_transform_stmt_begin: call to ic_transform_body failed");
        return 0;
    }

    if (!ic_transform_body_append(tbody, tstmt)) {
        puts("ic_transform_stmt_begin: call to ic_transform_body_append failed");
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
static unsigned int ic_transform_stmt_if(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_if *sif) {
    struct ic_transform_ir_stmt *tstmt = 0;
    struct ic_transform_ir_if *tif = 0;
    struct ic_symbol *cond_sym = 0;

    struct ic_expr_identifier *expr_id = 0;
    struct ic_decl_type *expr_id_type = 0;

    if (!kludge) {
        puts("ic_transform_stmt_if: kludge was null");
        return 0;
    }

    if (!fdecl) {
        puts("ic_transform_stmt_if: fdecl was null");
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

    if (!sif->expr) {
        puts("ic_transform_stmt_if: sif->expr was null");
        return 0;
    }

    /* if our if-expr is already an id to a boolean, no need to process */
    if (sif->expr->tag == ic_expr_type_identifier) {
        /* if this is an identifier, it must be of type boolean otherwise this
       * is an error
       */
        expr_id = ic_expr_get_identifier(sif->expr);
        if (!expr_id) {
            puts("ic_transform_stmt_if: call to ic_expr_get_identifier failed");
            return 0;
        }

        cond_sym = &(expr_id->identifier);

        /* FIXME TODO test if this is of type boolean */

        expr_id_type = ic_analyse_infer(kludge, fdecl, body->scope, sif->expr);
        if (!expr_id_type) {
            puts("ic_transform_stmt_if: call to ic_analyse_infer failed");
            return 0;
        }

        if (!ic_decl_type_isbool(expr_id_type)) {
            puts("ic_transform_stmt_if: error: identifier was not of type Bool");
            printf("  identifier: '%s'\n", ic_symbol_contents(cond_sym));
            return 0;
        }

    } else {
        /* process if-expr */
        cond_sym = ic_transform_new_temp(kludge, fdecl, tbody, body, sif->expr);
        if (!cond_sym) {
            puts("ic_transform_stmt_if: call to ic_transform_new_temp failed");
            return 0;
        }
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
        puts("ic_transform_stmt_if: call to ic_transform_body_new_child failed");
        return 0;
    }

    /* dispatch to transform_body for work */
    if (!ic_transform_body(kludge, fdecl, tif->then_tbody, sif->then_body)) {
        puts("ic_transform_stmt_if: call to ic_transform_body failed");
        return 0;
    }

    /* FIXME TODO
     * deal with optional else clause
     */
    if (sif->else_body) {
        /* create new nested body */
        tif->else_tbody = ic_transform_body_new_child(tbody);
        if (!tif->else_tbody) {
            puts("ic_transform_stmt_if: call to ic_transform_body_new failed");
            return 0;
        }

        /* dispatch to transform_body for work */
        if (!ic_transform_body(kludge, fdecl, tif->else_tbody, sif->else_body)) {
            puts("ic_transform_stmt_if: call to ic_transform_body failed");
            return 0;
        }
    }

    if (!ic_transform_body_append(tbody, tstmt)) {
        puts("ic_transform_stmt_if: call to ic_transform_body_append failed");
        return 0;
    }

    return 1;
}

/* perform translation of a single `match` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_match(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_match *match) {
    struct ic_transform_ir_stmt *tstmt = 0;
    struct ic_transform_ir_match *tmatch = 0;
    struct ic_transform_ir_match_case *tcase = 0;
    struct ic_symbol *match_symbol = 0;
    unsigned int len = 0;
    unsigned int i = 0;
    struct ic_stmt_case *scase = 0;
    struct ic_slot *slot = 0;
    struct ic_decl_type *tdecl = 0;

    if (!kludge) {
        puts("ic_transform_stmt_match: kludge was null");
        return 0;
    }

    if (!fdecl) {
        puts("ic_transform_stmt_match: fdecl was null");
        return 0;
    }

    if (!tbody) {
        puts("ic_transform_stmt_match: tbody was null");
        return 0;
    }

    if (!body) {
        puts("ic_transform_stmt_match: body was null");
        return 0;
    }

    if (!match) {
        puts("ic_transform_stmt_match: match was null");
        return 0;
    }

    /* get match_symbol ... */
    /* if our expr is already a symbol, then re-use */
    if (match->expr->tag == ic_expr_type_identifier) {
        match_symbol = &match->expr->u.id.identifier;

        /* fetch existing slot */
        slot = ic_scope_get_from_symbol(body->scope, match_symbol);
        if (!slot) {
            printf("ic_transform_stmt_match: lookup of '%s' failed\n", ic_symbol_contents(match_symbol));
            puts("ic_transform_stmt_match: call to ic_scope_get_from_symbol failed");
            return 0;
        }

        /* and get the type out */
        tdecl = slot->type;
        if (!tdecl) {
            puts("ic_transform_stmt_match: slot->type was null");
            return 0;
        }

    } else {
        /* otherwise compile down to a symbol */
        match_symbol = ic_transform_new_temp(kludge, fdecl, tbody, body, match->expr);
        if (!match_symbol) {
            puts("ic_transform_stmt_match: call to ic_transform_new_temp failed");
            return 0;
        }

        /* and work out type */
        tdecl = ic_analyse_infer(kludge, fdecl, body->scope, match->expr);
        if (!tdecl) {
            puts("ic_transform_stmt_match: call to ic_analyse_infer failed");
            return 0;
        }
    }

    tstmt = ic_transform_ir_stmt_match_new(tdecl, match_symbol);
    if (!tstmt) {
        puts("ic_transform_stmt_match: call to ic_transform_ir_stmt_match_new failed");
        return 0;
    }

    tmatch = ic_transform_ir_stmt_get_match(tstmt);
    if (!tmatch) {
        puts("ic_transform_stmt_match: call to ic_transform_ir_stmt_get_match failed");
        return 0;
    }

    /* append each case ... */
    len = ic_stmt_match_cases_length(match);
    for (i = 0; i < len; ++i) {
        scase = ic_stmt_match_cases_get(match, i);
        if (!scase) {
            puts("ic_transform_stmt_match: call to ic_stmt_match_cases_get failed");
            return 0;
        }

        tcase = ic_transform_ir_match_case_new(&(scase->field));
        if (!scase) {
            puts("ic_transform_stmt_match: call to ic_transform_ir_match_case_new failed");
            return 0;
        }

        /* populate tbody */
        tcase->tbody = ic_transform_body_new();
        if (!tcase->tbody) {
            puts("ic_transform_stmt_match: call to ic_transform_body_new failed");
            return 0;
        }

        /* need to create slot in scope to insert case expr */
        tdecl = ic_type_ref_get_type_decl(scase->field.type);
        if (!tdecl) {
            puts("ic_transform_stmt_match: call to ic_type_ref_get_type_dec; failed");
            return 0;
        }

        slot = ic_slot_new(&(scase->field.name), tdecl, 0, 0, ic_slot_type_let, scase);
        if (!slot) {
            puts("ic_transform_stmt_match: call to ic_slot_new failed");
            return 0;
        }

        /* dispatch to transform_body for work */
        if (!ic_transform_body(kludge, fdecl, tcase->tbody, scase->body)) {
            puts("ic_transform_stmt_match: call to ic_transform_body failed");
            return 0;
        }

        if (-1 == ic_pvector_append(&(tmatch->cases), tcase)) {
            puts("ic_transform_stmt_match: call to ic_pvector_append failed");
            return 0;
        }
    }

    /* append else if exists ... */
    if (match->else_body) {
        /* populate tbody */
        tmatch->else_body = ic_transform_body_new();
        if (!tmatch->else_body) {
            puts("ic_transform_stmt_match: call to ic_transform_body_new failed");
            return 0;
        }

        /* dispatch to transform_body for work */
        if (!ic_transform_body(kludge, fdecl, tmatch->else_body, match->else_body)) {
            puts("ic_transform_stmt_match: call to ic_transform_body failed");
            return 0;
        }
    }

    if (!ic_transform_body_append(tbody, tstmt)) {
        puts("ic_transform_stmt_match: call to ic_transform_body_append failed");
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
static unsigned int ic_transform_stmt_for(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_for *sfor) {
    if (!kludge) {
        puts("ic_transform_stmt_for: kludge was null");
        return 0;
    }

    if (!fdecl) {
        puts("ic_transform_stmt_for: fdecl was null");
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

    /* FIXME TODO once implemented remove pragma at top */
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
static unsigned int ic_transform_stmt_while(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_while *swhile) {
    if (!kludge) {
        puts("ic_transform_stmt_while: kludge was null");
        return 0;
    }

    if (!fdecl) {
        puts("ic_transform_stmt_while: fdecl was null");
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
static unsigned int ic_transform_stmt_expr(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *tbody, struct ic_body *body, struct ic_expr *expr) {
    struct ic_transform_ir_stmt *tir_stmt = 0;
    struct ic_expr_func_call *fcall = 0;

    if (!kludge) {
        puts("ic_transform_stmt_expr: kludge was null");
        return 0;
    }

    if (!fdecl) {
        puts("ic_transform_stmt_expr: fdecl was null");
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
            fcall = &(expr->u.fcall);
            tir_stmt = ic_transform_ir_stmt_new(ic_transform_ir_stmt_type_fcall);
            if (!tir_stmt) {
                puts("ic_transform_stmt_expr: call to ic_transform_ir_stmt_new failed");
                return 0;
            }
            tir_stmt->u.fcall = ic_transform_fcall(kludge, fdecl, tbody, body, fcall);
            if (!tir_stmt->u.fcall) {
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
static struct ic_symbol *ic_transform_new_temp(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *tbody, struct ic_body *body, struct ic_expr *expr) {
    /* the tir stmt we generate */
    struct ic_transform_ir_stmt *tir_stmt = 0;
    /* generated expr */
    struct ic_transform_ir_expr *tir_expr = 0;

    /* symbol */
    struct ic_symbol *sym = 0;
    /* type */
    struct ic_decl_type *type = 0;

    if (!kludge) {
        puts("ic_transform_new_temp: kludge was null");
        return 0;
    }

    if (!fdecl) {
        puts("ic_transform_new_temp: fdecl was null");
        return 0;
    }

    if (!tbody) {
        puts("ic_transform_new_temp: tbody was null");
        return 0;
    }

    if (!body) {
        puts("ic_transform_new_temp: body was null");
        return 0;
    }

    if (!expr) {
        puts("ic_transform_new_temp: expr was null");
        return 0;
    }

    /* generating a new name for an identifier doesn't make sense
     * since that identifier is already a valid name
     *
     * we could instead just return the inner symbol
     * but this error reporting makes catching bugs easier
     */
    if (expr->tag == ic_expr_type_identifier) {
        /* should never try to do this */
        puts("ic_transform_new_temp: caller passed in expr->tag identifier - doesn't make sense");
        return 0;
    }

    /* transform expr */
    tir_expr = ic_transform_expr(kludge, fdecl, tbody, body, expr);
    if (!tir_expr) {
        puts("ic_transform_new_temp: call to ic_transform_expr failed");
        return 0;
    }

    /* get type */
    type = ic_analyse_infer(kludge, fdecl, body->scope, expr);
    if (!type) {
        puts("ic_transform_new_temp: call to ic_analyse_infer failed");
        return 0;
    }

    /* generate new name after expanding expr
     * this gives a nice ordering to numbers
     *
     * for literals we have a special naming convention of _l<n>
     * for all other temporaries we use _t<n>
     */
    if (expr->tag == ic_expr_type_constant) {
        sym = ic_transform_body_register_literal(tbody);
    } else {
        sym = ic_transform_body_register_temporary(tbody);
    }
    if (!sym) {
        puts("ic_transform_new_temp: call to ic_transform_body_register_temporary failed");
        return 0;
    }

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

    return sym;
}

/* transform an expr to tir_expr
 *
 * returns * on success
 * returns 0 on failure
 */
static struct ic_transform_ir_expr *ic_transform_expr(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *tbody, struct ic_body *body, struct ic_expr *expr) {
    struct ic_transform_ir_expr *texpr = 0;
    struct ic_expr_func_call *fcall = 0;

    /* left_* and right_* are used for recursive field access */
    struct ic_symbol *left_sym = 0;
    struct ic_symbol *right_sym = 0;
    struct ic_decl_type *left_tdecl = 0;

    switch (expr->tag) {
        case ic_expr_type_identifier:
            texpr = ic_transform_ir_expr_new(ic_transform_ir_expr_type_var);
            if (!expr) {
                puts("ic_transform_expr: call to ic_transform_ir_expr_new failed");
                return 0;
            }

            texpr->u.var = ic_transform_ir_expr_var_new();
            if (!texpr->u.literal) {
                puts("ic_transform_expr: call to ic_transform_ir_expr_var_new failed");
                return 0;
            }

            texpr->u.var->sym = &(expr->u.id.identifier);
            return texpr;
            break;

        case ic_expr_type_constant:
            texpr = ic_transform_ir_expr_new(ic_transform_ir_expr_type_literal);
            if (!expr) {
                puts("ic_transform_expr: call to ic_transform_ir_expr_new failed");
                return 0;
            }

            texpr->u.literal = ic_transform_ir_expr_literal_new();
            if (!texpr->u.literal) {
                puts("ic_transform_expr: call to ic_transform_ir_expr_literal_new failed");
                return 0;
            }

            texpr->u.literal->literal = &(expr->u.cons);
            return texpr;
            break;

        case ic_expr_type_func_call:
            fcall = &(expr->u.fcall);
            goto TRANSFORM_EXPR_FCALL;
            break;

        case ic_expr_type_operator:
            if (!expr->u.op.fcall) {
                puts("ic_transform_expr: operator didn't have fcall set");
                return 0;
            }
            fcall = expr->u.op.fcall;
            goto TRANSFORM_EXPR_FCALL;
            break;

        case ic_expr_type_field_access:
            /* left can either be an id or a compound expression */
            if (expr->u.faccess.left->tag == ic_expr_type_identifier) {
                left_sym = &(expr->u.faccess.left->u.id.identifier);
            } else {
                left_sym = ic_transform_new_temp(kludge, fdecl, tbody, body, expr->u.faccess.left);
                if (!left_sym) {
                    puts("ic_transform_expr: call to ic_transform_new_temp failed");
                    return 0;
                }
            }

            texpr = ic_transform_ir_expr_new(ic_transform_ir_expr_type_faccess);
            if (!texpr) {
                puts("ic_transform_expr: call to ic_transform_ir_expr_new failed");
                return 0;
            }

            texpr->u.faccess = ic_transform_ir_expr_faccess_new();
            if (!texpr->u.faccess) {
                puts("ic_transform_expr: call to ic_transform_ir_expr_faccess_new failed");
                return 0;
            }

            left_tdecl = ic_analyse_infer(kludge, fdecl, body->scope, expr->u.faccess.left);
            if (!left_tdecl) {
                puts("ic_transform_expr: call to ic_analyse_infer failed for let->init->u.faccess.left");
                return 0;
            }

            /* right must always be an id */
            if (expr->u.faccess.right->tag != ic_expr_type_identifier) {
                puts("ic_transform_stmt_let: faccess right was not id");
                return 0;
            }
            right_sym = &(expr->u.faccess.right->u.id.identifier);

            texpr->u.faccess->left = left_sym;
            texpr->u.faccess->left_type = left_tdecl;
            texpr->u.faccess->right = right_sym;

            return texpr;
            break;

        default:
            puts("ic_transform_expr: unsupported case");
            return 0;
            break;
    }

TRANSFORM_EXPR_FCALL:

    texpr = ic_transform_ir_expr_new(ic_transform_ir_expr_type_fcall);
    if (!texpr) {
        puts("ic_transform_expr: call to ic_transform_ir_expr_new failed");
        return 0;
    }

    texpr->u.fcall = ic_transform_fcall(kludge, fdecl, tbody, body, fcall);
    if (!texpr->u.fcall) {
        puts("ic_transform_expr: call to ic_transform_fcall failed");
        return 0;
    }

    return texpr;
}

/* transform an fcall to tir_expr_fcall
 *
 * returns 1 on success
 * returns 0 on failure
 */
static struct ic_transform_ir_expr_fcall *ic_transform_fcall(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *tbody, struct ic_body *body, struct ic_expr_func_call *fcall) {

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
     * this is owned by the tir_expr_fcall
     */
    struct ic_pvector *new_args = 0;

    /* our eventual output tir_expr_fcall */
    struct ic_transform_ir_expr_fcall *tir_expr_fcall = 0;

    if (!kludge) {
        puts("ic_transform_fcall: kludge was null");
        return 0;
    }

    if (!fdecl) {
        puts("ic_transform_fcall: fdecl was null");
        return 0;
    }

    if (!tbody) {
        puts("ic_transform_fcall: tbody was null");
        return 0;
    }

    if (!body) {
        puts("ic_transform_fcall: body was null");
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
        sym = ic_transform_fcall_arg(kludge, fdecl, tbody, body, arg);
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
    tir_expr_fcall = ic_transform_ir_expr_fcall_new(fcall, new_args);
    if (!tir_expr_fcall) {
        puts("ic_transform_fcall: call to ic_transform_ir_expr_fcall_new failed");
        return 0;
    }

    return tir_expr_fcall;
}

/* transform an fcall argument to an symbol
 * if the arg is already a symbol, just return it
 * otherwise create a new let binding the result of expr
 *
 * returns * on success
 * returns 0 on failure
 */
static struct ic_symbol *ic_transform_fcall_arg(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *tbody, struct ic_body *body, struct ic_expr *arg) {
    /* FIXME ownership */
    struct ic_symbol *sym = 0;

    if (!kludge) {
        puts("ic_transform_fcall_arg: kludge was null");
        return 0;
    }

    if (!fdecl) {
        puts("ic_transform_fcall_arg: fdecl was null");
        return 0;
    }

    if (!tbody) {
        puts("ic_transform_fcall_arg: tbody was null");
        return 0;
    }

    if (!body) {
        puts("ic_transform_fcall_arg: body was null");
        return 0;
    }

    if (!arg) {
        puts("ic_transform_fcall_arg: arg was null");
        return 0;
    }

    switch (arg->tag) {
        case ic_expr_type_func_call:
            sym = ic_transform_new_temp(kludge, fdecl, tbody, body, arg);
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

            /* check sym exists in scope */
            if (!ic_scope_get_from_symbol(body->scope, sym)) {
                puts("ic_transform_fcall_arg: identifier not found in scope (for identifier)");
                printf("identifier not found: '%s'\n", ic_symbol_contents(sym));
                return 0;
            }
            return sym;
            break;

        case ic_expr_type_constant:
            sym = ic_transform_new_temp(kludge, fdecl, tbody, body, arg);
            if (!sym) {
                puts("ic_transform_fcall_arg: call to ic_transform_new_temp (for constant) failed");
                return 0;
            }
            return sym;
            break;

        case ic_expr_type_operator:
            sym = ic_transform_new_temp(kludge, fdecl, tbody, body, arg);
            if (!sym) {
                puts("ic_transform_fcall_arg: call to ic_transform_new_temp (for operator) failed");
                return 0;
            }
            return sym;
            break;

        case ic_expr_type_field_access:
            sym = ic_transform_new_temp(kludge, fdecl, tbody, body, arg);
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
