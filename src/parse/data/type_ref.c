#include <stdio.h>
#include <stdlib.h>

#include "decl.h"
#include "type_param.h"
#include "type_ref.h"

/* allocate and intialise a new type
 * this will set type.type to unknown
 *
 * returns new type on success
 * returns 0 on failure
 */
struct ic_type_ref *ic_type_ref_new(void) {
    struct ic_type_ref *type = 0;

    type = calloc(1, sizeof(struct ic_type_ref));
    if (!type) {
        puts("ic_type_ref_new: call to calloc failed");
        return 0;
    }

    if (!ic_type_ref_init(type)) {
        puts("ic_type_ref_new: call to ic_type_init failed");
        return 0;
    }

    return type;
}

/* intialise a type
 * this will set type.type to unknown
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_ref_init(struct ic_type_ref *type) {
    if (!type) {
        puts("ic_type_ref_init: type was null");
        return 0;
    }

    /* default to unknown types
     * other type_tag(s) are set via methods
     */
    type->tag = ic_type_ref_unknown;

    if (!ic_pvector_init(&(type->type_args), 0)) {
        puts("ic_type_ref_init: call to ic_pvector_init failed");
        return 0;
    }

    return 1;
}

/* allocate and intialise a new type
 * as a symbol
 *
 * returns new type on success
 * returns 0 on failure
 */
struct ic_type_ref *ic_type_ref_symbol_new(char *type_str, unsigned int type_len) {
    struct ic_type_ref *type = 0;

    if (!type_str) {
        puts("ic_type_ref_symbol_new: type_str was null");
        return 0;
    }

    /* construct base type */
    type = ic_type_ref_new();
    if (!type) {
        puts("ic_type_ref_symbol_new: call to ic_type_new failed");
        return 0;
    }

    /* set to symbol */
    if (!ic_type_ref_set_symbol(type, type_str, type_len)) {
        puts("ic_type_ref_symbol_new: call to ic_type_set_symbol failed");
        /* destroy type
         * free type as allocated with new
         */
        ic_type_ref_destroy(type, 1);
        return 0;
    }

    return type;
}

/* intialise a type as a  symbol
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_ref_symbol_init(struct ic_type_ref *type, char *type_str, unsigned int type_len) {
    if (!type) {
        puts("ic_type_ref_symbol_init: type_str was null");
        return 0;
    }
    if (!type_str) {
        puts("ic_type_ref_symbol_init: type_str was null");
        return 0;
    }

    /* init base type */
    if (!ic_type_ref_init(type)) {
        puts("ic_type_ref_symbol_init: call to ic_type_init failed");
        return 0;
    }

    /* set to symbol */
    if (!ic_type_ref_set_symbol(type, type_str, type_len)) {
        puts("ic_type_ref_symbol_init: call to ic_type_set_symbol failed");
        return 0;
    }

    return 1;
}

/* destroy type
 *
 * this will only free type if `free_type` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_ref_destroy(struct ic_type_ref *type, unsigned int free_type) {
    unsigned int len = 0;
    unsigned int i = 0;
    struct ic_type_ref *type_param = 0;

    if (!type) {
        puts("ic_type_ref_destroy: type was null");
        return 0;
    }

    len = ic_type_ref_type_args_length(type);
    for (i = 0; i < len; ++i) {
        type_param = ic_type_ref_type_args_get(type, i);
        if (!type_param) {
            puts("ic_type_ref_destroy: call to ic_type_ref_type_args_get failed");
            return 0;
        }

        if (!ic_type_ref_destroy(type_param, 1)) {
            puts("ic_type_ref_destroy: call to ic_type_ref_destroy failed");
            return 0;
        }
    }

    /* cleanup depends on type_tag */
    switch (type->tag) {
        case ic_type_ref_unknown:
            /* nothing to do */
            break;

        case ic_type_ref_symbol:
            /* clean up symbol, do not free as member */
            if (!ic_symbol_destroy(&(type->u.sym), 0)) {
                puts("ic_type_ref_destroy: call to ic_symbol_destroy failed");
                return 0;
            }
            break;

        case ic_type_ref_param:
            /* type param is owned by enclosing function/type */
            break;

        case ic_type_ref_resolved:
            /* type decl is owned by enclosing function/type */
            /* decl_type is freed elsewhere */
            break;

        default:
            puts("ic_type_ref_destroy: type->type was impossible type_tag");
            return 0;
            break;
    }

    /* if asked nicely */
    if (free_type) {
        free(type);
    }

    return 1;
}

/* perform a deep copy of a type_ref
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_type_ref *ic_type_ref_deep_copy(struct ic_type_ref *type) {
    struct ic_type_ref *new_type = 0;

    if (!type) {
        puts("ic_type_ref_deep_copy: type was null");
        return 0;
    }

    new_type = calloc(1, sizeof(struct ic_type_ref));
    if (!new_type) {
        puts("ic_type_ref_deep_copy: call to calloc");
        return 0;
    }

    if (!ic_type_ref_deep_copy_embedded(type, new_type)) {
        puts("ic_type_ref_deep_copy: call to ic_type_ref_deep_copy_embedded failed");
        return 0;
    }

    return new_type;
}

/* perform a deep copy of a type_ref embedded within an object
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_ref_deep_copy_embedded(struct ic_type_ref *from, struct ic_type_ref *to) {
    unsigned int i = 0;
    unsigned int len = 0;
    struct ic_type_ref *type_arg = 0;
    struct ic_type_ref *new_type_arg = 0;

    if (!from) {
        puts("ic_type_ref_deep_copy_embedded: from was null");
        return 0;
    }

    if (!to) {
        puts("ic_type_ref_deep_copy_embedded: to was null");
        return 0;
    }

    to->tag = from->tag;

    switch (to->tag) {
        case ic_type_ref_unknown:
            /* nothing to do */
            break;

        case ic_type_ref_symbol:
            if (!ic_symbol_deep_copy_embedded(&(from->u.sym), &(to->u.sym))) {
                puts("ic_type_ref_deep_copy_embedded: call to ic_symbol_deep_copy_embedded failed");
                return 0;
            }
            break;

        case ic_type_ref_param:
            to->u.tparam = from->u.tparam;
            break;

        case ic_type_ref_resolved:
            to->u.tdecl = from->u.tdecl;
            break;

        default:
            /* oops */
            puts("ic_type_ref_deep_copy_embedded: impossible tag");
            return 0;
            break;
    }

    if (!ic_pvector_init(&(to->type_args), 0)) {
        puts("ic_type_ref_deep_copy_embedded: call to ic_pvector_init failed");
        return 0;
    }

    /* copy over type args */
    len = ic_type_ref_type_args_length(from);
    for (i=0; i<len; ++i) {
        type_arg = ic_type_ref_type_args_get(from, i);
        if (!type_arg) {
            puts("ic_type_ref_deep_copy_embedded: call to ic_type_ref_type_args_get failed");
            return 0;
        }

        new_type_arg = ic_type_ref_deep_copy(type_arg);
        if (!new_type_arg) {
            puts("ic_type_ref_deep_copy_embedded: call to ic_type_ref_deep_copy failed");
            return 0;
        }

        if (!ic_type_ref_type_args_add(to, new_type_arg)) {
            puts("ic_type_ref_deep_copy_embedded: call to ic_type_ref_type_args_add failed");
            return 0;
        }
    }

    return 1;
}

/* set the sym on this type from the provided string
 * this will change type_ref.tag to sym
 *
 * this is an error if type_ref.tag was already resolved
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_ref_set_symbol(struct ic_type_ref *type, char *type_str, unsigned int type_len) {
    if (!type) {
        puts("ic_type_ref_set_symbol: type was null");
        return 0;
    }

    if (!type_str) {
        puts("ic_type_ref_set_symbol: type_str was null");
        return 0;
    }

    /* only allowed to set to symbol if current set
     * to unknown
     */
    switch (type->tag) {
        case ic_type_ref_unknown:
            /* nothing to do */
            break;

        case ic_type_ref_symbol:
            /* error, already a symbol */
            puts("ic_type_ref_set_symbol: type was already a symbol");
            return 0;
            break;

        case ic_type_ref_param:
            /* error, already a symbol */
            puts("ic_type_ref_set_symbol: type was already param");
            return 0;
            break;

        case ic_type_ref_resolved:
            /* error, already a symbol */
            puts("ic_type_ref_set_symbol: type was already resolved");
            return 0;
            break;

        default:
            puts("ic_type_ref_set_symbol: type->type was impossible type_tag");
            return 0;
            break;
    }

    /* set to type symbol */
    type->tag = ic_type_ref_symbol;

    /* set our symbol from the provider char * and len */
    if (!ic_symbol_init(&(type->u.sym), type_str, type_len)) {
        puts("ic_type_ref_set_symbol: call to ic_symbol_init failed");
        return 0;
    }

    return 1;
}

/* return a symbol representing this type
 *
 * if type_ref is unknown then 0 is returned
 * if type_ref is symbol then the symbol is returned
 * if type_ref is resolved then the symbol for that type is returned
 *
 * returns 0 on failure
 */
struct ic_symbol *ic_type_ref_get_symbol(struct ic_type_ref *type) {
    struct ic_symbol *sym = 0;

    if (!type) {
        puts("ic_type_ref_get_symbol: type was null");
        return 0;
    }

    switch (type->tag) {
        case ic_type_ref_unknown:
            /* error, nothing to return */
            puts("ic_type_ref_get_symbol: type was of type unknown");
            return 0;
            break;

        case ic_type_ref_symbol:
            /* just return the symbol */
            sym = &(type->u.sym);
            return sym;
            break;

        case ic_type_ref_param:
            sym = ic_type_param_get_name(type->u.tparam);
            if (!sym) {
                puts("ic_type_ref_get_symbol: call to ic_type_param_get_name failed");
                return 0;
            }
            return sym;
            break;

        case ic_type_ref_resolved:
            /* the decl_type already has a symbol name */
            sym = ic_decl_type_get_name(type->u.tdecl);
            if (!sym) {
                puts("ic_type_ref_get_symbol: call to ic_decl_type_get_name failed");
                return 0;
            }
            return sym;
            break;

        default:
            puts("ic_type_ref_get_symbol: type->type was impossible type_tag");
            return 0;
            break;
    }

    return 0;
}

/* set the decl_type on this type_ref
 * this will change type.tag to resolved
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_ref_set_type_decl(struct ic_type_ref *type, struct ic_decl_type *tdecl) {
    if (!type) {
        puts("ic_type_ref_set_type_decl: type was null");
        return 0;
    }

    if (!tdecl) {
        puts("ic_type_ref_set_type_decl: tdecl was null");
        return 0;
    }

    switch (type->tag) {
        case ic_type_ref_unknown:
            /* nothing to do */
            break;

        case ic_type_ref_symbol:
            /* do not free as symbol is not a pointer */
            if (!ic_symbol_destroy(&(type->u.sym), 0)) {
                puts("ic_type_ref_set_type_decl: call to ic_symbol_destroy failed");
                return 0;
            }
            break;

        case ic_type_ref_param:
            /* type_param can be overwritten with tdecl */
            break;

        case ic_type_ref_resolved:
            puts("ic_type_ref_set_type_decl: type was already resolved");
            return 0;
            break;

        default:
            break;
    }

    type->tag = ic_type_ref_resolved;
    type->u.tdecl = tdecl;

    return 1;
}

/* return the underlying decl_type
 *
 * if type_ref.tag is not resolved then this is an error
 *
 * return * on success
 * returns 0 on failure
 */
struct ic_decl_type *ic_type_ref_get_type_decl(struct ic_type_ref *type) {
    struct ic_decl_type *tdecl = 0;

    if (!type) {
        puts("ic_type_ref_get_type_decl: type was null");
        return 0;
    }

    switch (type->tag) {
        case ic_type_ref_unknown:
            /* error, nothing to return */
            puts("ic_type_ref_get_type_decl: type was of type unknown");
            return 0;
            break;

        case ic_type_ref_symbol:
            puts("ic_type_ref_get_type_decl: type was of type symbol");
            return 0;
            break;

        case ic_type_ref_param:
            tdecl = ic_type_param_get(type->u.tparam);
            if (!tdecl) {
                puts("ic_type_ref_get_type_decl: call to ic_type_param_get failed");
                return 0;
            }
            return tdecl;
            break;

        case ic_type_ref_resolved:
            tdecl = type->u.tdecl;
            return tdecl;
            break;

        default:
            puts("ic_type_ref_get_type_decl: type->type was impossible type_ref.tag");
            return 0;
            break;
    }

    return 0;
}

/* set the decl_param on this type_ref
 * this will change type.tag to param
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_ref_set_type_param(struct ic_type_ref *type, struct ic_type_param *tparam) {
    if (!type) {
        puts("ic_type_ref_set_type_param: type was null");
        return 0;
    }

    if (!tparam) {
        puts("ic_type_ref_set_type_param: tparam was null");
        return 0;
    }

    switch (type->tag) {
        case ic_type_ref_unknown:
            /* nothing to do */
            break;

        case ic_type_ref_symbol:
            /* do not free as symbol is not a pointer */
            if (!ic_symbol_destroy(&(type->u.sym), 0)) {
                puts("ic_type_ref_set_type_param: call to ic_symbol_destroy failed");
                return 0;
            }
            break;

        case ic_type_ref_param:
            puts("ic_type_ref_set_type_param: type was already a param");
            return 0;
            break;

        case ic_type_ref_resolved:
            puts("ic_type_ref_set_type_param: type was already resolved");
            return 0;
            break;

        default:
            break;
    }

    /* set to type symbol */
    type->tag = ic_type_ref_param;

    /* store our type param */
    type->u.tparam = tparam;

    return 1;
}

/* check if this type_ref refers to a currently generic param
 *
 * returns 1 for yes
 * returns 0 for no
 */
unsigned int ic_type_ref_is_type_param(struct ic_type_ref *type) {
    if (!type) {
        puts("ic_type_ref_is_type_param: type as null");
        return 0;
    }

    return type->tag == ic_type_ref_param;
}

/* check if this type_ref refers to a concrete type
 *
 * returns 1 for yes
 * returns 0 for no
 */
unsigned int ic_type_ref_is_resolved(struct ic_type_ref *type) {
    if (!type) {
        puts("ic_type_ref_is_resolved: type as null");
        return 0;
    }

    return type->tag == ic_type_ref_resolved;
}

/* return the underlying decl_param
 *
 * if type_ref.tag is not param then this is an error
 *
 * return * on success
 * returns 0 on failure
 */
struct ic_type_param *ic_type_ref_get_type_param(struct ic_type_ref *type) {
    struct ic_type_param *tparam = 0;

    if (!type) {
        puts("ic_type_ref_get_type_param: type was null");
        return 0;
    }

    switch (type->tag) {
        case ic_type_ref_unknown:
            /* error, nothing to return */
            puts("ic_type_ref_get_type_param: type was of type unknown");
            return 0;
            break;

        case ic_type_ref_symbol:
            puts("ic_type_ref_get_type_param: type was of type symbol");
            return 0;
            break;

        case ic_type_ref_param:
            tparam = type->u.tparam;
            return tparam;
            break;

        case ic_type_ref_resolved:
            puts("ic_type_ref_get_type_param: type was of type resolve");
            return 0;
            break;

        default:
            puts("ic_type_ref_get_type_decl: type->type was impossible type_ref.tag");
            return 0;
            break;
    }

    return 0;
}

/* add a type_arg
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_ref_type_args_add(struct ic_type_ref *type, struct ic_type_ref *type_arg) {
    if (!type) {
        puts("ic_type_ref_type_args_add: type was null");
        return 0;
    }

    if (!type_arg) {
        puts("ic_type_ref_type_args_add: type_arg was null");
        return 0;
    }

    if (-1 == ic_pvector_append(&(type->type_args), type_arg)) {
        puts("ic_type_ref_type_args_add: call to ic_pvector_append failed");
        return 0;
    }

    return 1;
}

/* get a type_arg
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_type_ref *ic_type_ref_type_args_get(struct ic_type_ref *type, unsigned int i) {
    struct ic_type_ref *type_arg = 0;

    if (!type) {
        puts("ic_type_ref_type_args_get: type was null");
        return 0;
    }

    type_arg = ic_pvector_get(&(type->type_args), i);
    if (!type_arg) {
        puts("ic_type_ref_type_args_get: call to ic_pvector_get failed");
        return 0;
    }

    return type_arg;
}

/* get length of type_args
 *
 * returns num on success
 * returns 0 on failure
 */
unsigned int ic_type_ref_type_args_length(struct ic_type_ref *type) {
    unsigned int len = 0;

    if (!type) {
        puts("ic_type_ref_type_args_length: type was null");
        return 0;
    }

    len = ic_pvector_length(&(type->type_args));
    return len;
}

/* return the name of this type as char*
 * this char* is still owned by this type_ref or the underlying type_decl
 *
 * returns * on success
 * returns 0 on failure
 */
char * ic_type_ref_get_type_name_ch(struct ic_type_ref *type) {
    char *str = 0;
    struct ic_symbol *sym = 0;

    if (!type) {
        puts("ic_type_ref_get_type_name_ch: type was null");
        return 0;
    }

    sym = ic_type_ref_get_type_name(type);
    if (!sym) {
        puts("ic_type_ref_get_type_name_ch: call to ic_type_ref_get_type_name failed");
        return 0;
    }

    str = ic_symbol_contents(sym);
    if (!str) {
        puts("ic_type_ref_get_type_name_ch: call to ic_symbol_contents failed");
        return 0;
    }

    return str;
}

/* return the name of this type as symbol*
 * this symbol* is still owned by this type_ref or the underlying type_decl
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_symbol * ic_type_ref_get_type_name(struct ic_type_ref *type) {
    struct ic_symbol *sym = 0;
    struct ic_decl_type *tdecl = 0;

    if (!type) {
        puts("ic_type_ref_get_type_name: type was null");
        return 0;
    }


    switch (type->tag) {
        case ic_type_ref_unknown:
            puts("ic_type_ref_get_type_name: ic_type_ref_unknown not supported");
            return 0;
            break;

        case ic_type_ref_symbol:
            sym = &(type->u.sym);
            return sym;
            break;

        case ic_type_ref_param:
            puts("ic_type_ref_get_type_name: ic_type_ref_param not supported");
            return 0;
            break;

        case ic_type_ref_resolved:
            tdecl = type->u.tdecl;
            sym = ic_decl_type_get_name(tdecl);
            if (!sym) {
                puts("ic_type_ref_get_type_name: call to ic_decl_type_get_name failed");
                return 0;
            }

            return sym;
            break;

        default:
            puts("ic_type_ref_get_type_name: impossible type ref tag");
            return 0;
            break;
    }
}

/* print this this type */
void ic_type_ref_print(FILE *fd, struct ic_type_ref *type) {
    struct ic_symbol *sym = 0;
    unsigned int len = 0;
    unsigned int i = 0;
    struct ic_type_ref *type_param = 0;

    if (!type) {
        puts("ic_type_ref_print: type was null");
        return;
    }
    switch (type->tag) {
        case ic_type_ref_unknown:
            /* nothing to do */
            break;

        case ic_type_ref_symbol:
            /* if we are of type symbol then just print that symbol */
            sym = &(type->u.sym);
            ic_symbol_print(fd, sym);
            break;

        case ic_type_ref_param:
            ic_type_param_print(fd, type->u.tparam);
            break;

        case ic_type_ref_resolved:
            sym = ic_decl_type_get_name(type->u.tdecl);
            if (!sym) {
                puts("ic_type_ref_print: call to ic_decl_type_get_name failed");
                return;
            }
            ic_symbol_print(fd, sym);
            break;

        default:
            puts("ic_type_ref_print: type->type was impossible type_tag");
            return;
            break;
    }

    len = ic_type_ref_type_args_length(type);
    if (len) {
        fputs("[", fd);
        for (i = 0; i < len; ++i) {
            if (i > 0) {
                fputs(", ", fd);
            }
            type_param = ic_type_ref_type_args_get(type, i);
            if (!type_param) {
                puts("ic_type_ref_print: call to ic_type_ref_type_args_get failed");
                return;
            }
            ic_type_ref_print(fd, type_param);
        }
        fputs("]", fd);
    }

    return;
}
