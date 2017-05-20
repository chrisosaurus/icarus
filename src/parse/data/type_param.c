#include <stdio.h>
#include <stdlib.h>

#include "type_param.h"

/* allocate and initialise a new type_param
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_type_param *ic_type_param_new(char *name_src, unsigned int name_len) {
    struct ic_type_param *tparam = 0;

    if (!name_src) {
        puts("ic_type_param_new: name_src was null");
        return 0;
    }

    tparam = calloc(1, sizeof(struct ic_type_param));
    if (!tparam) {
        puts("ic_type_param_new: call to calloc failed");
        return 0;
    }

    if (!ic_type_param_init(tparam, name_src, name_len)) {
        puts("ic_type_param_new: call to ic_type_param_init failed");
        return 0;
    }

    return tparam;
}

/* initialise an existing type_param
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_param_init(struct ic_type_param *tparam, char *name_src, unsigned int name_len) {
    if (!tparam) {
        puts("ic_type_param_init: tparam was null");
        return 0;
    }

    if (!name_src) {
        puts("ic_type_param_init: name_src was null");
        return 0;
    }

    if (!ic_symbol_init(&(tparam->name), name_src, name_len)) {
        puts("ic_type_param_init: call to ic_symbol_init failed");
        return 0;
    }

    tparam->tdecl = 0;

    return 1;
}

/* destroy a type_param
 *
 * will only free tparam is `free_tparam` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_param_destroy(struct ic_type_param *tparam, unsigned int free_tparam) {
    if (!tparam) {
        puts("ic_type_param_destroy: tparam was null");
        return 0;
    }

    if (!ic_symbol_destroy(&(tparam->name), 0)) {
        puts("ic_type_param_destroy: call to ic_symbol_destry failed");
        return 0;
    }

    if (free_tparam) {
        free(tparam);
    }

    return 1;
}

/* get pointer to internal name
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_symbol *ic_type_param_get_name(struct ic_type_param *tparam) {
    if (!tparam) {
        puts("ic_type_param_get_name: tparam was null");
        return 0;
    }

    return &(tparam->name);
}

/* set the tdecl on this type_param
 *
 * error if already set
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_param_set(struct ic_type_param *tparam, struct ic_decl_type *tdecl) {
    if (!tparam) {
        puts("ic_type_param_set: tparam was null");
        return 0;
    }

    if (!tdecl) {
        puts("ic_type_param_set: tdecl was null");
        return 0;
    }

    if (tparam->tdecl) {
        puts("ic_type_param_set: tparam->tdecl was already set");
        return 0;
    }

    tparam->tdecl = tdecl;
    return 1;
}

/* get the tdecl on this type param
 *
 * error if not set
 *
 * returns 1 on success
 * returns 0 on failure
 */
struct ic_decl_type *ic_type_param_get(struct ic_type_param *tparam) {
    if (!tparam) {
        puts("ic_type_param_get: tparam was null");
        return 0;
    }

    if (!tparam->tdecl) {
        puts("ic_type_param_get: tparam->tdecl was not set");
        return 0;
    }

    return tparam->tdecl;
}

/* check if this type param is already resolved to a type
 * (if tdecl is set)
 *
 * returns 1 if set
 * returns 0 if not set
 */
unsigned int ic_type_param_check_set(struct ic_type_param *tparam) {
    if (!tparam) {
        puts("ic_type_param_check_set: tparam was null");
        return 0;
    }

    if (tparam->tdecl) {
        return 1;
    }

    return 0;
}

/* print this type_param */
void ic_type_param_print(FILE *fd, struct ic_type_param *tparam) {
    struct ic_symbol *tdecl_name = 0;

    if (!tparam) {
        puts("ic_type_param_print: tparam was null");
        return;
    }

    /* if no tdecl just print param name `T`
     * if tdecl is bound, then print type param name :: type name `T::Type`
     */

    ic_symbol_print(fd, &(tparam->name));

    if (tparam->tdecl) {
        tdecl_name = ic_decl_type_get_name(tparam->tdecl);
        if (!tdecl_name) {
            puts("ic_type_param_print: call to ic_decl_type_get_name failed");
            return;
        }

        fprintf(fd, "::%s", ic_symbol_contents(tdecl_name));
    }
}
