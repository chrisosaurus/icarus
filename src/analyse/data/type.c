#include <stdio.h>  /* puts */
#include <stdlib.h> /* calloc */

#include "type.h"

/* alloc and init a new type representing a tdecl
 *
 * returns new type on success
 * returns 0 on failure
 */
struct ic_type *ic_type_new_tdecl(struct ic_decl_type *decl) {
    struct ic_type *type = 0;

    if (!decl) {
        puts("ic_type_new_tdecl: decl was null");
        return 0;
    }

    type = calloc(1, sizeof(struct ic_type));
    if (!type) {
        puts("ic_type_new_tdecl: call to calloc failed");
        return 0;
    }

    if (!ic_type_init_tdecl(type, decl)) {
        puts("ic_type_new_tdecl: call to ic_type_init_tdecl failed");
        return 0;
    }

    return type;
}

/* init an existing type representing a tdecl
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_init_tdecl(struct ic_type *type, struct ic_decl_type *decl) {
    if (!type) {
        puts("ic_type_init_tdecl: type was null");
        return 0;
    }

    if (!decl) {
        puts("ic_type_init_tdecl: decl was null");
        return 0;
    }

    type->tag = ic_type_user;
    type->u.decl = decl;

    return 1;
}

/* destroy a type
 *
 * will only free the type if `free_type` is truthy
 *
 * will NOT free any decl objects
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_destroy(struct ic_type *type, unsigned int free_type) {
    if (!type) {
        puts("ic_type_destroy: type was null");
        return 0;
    }

    switch (type->tag) {
        case ic_type_user:
        case ic_type_builtin:
            type->u.decl = 0;
            break;

        default:
            break;
    }

    if (free_type) {
        free(type);
    }

    return 1;
}

/* get decl from type
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_decl_type *ic_type_get_decl(struct ic_type *type) {
    if (!type) {
        puts("ic_type_get_decl: type was null");
        return 0;
    }

    if (type->tag != ic_type_user) {
        puts("ic_type_get_decl: type was not a user declared type");
        return 0;
    }

    return type->u.decl;
}

/* return a symbol representing the name of this type
 *
 * returns ic_symbol * on sucess
 * returns 0 on failure
 */
struct ic_symbol *ic_type_name(struct ic_type *type) {
    if (!type) {
        puts("ic_type_name: type was null");
        return 0;
    }

    switch (type->tag) {

        case ic_type_user:
        case ic_type_builtin:
            return &(type->u.decl->name);
            break;

        default:
            puts("ic_type_name: impossible type->type");
            return 0;
            break;
    }

    puts("ic_type_name: impossible case");
    return 0;
}

/* mark as builtin
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_mark_builtin(struct ic_type *type) {
    if (!type) {
        puts("ic_type_mark_builtin: type was null");
        return 0;
    }

    type->tag = ic_type_builtin;
    return 1;
}

/* check if type is builtin
 *
 * returns 1 if builtin
 * returns 0 otherwise
 */
unsigned int ic_type_isbuiltin(struct ic_type *type) {
    if (!type) {
        puts("ic_type_isbuiltin: type was null");
        return 0;
    }

    if (type->tag == ic_type_builtin) {
        return 1;
    }

    return 0;
}

/* is this type void
 *
 * returns 1 if provided type is Void
 * returns 0 if provided type is NOT void
 */
unsigned int ic_type_isvoid(struct ic_type *type) {
    if (!type) {
        puts("ic_type_isvoid: type was null");
        return 0;
    }

    if (type->tag != ic_type_user) {
        return 0;
    }

    return type->u.decl->isvoid;
}

/* is this type bool
 *
 * returns 1 if provided type is Void
 * returns 0 if provided type is NOT void
 */
unsigned int ic_type_isbool(struct ic_type *type) {
    if (!type) {
        puts("ic_type_isbool: type was null");
        return 0;
    }

    if (type->tag != ic_type_user) {
        puts("isbool: not user");
        return 0;
    }

    return type->u.decl->isbool;
}

/* are these 2 types the equal
 *
 * returns 1 if they are equal
 * returns 0 of they are NOT equal
 */
unsigned int ic_type_equal(struct ic_type *a, struct ic_type *b) {
    if (!a) {
        puts("ic_type_equal: type a was null");
        return 0;
    }

    if (!b) {
        puts("ic_type_equal: type b was null");
        return 0;
    }

    return a == b;
}

/* print type name */
void ic_type_print(struct ic_type *type) {
    if (!type) {
        puts("ic_type_print: type was null");
        return;
    }

    ic_symbol_print(&(type->u.decl->name));
}

/* print debug information about a type */
void ic_type_print_debug(struct ic_type *type) {
    unsigned int fake_indent = 0;

    if (!type) {
        puts("ic_type_print_debug: type was null");
        return;
    }

    fputs("found: ", stdout);
    ic_decl_type_print_header(type->u.decl, &fake_indent);
}

