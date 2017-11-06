#include <stdio.h>  /* puts, printf */
#include <stdlib.h> /* calloc */
#include <string.h> /* memset */

#include "../../data/pvector.h"
#include "../../data/symbol.h"
#include "../parse.h"
#include "../permissions.h"
#include "expr.h"

/* allocate and initialise a new func call
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr_func_call *ic_expr_func_call_new(struct ic_expr *func_name) {
    struct ic_expr_func_call *fcall = 0;

    if (!func_name) {
        puts("ic_expr_func_call_new: func_name was null");
        return 0;
    }

    fcall = calloc(1, sizeof(struct ic_expr_func_call));
    if (!fcall) {
        puts("ic_expr_func_call_init: calloc failed");
        return 0;
    }

    if (!ic_expr_func_call_init(fcall, func_name)) {
        puts("ic_expr_func_call_init: call to ic_expr_func_call_init failed");
        free(fcall);
        return 0;
    }

    return fcall;
}

/* intialise an existing func call
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_expr_func_call_init(struct ic_expr_func_call *fcall, struct ic_expr *func_name) {
    if (!fcall) {
        puts("ic_expr_func_call_init: fcall was null");
        return 0;
    }

    if (!func_name) {
        puts("ic_expr_func_call_init: func_name was null");
        return 0;
    }

    /* call init on components */

    if (!ic_pvector_init(&(fcall->type_refs), 0)) {
        puts("ic_expr_func_call_init: call to ic_pvector_init failed");
        return 0;
    }

    if (!ic_pvector_init(&(fcall->args), 0)) {
        puts("ic_expr_func_call_init: call to ic_pvector_init failed");
        return 0;
    }

    /* fstr is set later by ic_analyse_fcall_str */
    fcall->string = 0;
    /* set by expr_func_call_sig_param */
    fcall->sig_param = 0;
    /* fdecl is used in the analyse phase */
    fcall->fdecl = 0;
    /* set our func_name */
    fcall->fname = func_name;

    return 1;
}

/* destroy fcall
 *
 * only free fcall if `free_fcall` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_expr_func_call_destroy(struct ic_expr_func_call *fcall, unsigned int free_fcall) {
    int i = 0;
    int len = 0;
    struct ic_type_ref *type_ref = 0;
    struct ic_expr *expr = 0;

    if (!fcall) {
        puts("ic_expr_func_call_destroy: fcall was null");
        return 0;
    }

    /* free = 0 as member */
    if (!ic_expr_destroy(fcall->fname, 0)) {
        puts("ic_expr_func_call_destroy: call to ic_expr_destroy failed");
        return 0;
    }

    len = ic_expr_func_call_type_refs_length(fcall);
    for (i = 0; i < len; ++i) {
        type_ref = ic_expr_func_call_get_type_ref(fcall, i);
        if (!type_ref) {
            puts("ic_expr_func_call_destroy: call to ic_expr_func_call_get_type_ref failed");
            return 0;
        }

        /* free = 1 as pointer element */
        if (!ic_type_ref_destroy(type_ref, 1)) {
            puts("ic_expr_func_call_destroy: call to ic_type_ref_destroy failed");
            return 0;
        }
    }

    len = ic_expr_func_call_length(fcall);
    for (i = 0; i < len; ++i) {
        expr = ic_expr_func_call_get_arg(fcall, i);
        if (!expr) {
            puts("ic_expr_func_call_destroy: call to ic_expr_func_call_get_arg failed");
            return 0;
        }

        /* free = 1 as pointer element */
        if (!ic_expr_destroy(expr, 1)) {
            puts("ic_expr_func_call_destroy: call to ic_expr_destroy failed");
            return 0;
        }
    }

    if (fcall->string) {
        if (!ic_string_destroy(fcall->string, 1)) {
            puts("ic_expr_func_call_destroy: call to ic_string_destroy failed");
            return 0;
        }
        fcall->string = 0;
    }

    if (fcall->sig_param) {
        if (!ic_string_destroy(fcall->sig_param, 1)) {
            puts("ic_expr_func_call_destroy: call to ic_string_destroy failed");
            return 0;
        }
        fcall->sig_param = 0;
    }

    fcall->fdecl = 0;

    /* if asked */
    if (free_fcall) {
        free(fcall);
    }

    return 1;
}

/* perform a deep copy of func_call
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_expr_func_call *ic_expr_func_call_deep_copy(struct ic_expr_func_call *func_call) {
    struct ic_expr_func_call *new_func_call = 0;

    if (!func_call) {
        puts("ic_expr_func_call_deep_copy: func_call was null");
        return 0;
    }

    new_func_call = calloc(1, sizeof(struct ic_expr_func_call));
    if (!new_func_call) {
        puts("ic_expr_func_call_deep_copy: call to calloc failed");
        return 0;
    }

    if (!ic_expr_func_call_deep_copy_embedded(func_call, new_func_call)) {
        puts("ic_expr_func_call_deep_copy: call to ic_expr_func_call_deep_copy_embedded failed");
        return 0;
    }

    return new_func_call;
}

/* perform a deep copy of func_call embedded within an object
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_expr_func_call_deep_copy_embedded(struct ic_expr_func_call *from, struct ic_expr_func_call *to) {
    if (!from) {
        puts("ic_expr_func_call_deep_copy_embedded: from was null");
        return 0;
    }

    if (!to) {
        puts("ic_expr_func_call_deep_copy_embedded: to was null");
        return 0;
    }

    puts("ic_expr_func_call_deep_copy_embedded: unimplemented");
    return 0;

}

/* set fdecl on fcall
 * must not already be set
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_expr_func_call_set_fdecl(struct ic_expr_func_call *fcall, struct ic_decl_func *fdecl) {
    if (!fcall) {
        puts("ic_expr_func_call_set_fdecl: fcall was null");
        return 0;
    }

    if (!fdecl) {
        puts("ic_expr_func_call_set_fdecl: fdecl was null");
        return 0;
    }

    if (fcall->fdecl) {
        puts("ic_expr_func_call_set_fdecl: fdecl was already set");
        return 0;
    }

    fcall->fdecl = fdecl;

    return 1;
}

/* get fdecl on fcall
 * must already be set
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_decl_func *ic_expr_func_call_get_fdecl(struct ic_expr_func_call *fcall) {
    if (!fcall) {
        puts("ic_expr_func_call_get_fdecl: fcall was null");
        return 0;
    }

    if (!fcall->fdecl) {
        puts("ic_expr_func_call_set_fdecl: fdecl was not set");
        return 0;
    }

    return fcall->fdecl;
}

/* check if fdecl is already set
 *
 * returns 1 if fdecl is set
 * returns 0 otherwise
 */
unsigned int ic_expr_func_call_has_fdecl(struct ic_expr_func_call *fcall) {
    if (!fcall) {
        puts("ic_expr_func_call_get_fdecl: fcall was null");
        return 0;
    }

    if (!fcall->fdecl) {
        return 0;
    }

    return 1;
}

/* add a new argument to this function call
 *
 * returns 1 on success
 * returns 0 on failure
 */
int ic_expr_func_call_add_arg(struct ic_expr_func_call *fcall, struct ic_expr *expr) {

    if (!fcall) {
        puts("ic_expr_func_call_add_arg: fcall was null");
        return 0;
    }
    if (!expr) {
        puts("ic_expr_func_call_add_arg: expr was null");
        return 0;
    }

    /* let pvector do al the work */
    if (-1 == ic_pvector_append(&(fcall->args), expr)) {
        puts("ic_expr_func_call_add_arg: call to ic_pvector_append failed");
        return 0;
    }

    return 1;
}

/* get argument
 *
 * returns field at offset on success
 * returns 0 on failure
 */
struct ic_expr *ic_expr_func_call_get_arg(struct ic_expr_func_call *fcall, unsigned int i) {
    if (!fcall) {
        puts("ic_expr_func_call_get_arg: fcall was null");
        return 0;
    }

    /* let pvector do al the work */
    return ic_pvector_get(&(fcall->args), i);
}

/* returns number of arguments on success
 * returns 0 on failure
 */
unsigned int ic_expr_func_call_length(struct ic_expr_func_call *fcall) {
    if (!fcall) {
        puts("ic_expr_func_call_length: fcall was null");
        return 0;
    }

    /* let pvector do al the work */
    return ic_pvector_length(&(fcall->args));
}

/* add a new type ref to this function call
 *
 * returns 1 on success
 * returns 0 on failure
 */
int ic_expr_func_call_add_type_ref(struct ic_expr_func_call *fcall, struct ic_type_ref *type_ref) {
    if (!fcall) {
        puts("ic_expr_func_call_add_type_ref: fcall was null");
        return 0;
    }
    if (!type_ref) {
        puts("ic_expr_func_call_add_type_ref: type_ref was null");
        return 0;
    }

    /* let pvector do al the work */
    if (-1 == ic_pvector_append(&(fcall->type_refs), type_ref)) {
        puts("ic_expr_func_call_add_type_ref: call to ic_pvector_append failed");
        return 0;
    }

    return 1;
}

/* get type ref
 *
 * returns field at offset on success
 * returns 0 on failure
 */
struct ic_type_ref *ic_expr_func_call_get_type_ref(struct ic_expr_func_call *fcall, unsigned int i) {
    if (!fcall) {
        puts("ic_expr_func_call_get_type_refs: fcall was null");
        return 0;
    }

    /* let pvector do al the work */
    return ic_pvector_get(&(fcall->type_refs), i);
}

/* returns number of type_refs on success
 * returns 0 on failure
 */
unsigned int ic_expr_func_call_type_refs_length(struct ic_expr_func_call *fcall) {
    if (!fcall) {
        puts("ic_expr_func_call_type_refs_length: fcall was null");
        return 0;
    }

    /* let pvector do al the work */
    return ic_pvector_length(&(fcall->type_refs));
}

/* get internal sig_param
 *
 * will generate is not already generated
 *
 * returns * on success
 * returns 0 on failure
 */
char *ic_expr_func_call_sig_param(struct ic_expr_func_call *fcall) {
    struct ic_string *str = 0;
    struct ic_symbol *sym = 0;
    unsigned int i = 0;
    unsigned int len = 0;
    char *ch = 0;

    if (!fcall) {
        puts("ic_expr_func_call_sig_param: fcall was null");
        return 0;
    }

    if (fcall->sig_param) {
        ch = ic_string_contents(fcall->sig_param);
        if (!ch) {
            puts("ic_expr_func_call_sig_param: call to ic_string_contents failed");
            return 0;
        }
        return ch;
    }

    str = ic_string_new_empty();
    if (!str) {
        puts("ic_expr_func_call_sig_param: call to ic_string_new_empty failed");
        return 0;
    }

    sym = ic_expr_func_call_get_symbol(fcall);
    if (!sym) {
        puts("ic_expr_func_call_sig_param: call to ic_expr_func_call_get_symbol failed");
        return 0;
    }

    if (!ic_string_append_symbol(str, sym)) {
        puts("ic_expr_func_call_sig_param: call to ic_string_append_symbol failed");
        return 0;
    }

    len = ic_expr_func_call_type_refs_length(fcall);
    if (len) {
        if (!ic_string_append_char(str, "[", 1)) {
            puts("ic_expr_func_call_sig_param: call to ic_string_append_symbol failed");
            return 0;
        }

        for (i = 0; i < len; ++i) {
            if (i == 0) {
                if (!ic_string_append_char(str, "_", 1)) {
                    puts("ic_expr_func_call_sig_param: call to ic_string_append_symbol failed");
                    return 0;
                }
            } else {
                if (!ic_string_append_char(str, ",_", 2)) {
                    puts("ic_expr_func_call_sig_param: call to ic_string_append_symbol failed");
                    return 0;
                }
            }
        }

        if (!ic_string_append_char(str, "]", 1)) {
            puts("ic_expr_func_call_sig_param: call to ic_string_append_symbol failed");
            return 0;
        }
    }

    if (!ic_string_append_char(str, "(", 1)) {
        puts("ic_expr_func_call_sig_param: call to ic_string_append_symbol failed");
        return 0;
    }
    len = ic_expr_func_call_length(fcall);
    for (i = 0; i < len; ++i) {
        if (i == 0) {
            if (!ic_string_append_char(str, "_", 1)) {
                puts("ic_expr_func_call_sig_param: call to ic_string_append_symbol failed");
                return 0;
            }
        } else {
            if (!ic_string_append_char(str, ",_", 2)) {
                puts("ic_expr_func_call_sig_param: call to ic_string_append_symbol failed");
                return 0;
            }
        }
    }
    if (!ic_string_append_char(str, ")", 1)) {
        puts("ic_expr_func_call_sig_param: call to ic_string_append_symbol failed");
        return 0;
    }

    fcall->sig_param = str;

    ch = ic_string_contents(fcall->sig_param);
    if (!ch) {
        puts("ic_expr_func_call_sig_param: call to ic_string_contents failed");
        return 0;
    }

    return ch;
}

/* get internal symbol for function name
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_symbol *ic_expr_func_call_get_symbol(struct ic_expr_func_call *fcall) {
    struct ic_expr_identifier *id = 0;
    struct ic_symbol *symbol = 0;

    if (!fcall) {
        puts("ic_expr_func_call_get_symbol: fcall was null");
        return 0;
    }

    id = ic_expr_get_identifier(fcall->fname);
    if (!id) {
        puts("ic_expr_func_call_get_symbol: call to ic_expr_get_identifier failed");
        return 0;
    }

    symbol = ic_expr_identifier_symbol(id);
    if (!symbol) {
        puts("ic_expr_func_call_get_symbol: call to ic_expr_identifier_symbol failed");
        return 0;
    }

    return symbol;
}

/* get name
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_symbol *ic_expr_func_call_get_name(struct ic_expr_func_call *fcall) {
    struct ic_symbol *sym = 0;

    if (!fcall) {
        puts("ic_expr_func_call_get_name: fcall was null");
        return 0;
    }

    sym = ic_expr_func_call_get_symbol(fcall);
    if (!sym) {
        puts("ic_expr_func_call_get_name: call to ic_expr_func_call_get_symbol failed");
        return 0;
    }

    return sym;
}

/* print this func call */
void ic_expr_func_call_print(FILE *fd, struct ic_expr_func_call *fcall, unsigned int *indent_level) {
    /* our eventual return value */
    struct ic_expr *arg = 0;
    /* out iterator through args */
    unsigned int i = 0;
    /* cached length of vector */
    unsigned int len = 0;
    /* our fake indent level we use
     * as args do not need to be indented
     */
    unsigned int fake_indent = 0;
    struct ic_type_ref *type_ref = 0;

    if (!fcall) {
        puts("ic_expr_func_call_print: fcall was null");
        return;
    }
    if (!indent_level) {
        puts("ic_expr_func_call_print: indent_level was null");
        return;
    }

    /* print indent */
    ic_parse_print_indent(fd, *indent_level);

    /* print function name */
    ic_expr_print(fd, fcall->fname, &fake_indent);

    len = ic_expr_func_call_type_refs_length(fcall);
    if (len) {
        fputs("[", fd);
        for (i = 0; i < len; ++i) {
            if (i > 0) {
                fputs(", ", fd);
            }
            type_ref = ic_expr_func_call_get_type_ref(fcall, i);
            if (!type_ref) {
                puts("ic_expr_func_call_print: call to ic_expr_func_call_get_type_ref failed");
                return;
            }
            ic_type_ref_print(fd, type_ref);
        }
        fputs("]", fd);
    }

    /* print bracket */
    fputs("(", fd);

    len = ic_expr_func_call_length(fcall);

    /* print args */
    for (i = 0; i < len; ++i) {
        arg = ic_expr_func_call_get_arg(fcall, i);
        if (!arg) {
            puts("ic_expr_func_call_print: call to ic_expr_func_call_get_arg failed");
            continue;
        }

        ic_expr_print(fd, arg, &fake_indent);

        /* if we are not the last argument then print a comma and space */
        if (i < (len - 1)) {
            fputs(", ", fd);
        }
    }

    /* closing bracket */
    fputs(")", fd);

    /* up to caller to work out \n placement */
}

/* allocate and initialise a new identifier
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr_identifier *ic_expr_identifier_new(char *id, unsigned int id_len, unsigned int permissions) {
    struct ic_expr_identifier *identifier = 0;

    if (!id) {
        puts("ic_expr_identifier_new: id was null");
        return 0;
    }

    /* allocate */
    identifier = calloc(1, sizeof(struct ic_expr_identifier));
    if (!identifier) {
        puts("ic_expr_identifier_new: calloc failed");
        return 0;
    }

    /* initialise */
    if (!ic_expr_identifier_init(identifier, id, id_len, permissions)) {
        puts("ic_expr_identifier_new: call to ic_expr_identifier_init failed");
        free(identifier);
        return 0;
    }

    /*return */
    return identifier;
}

/* initialise an existing identifier
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_expr_identifier_init(struct ic_expr_identifier *identifier, char *id, unsigned int id_len, unsigned int permissions) {
    if (!identifier) {
        puts("ic_expr_identifier_init: identifier was null");
        return 0;
    }

    if (!id) {
        puts("ic_expr_identifier_init: id was null");
        return 0;
    }

    /* init our symbol name */
    if (!ic_symbol_init(&(identifier->identifier), id, id_len)) {
        puts("ic_expr_identifier_init: call to ic_symbol_init failed");
        return 0;
    }

    /* if permissions are not set, then go to default */
    if (!permissions) {
        permissions = ic_parse_perm_default();
    }

    identifier->permissions = permissions;

    return 1;
}

/* destroy identifier
 *
 * will free id if `free_id` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_expr_identifier_destroy(struct ic_expr_identifier *identifier, unsigned int free_id) {
    if (!identifier) {
        puts("ic_expr_identifier_destroy: identifier was null");
        return 0;
    }

    /* free = 0 as member */
    if (!ic_symbol_destroy(&(identifier->identifier), 0)) {
        puts("ic_expr_identifier_destroy: identifier was null");
        return 0;
    }

    /* if asked nicely */
    if (free_id) {
        free(identifier);
    }

    return 1;
}

/* perform a deep copy of identifier
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_expr_identifier *ic_expr_identifier_deep_copy(struct ic_expr_identifier *id) {
    struct ic_expr_identifier *new_id = 0;

    if (!id) {
        puts("ic_expr_identifier_deep_copy: id was null");
        return 0;
    }

    new_id = calloc(1, sizeof(struct ic_expr_identifier));
    if (!new_id) {
        puts("ic_expr_identifier_deep_copy: call to calloc failed");
        return 0;
    }

    if (!ic_expr_identifier_deep_copy_embedded(id, new_id)) {
        puts("ic_expr_identifier_deep_copy: call to ic_expr_identifier_deep_copy_embedded failed");
        return 0;
    }

    return new_id;
}

/* perform a deep copy of identifier embedded within an object
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_expr_identifier_deep_copy_embedded(struct ic_expr_identifier *from, struct ic_expr_identifier *to) {
    if (!from) {
        puts("ic_expr_identifier_deep_copy_embedded: from was null");
        return 0;
    }

    if (!to) {
        puts("ic_expr_identifier_deep_copy_embedded: to was null");
        return 0;
    }

    if (!ic_symbol_deep_copy_embedded(&(from->identifier), &(to->identifier))) {
        puts("ic_expr_identifier_deep_copy_embedded: call to ic_symbol_deep_copy_embedded failed");
        return 0;
    }

    to->permissions = from->permissions;

    return 1;
}

/* get idenifier symbol
 *
 * returns ic_symbol * on success
 * returns 0 on error
 */
struct ic_symbol *ic_expr_identifier_symbol(struct ic_expr_identifier *identifier) {
    if (!identifier) {
        puts("ic_expr_identifier_symbol: identifier was null");
        return 0;
    }

    return &(identifier->identifier);
}

/* allocate and init a new constant
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr_constant *ic_expr_constant_new(enum ic_expr_constant_tag tag) {
    struct ic_expr_constant *constant = 0;

    /* alloc */
    constant = calloc(1, sizeof(struct ic_expr_constant));
    if (!constant) {
        puts("ic_expr_constant_new: call to calloc failed");
        return 0;
    }

    /* init */
    if (!ic_expr_constant_init(constant, tag)) {
        puts("ic_expr_constant_new: call to ic_expr_constant_init failed");
        return 0;
    }

    /* return */
    return constant;
}

/* initialise an existing constant
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_expr_constant_init(struct ic_expr_constant *constant, enum ic_expr_constant_tag tag) {
    if (!constant) {
        puts("ic_expr_constant_init: constant was null");
        return 0;
    }

    /* for now our only job is to set the type */
    constant->tag = tag;

    /* victory */
    return 1;
}

/* destroy const
 *
 * will free const if `free_const` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_expr_constant_destroy(struct ic_expr_constant *constant, unsigned int free_const) {
    if (!constant) {
        puts("ic_expr_constant_destroy: constant was null");
        return 0;
    }

    switch (constant->tag) {
        case ic_expr_constant_type_unsigned_integer:
        case ic_expr_constant_type_signed_integer:
            /* nothing to do */
            break;

        case ic_expr_constant_type_string:
            /* free = 0 as member */
            if (!ic_string_destroy(&(constant->u.string), 0)) {
                puts("ic_expr_constant_destroy: call to ic_string_destroy failed");
                return 0;
            }
            break;

        case ic_expr_constant_type_boolean:
            /* nothing to do */
            break;

        default:
            puts("ic_expr_constant_destroy: impossible type");
            return 0;
            break;
    }

    if (free_const) {
        free(constant);
    }

    return 1;
}

/* perform a deep copy of constant
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_expr_constant *ic_expr_constant_deep_copy(struct ic_expr_constant *constant) {
    struct ic_expr_constant *new_cons = 0;

    if (!constant) {
        puts("ic_expr_constant_deep_copy: constant was null");
        return 0;
    }

    new_cons = calloc(1, sizeof(struct ic_expr_constant));
    if (!new_cons) {
        puts("ic_expr_constant_deep_copy: call to calloc failed");
        return 0;
    }

    if (!ic_expr_constant_deep_copy_embedded(constant, new_cons)) {
        puts("ic_expr_constant_deep_copy: call to ic_expr_constant_deep_copy_embedded failed");
        return 0;
    }

    return new_cons;
}

/* perform a deep copy of constant embedded within an object
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_expr_constant_deep_copy_embedded(struct ic_expr_constant *from, struct ic_expr_constant *to) {
    if (!from) {
        puts("ic_expr_constant_deep_copy_embedded: from was null");
        return 0;
    }

    if (!to) {
        puts("ic_expr_constant_deep_copy_embedded: to was null");
        return 0;
    }

    to->tag = from->tag;

    switch (to->tag) {
        case ic_expr_constant_type_unsigned_integer:
            to->u.unsigned_integer = from->u.unsigned_integer;
            break;

        case ic_expr_constant_type_signed_integer:
            to->u.signed_integer = from->u.signed_integer;
            break;

        case ic_expr_constant_type_string:
            if (!ic_string_deep_copy_embedded(&(from->u.string), &(to->u.string))) {
                puts("ic_expr_constant_deep_copy_embedded: call to ic_string_deep_copy_embedded failed");
                return 0;
            }
            break;

        case ic_expr_constant_type_boolean:
            to->u.boolean = from->u.boolean;
            break;

        default:
            puts("ic_expr_constant_deep_copy_embedded: impossible tag");
            return 0;
            break;
    }

    return 1;
}

/* return pointer to unsigned integer within,
 * will only succeed if constant is of the correct type
 *
 * returns pointers on success
 * returns 0 on failure
 */
unsigned long int *ic_expr_constant_get_unsigned_integer(struct ic_expr_constant *constant) {
    if (!constant) {
        puts("ic_expr_constant_get_unsigned_integer: constant was null");
        return 0;
    }

    /* check type before handing out pointer */
    if (constant->tag != ic_expr_constant_type_unsigned_integer) {
        puts("ic_expr_constant_get_unsigned_integer: not an integer");
        return 0;
    }

    /* give them what they want */
    return &(constant->u.unsigned_integer);
}

/* return pointer to signed integer within,
 * will only succeed if constant is of the correct type
 *
 * returns pointers on success
 * returns 0 on failure
 */
long int *ic_expr_constant_get_signed_integer(struct ic_expr_constant *constant) {
    if (!constant) {
        puts("ic_expr_constant_get_signed_integer: constant was null");
        return 0;
    }

    /* check type before handing out pointer */
    if (constant->tag != ic_expr_constant_type_signed_integer) {
        puts("ic_expr_constant_get_signed_integer: not an integer");
        return 0;
    }

    /* give them what they want */
    return &(constant->u.signed_integer);
}

/* return pointer to ic_string within,
 * will only succeed if constant is of the correct type
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_string *ic_expr_constant_get_string(struct ic_expr_constant *constant) {
    if (!constant) {
        puts("ic_expr_constant_get_string: constant was null");
        return 0;
    }

    /* check type before handing out pointer */
    if (constant->tag != ic_expr_constant_type_string) {
        puts("ic_expr_constant_get_string: not an string");
        return 0;
    }

    /* give them what they want */
    return &(constant->u.string);
}

/* return pointer boolean within,
 * will only succeed if constant is of the correct type
 *
 * returns pointers on success
 * returns 0 on failure
 */
unsigned int *ic_expr_constant_get_boolean(struct ic_expr_constant *constant) {
    if (!constant) {
        puts("ic_expr_constant_get_boolean: constant was null");
        return 0;
    }

    /* check type before handing out pointer */
    if (constant->tag != ic_expr_constant_type_boolean) {
        puts("ic_expr_constant_get_boolean: not an boolean");
        return 0;
    }

    /* give them what they want */
    return &(constant->u.boolean);
}

/* print this constant */
void ic_expr_constant_print(FILE *fd, struct ic_expr_constant *constant, unsigned int *indent_level) {
    /* pointer to string if we need it */
    struct ic_string *string = 0;
    /* pointer to long int if we need it */
    long int *signed_integer = 0;
    unsigned long int *unsigned_integer = 0;
    /* boolean */
    unsigned int *boolean = 0;
    /* output */
    char *output = 0;

    if (!constant) {
        puts("ic_expr_constant_print: constant was null");
        return;
    }
    if (!indent_level) {
        puts("ic_expr_constant_print: indent_level was null");
        return;
    }

    switch (constant->tag) {
        case ic_expr_constant_type_string:
            /* pull out our string */
            string = ic_expr_constant_get_string(constant);
            if (!string) {
                puts("ic_expr_constant_print: call to ic_expr_constant_get_string failed");
                return;
            }

            /* print indent */
            ic_parse_print_indent(fd, *indent_level);

            /* print out the backing string surrounded by double quotes */
            fprintf(fd, "\"%s\"", ic_string_contents(string));
            break;

        case ic_expr_constant_type_unsigned_integer:
            /* print indent */
            ic_parse_print_indent(fd, *indent_level);

            /* pull out our integer */
            unsigned_integer = ic_expr_constant_get_unsigned_integer(constant);
            if (!unsigned_integer) {
                puts("ic_expr_constant_print: call to ic_expr_constant_get_unsigned_integer failed");
                return;
            }

            /* print out long integer */
            fprintf(fd, "%ldu", *unsigned_integer);
            break;

        case ic_expr_constant_type_signed_integer:
            /* print indent */
            ic_parse_print_indent(fd, *indent_level);

            /* pull out our integer */
            signed_integer = ic_expr_constant_get_signed_integer(constant);
            if (!signed_integer) {
                puts("ic_expr_constant_print: call to ic_expr_constant_get_signed_integer failed");
                return;
            }

            /* print out long integer */
            fprintf(fd, "%lds", *signed_integer);
            break;

        case ic_expr_constant_type_boolean:
            ic_parse_print_indent(fd, *indent_level);

            /* pull out our boolean */
            boolean = ic_expr_constant_get_boolean(constant);
            if (!boolean) {
                puts("ic_expr_constant_print: call to ic_expr_constant_get_boolean failed");
                return;
            }

            if (*boolean) {
                output = ic_token_id_get_representation(IC_TRUE);
            } else {
                output = ic_token_id_get_representation(IC_FALSE);
            }

            if (!output) {
                printf("ic_expr_constant_print: failed to get representation for boolean '%d'\n", *boolean);
                return;
            }

            fprintf(fd, "%s", output);
            break;

        default:
            puts("ic_expr_constant_print: unsupported type");
            break;
    }
}

/* allocate and initialise a new unary op
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr_operator *ic_expr_operator_new_unary(struct ic_expr *first, struct ic_token *token) {
    struct ic_expr_operator *operator= 0;

    if (!first) {
        puts("ic_expr_operator_new_unary: first was null");
        return 0;
    }

    if (!token) {
        puts("ic_expr_operator_new_unary: token  was null");
        return 0;
    }

    /* allocate */
    operator= calloc(1, sizeof(struct ic_expr_operator));
    if (!operator) {
        puts("ic_expr_operator_new_unary: calloc failed");
        return 0;
    }

    /* initialise */
    if (!ic_expr_operator_init_unary(operator, first, token)) {
        puts("ic_expr_operator_new_unary: call to ic_expr_operator_init_unary failed");
        free(operator);
        return 0;
    }

    /* success */
    return operator;
}

/* initialise an existing unary op
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_expr_operator_init_unary(struct ic_expr_operator *operator, struct ic_expr *first, struct ic_token *token) {
    return ic_expr_operator_init(operator, ic_expr_operator_type_unary, first, 0, token);
}

/* allocate and initialise a new binary op
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr_operator *ic_expr_operator_new_binary(struct ic_expr *first, struct ic_expr *second, struct ic_token *token) {
    struct ic_expr_operator *operator= 0;

    if (!first) {
        puts("ic_expr_operator_new_binary: first was null");
        return 0;
    }

    if (!second) {
        puts("ic_expr_operator_new_binary: second was null");
        return 0;
    }

    if (!token) {
        puts("ic_expr_operator_new_binary: token was null");
        return 0;
    }

    /* allocate */
    operator= calloc(1, sizeof(struct ic_expr_operator));
    if (!operator) {
        puts("ic_expr_operator_new_binary: calloc failed");
        return 0;
    }

    /* initialise */
    if (!ic_expr_operator_init_binary(operator, first, second, token)) {
        puts("ic_expr_operator_new_binary: call to ic_expr_operator_init_binary failed");
        free(operator);
        return 0;
    }

    /* success */
    return operator;
}

/* initialise an existing binary op
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_expr_operator_init_binary(struct ic_expr_operator *operator, struct ic_expr *first, struct ic_expr *second, struct ic_token *token) {
    return ic_expr_operator_init(operator, ic_expr_operator_type_binary, first, second, token);
}

/* initialise an existing op
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_expr_operator_init(struct ic_expr_operator *operator, enum ic_expr_operator_tag tag, struct ic_expr *first, struct ic_expr *second, struct ic_token *token) {
    if (!operator) {
        puts("ic_expr_operator_init_binary: operator was null");
        return 0;
    }

    if (!first) {
        puts("ic_expr_operator_init_binary: first was null");
        return 0;
    }

    if (!token) {
        puts("ic_expr_operator_init_binary: token was null");
        return 0;
    }

    if (tag == ic_expr_operator_type_binary) {
        if (!second) {
            puts("ic_expr_operator_init_binary: second was null");
            return 0;
        }

        operator->second = second;
    }

    operator->token = token;

    /* assign sub expressions */
    operator->first = first;
    operator->tag = tag;

    operator->fcall = 0;

    /* success */
    return 1;
}

/* destroy operator
 *
 * will free op if `free_op` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_expr_operator_destroy(struct ic_expr_operator *op, unsigned int free_op) {
    if (!op) {
        puts("ic_expr_operator_destroy: operator was null");
        return 0;
    }

    if (op->first) {
        /* free = 1 as pointer */
        if (!ic_expr_destroy(op->first, 1)) {
            puts("ic_expr_operator_destroy: first : call to ic_expr_destroy failed");
            return 0;
        }
    }

    if (op->second) {
        /* free = 1 as pointer */
        if (!ic_expr_destroy(op->second, 1)) {
            puts("ic_expr_operator_destroy: second : call to ic_expr_destroy failed");
            return 0;
        }
    }

    /* FIXME TODO avoiding calling this
     * as operators are a little different to normal fcalls
     * and the below code isn't operator-safe yet
     */
    /*
    if (op->fcall) {
        if (!ic_expr_func_call_destroy(op->fcall, 1)) {
            puts("ic_expr_operator_destroy: call to ic_expr_func_call_destroy failed");
            return 0;
        }
    }
    */

    if (free_op) {
        free(op);
    }

    return 1;
}

/* perform a deep copy of operator
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_expr_operator *ic_expr_operator_deep_copy(struct ic_expr_operator *operator) {
    struct ic_expr_operator *new_op = 0;

    if (!operator) {
        puts("ic_expr_operator_deep_copy: operator was null");
        return 0;
    }

    new_op = calloc(1, sizeof(struct ic_expr_operator));
    if (!new_op) {
        puts("ic_expr_operator_deep_copy: call to calloc failed");
        return 0;
    }

    if (!ic_expr_operator_deep_copy_embedded(operator, new_op)) {
        puts("ic_expr_operator_deep_copy: call to ic_expr_operator_deep_copy_embedded failed");
        return 0;
    }

    return new_op;
}

/* perform a deep copy of operator embedded within an object
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_expr_operator_deep_copy_embedded(struct ic_expr_operator *from, struct ic_expr_operator *to) {
    if (!from) {
        puts("ic_expr_operator_deep_copy_embedded: from was null");
        return 0;
    }

    if (!to) {
        puts("ic_expr_operator_deep_copy_embedded: to was null");
        return 0;
    }

    puts("ic_expr_operator_deep_copy_embedded: unimplemented");
    return 0;
}

/* print this operator */
void ic_expr_operator_print(FILE *fd, struct ic_expr_operator *op, unsigned int *indent_level) {
    /* fake indent we pass into sub expr */
    unsigned int fake_indent = 0;

    if (!op) {
        puts("ic_expr_operator_print: op was null");
        return;
    }
    if (!indent_level) {
        puts("ic_expr_operator_print: indent_level was null");
        return;
    }

    /* print indent before expr */
    ic_parse_print_indent(fd, *indent_level);

    if (!op->token) {
        puts("ic_expr_operator_print: asked to print null token!");
        printf("for op '%p'\n", (void *)op);
        return;
    }

    switch (op->tag) {
        case ic_expr_operator_type_unary:
            /* print prefix
             * print:
             *  op first
             */
            ic_token_print(fd, op->token);
            fputs(" ", fd);
            ic_expr_print(fd, op->first, &fake_indent);
            break;

        case ic_expr_operator_type_binary:
            /* print infix
             * print:
             *  first op second
             */
            ic_expr_print(fd, op->first, &fake_indent);
            fputs(" ", fd);
            ic_token_print(fd, op->token);
            fputs(" ", fd);
            ic_expr_print(fd, op->second, &fake_indent);

            break;
        default:
            puts("ic_expr_operator_print: unknown operator type");
            break;
    }
}

/* allocate and initialise a field access
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr_faccess *ic_expr_faccess_new(struct ic_expr *left, struct ic_expr *right) {
    struct ic_expr_faccess *faccess = 0;

    if (!left) {
        puts("ic_expr_faccess_new: left was null");
        return 0;
    }

    if (!right) {
        puts("ic_expr_faccess_new: right was null");
        return 0;
    }

    faccess = calloc(1, sizeof(struct ic_expr_faccess));
    if (!faccess) {
        puts("ic_expr_faccess_new: call to calloc failed");
        return 0;
    }

    if (!ic_expr_faccess_init(faccess, left, right)) {
        puts("ic_expr_faccess_new: call to ic_expr_faccess_init failed");
        free(faccess);
        return 0;
    }

    return faccess;
}

/* initialise an existing field access
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_expr_faccess_init(struct ic_expr_faccess *faccess, struct ic_expr *left, struct ic_expr *right) {
    if (!faccess) {
        puts("ic_expr_faccess_init: faccess was null");
        return 0;
    }

    if (!left) {
        puts("ic_expr_faccess_init: left was null");
        return 0;
    }

    if (!right) {
        puts("ic_expr_faccess_init: right was null");
        return 0;
    }

    if (right->tag != ic_expr_type_identifier) {
        puts("ic_expr_faccess_init: right was not an identifier");
        return 0;
    }

    /* just attach left and right
     * this pushes all the work onto our caller
     */
    faccess->left = left;
    faccess->right = right;

    return 1;
}

/* destroy fieldaccess
 *
 * will free op if `free_faccess` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_expr_faccess_destroy(struct ic_expr_faccess *faccess, unsigned int free_faccess) {
    if (!faccess) {
        puts("ic_expr_faccess_destroy: faccess was null");
        return 0;
    }

    /* FIXME are we sure we should free left and right? */

    if (faccess->left) {
        /* free = 1 as pointer */
        if (!ic_expr_destroy(faccess->left, 1)) {
            puts("ic_expr_faccess_destroy: first : call to ic_expr_destroy failed");
            return 0;
        }
    }

    if (faccess->right) {
        /* free = 1 as pointer */
        if (!ic_expr_destroy(faccess->right, 1)) {
            puts("ic_expr_operator_destroy: second : call to ic_expr_destroy failed");
            return 0;
        }
    }

    if (free_faccess) {
        free(faccess);
    }

    return 1;
}

/* perform a deep copy of faccess
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_expr_faccess *ic_expr_faccess_deep_copy(struct ic_expr_faccess *faccess) {
    struct ic_expr_faccess *new_faccess = 0;

    if (!faccess) {
        puts("ic_expr_faccess_deep_copy: faccess was null");
        return 0;
    }

    new_faccess = calloc(1, sizeof(struct ic_expr_faccess));
    if (!new_faccess) {
        puts("ic_expr_faccess_deep_copy: call to calloc failed");
        return 0;
    }

    if (!ic_expr_faccess_deep_copy_embedded(faccess, new_faccess)) {
        puts("ic_expr_faccess_deep_copy: call to ic_expr_faccess_deep_copy_embedded failed");
        return 0;
    }

    return new_faccess;
}

/* perform a deep copy of faccess embedded within an object
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_expr_faccess_deep_copy_embedded(struct ic_expr_faccess *from, struct ic_expr_faccess *to) {
    if (!from) {
        puts("ic_expr_faccess_deep_copy_embedded: from was null");
        return 0;
    }

    if (!to) {
        puts("ic_expr_faccess_deep_copy_embedded: to was null");
        return 0;
    }

    puts("ic_expr_faccess_deep_copy_embedded: unimplemented");
    return 0;
}

/* print this fieldaccess */
void ic_expr_faccess_print(FILE *fd, struct ic_expr_faccess *faccess, unsigned int *indent_level) {
    /* fake indent for printing of right */
    unsigned int fake_indent = 0;

    if (!faccess) {
        puts("ic_expr_faccess_print: faccess was null");
        return;
    }

    ic_expr_print(fd, faccess->left, indent_level);
    fputs(".", fd);
    ic_expr_print(fd, faccess->right, &fake_indent);
}

/* allocate and initialise a new ic_expr
 * will not initialise union members
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr *ic_expr_new(enum ic_expr_tag tag) {
    struct ic_expr *expr = 0;

    /* alloc */
    expr = calloc(1, sizeof(struct ic_expr));
    if (!expr) {
        puts("ic_expr_new: calloc failed");
        return 0;
    }

    /* init */
    if (!ic_expr_init(expr, tag)) {
        puts("ic_expr_new: call to ic_expr_init failed");
        return 0;
    }

    return expr;
}

/* initialise an existing ic_expr
 * will not initialise union members
 *
 * returns 1 on success
 * returns 0 on failure
 */
int ic_expr_init(struct ic_expr *expr, enum ic_expr_tag tag) {
    if (!expr) {
        puts("ic_expr_init: expr was null");
        return 0;
    }

    /* we only initialise the type */
    expr->tag = tag;

    /* we do NOT initialise the union members */

    return 1;
}

/* takes an existing expr and converts it to a binary operator
 *
 * this will overwrite any data already here
 * make sure you have cloned or zerod the existing data first
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_expr_opify(struct ic_expr *expr, struct ic_expr *first, struct ic_expr *second, struct ic_token *token) {
    if (!expr) {
        puts("ic_expr_opify: expr was null");
        return 0;
    }

    expr->tag = ic_expr_type_operator;
    if (!ic_expr_operator_init_binary(&(expr->u.op), first, second, token)) {
        puts("ic_expr_opify: call to ic_expr_operator_init_binary failed");
        return 0;
    }

    return 1;
}

/* takes an existing expr and converts it to a faccess
 *
 * this will overwrite any data already here
 * make sure you have cloned or zerod the existing data first
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_expr_faccessify(struct ic_expr *expr, struct ic_expr *left, struct ic_expr *right) {
    if (!expr) {
        puts("ic_expr_faccessify: expr was null");
        return 0;
    }

    expr->tag = ic_expr_type_field_access;
    if (!ic_expr_faccess_init(&(expr->u.faccess), left, right)) {
        puts("ic_expr_faccessify: call to ic_expr_faccess_init failed");
        return 0;
    }

    return 1;
}

/* destroy expr
 *
 * will only free expr if `free_expr` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_expr_destroy(struct ic_expr *expr, unsigned int free_expr) {
    if (!expr) {
        puts("ic_expr_destroy: expr was null");
        return 0;
    }

    /* dispatch on type */
    switch (expr->tag) {
        case ic_expr_type_func_call:
            /* free = 0 as member */
            if (!ic_expr_func_call_destroy(&(expr->u.fcall), 0)) {
                puts("ic_expr_destroy: call to ic_expr_func_call_destroy failed");
                return 0;
            }
            break;

        case ic_expr_type_identifier:
            /* free = 0 as member */
            if (!ic_expr_identifier_destroy(&(expr->u.id), 0)) {
                puts("ic_expr_destroy: call to ic_expr_identifier_destroy failed");
                return 0;
            }
            break;

        case ic_expr_type_constant:
            /* free = 0 as member */
            if (!ic_expr_constant_destroy(&(expr->u.cons), 0)) {
                puts("ic_expr_destroy: call to ic_expr_constant_destroy failed");
                return 0;
            }
            break;

        case ic_expr_type_operator:
            /* free = 0 as member */
            if (!ic_expr_operator_destroy(&(expr->u.op), 0)) {
                puts("ic_expr_destroy: call to ic_expr_operator_destroy failed");
                return 0;
            }
            break;

        case ic_expr_type_field_access:
            /* free = 0 as member */
            if (!ic_expr_faccess_destroy(&(expr->u.faccess), 0)) {
                puts("ic_expr_destroy: call to ic_expr_faccess_destroy failed");
                return 0;
            }
            break;

        default:
            puts("ic_expr_destroy: impossible tag");
            return 0;
            break;
    }

    /* if asked */
    if (free_expr) {
        free(expr);
    }

    return 1;
}

/* perform a deep copy of an expr
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_expr *ic_expr_deep_copy(struct ic_expr *expr) {
    struct ic_expr *new_expr = 0;

    if (!expr) {
        puts("ic_expr_deep_copy: expr was null");
        return 0;
    }

    new_expr = calloc(1, sizeof(struct ic_expr));
    if (!new_expr) {
        puts("ic_expr_deep_copy: call to calloc failed");
        return 0;
    }

    if (!ic_expr_deep_copy_embedded(expr, new_expr)) {
        puts("ic_expr_deep_copy: call to ic_expr_deep_copy_embedded failed");
        return 0;
    }

    return new_expr;
}

/* perform a deep copy of an expr embedded within an object
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_expr_deep_copy_embedded(struct ic_expr *from, struct ic_expr *to) {
    to->tag = from->tag;

    /* dispatch on type */
    switch (from->tag) {
        case ic_expr_type_func_call:
            if (!ic_expr_func_call_deep_copy_embedded(&(from->u.fcall), &(to->u.fcall))) {
                puts("ic_expr_deep_copy_embedded: identifier: call to ic_expr_func-call_deep_copy_embedded failed");
                return 0;
            }
            break;

        case ic_expr_type_identifier:
            if (!ic_expr_identifier_deep_copy_embedded(&(from->u.id), &(to->u.id))) {
                puts("ic_expr_deep_copy_embedded: identifier: call to ic_expr_identifier_deep_copy_embedded failed");
                return 0;
            }
            break;

        case ic_expr_type_constant:
            if (!ic_expr_constant_deep_copy_embedded(&(from->u.cons), &(to->u.cons))) {
                puts("ic_expr_deep_copy_embedded: identifier: call to ic_expr_constant_deep_copy_embedded failed");
                return 0;
            }
            break;

        case ic_expr_type_operator:
            if (!ic_expr_operator_deep_copy_embedded(&(from->u.op), &(to->u.op))) {
                puts("ic_expr_deep_copy_embedded: identifier: call to ic_expr_operator_deep_copy_embedded failed");
                return 0;
            }
            break;

        case ic_expr_type_field_access:
            if (!ic_expr_faccess_deep_copy_embedded(&(from->u.faccess), &(to->u.faccess))) {
                puts("ic_expr_deep_copy_embedded: identifier: call to ic_expr_faccess_deep_copy_embedded failed");
                return 0;
            }
            break;

        default:
            puts("ic_expr_deep_copy_embedded: impossible tag");
            return 0;
            break;
    }

    return 1;
}

/* return pointer to fcall within,
 * will only succeed if expr is of the correct type
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_expr_func_call *ic_expr_get_fcall(struct ic_expr *expr) {
    if (!expr) {
        puts("ic_expr_get_fcall: expr was null");
        return 0;
    }

    /* check type before breaking into union */
    if (expr->tag != ic_expr_type_func_call) {
        puts("ic_expr_get_fcall: type was incorrect");
        return 0;
    }

    /* all is clear, give them what they want */
    return &(expr->u.fcall);
}

/* return pointer to identifier within,
 * will only succeed if expr is of the correct type
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_expr_identifier *ic_expr_get_identifier(struct ic_expr *expr) {
    if (!expr) {
        puts("ic_expr_get_identifier: expr was null");
        return 0;
    }

    /* check type before breaking into union */
    if (expr->tag != ic_expr_type_identifier) {
        puts("ic_expr_get_identifier: type was incorrect");
        return 0;
    }

    /* all is clear, give them what they want */
    return &(expr->u.id);
}

/* print this identifier */
void ic_expr_identifier_print(FILE *fd, struct ic_expr_identifier *identifier, unsigned int *indent_level) {
    if (!identifier) {
        puts("ic_expr_identifier_print: identifier was null");
        return;
    }
    if (!indent_level) {
        puts("ic_expr_identifier_print: indent_level was null");
        return;
    }

    /* print out indent */
    ic_parse_print_indent(fd, *indent_level);

    /* print out permission if not default */
    if (!ic_parse_perm_is_default(identifier->permissions)) {
        printf("%s", ic_parse_perm_str(identifier->permissions));
    }

    /* print our identifier */
    ic_symbol_print(fd, &(identifier->identifier));
}

/* return pointer to constant within,
 * will only succeed if expr is of the correct type
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_expr_constant *ic_expr_get_constant(struct ic_expr *expr) {
    if (!expr) {
        puts("ic_expr_get_constant: expr was null");
        return 0;
    }

    /* check type before breaking into union */
    if (expr->tag != ic_expr_type_constant) {
        puts("ic_expr_get_constant: type was incorrect");
        return 0;
    }

    /* all is clear, give them what they want */
    return &(expr->u.cons);
}

/* return pointer to operator within,
 * will only succeed if expr is of the correct type
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_expr_operator *ic_expr_get_operator(struct ic_expr *expr) {
    if (!expr) {
        puts("ic_expr_get_operator: expr was null");
        return 0;
    }

    /* check type before breaking into union */
    if (expr->tag != ic_expr_type_operator) {
        puts("ic_expr_get_operator: type was incorrect");
        return 0;
    }

    /* all is clear, give them what they want */
    return &(expr->u.op);
}

/* return pointer to fieldaccess within,
 * will only succeed if expr is of the correct type
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_expr_faccess *ic_expr_get_faccess(struct ic_expr *expr) {
    if (!expr) {
        puts("ic_expr_get_faccess: expr was null");
        return 0;
    }

    /* check type before breaking into union */
    if (expr->tag != ic_expr_type_field_access) {
        puts("ic_expr_get_faccess: type was incorrect");
        return 0;
    }

    /* all is clear, give them what they want */
    return &(expr->u.faccess);
}

/* print this expr */
void ic_expr_print(FILE *fd, struct ic_expr *expr, unsigned int *indent_level) {
    if (!expr) {
        puts("ic_expr_print: called with null expr");
        return;
    }
    if (!indent_level) {
        puts("ic_expr_print: called with indent_level expr");
        return;
    }

    switch (expr->tag) {

        case ic_expr_type_func_call:
            ic_expr_func_call_print(fd, &(expr->u.fcall), indent_level);
            break;

        case ic_expr_type_identifier:
            ic_expr_identifier_print(fd, &(expr->u.id), indent_level);
            break;

        case ic_expr_type_constant:
            ic_expr_constant_print(fd, &(expr->u.cons), indent_level);
            break;

        case ic_expr_type_operator:
            ic_expr_operator_print(fd, &(expr->u.op), indent_level);
            break;

        case ic_expr_type_field_access:
            ic_expr_faccess_print(fd, &(expr->u.faccess), indent_level);
            break;

        default:
            puts("ic_expr_print: unsupported type");
            break;
    }
}
