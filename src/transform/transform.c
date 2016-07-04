#include <stdio.h>

#include "../analyse/data/kludge.h"
#include "../data/pvector.h"
#include "../parse/data/body.h"
#include "../parse/data/decl.h"
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

    /* FIXME TODO */
    /* step through body stmts */
    /* for each stmt */
    /* transform - dispatch to function */

    /* FIXME TODO */
    puts("ic_transform_body: transform implementation pending");
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

    /* FIXME TODO */
    /* dispatch to functions for each stmt type */

    /* FIXME TODO */
    puts("ic_transform_stmt: transform implementation pending");
    return 1;
}
