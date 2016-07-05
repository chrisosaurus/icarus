#include <stdio.h>

#include "../analyse/data/kludge.h"
#include "../data/pvector.h"
#include "../parse/data/body.h"
#include "../parse/data/decl.h"
#include "../parse/data/stmt.h"
#include "data/tbody.h"
#include "data/tcounter.h"
#include "transform.h"

#pragma GCC diagnostic ignored "-Wunused-function"

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
static struct ic_symbol *ic_transform_new_temp(struct ic_transform_body *tbody, struct ic_expr *expr);

/* transform an fcall to tir_expr
 *
 * returns * on success
 * returns 0 on failure
 */
static struct ic_transform_ir_expr * ic_transform_fcall(struct ic_transform_body *tbody, struct ic_expr_func_call *fcall);

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
    struct ic_decl_func *fdecl;

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
    if (!len) {
        puts("ic_transform_body: got len `0` from ic_body_length");
        return 0;
    }

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
    new_tmp = ic_transform_new_temp(tbody, expr);
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
            expr = ic_transform_fcall(tbody, fcall);
            if( ! expr){
                puts("ic_transform_stmt_let: call to ic_transform_ir_fcall failed");
                return 0;
            }
            stmt = ic_transform_ir_stmt_let_expr_new(&(let->identifier), let->inferred_type, expr);
            if( ! stmt){
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

    switch( expr->tag){
      case ic_expr_type_func_call:
        puts("ic_transform_stmt_expr: unimplemented expr->tag");
        return 0;
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
static struct ic_symbol *ic_transform_new_temp(struct ic_transform_body *tbody, struct ic_expr *expr) {

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

    /* FIXME what is expr is already a symbol?
   * just return it!
   */

    puts("ic_transform_new_temp: unimplemented");
    return 0;
}

/* transform an fcall to tir_expr
 *
 * returns 1 on success
 * returns 0 on failure
 */
static struct ic_transform_ir_expr * ic_transform_fcall(struct ic_transform_body *tbody, struct ic_expr_func_call *fcall){
    if (!tbody) {
        puts("ic_transform_fcall: tbody was null");
        return 0;
    }

    if (!fcall) {
        puts("ic_transform_fcall: fcall was null");
        return 0;
    }

    puts("ic_transform_fcall: unimplemented");
    return 0;
}

