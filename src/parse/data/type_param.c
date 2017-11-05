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

/* perform a deep copy of a type_param
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_type_param *ic_type_param_deep_copy(struct ic_type_param *tparam) {
    struct ic_type_param *new_tparam = 0;

    if (!tparam) {
        puts("ic_type_param_deep_copy: tparam was null");
        return 0;
    }

    new_tparam = calloc(1, sizeof(struct ic_type_param));
    if (!new_tparam) {
        puts("ic_type_param_deep_copy: call to calloc failed");
        return 0;
    }

    if (!ic_type_param_deep_copy_embedded(tparam, new_tparam)) {
        puts("ic_type_param_deep_copy: call to ic_type_param_deep_copy_embedded failed");
        return 0;
    }

    return new_tparam;
}

/* perform a deep copy of a type_param embedded within an object
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_param_deep_copy_embedded(struct ic_type_param *from, struct ic_type_param *to) {
    if (!from) {
        puts("ic_type_param_deep_copy_embedded: from was null");
        return 0;
    }

    if (!to) {
        puts("ic_type_param_deep_copy_embedded: to was null");
        return 0;
    }

    if (!ic_symbol_deep_copy_embedded(&(from->name), &(to->name))) {
        puts("ic_type_param_deep_copy_embedded: call to ic_symbol_deep_copy_embedded failed");
        return 0;
    }

    to->tdecl = from->tdecl;

    return 1;
}

/* perform a deep copy of a pvector of type_param(s)
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_pvector *ic_type_param_pvector_deep_copy(struct ic_pvector *tparams) {
    struct ic_pvector *new = 0;

    if (!tparams) {
        puts("ic_type_param_pvector_deep_copy: tparams was null");
        return 0;
    }

    new = calloc(1, sizeof(struct ic_pvector));
    if (!new) {
        puts("ic_type_param_pvector_deep_copy: call to calloc failed");
        return 0;
    }

    if (!ic_type_param_pvector_deep_copy_embedded(tparams, new)) {
        puts("ic_type_param_pvector_deep_copy: call to ic_type_param_pvector_deep_copy_embedded failed");
        return 0;
    }

    return new;
}

/* perform a deep copy of a pvector of type_param(s) embedded within an object
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_param_pvector_deep_copy_embedded(struct ic_pvector *from, struct ic_pvector *to) {
    unsigned int i = 0;
    unsigned int len = 0;
    struct ic_type_param *from_tparam = 0;
    struct ic_type_param *to_tparam = 0;

    if (!from) {
        puts("ic_type_param_pvector_deep_copy_embedded: from was null");
        return 0;
    }

    if (!to) {
        puts("ic_type_param_pvector_deep_copy_embedded: to was null");
        return 0;
    }

    len = ic_pvector_length(from);

    if (!ic_pvector_init(to, len)) {
        puts("ic_type_param_pvector_deep_copy_embedded: call to ic_pvector_init failed");
        return 0;
    }

    for (i=0; i<len; ++i ){
        from_tparam = ic_pvector_get(from, i);
        if (!from_tparam) {
            puts("ic_type_param_pvector_deep_copy_embedded: call to ic_pvector_get failed");
            return 0;
        }

        to_tparam = ic_type_param_deep_copy(from_tparam);
        if (!to_tparam) {
            puts("ic_type_param_pvector_deep_copy_embedded: call to ic_type_param_deep_copy failed");
            return 0;
        }

        if ((int)i != ic_pvector_append(to, to_tparam)) {
            puts("ic_type_param_pvector_deep_copy_embedded: call to ic_pvector_append failed");
            return 0;
        }
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

/* find a type_param by string
 *
 * this could probably go somewhere else better.
 *
 * returns * on successful find
 * returns 0 on failure to find
 * returns 0 an failure
 */
struct ic_type_param *ic_type_param_search(struct ic_pvector *type_params, char *tname) {
    unsigned int i = 0;
    unsigned int len = 0;
    struct ic_type_param *type_param = 0;

    if (!type_params) {
        puts("ic_type_param_search: type_params was null");
        return 0;
    }

    if (!tname) {
        puts("ic_type_param_search: tname was null");
        return 0;
    }

    len = ic_pvector_length(type_params);
    for (i = 0; i < len; ++i) {
        type_param = ic_pvector_get(type_params, i);
        if (!type_param) {
            puts("ic_type_param_search: call to ic_pvector_get failed");
            return 0;
        }

        if (ic_symbol_equal_char(&(type_param->name), tname)) {
            /* found */
            return type_param;
        }
    }

    /* could not find */
    return 0;
}

/* get mangled_name
 * this is owned by the underlying decl_type
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_symbol *ic_type_param_mangled_name(struct ic_type_param *tparam) {
    struct ic_symbol *sym = 0;

    if (!tparam) {
        puts("ic_type_param_mangled_name: tparam was null");
        return 0;
    }

    if (tparam->tdecl) {
        sym = ic_decl_type_mangled_name(tparam->tdecl);
        if (!sym) {
            puts("ic_type_param_mangled_name: call to ic_decl_type_mangled_name failed");
            return 0;
        }

    } else {
        sym = &(tparam->name);
    }

    return sym;
}

/* get full_name
 * this is owned by the underlying decl_type
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_symbol *ic_type_param_full_name(struct ic_type_param *tparam) {
    struct ic_symbol *sym = 0;

    if (!tparam) {
        puts("ic_type_param_full_name: tparam was null");
        return 0;
    }

    if (tparam->tdecl) {
        sym = ic_decl_type_full_name(tparam->tdecl);
        if (!sym) {
            puts("ic_type_param_full_name: call to ic_decl_type_full_name failed");
            return 0;
        }
    } else {
        sym = &(tparam->name);
    }

    return sym;
}

