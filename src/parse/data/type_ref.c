#include <stdio.h>
#include <stdlib.h>

#include "decl.h"
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
    if (!type) {
        puts("ic_type_ref_destroy: type was null");
        return 0;
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

        case ic_type_ref_resolved:
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

        case ic_type_ref_resolved:
            puts("ic_type_ref_set_symbol: type was already resolved");
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

        case ic_type_ref_resolved:
            return type->u.tdecl;
            break;

        default:
            puts("ic_type_ref_get_type_decl: type->type was impossible type_ref.tag");
            return 0;
            break;
    }

    return 0;
}

/* print this this type */
void ic_type_ref_print(FILE *fd, struct ic_type_ref *type) {
    struct ic_symbol *sym = 0;

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

    return;
}
