#include <stdio.h>

#include "../analyse/data/kludge.h"
#include "../analyse/helpers.h"
#include "../data/pvector.h"
#include "../parse/data/body.h"
#include "../parse/data/decl.h"
#include "../parse/data/stmt.h"
#include "data/tbody.h"
#include "data/tcounter.h"
#include "transform.h"

#pragma GCC diagnostic ignored "-Wunused-function"

#define TCOUNT_MAX_SIZE 10

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
static unsigned int ic_transform_body(struct ic_kludge *kludge, struct ic_transform_body *tbody, struct ic_body *body);

/* perform translation of a single stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt(struct ic_kludge *kludge, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt *stmt);

/* perform translation of a single `ret` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_ret(struct ic_kludge *kludge, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_ret *ret);

/* perform translation of a single `let` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_let(struct ic_kludge *kludge, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_let *let);

/* perform translation of a single `assign` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_assign(struct ic_kludge *kludge, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_assign *assign);

/* perform translation of a single `if` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_if(struct ic_kludge *kludge, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_if *sif);

/* perform translation of a single `for` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_for(struct ic_kludge *kludge, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_for *sfor);

/* perform translation of a single `while` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_while(struct ic_kludge *kludge, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_while *swhile);

/* perform translation of a single `expr` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_expr(struct ic_kludge *kludge, struct ic_transform_body *tbody, struct ic_body *body, struct ic_expr *expr);

/* register and append a new temporary expr as a let
 * generates a new symbol name and returns it
 *
 * returns * on success
 * returns 0 on failure
 */
static struct ic_symbol *ic_transform_new_temp(struct ic_kludge *kludge, struct ic_transform_body *tbody, struct ic_expr *expr);

/* transform an fcall to tir_fcall
 *
 * returns * on success
 * returns 0 on failure
 */
static struct ic_transform_ir_fcall *ic_transform_fcall(struct ic_kludge *kludge, struct ic_transform_body *tbody, struct ic_expr_func_call *fcall);

/* transform an fcall argument to an symbol
 * if the arg is already a symbol, just return it
 * otherwise create a new let binding the result of expr
 *
 * returns * on success
 * returns 0 on failure
 */
static struct ic_symbol *ic_transform_fcall_arg(struct ic_kludge *kludge, struct ic_transform_body *tbody, struct ic_expr *arg);

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

    puts("ic_transform: transform implementation pending");
    return 1;
}

/* print out all transformed items within kludge
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_print(struct ic_kludge *kludge) {
    /* i for kludge fdecls */
    unsigned int i = 0;
    /* len for kludge fdecls */
    unsigned int len = 0;
    /* current tdecl we are considering */
    struct ic_decl_func *fdecl = 0;
    /* pointer to tbody within */
    struct ic_transform_body *tbody = 0;
    /* indent level */
    unsigned int indent = 1;

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

        tbody = fdecl->tbody;
        if (!tbody) {
            puts("ic_transform_print: fdecl lacked tbody");
            return 0;
        }

        if (!ic_transform_body_print(tbody, &indent)) {
            puts("ic_transform_print: call to ic_transform_body_print failed");
            return 0;
        }
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
    struct ic_transform_counter *tcounter = 0;

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

    /* create tcounter */
    tcounter = ic_transform_counter_new();
    if (!tcounter) {
        puts("ic_transform_fdecl: call to ic_transform_counter_new failed");
        return 0;
    }

    /* populate tbody */
    func->tbody = ic_transform_body_new(tcounter);
    if (!func->tbody) {
        puts("ic_transform_fdecl: call to ic_transform_body_new failed");
        return 0;
    }

    /* dispatch to transform_body for work */
    if (!ic_transform_body(kludge, func->tbody, &(func->body))) {
        puts("ic_transform_fdecl: call to ic_transform_body failed");
        return 0;
    }

    return 1;
}

static unsigned int ic_transform_body(struct ic_kludge *kludge, struct ic_transform_body *tbody, struct ic_body *body) {
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
        if (!ic_transform_stmt(kludge, tbody, body, stmt)) {
            puts("ic_transform_body: call to ic_transform_stmt failed");
            return 0;
        }
    }

    return 1;
}

static unsigned int ic_transform_stmt(struct ic_kludge *kludge, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt *stmt) {
    if (!kludge) {
        puts("ic_transform_stmt: kludge was null");
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
            if (!ic_transform_stmt_ret(kludge, tbody, body, &(stmt->u.ret))) {
                puts("ic_transform_stmt: call to ic_transform_stmt_ret failed");
                return 0;
            }
            break;

        case ic_stmt_type_let:
            if (!ic_transform_stmt_let(kludge, tbody, body, &(stmt->u.let))) {
                puts("ic_transform_stmt: call to ic_transform_stmt_let failed");
                return 0;
            }
            break;

        case ic_stmt_type_assign:
            if (!ic_transform_stmt_assign(kludge, tbody, body, &(stmt->u.assign))) {
                puts("ic_transform_stmt: call to ic_transform_stmt_assign failed");
                return 0;
            }
            break;

        case ic_stmt_type_if:
            if (!ic_transform_stmt_if(kludge, tbody, body, &(stmt->u.sif))) {
                puts("ic_transform_stmt: call to ic_transform_stmt_if failed");
                return 0;
            }
            break;

        case ic_stmt_type_for:
            if (!ic_transform_stmt_for(kludge, tbody, body, &(stmt->u.sfor))) {
                puts("ic_transform_stmt: call to ic_transform_stmt_for failed");
                return 0;
            }
            break;

        case ic_stmt_type_while:
            if (!ic_transform_stmt_while(kludge, tbody, body, &(stmt->u.swhile))) {
                puts("ic_transform_stmt: call to ic_transform_stmt_while failed");
                return 0;
            }
            break;

        case ic_stmt_type_expr:
            if (!ic_transform_stmt_expr(kludge, tbody, body, stmt->u.expr)) {
                puts("ic_transform_stmt: call to ic_transform_stmt_expr failed");
                return 0;
            }
            break;

        default:
            puts("ic_transform_stmt: impossible stmt->tag");
            return 0;
            break;
    }

    puts("ic_transform_stmt: transform implementation pending");
    return 1;
}

/* perform translation of a single `ret` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_ret(struct ic_kludge *kludge, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_ret *ret) {
    struct ic_expr *expr = 0;
    struct ic_transform_ir_stmt *stmt = 0;
    struct ic_symbol *new_tmp = 0;

    if (!kludge) {
        puts("ic_transform_stmt_ret: kludge was null");
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
    new_tmp = ic_transform_new_temp(kludge, tbody, expr);
    if (!new_tmp) {
        puts("ic_transform_stmt_ret: call to ic_transform_new_temp failed");
        return 0;
    }

    stmt = ic_transform_ir_stmt_ret_new(new_tmp);
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
static unsigned int ic_transform_stmt_let(struct ic_kludge *kludge, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_let *let) {
    struct ic_expr_constant *cons = 0;
    struct ic_expr_func_call *fcall = 0;
    struct ic_transform_ir_expr *expr = 0;
    struct ic_transform_ir_stmt *stmt = 0;
    struct ic_transform_ir_fcall *tir_fcall = 0;

    if (!kludge) {
        puts("ic_transform_stmt_let: kludge was null");
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

    switch (let->init->tag) {
        case ic_expr_type_func_call:
            fcall = &(let->init->u.fcall);
            tir_fcall = ic_transform_fcall(kludge, tbody, fcall);
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

            /* success */
            return 1;
            break;

        case ic_expr_type_identifier:
            puts("ic_transform_stmt_let: unsupported let->init->tag: identifier");
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

            /* success */
            return 1;
            break;

        default:
            puts("ic_transform_stmt_let: unsupported let->init->tag");
            return 0;
            break;
    }

    puts("ic_transform_stmt_let: implementation pending");
    return 1;
}

/* perform translation of a single `assign` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_assign(struct ic_kludge *kludge, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_assign *assign) {
    if (!kludge) {
        puts("ic_transform_stmt_assign: kludge was null");
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

    puts("ic_transform_stmt_assign: implementation pending");
    return 1;
}

/* perform translation of a single `if` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_if(struct ic_kludge *kludge, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_if *sif) {
    if (!kludge) {
        puts("ic_transform_stmt_if: kludge was null");
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

    puts("ic_transform_stmt_if: implementation pending");
    return 1;
}

/* perform translation of a single `for` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_for(struct ic_kludge *kludge, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_for *sfor) {
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

    puts("ic_transform_stmt_for: implementation pending");
    return 1;
}

/* perform translation of a single `while` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_while(struct ic_kludge *kludge, struct ic_transform_body *tbody, struct ic_body *body, struct ic_stmt_while *swhile) {
    if (!kludge) {
        puts("ic_transform_stmt_while: kludge was null");
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

    puts("ic_transform_stmt_while: implementation pending");
    return 1;
}

/* perform translation of a single `expr` stmt within a body
 *
 * appends tir stmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_transform_stmt_expr(struct ic_kludge *kludge, struct ic_transform_body *tbody, struct ic_body *body, struct ic_expr *expr) {
    struct ic_transform_ir_stmt *tir_stmt = 0;
    struct ic_transform_ir_expr *tir_expr = 0;
    struct ic_expr_func_call *fcall = 0;

    if (!kludge) {
        puts("ic_transform_stmt_expr: kludge was null");
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
            tir_expr->fcall = ic_transform_fcall(kludge, tbody, fcall);
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

    puts("ic_transform_stmt_expr: implementation pending");
    return 1;
}

/* register and append a new temporary expr as a let
 * generates a new symbol name and returns it
 *
 * returns * on success
 * returns 0 on failure
 */
static struct ic_symbol *ic_transform_new_temp(struct ic_kludge *kludge, struct ic_transform_body *tbody, struct ic_expr *expr) {
    /* the tir stmt we generate */
    struct ic_transform_ir_stmt *tir_stmt = 0;
    /* string used for generating symbol */
    struct ic_string *str = 0;
    char *str_ch = 0;
    int str_len = 0;
    /* our temporary count */
    unsigned int tcount = 0;
    /* maximum of TCOUNT_MAX_SIZE digits */
    char tcount_ch[TCOUNT_MAX_SIZE];
    int tcount_ch_printed = 0;

    /* symbol */
    struct ic_symbol *sym = 0;
    /* type */
    struct ic_type *type = 0;

    /* constant used in literal case */
    struct ic_expr_constant *constant = 0;

    if (!kludge) {
        puts("ic_transform_new_temp: kludge was null");
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
    /* register on tbody->tcounter */
    /* generate new unique temporary symbol */
    /* create a new let for this expr */
    /* append new let to tbody */
    /* return sym name*/

    switch (expr->tag) {
        case ic_expr_type_func_call:
            puts("ic_transform_new_temp: expr->tag func_call not yet supported");
            return 0;
            break;

        case ic_expr_type_identifier:
            /* should never try to do this */
            puts("ic_transform_new_temp: caller passed in expr->tag identifier - doesn't make sense");
            return 0;
            break;

        case ic_expr_type_constant:
            /* generate name */
            /* base of _t for temp */
            str = ic_string_new("_t", 2);
            if (!str) {
                puts("ic_transform_new_temp: call to ic_string_new failed");
                return 0;
            }

            /* register on tcounter to get our number */
            tcount = ic_transform_counter_register_temporary(tbody->tcounter);
            if (!tcount) {
                puts("ic_transform_new_temp: call to ic_transform_counter_registery_temporary failed");
                return 0;
            }

            /* convert tcount to string */
            tcount_ch_printed = snprintf(tcount_ch, TCOUNT_MAX_SIZE, "%d", tcount);
            if (tcount_ch_printed == TCOUNT_MAX_SIZE) {
                puts("WARNING: ic_transform_new_temp: snprintf printed same TCOUNT_MAX_SIZE chars");
            }

            /* concat */
            if (!ic_string_append_char(str, tcount_ch, tcount_ch_printed)) {
                puts("ic_transform_new_temp: call to ic_string_append_char failed");
                return 0;
            }

            /* convert to symbol */
            str_ch = ic_string_contents(str);
            if (!str_ch) {
                puts("ic_transform_new_temp: call to ic_string_contents failed");
                return 0;
            }

            str_len = ic_string_length(str);
            if (-1 == str_len) {
                puts("ic_transform_new_temp: call to ic_string_length failed");
                return 0;
            }

            sym = ic_symbol_new(str_ch, str_len);
            if (!sym) {
                puts("ic_transform_new_temp: call to ic_symbol_new failed");
                return 0;
            }

            /* destroy string
             * safe as symbol_new performs a strncpy
             */
            str_ch = 0;
            if (!ic_string_destroy(str, 1)) {
                puts("ic_transform_new_temp: call to ic_string_destroy failed");
                return 0;
            }

            /* unpack literal */
            constant = ic_expr_get_constant(expr);
            if (!constant) {
                puts("ic_transform_new_temp: call to ic_expr_get_constant failed");
                return 0;
            }

            /* FIXME TODO infer type */
            type = 0; /* FIXME TODO */
            type = ic_analyse_infer_constant(kludge, constant);

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
            /* all operators should now be functions ??? */
            puts("ic_transform_new_temp: caller passed in expr->tag operator - should already be transformed at this point");
            return 0;
            break;

        case ic_expr_type_field_access:
            puts("ic_transform_new_temp: expr->tag field_access not yet supported");
            return 0;
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
static struct ic_transform_ir_fcall *ic_transform_fcall(struct ic_kludge *kludge, struct ic_transform_body *tbody, struct ic_expr_func_call *fcall) {

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
        sym = ic_transform_fcall_arg(kludge, tbody, arg);
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
static struct ic_symbol *ic_transform_fcall_arg(struct ic_kludge *kludge, struct ic_transform_body *tbody, struct ic_expr *arg) {
    /* FIXME ownership */
    struct ic_symbol *sym = 0;

    if (!tbody) {
        puts("ic_transform_fcall_arg: tbody was null");
        return 0;
    }

    if (!arg) {
        puts("ic_transform_fcall_arg: arg was null");
        return 0;
    }

    switch (arg->tag) {
        case ic_expr_type_identifier:
            /* FIXME ownership */
            sym = &(arg->u.id.identifier);
            /* FIXME ownership */
            return sym;
            break;

        case ic_expr_type_constant:
            sym = ic_transform_new_temp(kludge, tbody, arg);
            if (!sym) {
                puts("ic_transform_fcall_arg: call to ic_transform_new_temp (for constant) failed");
                return 0;
            }
            return sym;
            break;

        case ic_expr_type_func_call:
            sym = ic_transform_new_temp(kludge, tbody, arg);
            if (!sym) {
                puts("ic_transform_fcall_arg: call to ic_transform_new_temp (for func_call) failed");
                return 0;
            }
            return sym;
            break;

        default:
            puts("ic_transform_fcall_arg: unsupported/unimplemented arg->tag");
            return 0;
            break;
    }

    puts("ic_transform_fcall_arg: implementation pending");
    return 0;
}
