#include <stdio.h>

#include "../analyse/data/kludge.h"
#include "../data/pvector.h"
#include "../parse/data/decl.h"
#include "transform.h"

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
    if (!kludge) {
        puts("ic_transform_fdecl: kludge was null");
        return 0;
    }

    if (!func) {
        puts("ic_transform_fdecl: func was null");
        return 0;
    }

    puts("ic_transform_fecl: transform implementation pending");
    return 1;
}
