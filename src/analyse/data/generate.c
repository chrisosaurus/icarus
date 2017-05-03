#include <stdio.h>
#include <stdlib.h>

#include "generate.h"

/* allocate and initalise a new ic_generate
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_generate *ic_generate_new(enum ic_generate_tag tag, struct ic_decl_func *fdecl, struct ic_decl_type *tdecl) {
    struct ic_generate *gen = 0;

    if (!fdecl) {
        puts("ic_generate_new: fdecl was null");
        return 0;
    }

    if (!tdecl) {
        puts("ic_generate_new: tdecl was null");
        return 0;
    }

    gen = calloc(1, sizeof(struct ic_generate));
    if (!gen) {
        puts("ic_generate_new: call to calloc failed");
        return 0;
    }

    if (!ic_generate_init(gen, tag, fdecl, tdecl)) {
        puts("ic_generate_new: call to ic_generate_init failed");
        return 0;
    }

    return gen;
}

/* initialise an existing ic_generate
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_generate_init(struct ic_generate *gen, enum ic_generate_tag tag, struct ic_decl_func *fdecl, struct ic_decl_type *tdecl) {
    if (!gen) {
        puts("ic_generate_init: gen was null");
        return 0;
    }

    if (!fdecl) {
        puts("ic_generate_init: fdecl was null");
        return 0;
    }

    if (!tdecl) {
        puts("ic_generate_init: tdecl was null");
        return 0;
    }

    gen->tag = tag;
    gen->fdecl = fdecl;
    gen->tdecl = tdecl;

    gen->u.union_field = 0;

    return 1;
}

/* destroy an ic_generate
 *
 * will only free gen is `free_gen` is truthy
 *
 * will free fdecl if set
 * will not free other fields (as they are not owned)
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_generate_destroy(struct ic_generate *gen, unsigned int free_gen) {
    if (!gen) {
        puts("ic_generate_destroy: gen was null");
        return 0;
    }

    /* free fdecl as created just for generation */
    if (!ic_decl_func_destroy(gen->fdecl, 1)) {
        puts("ic_generate_destroy: call to ic_decl_func_destroy failed");
        return 0;
    }

    gen->fdecl = 0;

    if (free_gen) {
        free(gen);
    }

    return 1;
}

/* get fdecl field
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_decl_func *ic_generate_get_fdecl(struct ic_generate *gen) {
    if (!gen) {
        puts("ic_generate_get_fdecl: gen was null");
        return 0;
    }

    return gen->fdecl;
}

/* get tdecl field
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_decl_type *ic_generate_get_tdecl(struct ic_generate *gen) {
    if (!gen) {
        puts("ic_generate_get_tdecl: gen was null");
        return 0;
    }

    return gen->tdecl;
}

/* get union_field
 *
 * only legal if tag is cons_union
 *
 * returns * on success
 * returns 0 on failure
 */

struct ic_field *ic_generate_get_union_field(struct ic_generate *gen) {
    if (!gen) {
        puts("ic_generate_get_union_field: gen was null");
        return 0;
    }

    if (gen->tag != ic_generate_tag_cons_union) {
        puts("ic_generate_get_union_field: can only be called on cons_union");
        return 0;
    }

    if (!gen->u.union_field) {
        puts("ic_generate_get_union_field: union_field was null");
        return 0;
    }

    return gen->u.union_field;
}

/* set union_field
 *
 * only legal if tag is cons_union
 * only legal if union_field not already set
 *
 * returns 1 on success
 * returns 0 on failure
 */

unsigned int ic_generate_set_union_field(struct ic_generate *gen, struct ic_field *union_field) {
    if (!gen) {
        puts("ic_generate_set_union_field: gen was null");
        return 0;
    }

    if (gen->tag != ic_generate_tag_cons_union) {
        puts("ic_generate_set_union_field: can only be called on cons_union");
        return 0;
    }

    if (gen->u.union_field) {
        puts("ic_generate_set_union_field: union_field was already set");
        return 0;
    }

    gen->u.union_field = union_field;

    return 1;
}
