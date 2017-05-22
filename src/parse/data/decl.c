#include <stdio.h>  /* puts, printf */
#include <stdlib.h> /* calloc */
#include <string.h> /* strncpy */

#include "../../data/pvector.h"
#include "../../data/string.h"
#include "../../data/symbol.h"
#include "../../transform/data/tbody.h"
#include "../parse.h"
#include "../permissions.h"
#include "decl.h"
#include "field.h"
#include "stmt.h"

/* allocate and initialise a new decl_func
 *
 * returns new decl_func on success
 * returns 0 on failure
 */
struct ic_decl_func *ic_decl_func_new(char *name, unsigned int name_len) {
    struct ic_decl_func *fdecl = 0;

    if (!name) {
        puts("ic_decl_func_new: name was null");
        return 0;
    }

    /* allocate new fdecl */
    fdecl = calloc(1, sizeof(struct ic_decl_func));
    if (!fdecl) {
        puts("ic_decl_func_new: calloc failed");
        return 0;
    }

    /* initialise */
    if (!ic_decl_func_init(fdecl, name, name_len)) {
        puts("ic_decl_func_new: call to ic_decl_func_init failed");
        free(fdecl);
        return 0;
    }

    return fdecl;
}

/* initialise an existing decl_func
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_func_init(struct ic_decl_func *fdecl, char *name, unsigned int name_len) {
    if (!fdecl) {
        puts("ic_decl_func_init: fdecl was null");
        return 0;
    }

    if (!name) {
        puts("ic_decl_func_init: name was null");
        return 0;
    }

    /* initialise name symbol */
    if (!ic_symbol_init(&(fdecl->name), name, name_len)) {
        puts("ic_decl_func_init: call to ic_symbol_init for name failed");
        return 0;
    }

    /* initialise empty type_params pvector */
    if (!ic_pvector_init(&(fdecl->type_params), 0)) {
        puts("ic_decl_func_init: call to ic_pvector_init failed");
        return 0;
    }

    /* initialise empty args pvector */
    if (!ic_pvector_init(&(fdecl->args), 0)) {
        puts("ic_decl_func_init: call to ic_pvector_init failed");
        return 0;
    }

    /* initialise empty string fdecl->sig_call */
    if (!ic_string_init_empty(&(fdecl->sig_call))) {
        puts("ic_decl_func_init: call to ic_string_init_empty for sig_call failed");
        return 0;
    }

    /* initialise empty string fdecl->sig_full */
    if (!ic_string_init_empty(&(fdecl->sig_full))) {
        puts("ic_decl_func_init: call to ic_string_init_empty for sig_full failed");
        return 0;
    }

    /* initialise empty string fdecl->sig_mangled */
    if (!ic_string_init_empty(&(fdecl->sig_mangled))) {
        puts("ic_decl_func_init: call to ic_string_init_empty for sig_mangled failed");
        return 0;
    }

    /* initialise return type to 0 (void) */
    fdecl->ret_type = 0;
    fdecl->builtin = 0;

    /* make sure to init tbody */
    fdecl->tbody = 0;

    /* initialise our empty body */
    if (!ic_body_init(&(fdecl->body))) {
        puts("ic_decl_func_init: call to ic_body_init failed");
        return 0;
    }

    return 1;
}

/* calls destroy on every element within
 *
 * this will only free the fdecl if `free_fdecl` is truthy
 *
 * the caller must determine if it is appropriate
 * or not to call free(decl)
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_func_destroy(struct ic_decl_func *fdecl, unsigned int free_fdecl) {
    int i = 0;
    int len = 0;

    struct ic_type_param *tparam = 0;
    struct ic_field *field = 0;

    if (!fdecl) {
        puts("ic_decl_func_destroy: fdecl was null");
        return 0;
    }

    /* free symbol contents but do not free symbol itself
     * since it is an element on fdecl
     */
    if (!ic_symbol_destroy(&(fdecl->name), 0)) {
        puts("ic_decl_func_destroy: for name call to ic_symbol_destroy failed");
        return 0;
    }

    /* free sig_call contents but do not free string itself
     * since it is an element on fdecl
     */
    if (!ic_string_destroy(&(fdecl->sig_call), 0)) {
        puts("ic_decl_func_destroy: for sig_call call to ic_string_destroy failed");
        return 0;
    }

    /* free sig_full contents but do not free string itself
     * since it is an element on fdecl
     */
    if (!ic_string_destroy(&(fdecl->sig_full), 0)) {
        puts("ic_decl_func_destroy: for sig_full call to ic_string_destroy failed");
        return 0;
    }

    /* free sig_mangled contents but do not free string itself
     * since it is an element on fdecl
     */
    if (!ic_string_destroy(&(fdecl->sig_mangled), 0)) {
        puts("ic_decl_func_destroy: for sig_full call to ic_string_mangled failed");
        return 0;
    }

    len = ic_pvector_length(&(fdecl->type_params));
    for (i = 0; i < len; ++i) {
        tparam = ic_pvector_get(&(fdecl->type_params), i);
        if (!tparam) {
            puts("ic_decl_func_destroy: call to ic_pvector_get failed");
            return 0;
        }

        /* dispatch to type_param destroy
         * free_tparam set to 1
         */
        if (!ic_type_param_destroy(tparam, 1)) {
            puts("ic_decl_func_destroy: call to ii_type_param_destroy failed");
            return 0;
        }
    }

    /* destroy args pvector itself
     * do not free as member
     * do not clean up elems as done above
     */
    if (!ic_pvector_destroy(&(fdecl->type_params), 0, 0)) {
        puts("ic_decl_func_destroy: call to ic_pvector_destroy failed");
        return 0;
    }

    len = ic_pvector_length(&(fdecl->args));
    for (i = 0; i < len; ++i) {
        field = ic_pvector_get(&(fdecl->args), i);
        if (!field) {
            puts("ic_decl_func_destroy: call to ic_pvector_get failed");
            return 0;
        }

        /* dispatch to field destroy
         * free_field set to 1
         */
        if (!ic_field_destroy(field, 1)) {
            puts("ic_decl_func_destroy: call to ic_field_destroy failed");
            return 0;
        }
    }

    /* destroy args pvector itself
     * do not free as member
     * do not clean up elems as done above
     */
    if (!ic_pvector_destroy(&(fdecl->args), 0, 0)) {
        puts("ic_decl_func_destroy: call to ic_pvector_destroy failed");
        return 0;
    }

    /* only need to free if we have a ret_type */
    if (fdecl->ret_type) {
        /* free symbol contents and free symbol
         * as our ret type is always allocated
         *
         *    // ic_decl_func_set_return :
         *    fdecl->ret_type = ic_symbol_new(type, type_len);
         */
        if (!ic_symbol_destroy(fdecl->ret_type, 1)) {
            puts("ic_decl_func_destroy: for ret_type call to ic_symbol_destroy failed");
            return 0;
        }
    }

    /* free body contents but do not free body itself
     * since it is an element on fdecl
     */
    if (!ic_body_destroy(&(fdecl->body), 0)) {
        puts("ic_decl_func_destroy: for body call to ic_body_destroy failed");
        return 0;
    }

    if (fdecl->tbody) {
        /* call destroy on transform_body
       * free as it is a pointer member
       */
        if (!ic_transform_body_destroy(fdecl->tbody, 1)) {
            puts("ic_decl_func_destroy: for tbody call to ic_transform_body_destroy failed");
            return 0;
        }
    }

    /* only free if caller asked */
    if (free_fdecl) {
        free(fdecl);
    }

    /* success */
    return 1;
}

/* add new type_param to decl_func
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_func_type_params_add(struct ic_decl_func *fdecl, struct ic_type_param *tparam) {
    if (!fdecl) {
        puts("ic_decl_func_type_params_add: fdecl was null");
        return 0;
    }

    if (!tparam) {
        puts("ic_decl_func_type_params_add: tparam was null");
        return 0;
    }

    /* append field returns -1 on failure */
    if (-1 == ic_pvector_append(&(fdecl->type_params), tparam)) {
        puts("ic_decl_func_type_params_add: call to ic_pvector_append failed");
        return 0;
    }

    return 1;
}

/* get length of type_params
 *
 * returns len on success
 * returns 0 on failure
 */
unsigned int ic_decl_func_type_params_length(struct ic_decl_func *fdecl) {
    unsigned int len = 0;
    if (!fdecl) {
        puts("ic_decl_func_type_params_length: fdecl was null");
        return 0;
    }

    len = ic_pvector_length(&(fdecl->type_params));
    return len;
}

/* get type_param at i
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_type_param *ic_decl_func_type_params_get(struct ic_decl_func *fdecl, unsigned int i) {
    struct ic_type_param *tparam = 0;

    if (!fdecl) {
        puts("ic_decl_func_type_params_get: fdecl null");
        return 0;
    }

    tparam = ic_pvector_get(&(fdecl->type_params), i);
    if (!tparam) {
        puts("ic_decl_func_type_params_get: call to ic_pvector_get failed");
        return 0;
    }

    return tparam;
}

/* add new arg field to decl_func
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_func_args_add(struct ic_decl_func *fdecl, struct ic_field *field) {
    if (!fdecl) {
        puts("ic_decl_func_args_add: fdecl was null");
        return 0;
    }

    if (!field) {
        puts("ic_decl_func_args_add: field was null");
        return 0;
    }

    /* append field returns -1 on failure */
    if (-1 == ic_pvector_append(&(fdecl->args), field)) {
        puts("ic_decl_func_args_add: call to ic_pvector_append failed");
        return 0;
    }

    return 1;
}

/* get length of args
 *
 * returns len on success
 * returns 0 on failure
 */
unsigned int ic_decl_func_args_length(struct ic_decl_func *fdecl) {
    unsigned int len = 0;
    if (!fdecl) {
        puts("ic_decl_func_args_length: fdecl was null");
        return 0;
    }

    len = ic_pvector_length(&(fdecl->args));
    return len;
}

/* get arg at i
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_field *ic_decl_func_args_get(struct ic_decl_func *fdecl, unsigned int i) {
    struct ic_field *field = 0;
    if (!fdecl) {
        puts("ic_decl_func_args_get: fdecl was null");
        return 0;
    }

    field = ic_pvector_get(&(fdecl->args), i);
    if (!field) {
        puts("ic_decl_func_args_get: call to ic_pvector_get failed");
        return 0;
    }

    return field;
}

/* set return type
 *
 * this function will fail if the return type is already set
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_func_set_return(struct ic_decl_func *fdecl, char *type, unsigned int type_len) {
    if (!fdecl) {
        puts("ic_decl_func_set_return: fdecl was null");
        return 0;
    }
    if (!type) {
        puts("ic_decl_func_set_return: type was null");
        return 0;
    }
    if (!type_len) {
        puts("ic_decl_func_set_return: type_len was 0");
        return 0;
    }

    /* check we haven't already set */
    if (fdecl->ret_type) {
        /* it is an error to re-set the return type */
        puts("ic_decl_func_set_return: attempt to re-set return type");
        return 0;
    }

    /* create our return type */
    fdecl->ret_type = ic_symbol_new(type, type_len);
    if (!fdecl->ret_type) {
        puts("ic_decl_func_set_return: call to ic_symbol_new failed");
        return 0;
    }

    /* success */
    return 1;
}

/* add new stmt to the body
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_func_add_stmt(struct ic_decl_func *fdecl, struct ic_stmt *stmt) {
    if (!fdecl) {
        puts("ic_decl_func_add_stmt: fdecl was null");
        return 0;
    }
    if (!stmt) {
        puts("ic_decl_func_add_stmt: stmt was null");
        return 0;
    }

    /* add our element */
    if (-1 == ic_body_append(&(fdecl->body), stmt)) {
        puts("ic_decl_func_add_stmt: call to ic_body_append failed");
        return 0;
    }

    return 1;
}

/* mark this fdecl as being a builtin
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_func_mark_builtin(struct ic_decl_func *fdecl) {
    if (!fdecl) {
        puts("ic_decl_func_mark_builtin: fdecl was null");
        return 0;
    }

    /* to mark fdecl itself */
    fdecl->builtin = 1;

    return 1;
}

/* test if builtin
 *
 * returns 1 if builtin
 * returns 0 otherwise
 */
unsigned int ic_decl_func_isbuiltin(struct ic_decl_func *fdecl) {
    if (!fdecl) {
        puts("ic_decl_func_isbuiltin: fdecl was null");
        return 0;
    }

    return fdecl->builtin == 1;
}

/* print decl_func
 * this will print a reproduction of the function from the ast
 */
void ic_decl_func_print(FILE *fd, struct ic_decl_func *fdecl, unsigned int *indent_level) {
    char *fstr = 0;

    if (!fdecl) {
        puts("ic_decl_func_print: fdecl was null");
        return;
    }

    if (!indent_level) {
        puts("ic_decl_func_print: indent_level was null");
        return;
    }

    fstr = ic_decl_func_sig_call(fdecl);
    /* guarantee generation of function string */
    if (!fstr) {
        puts("ic_decl_func_print_header: call to ic_decl_func_sig_call failed");
        return;
    }

    /* print comment and then function decl string */
    fprintf(fd, "# %s\n", fstr);

    ic_decl_func_print_header(fd, fdecl, indent_level);
    ic_decl_func_print_body(fd, fdecl, indent_level);
}

/* print decl_func header
 * this will print a reproduction of the function header from the ast
 */
void ic_decl_func_print_header(FILE *fd, struct ic_decl_func *fdecl, unsigned int *indent_level) {
    /* offset into args */
    unsigned int i = 0;
    /* len of args */
    unsigned int len = 0;

    if (!fdecl) {
        puts("ic_decl_func_print_header: fdecl was null");
        return;
    }

    if (!indent_level) {
        puts("ic_decl_func_print_header: indent_level was null");
        return;
    }

    /* print `function`, name, and opening bracket */
    fprintf(fd, "fn %s", ic_symbol_contents(&(fdecl->name)));

    /* print type params, if they exist */
    len = ic_pvector_length(&(fdecl->type_params));
    if (len > 0) {
        fputs("[", fd);
        for (i = 0; i < len; ++i) {
            /* add comma and space between type_params */
            if (i > 0) {
                fputs(", ", fd);
            }

            ic_type_param_print(fd, ic_pvector_get(&(fdecl->type_params), i));
        }
        fputs("]", fd);
    }

    fputs("(", fd);
    len = ic_pvector_length(&(fdecl->args));

    /* print arguments */
    for (i = 0; i < len; ++i) {
        /* print a comma and space between each arg */
        if (i > 0) {
            fputs(", ", fd);
        }

        ic_field_print(fd, ic_pvector_get(&(fdecl->args), i));
    }

    /* closing bracket and return type arrow */
    fputs(") -> ", fd);

    /* print return type if we have one */
    if (fdecl->ret_type) {
        ic_symbol_print(fd, fdecl->ret_type);
        /* trailing \n */
        fputs("\n", fd);
    } else {
        /* otherwise print Void */
        fputs("Void\n", fd);
    }
}

/* print decl_func body
 * this will print a reproduction of the function body from the ast
 */
void ic_decl_func_print_body(FILE *fd, struct ic_decl_func *fdecl, unsigned int *indent_level) {
    if (!fdecl) {
        puts("ic_decl_func_print_body: fdecl was null");
        return;
    }

    if (!indent_level) {
        puts("ic_decl_func_print_body: indent_level was null");
        return;
    }

    /* print body
     * body will handle indent_level incr and decr for us
     */
    ic_body_print(fd, &(fdecl->body), indent_level);

    /* print end\n */
    fputs("end\n", fd);
}

/* return a string representation of this function signature
 *
 * for a function signature
 *      fn foo(a::Sint, b::Sint) -> Sint
 *
 * this function will return
 *      foo(Sint,Sint)
 *
 * for a function signature
 *      fn bar[A,B](a::A, b::B) -> A
 *
 * this function will return
 *      bar[A,B](A,B)
 *
 * the char* returned is a string stored within fdecl,
 * this means the caller must not free or mutate this string
 *
 * returns char* on success
 * returns 0 on failure
 */
char *ic_decl_func_sig_call(struct ic_decl_func *fdecl) {
    /* offset into args pvector */
    unsigned int i = 0;
    /* cached len */
    unsigned int len = 0;
    /* cache of string */
    struct ic_string *fstr = 0;
    /* each field we consider in args */
    struct ic_field *field = 0;
    /* each type param we consider in type_params */
    struct ic_type_param *tparam = 0;
    /* temporary symbol for current field type */
    struct ic_symbol *cur_type = 0;
    /* permission str */
    char *perm_str = 0;

    if (!fdecl) {
        puts("ic_decl_func_sig_call: fdecl was null");
        return 0;
    }

    /* cache string pointer */
    fstr = &(fdecl->sig_call);

    /* if a non-zero length fecl->string is found then return it */
    if (ic_string_length(fstr)) {
        return ic_string_contents(fstr);
    }

    /* fdecl->name */
    if (!ic_string_append_symbol(fstr, &(fdecl->name))) {
        puts("ic_decl_func_sig_call: name: call to ic_string_append_symbol failed");
        return 0;
    }

    /* check if we have any type params to do... */
    len = ic_decl_func_type_params_length(fdecl);
    /* iterate through, only print [] if type_params was non zero */
    if (len > 0) {
        /* opening [ bracket */
        if (!ic_string_append_char(fstr, "[", 1)) {
            puts("ic_decl_func_sig_call: opening brace: call to ic_string_append_char failed");
            return 0;
        }

        for (i = 0; i < len; ++i) {
            /* insert a comma if we are not the first argument */
            if (i > 0) {
                if (!ic_string_append_char(fstr, ",", 1)) {
                    puts("ic_decl_func_sig_call: arg: call to ic_string_append_char failed");
                    return 0;
                }
            }

            tparam = ic_decl_func_type_params_get(fdecl, i);
            if (!tparam) {
                puts("ic_decl_func_sig_call: call to ic_decl_func_type_params_get failed");
                return 0;
            }

            if (!ic_string_append_symbol(fstr, &(tparam->name))) {
                puts("ic_decl_func_sig_call: arg: call to ic_string_append_symbol failed");
                return 0;
            }
        }

        /* closing ] bracket */
        if (!ic_string_append_char(fstr, "]", 1)) {
            puts("ic_decl_func_sig_call: opening brace: call to ic_string_append_char failed");
            return 0;
        }
    }

    /* opening bracket */
    if (!ic_string_append_char(fstr, "(", 1)) {
        puts("ic_decl_func_sig_call: opening brace: call to ic_string_append_char failed");
        return 0;
    }

    /* note that we do not check for length as a length of 0 is valid */
    len = ic_pvector_length(&(fdecl->args));

    /* iterate through args appending the type name to our string representation
     */
    for (i = 0; i < len; ++i) {
        /* insert a comma if we are not the first argument */
        if (i > 0) {
            if (!ic_string_append_char(fstr, ",", 1)) {
                puts("ic_decl_func_sig_call: arg: call to ic_string_append_char failed");
                return 0;
            }
        }

        /* capture our field */
        field = ic_pvector_get(&(fdecl->args), i);
        if (!field) {
            puts("ic_decl_func_sig_call: arg: call to ic_pvector_get failed");
            return 0;
        }

        /* add any permissions
         * only need to show if not default
         */
        if (!ic_parse_perm_is_default(field->permissions)) {
            perm_str = ic_parse_perm_str(field->permissions);

            if (!perm_str) {
                puts("ic_decl_func_sig_call: arg: call to ic_parse_perm_str failed");
                return 0;
            }

            if (!ic_string_append_cstr(fstr, perm_str)) {
                puts("ic_decl_func_sig_call: arg: call to ic_string_append_cstr failed");
                return 0;
            }
        }

        /* add type */
        cur_type = ic_type_ref_get_symbol(&(field->type));
        if (!cur_type) {
            puts("ic_decl_func_sig_call: arg: call to ic_type_get_symbol failed");
            return 0;
        }

        if (!ic_string_append_symbol(fstr, cur_type)) {
            puts("ic_decl_func_sig_call: arg: call to ic_string_append_symbol failed");
            return 0;
        }
    }

    /* final bracket */
    if (!ic_string_append_char(fstr, ")", 1)) {
        puts("ic_decl_func_sig_call: closing brace: call to ic_string_append_char failed");
        return 0;
    }

    /* we rely on the fdecl storing the string */
    return ic_string_contents(fstr);
}

/* return a full string representation of this function signature
 *
 * for a function signature
 *      fn foo(a::Sint, b::Sint) -> Sint
 *
 * this function will return
 *      foo(Sint,Sint) -> Sint
 *
 * the char* returned is a string stored within fdecl,
 * this means the caller must not free or mutate this string
 *
 * returns char* on success
 * returns 0 on failure
 */
char *ic_decl_func_sig_full(struct ic_decl_func *fdecl) {
    /* offset into args pvector */
    unsigned int i = 0;
    /* cached len */
    unsigned int len = 0;
    /* cache of string */
    struct ic_string *fstr = 0;
    /* each field we consider in args */
    struct ic_field *field = 0;
    /* temporary symbol for current field type */
    struct ic_symbol *cur_type = 0;
    /* permission str */
    char *perm_str = 0;

    if (!fdecl) {
        puts("ic_decl_func_sig_full: fdecl was null");
        return 0;
    }

    /* cache string pointer */
    fstr = &(fdecl->sig_full);

    /* if a non-zero length fecl->string is found then return it */
    if (ic_string_length(fstr)) {
        return ic_string_contents(fstr);
    }

    /* note that we do not check for length as a length of 0 is valid */
    len = ic_pvector_length(&(fdecl->args));

    /* fdecl->name */
    if (!ic_string_append_symbol(fstr, &(fdecl->name))) {
        puts("ic_decl_func_sig_full: name: call to ic_string_append_symbol failed");
        return 0;
    }

    /* opening bracket */
    if (!ic_string_append_char(fstr, "(", 1)) {
        puts("ic_decl_func_sig_full: opening brace: call to ic_string_append_char failed");
        return 0;
    }

    /* iterate through args appending the type name to our string representation
     */
    for (i = 0; i < len; ++i) {
        /* insert a comma if we are not the first argument */
        if (i > 0) {
            if (!ic_string_append_char(fstr, ",", 1)) {
                puts("ic_decl_func_sig_full: arg: call to ic_string_append_char failed");
                return 0;
            }
        }

        /* capture our field */
        field = ic_pvector_get(&(fdecl->args), i);
        if (!field) {
            puts("ic_decl_func_sig_full: arg: call to ic_pvector_get failed");
            return 0;
        }

        /* add any permissions
         * only need to show if not default
         */
        if (!ic_parse_perm_is_default(field->permissions)) {
            perm_str = ic_parse_perm_str(field->permissions);

            if (!perm_str) {
                puts("ic_decl_func_sig_full: arg: call to ic_parse_perm_str failed");
                return 0;
            }

            if (!ic_string_append_cstr(fstr, perm_str)) {
                puts("ic_decl_func_sig_full: arg: call to ic_string_append_cstr failed");
                return 0;
            }
        }

        /* add type */
        cur_type = ic_type_ref_get_symbol(&(field->type));
        if (!cur_type) {
            puts("ic_decl_func_sig_full: arg: call to ic_type_get_symbol failed");
            return 0;
        }

        if (!ic_string_append_symbol(fstr, cur_type)) {
            puts("ic_decl_func_sig_full: arg: call to ic_string_append_symbol failed");
            return 0;
        }
    }

    /* final bracket and return arrow */
    if (!ic_string_append_char(fstr, ") -> ", 5)) {
        puts("ic_decl_func_sig_full: closing brace and return arrow: call to ic_string_append_char failed");
        return 0;
    }

    /* print return type if we have one */
    if (fdecl->ret_type) {
        if (!ic_string_append_symbol(fstr, fdecl->ret_type)) {
            puts("ic_decl_func_sig_full: return type (nonvoid): call to ic_string_append_symbol failed");
            return 0;
        }
    } else {
        /* otherwise print Void */
        if (!ic_string_append_char(fstr, "Void", 4)) {
            puts("ic_decl_func_sig_full: return type (void): call to ic_string_append_char failed");
            return 0;
        }
    }

    /* we rely on the fdecl storing the string */
    return ic_string_contents(fstr);
}

/* return a mangled string representation of this function signature
 *
 * for a function signature
 *      fn foo(a::Sint, b::Sint) -> Sint
 *
 * this function will return
 *      i_foo_a_Sint_Sint
 *
 * the char* returned is a string stored within fdecl,
 * this means the caller must not free or mutate this string
 *
 * returns char* on success
 * returns 0 on failure
 */
char *ic_decl_func_sig_mangled(struct ic_decl_func *fdecl) {
    /* offset into args pvector */
    unsigned int i = 0;
    /* cached len */
    unsigned int len = 0;
    /* cache of string */
    struct ic_string *fstr = 0;
    /* each field we consider in args */
    struct ic_field *field = 0;
    /* temporary symbol for current field type */
    struct ic_symbol *cur_type = 0;

    if (!fdecl) {
        puts("ic_decl_func_sig_mangled: fdecl was null");
        return 0;
    }

    /* cache string pointer */
    fstr = &(fdecl->sig_mangled);

    /* if a non-zero length fecl->string is found then return it */
    if (ic_string_length(fstr)) {
        return ic_string_contents(fstr);
    }

    /* note that we do not check for length as a length of 0 is valid */
    len = ic_pvector_length(&(fdecl->args));

    /* `i_` prefix */
    if (!ic_string_append_char(fstr, "i_", 2)) {
        puts("ic_decl_func_sig_mangled: i_ prefix: call to ic_string_append_char failed");
        return 0;
    }

    /* fdecl->name */
    if (!ic_string_append_symbol(fstr, &(fdecl->name))) {
        puts("ic_decl_func_sig_mangled: name: call to ic_string_append_symbol failed");
        return 0;
    }

    /* `_a` */
    if (!ic_string_append_char(fstr, "_a", 2)) {
        puts("ic_decl_func_sig_mangled: _a suffix: call to ic_string_append_char failed");
        return 0;
    }

    /* iterate through args appending the type name to our string representation
     */
    for (i = 0; i < len; ++i) {
        /* insert an `_` before each type */
        if (!ic_string_append_char(fstr, "_", 1)) {
            puts("ic_decl_func_sig_mangled: arg: call to ic_string_append_char failed");
            return 0;
        }

        /* capture our field */
        field = ic_pvector_get(&(fdecl->args), i);
        if (!field) {
            puts("ic_decl_func_sig_mangled: arg: call to ic_pvector_get failed");
            return 0;
        }

        /* FIXME TODO name mangling will have to include permission info
         * see ic_decl_func_sig_full
         */

        /* get argument type */
        cur_type = ic_type_ref_get_symbol(&(field->type));
        if (!cur_type) {
            puts("ic_decl_func_sig_mangled: arg: call to ic_type_get_symbol failed");
            return 0;
        }

        /* append argument type */
        if (!ic_string_append_symbol(fstr, cur_type)) {
            puts("ic_decl_func_sig_mangled: arg: call to ic_string_append_symbol failed");
            return 0;
        }
    }

    /* we rely on the fdecl storing the string */
    return ic_string_contents(fstr);
}

/* check if this function returns void
 *
 * returns boolean on success
 * returns 0 on failure
 */
unsigned int ic_decl_func_is_void(struct ic_decl_func *fdecl) {
    struct ic_symbol *ret = 0;
    char *ret_ch = 0;

    if (!fdecl) {
        puts("ic_decl_func_is_void: fdecl was null");
        return 0;
    }

    /* ret_type is 0 for void */
    if (fdecl->ret_type == 0) {
        return 1;
    }

    /* also capturing an explicit "void" type */
    ret = fdecl->ret_type;

    ret_ch = ic_symbol_contents(ret);
    if (!ret_ch) {
        puts("ic_decl_func_is_void: call to ic_symbol_contents failed");
        return 0;
    }

    if (!strncmp("Void", ret_ch, 5)) {
        return 1;
    }

    return 0;
}

/* allocate and return a new decl_type
 * only needs name and len
 * will also allocate an empty pvector for fields
 *
 * returns new ic_field * on success
 * returns 0 on failure
 */
struct ic_decl_type_struct *ic_decl_type_struct_new(char *name_src, unsigned int name_len) {
    struct ic_decl_type_struct *tdecl = 0;

    /* allocate decl_type */
    tdecl = calloc(1, sizeof(struct ic_decl_type_struct));
    if (!tdecl) {
        puts("ic_decl_type_struct_new: calloc failed");
        return 0;
    }

    if (!ic_decl_type_struct_init(tdecl, name_src, name_len)) {
        puts("ic_decl_type_struct_new: call to ic_decl_type_struct_init failed");
        free(tdecl);
        return 0;
    }

    return tdecl;
}

/* initialise an existing decl_type
 * only needs name and len
 * will also allocate an empty pvector for fields
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_struct_init(struct ic_decl_type_struct *tdecl, char *name_src, unsigned int name_len) {
    if (!tdecl) {
        puts("ic_decl_type_struct_init: tdecl was null");
        return 0;
    }

    if (!name_src) {
        puts("ic_decl_type_struct_init: name_src was null");
        return 0;
    }

    if (!name_len) {
        puts("ic_decl_type_struct_init: name_len was zero");
        return 0;
    }

    /* initialise name */
    if (!ic_symbol_init(&(tdecl->name), name_src, name_len)) {
        puts("ic_decl_type_struct_init: call to ic_symbol_init for name failed");
        return 0;
    }

    /* init fields pvector type_params */
    if (!ic_pvector_init(&(tdecl->type_params), 0)) {
        puts("ic_decl_type_struct_init: call to ic_pvector_init for type_params failed");
        return 0;
    }

    /* init fields pvector fields */
    if (!ic_pvector_init(&(tdecl->fields), 0)) {
        puts("ic_decl_type_struct_init: call to ic_pvector_init for fields failed");
        return 0;
    }

    /* init field dict */
    if (!ic_dict_init(&(tdecl->field_dict))) {
        puts("ic_decl_type_struct_init: call to ic_dict_init for field_dict failed");
        return 0;
    }

    /* init sig mangled full */
    if (!ic_string_init_empty(&(tdecl->sig_mangled_full))) {
        puts("ic_decl_type_struct_init: call to ic_string_init_empty for field_dict failed");
        return 0;
    }

    tdecl->isvoid = 0;
    tdecl->isbool = 0;
    tdecl->isstring = 0;
    tdecl->isuint = 0;
    tdecl->issint = 0;

    return 1;
}

/* calls destroy on every element within
 *
 * this will only free the tdecl if `free_tdecl` is truthy
 *
 * the caller must determine if it is appropriate
 * or not to call free(decl)
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_struct_destroy(struct ic_decl_type_struct *tdecl, unsigned int free_tdecl) {
    unsigned int i = 0;
    unsigned int len = 0;
    struct ic_type_param *tparam = 0;
    struct ic_field *field = 0;

    if (!tdecl) {
        puts("ic_decl_type_struct_destroy: tdecl was null");
        return 0;
    }

    /* free symbol contents but do not free symbol itself
     * since it is an element on tdecl
     */
    if (!ic_symbol_destroy(&(tdecl->name), 0)) {
        puts("ic_decl_type_struct_destroy: call to ic_symbol_destroy failed");
        return 0;
    }

    len = ic_decl_type_struct_type_params_length(tdecl);

    /* loop through each item destroying */
    for (i = 0; i < len; ++i) {
        tparam = ic_decl_type_struct_type_params_get(tdecl, i);
        if (!tparam) {
            puts("ic_decl_type_struct_destroy: call to ic_decl_type_struct_type_params_get failed");
            return 0;
        }

        /* dispatch to field destroy
         * free_field set to 1
         */
        if (!ic_type_param_destroy(tparam, 1)) {
            puts("ic_decl_type_struct_destroy: call to ic_type_param_destroy failed");
            return 0;
        }
    }

    if (!ic_pvector_destroy(&(tdecl->type_params), 0, 0)) {
        puts("ic_decl_type_struct_destroy: call to ic_pvector_destroy failed");
        return 0;
    }

    len = ic_decl_type_struct_field_length(tdecl);

    /* loop through each item destroying */
    for (i = 0; i < len; ++i) {
        field = ic_decl_type_struct_field_get(tdecl, i);
        if (!field) {
            puts("ic_decl_type_struct_destroy: call to ic_decl_type_struct_field_get failed");
            return 0;
        }

        /* dispatch to field destroy
         * free_field set to 1
         */
        if (!ic_field_destroy(field, 1)) {
            puts("ic_decl_type_struct_destroy: call to ic_field_destroy failed");
            return 0;
        }
    }

    if (!ic_pvector_destroy(&(tdecl->fields), 0, 0)) {
        puts("ic_decl_type_struct_destroy: call to ic_pvector_destroy failed");
        return 0;
    }

    /* destroy field_dict */
    if (!ic_dict_destroy(&(tdecl->field_dict), 0, 0)) {
        puts("ic_decl_type_struct_destroy: call to ic_dict_destroy failed");
        return 0;
    }

    /* destroy sig mangled full */
    if (!ic_string_destroy(&(tdecl->sig_mangled_full), 0)) {
        puts("ic_decl_type_struct_destroy: call to ic_string_destroy failed");
        return 0;
    }

    /* only free if caller asked */
    if (free_tdecl) {
        free(tdecl);
    }

    /* success */
    return 1;
}

/* get symbol of name for this type
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_symbol *ic_decl_type_struct_get_name(struct ic_decl_type_struct *tdecl) {
    if (!tdecl) {
        puts("ic_decl_type_struct_get_name: tdecl was null");
        return 0;
    }

    return &(tdecl->name);
}

/* mark supplied tdecl as being the void type
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_struct_mark_void(struct ic_decl_type_struct *tdecl) {
    if (!tdecl) {
        puts("ic_decl_type_struct_mark_void: tdecl was null");
        return 0;
    }

    tdecl->isvoid = 1;

    return 1;
}

/* mark supplied tdecl as being the bool type
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_struct_mark_bool(struct ic_decl_type_struct *tdecl) {
    if (!tdecl) {
        puts("ic_decl_type_struct_mark_bool: tdecl was null");
        return 0;
    }

    tdecl->isbool = 1;

    return 1;
}

/* mark supplied tdecl as being the string type
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_struct_mark_string(struct ic_decl_type_struct *tdecl) {
    if (!tdecl) {
        puts("ic_decl_type_struct_mark_string: tdecl was null");
        return 0;
    }

    tdecl->isstring = 1;

    return 1;
}

/* mark supplied tdecl as being the uint type
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_struct_mark_uint(struct ic_decl_type_struct *tdecl) {
    if (!tdecl) {
        puts("ic_decl_type_struct_mark_uint: tdecl was null");
        return 0;
    }

    tdecl->isuint = 1;

    return 1;
}

/* mark supplied tdecl as being the sint type
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_struct_mark_sint(struct ic_decl_type_struct *tdecl) {
    if (!tdecl) {
        puts("ic_decl_type_struct_mark_sint: tdecl was null");
        return 0;
    }

    tdecl->issint = 1;

    return 1;
}

/* add a new field to types list of fields
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_struct_add_field(struct ic_decl_type_struct *tdecl, struct ic_field *field) {
    if (!tdecl) {
        puts("ic_decl_type_struct_add_field: tdecl was null");
        return 0;
    }

    if (!field) {
        puts("ic_decl_type_struct_add_field: field was null");
        return 0;
    }

    if (-1 == ic_pvector_append(&(tdecl->fields), field)) {
        puts("ic_decl_type_struct_add_field: call to ic_pvector_append failed");
        return 0;
    }

    return 1;
}

/* test if bool
 *
 * returns 1 if bool
 * returns 0 otherwise
 */
unsigned int ic_decl_type_struct_isbool(struct ic_decl_type_struct *tdecl) {
    if (!tdecl) {
        puts("ic_decl_type_struct_isbool: tdecl was null");
        return 0;
    }

    return tdecl->isbool == 1;
}

/* test if string
 *
 * returns 1 if string
 * returns 0 otherwise
 */
unsigned int ic_decl_type_struct_isstring(struct ic_decl_type_struct *tdecl) {
    if (!tdecl) {
        puts("ic_decl_type_struct_isstring: tdecl was null");
        return 0;
    }

    return tdecl->isstring == 1;
}

/* test if uint
 *
 * returns 1 if uint
 * returns 0 otherwise
 */
unsigned int ic_decl_type_struct_isuint(struct ic_decl_type_struct *tdecl) {
    if (!tdecl) {
        puts("ic_decl_type_struct_isuint: tdecl was null");
        return 0;
    }

    return tdecl->isuint == 1;
}

/* test if sint
 *
 * returns 1 if sint
 * returns 0 otherwise
 */
unsigned int ic_decl_type_struct_issint(struct ic_decl_type_struct *tdecl) {
    if (!tdecl) {
        puts("ic_decl_type_struct_issint: tdecl was null");
        return 0;
    }

    return tdecl->issint == 1;
}

/* test if void
 *
 * returns 1 if void
 * returns 0 otherwise
 */
unsigned int ic_decl_type_struct_isvoid(struct ic_decl_type_struct *tdecl) {
    if (!tdecl) {
        puts("ic_decl_type_struct_isvoid: tdecl was null");
        return 0;
    }

    return tdecl->isvoid == 1;
}

/* print the decl_type to stdout */
void ic_decl_type_struct_print(FILE *fd, struct ic_decl_type_struct *tdecl, unsigned int *indent_level) {
    if (!tdecl) {
        puts("ic_decl_type_struct_print: tdecl was null");
        return;
    }
    if (!indent_level) {
        puts("ic_decl_type_struct_print: indent_level was null");
        return;
    }

    ic_decl_type_struct_print_header(fd, tdecl, indent_level);
    ic_decl_type_struct_print_body(fd, tdecl, indent_level);
}

void ic_decl_type_struct_print_header(FILE *fd, struct ic_decl_type_struct *tdecl, unsigned int *indent_level) {
    unsigned int i = 0;
    unsigned int len = 0;

    if (!tdecl) {
        puts("ic_decl_type_struct_print_header: tdecl was null");
        return;
    }
    if (!indent_level) {
        puts("ic_decl_type_struct_print_header: indent_level was null");
        return;
    }

    /* print type and name */
    fprintf(fd, "type %s", ic_symbol_contents(&(tdecl->name)));

    /* print type params, if they exist */
    len = ic_pvector_length(&(tdecl->type_params));
    if (len > 0) {
        fputs("[", fd);
        for (i = 0; i < len; ++i) {
            /* add comma and space between type_params */
            if (i > 0) {
                fputs(", ", fd);
            }

            ic_type_param_print(fd, ic_pvector_get(&(tdecl->type_params), i));
        }
        fputs("]", fd);
    }

    fputs("\n", fd);
}

void ic_decl_type_struct_print_body(FILE *fd, struct ic_decl_type_struct *tdecl, unsigned int *indent_level) {
    unsigned int i = 0;

    if (!tdecl) {
        puts("ic_decl_type_struct_print_body: tdecl was null");
        return;
    }
    if (!indent_level) {
        puts("ic_decl_type_struct_print_body: indent_level was null");
        return;
    }

    /* increment indent level before body */
    ++*indent_level;

    /* iterate through pvector fields
     * prefix each field with a 2 spaces  and postfix each with a \n
     */
    for (i = 0; i < ic_pvector_length(&(tdecl->fields)); ++i) {
        /* print indent */
        ic_parse_print_indent(fd, *indent_level);

        /* print field contents */
        ic_field_print(fd, ic_pvector_get(&(tdecl->fields), i));

        /* postfix newline */
        fputs("\n", fd);
    }

    /* decrement indent level after body */
    --*indent_level;

    fputs("end\n", fd);
}

/* print debug information for this type */
void ic_decl_type_struct_print_debug(FILE *fd, struct ic_decl_type_struct *tdecl) {
    unsigned int fake_indent = 0;

    if (!tdecl) {
        puts("ic_decl_type_struct_print_debug: tdecl was null");
        return;
    }

    fputs("found: ", fd);
    ic_decl_type_struct_print_header(fd, tdecl, &fake_indent);
}

/* return the symbol name of this types
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_symbol *ic_decl_type_struct_name(struct ic_decl_type_struct *tdecl) {
    if (!tdecl) {
        puts("ic_decl_type_struct_name: tdecl was null");
        return 0;
    }

    return &(tdecl->name);
}

/* get the char * contents of the name
 *
 * returns char * on success
 * returns 0 on failure
 */
char *ic_decl_type_struct_str(struct ic_decl_type_struct *tdecl) {
    if (!tdecl) {
        puts("ic_decl_type_str: tdecl was null");
        return 0;
    }

    return ic_symbol_contents(&(tdecl->name));
}

/* return a mangled representation of this function full signature
 *
 * the char* returned is a string stored within tdecl,
 * this means the caller must not free or mutate this string
 *
 * for the type
 *  type Foo
 *    i::Sint
 *    u::Uint
 *    b::Bar
 *  end
 *
 * this should generate the function
 *
 *  struct Foo * ic_Foo_a_Sint_Uint_Bar(Sint *i, Uint *u, Bar *b);
 *
 * returns char* on success
 * returns 0 on failure
 */
char *ic_decl_type_struct_sig_mangled_full(struct ic_decl_type_struct *tdecl) {
    struct ic_string *sig_str = 0;

    unsigned int n_fields = 0;
    unsigned int i_field = 0;
    struct ic_field *field = 0;
    struct ic_symbol *field_name_sym = 0;
    struct ic_symbol *field_type_sym = 0;

    struct ic_string *generated_fargs;

    char *ch = 0;

    if (!tdecl) {
        puts("ic_decl_type_struct_sig_mangled_full: tdecl was null");
        return 0;
    }

    /* cache str pointer */
    sig_str = &(tdecl->sig_mangled_full);

    if (ic_string_length(sig_str) == 0) {
        /* string was empty, generate */

        /* return type */
        if (!ic_string_append_symbol(sig_str, &(tdecl->name))) {
            puts("ic_decl_type_struct_sig_mangled_full: call to ic_string_append_char failed");
            return 0;
        }

        /* function name */
        if (!ic_string_append_char(sig_str, " i_", 3)) {
            puts("ic_decl_type_struct_sig_mangled_full: call to ic_string_append_char failed");
            return 0;
        }

        if (!ic_string_append_symbol(sig_str, &(tdecl->name))) {
            puts("ic_decl_type_struct_sig_mangled_full: call to ic_string_append_symbol failed");
            return 0;
        }

        if (!ic_string_append_char(sig_str, "_a", 2)) {
            puts("ic_decl_type_struct_sig_mangled_full: call to ic_string_append_char failed");
            return 0;
        }

        /* arguments */

        generated_fargs = ic_string_new_empty();
        if (!generated_fargs) {
            puts("ic_decl_type_struct_sig_mangled_full: call to ic_string_new_empty failed");
            return 0;
        }

        n_fields = ic_decl_type_struct_field_length(tdecl);

        for (i_field = 0; i_field < n_fields; ++i_field) {
            if (i_field > 0) {
                if (!ic_string_append_char(generated_fargs, ", ", 2)) {
                    puts("ic_decl_type_struct_sig_mangled_full: call to ic_string_append_char failed");
                    return 0;
                }
            }

            if (!ic_string_append_char(sig_str, "_", 1)) {
                puts("ic_decl_type_struct_sig_mangled_full: call to ic_string_append_char failed");
                return 0;
            }

            field = ic_decl_type_struct_field_get(tdecl, i_field);
            if (!field) {
                puts("ic_decl_type_struct_sig_mangled_full: call to ic_decl_type_struct_field_get failed");
                return 0;
            }

            field_name_sym = &(field->name);

            field_type_sym = ic_type_ref_get_symbol(&(field->type));
            if (!field_type_sym) {
                puts("ic_decl_type_struct_sig_mangled_full: call to ic_type_ref_get_symbol failed");
                return 0;
            }

            if (!ic_string_append_symbol(sig_str, field_type_sym)) {
                puts("ic_decl_type_struct_sig_mangled_full: call to ic_string_append_symbol failed");
                return 0;
            }

            if (!ic_string_append_symbol(generated_fargs, field_type_sym)) {
                puts("ic_decl_type_struct_sig_mangled_full: call to ic_string_append_symbol failed");
                return 0;
            }

            if (!ic_string_append_char(generated_fargs, " ", 1)) {
                puts("ic_decl_type_struct_sig_mangled_full: call to ic_string_append_char failed");
                return 0;
            }

            if (!ic_string_append_symbol(generated_fargs, field_name_sym)) {
                puts("ic_decl_type_struct_sig_mangled_full: call to ic_string_append_symbol failed");
                return 0;
            }
        }

        if (!ic_string_append_char(sig_str, "(", 3)) {
            puts("ic_decl_type_struct_sig_mangled_full: call to ic_string_append_char failed");
            return 0;
        }

        if (!ic_string_append(sig_str, generated_fargs)) {
            puts("ic_decl_type_struct_sig_mangled_full: call to ic_string_append failed");
            return 0;
        }

        if (!ic_string_append_char(sig_str, ")", 1)) {
            puts("ic_decl_type_struct_sig_mangled_full: call to ic_string_append_char failed");
            return 0;
        }

        if (!ic_string_destroy(generated_fargs, 1)) {
            puts("ic_decl_type_struct_sig_mangled_full: call to ic_string_destroy failed");
            return 0;
        }
    }

    ch = ic_string_contents(sig_str);
    if (!ch) {
        puts("ic_decl_type_struct_sig_mangled_full: failed to get string contents");
        return 0;
    }

    return ch;
}

/* add new type_param to decl_struct
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_struct_type_params_add(struct ic_decl_type_struct *tdecl, struct ic_type_param *tparam) {
    if (!tdecl) {
        puts("ic_decl_type_struct_type_params_add: tdecl was null");
        return 0;
    }

    if (!tparam) {
        puts("ic_decl_type_struct_type_params_add: tparam was null");
        return 0;
    }

    if (-1 == ic_pvector_append(&(tdecl->type_params), tparam)) {
        puts("ic_decl_type_struct_type_params_add: call to ic_pvector_append failed");
        return 0;
    }

    return 1;
}

/* get length of type_params
 *
 * returns len on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_struct_type_params_length(struct ic_decl_type_struct *tdecl) {
    if (!tdecl) {
        puts("ic_decl_type_struct_type_params_length: tdecl was null");
        return 0;
    }

    return ic_pvector_length(&(tdecl->type_params));
}

/* get type_param at i
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_type_param *ic_decl_type_struct_type_params_get(struct ic_decl_type_struct *tdecl, unsigned int i) {
    struct ic_type_param *tparam = 0;

    if (!tdecl) {
        puts("ic_decl_type_struct_type_params_get: tdecl was null");
        return 0;
    }

    tparam = ic_pvector_get(&(tdecl->type_params), i);
    if (!tparam) {
        puts("ic_decl_type_struct_type_params_get: call to ic_pvector_get failed");
        return 0;
    }

    return tparam;
}

/* returns number of fields
 *
 * returns number on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_struct_field_length(struct ic_decl_type_struct *tdecl) {
    unsigned int n = 0;

    if (!tdecl) {
        puts("ic_decl_type_struct_field_struct_length: tdecl was null");
        return 0;
    }

    n = ic_pvector_length(&(tdecl->fields));

    return n;
}

/* get field by number
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_field *ic_decl_type_struct_field_get(struct ic_decl_type_struct *tdecl, unsigned int field_number) {
    struct ic_field *field = 0;

    if (!tdecl) {
        puts("ic_decl_type_struct_field_get: tdecl was null");
        return 0;
    }

    field = ic_pvector_get(&(tdecl->fields), field_number);
    if (!field) {
        puts("ic_decl_type_struct_field_struct_get: call to ic_pvector_get failed");
        return 0;
    }

    return field;
}

/* get the type of a field by name
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_decl_type *ic_decl_type_struct_get_field_type(struct ic_decl_type_struct *tdecl, char *field_name) {
    struct ic_decl_type *type = 0;

    if (!tdecl) {
        puts("ic_decl_type_struct_get_field_type: tdecl was null");
        return 0;
    }

    if (!field_name) {
        puts("ic_decl_type_struct_get_field_type: field_name was null");
        return 0;
    }

    type = ic_dict_get(&(tdecl->field_dict), field_name);
    if (!type) {
        printf("ic_decl_type_struct_get_field_type: failed to get type for field '%s' on type '%s'\n", field_name, ic_symbol_contents(&(tdecl->name)));
        return 0;
    }

    return type;
}

/* get the offset of a field by name
 *
 * TODO FIXME no way to indicate failure
 *
 * returns n on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_struct_get_field_offset(struct ic_decl_type_struct *tdecl, char *field_name) {
    unsigned int offset = 0;

    unsigned int i = 0;
    unsigned int len = 0;
    struct ic_field *field = 0;
    struct ic_symbol *field_sym = 0;

    if (!tdecl) {
        puts("ic_decl_type_struct_get_field_offset: tdecl was null");
        return 0;
    }

    if (!field_name) {
        puts("ic_decl_type_struct_get_field_offset: field_name was null");
        return 0;
    }

    len = ic_decl_type_struct_field_length(tdecl);

    for (i = 0; i < len; ++i) {
        field = ic_decl_type_struct_field_get(tdecl, i);
        if (!field) {
            puts("ic_decl_type_struct_get_field_offset: call to ic_decl_type_struct_field_get failed");
            return 0;
        }

        field_sym = &(field->name);

        if (1 == ic_symbol_equal_char(field_sym, field_name)) {
            offset = i;
            goto FOUND;
        }
    }

    puts("ic_decl_type_struct_get_field_offset: ERROR: failed to field field name");
    printf("Was unable to find field_name '%s' in type '%s'\n", field_name, ic_symbol_contents(&(tdecl->name)));
    return 0;

FOUND:

    return offset;
}

/* add field to field_dict
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_struct_add_field_type(struct ic_decl_type_struct *tdecl, char *field_name, struct ic_decl_type *type) {
    if (!tdecl) {
        puts("ic_decl_type_struct_add_field_type: tdecl was null");
        return 0;
    }

    if (!field_name) {
        puts("ic_decl_type_struct_add_field_type: field_name was null");
        return 0;
    }

    if (!type) {
        puts("ic_decl_type_struct_add_field_type: type was null");
        return 0;
    }

    if (!ic_dict_insert(&(tdecl->field_dict), field_name, type)) {
        printf("ic_decl_type_struct_add_field_type: failed to insert type for field name '%s' into dict\n", field_name);
        return 0;
    }

    return 1;
}

/* compare 2 types for equality
 *
 * returns 1 if equal
 * returns 0 otherwise
 */
unsigned int ic_decl_type_struct_equal(struct ic_decl_type_struct *a, struct ic_decl_type_struct *b) {
    if (!a) {
        puts("ic_decl_type_struct_equal: a was null");
        return 0;
    }

    if (!b) {
        puts("ic_decl_type_struct_equal: b was null");
        return 0;
    }

    return a == b;
}

/* allocate and return a new decl_union
 * only needs name and len
 * will also allocate an empty pvector for fields
 *
 * returns new ic_field * on success
 * returns 0 on failure
 */
struct ic_decl_type_union *ic_decl_type_union_new(char *name_src, unsigned int name_len) {
    struct ic_decl_type_union *udecl = 0;

    if (!name_src) {
        puts("ic_decl_type_union_new: name_src was null");
        return 0;
    }

    if (!name_len) {
        puts("ic_decl_type_union_new: name_len was zero");
        return 0;
    }

    udecl = calloc(1, sizeof(struct ic_decl_type_union));
    if (!udecl) {
        puts("ic_decl_type_union_new: call to calloc failed");
        return 0;
    }

    if (!ic_decl_type_union_init(udecl, name_src, name_len)) {
        puts("ic_decl_type_union_new: call to ic_decl_type_union_init failed");
        return 0;
    }

    return udecl;
}

/* initialise an existing decl_union
 * only needs name and len
 * will also allocate an empty pvector for fields
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_union_init(struct ic_decl_type_union *udecl, char *name_src, unsigned int name_len) {
    if (!udecl) {
        puts("ic_decl_type_union_init: udecl was null");
        return 0;
    }

    if (!name_src) {
        puts("ic_decl_type_union_init: name_src was null");
        return 0;
    }

    if (!name_len) {
        puts("ic_decl_type_union_init: name_len was zero");
        return 0;
    }

    /* initialise name */
    if (!ic_symbol_init(&(udecl->name), name_src, name_len)) {
        puts("ic_decl_type_union_init: call to ic_symbol_init for name failed");
        return 0;
    }

    /* init fields pvector type_params */
    if (!ic_pvector_init(&(udecl->type_params), 0)) {
        puts("ic_decl_type_union_init: call to ic_pvector_init for type_params failed");
        return 0;
    }

    /* init fields pvector fields */
    if (!ic_pvector_init(&(udecl->fields), 0)) {
        puts("ic_decl_type_union_init: call to ic_pvector_init for fields failed");
        return 0;
    }

    /* init field dict */
    if (!ic_dict_init(&(udecl->field_dict))) {
        puts("ic_decl_type_union_init: call to ic_dict_init for field_dict failed");
        return 0;
    }

    return 1;
}

/* calls destroy on every element within
 *
 * this will only free the udecl if `free_udecl` is truthy
 *
 * the caller must determine if it is appropriate
 * or not to call free(decl)
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_union_destroy(struct ic_decl_type_union *udecl, unsigned int free_udecl) {
    struct ic_type_param *tparam = 0;
    struct ic_field *field = 0;
    unsigned int i = 0;
    unsigned int len = 0;

    if (!udecl) {
        puts("ic_decl_type_union_destroy: udecl was null");
        return 0;
    }

    /* free symbol contents but do not free symbol itself
     * since it is an element on tdecl
     */
    if (!ic_symbol_destroy(&(udecl->name), 0)) {
        puts("ic_decl_type_union_destroy: call to ic_symbol_destroy failed");
        return 0;
    }

    len = ic_decl_type_union_type_params_length(udecl);

    /* loop through each item destroying */
    for (i = 0; i < len; ++i) {
        tparam = ic_decl_type_union_type_params_get(udecl, i);
        if (!tparam) {
            puts("ic_decl_type_union_destroy: call to ic_decl_type_union_type_params_get failed");
            return 0;
        }

        /* dispatch to field destroy
         * free_field set to 1
         */
        if (!ic_type_param_destroy(tparam, 1)) {
            puts("ic_union_type_destroy: call to ic_type_param_destroy failed");
            return 0;
        }
    }

    if (!ic_pvector_destroy(&(udecl->type_params), 0, 0)) {
        puts("ic_union_type_destroy: call to ic_pvector_destroy failed");
        return 0;
    }

    len = ic_decl_type_union_field_length(udecl);

    /* loop through each item destroying */
    for (i = 0; i < len; ++i) {
        field = ic_decl_type_union_field_get(udecl, i);
        if (!field) {
            puts("ic_decl_type_union_destroy: call to ic_decl_type_union_field_get failed");
            return 0;
        }

        /* dispatch to field destroy
         * free_field set to 1
         */
        if (!ic_field_destroy(field, 1)) {
            puts("ic_union_type_destroy: call to ic_field_destroy failed");
            return 0;
        }
    }

    if (!ic_pvector_destroy(&(udecl->fields), 0, 0)) {
        puts("ic_union_type_destroy: call to ic_pvector_destroy failed");
        return 0;
    }

    /* destroy field_dict */
    if (!ic_dict_destroy(&(udecl->field_dict), 0, 0)) {
        puts("ic_decl_type_union_destroy: call to ic_dict_destroy failed");
        return 0;
    }

    if (free_udecl) {
        free(udecl);
    }

    return 1;
}

/* get symbol of name for this type
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_symbol *ic_decl_type_union_get_name(struct ic_decl_type_union *udecl) {
    if (!udecl) {
        puts("ic_decl_type_union_get_name: udecl was null");
        return 0;
    }

    return &(udecl->name);
}

/* add a new field to types list of fields
 *
 * returns 1 on success
 * returns 0 on failure
 */

unsigned int ic_decl_type_union_add_field(struct ic_decl_type_union *udecl, struct ic_field *field) {
    if (!udecl) {
        puts("ic_decl_type_union_add_field: udecl was null");
        return 0;
    }

    if (!field) {
        puts("ic_decl_type_union_add_field: field was null");
        return 0;
    }

    if (-1 == ic_pvector_append(&(udecl->fields), field)) {
        puts("ic_decl_type_union_add_field: call to ic_pvector_append failed");
        return 0;
    }

    return 1;
}

/* print the decl_union to provided fd */
void ic_decl_type_union_print(FILE *fd, struct ic_decl_type_union *udecl, unsigned int *indent_level) {
    if (!udecl) {
        puts("ic_decl_type_union_print: udecl was null");
        return;
    }
    if (!indent_level) {
        puts("ic_decl_type_union_print: indent_level was null");
        return;
    }

    ic_decl_type_union_print_header(fd, udecl, indent_level);
    ic_decl_type_union_print_body(fd, udecl, indent_level);
}

void ic_decl_type_union_print_header(FILE *fd, struct ic_decl_type_union *udecl, unsigned int *indent_level) {
    unsigned int i = 0;
    unsigned int len = 0;

    if (!udecl) {
        puts("ic_decl_type_union_print_header: udecl was null");
        return;
    }
    if (!indent_level) {
        puts("ic_decl_type_union_print_header: indent_level was null");
        return;
    }

    /* print type and name */
    fprintf(fd, "union %s", ic_symbol_contents(&(udecl->name)));

    /* print type params, if they exist */
    len = ic_pvector_length(&(udecl->type_params));
    if (len > 0) {
        fputs("[", fd);
        for (i = 0; i < len; ++i) {
            /* add comma and space between type_params */
            if (i > 0) {
                fputs(", ", fd);
            }

            ic_type_param_print(fd, ic_pvector_get(&(udecl->type_params), i));
        }
        fputs("]", fd);
    }

    fputs("\n", fd);
}

void ic_decl_type_union_print_body(FILE *fd, struct ic_decl_type_union *udecl, unsigned int *indent_level) {
    unsigned int i = 0;

    if (!udecl) {
        puts("ic_decl_type_union_print_body: udecl was null");
        return;
    }
    if (!indent_level) {
        puts("ic_decl_type_union_print_body: indent_level was null");
        return;
    }

    /* increment indent level before body */
    ++*indent_level;

    /* iterate through pvector fields
     * prefix each field with a 2 spaces  and postfix each with a \n
     */
    for (i = 0; i < ic_pvector_length(&(udecl->fields)); ++i) {
        /* print indent */
        ic_parse_print_indent(fd, *indent_level);

        /* print field contents */
        ic_field_print(fd, ic_pvector_get(&(udecl->fields), i));

        /* postfix newline */
        fputs("\n", fd);
    }

    /* decrement indent level after body */
    --*indent_level;

    fputs("end\n", fd);
}

/* print debug information for this type */
void ic_decl_type_union_print_debug(FILE *fd, struct ic_decl_type_union *udecl) {
    unsigned int fake_indent = 0;

    if (!udecl) {
        puts("ic_decl_type_union_print_debug: udecl was null");
        return;
    }

    fputs("found: ", fd);
    ic_decl_type_union_print_header(fd, udecl, &fake_indent);
}

/* return the symbol name of this types
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_symbol *ic_decl_type_union_name(struct ic_decl_type_union *tdecl) {
    if (!tdecl) {
        puts("ic_decl_type_union_name: tdecl was null");
        return 0;
    }

    return &(tdecl->name);
}

/* get the char * contents of the name
 *
 * returns char * on success
 * returns 0 on failure
 */
char *ic_decl_type_union_str(struct ic_decl_type_union *tdecl) {
    char *ch = 0;

    if (!tdecl) {
        puts("ic_decl_type_union_str: tdecl was null");
        return 0;
    }

    ch = ic_symbol_contents(&(tdecl->name));
    if (!ch) {
        puts("ic_decl_type_union_str: call to ic_symbol_contents failed");
        return 0;
    }

    return ch;
}

/* add new type_param to decl_union
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_union_type_params_add(struct ic_decl_type_union *udecl, struct ic_type_param *tparam) {

    if (!udecl) {
        puts("ic_decl_type_union_type_params_add: udecl was null");
        return 0;
    }

    if (!tparam) {
        puts("ic_decl_type_union_type_params_add: tparam was null");
        return 0;
    }

    if (-1 == ic_pvector_append(&(udecl->type_params), tparam)) {
        puts("ic_decl_type_union_type_params_add: call to ic_pvector_append failed");
        return 0;
    }

    return 1;
}

/* get length of type_params
 *
 * returns len on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_union_type_params_length(struct ic_decl_type_union *udecl) {

    if (!udecl) {
        puts("ic_decl_type_union_type_params_length: udecl was null");
        return 0;
    }

    return ic_pvector_length(&(udecl->type_params));
}

/* get type_param at i
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_type_param *ic_decl_type_union_type_params_get(struct ic_decl_type_union *udecl, unsigned int i) {
    struct ic_type_param *tparam = 0;

    if (!udecl) {
        puts("ic_decl_type_union_type_params_get: udecl was null");
        return 0;
    }

    tparam = ic_pvector_get(&(udecl->type_params), i);
    if (!tparam) {
        puts("ic_decl_type_union_type_params_get: call to ic_pvector_get failed");
        return 0;
    }

    return tparam;
}

/* get number of fields
 *
 * returns number on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_union_field_length(struct ic_decl_type_union *udecl) {
    unsigned int n = 0;

    if (!udecl) {
        puts("ic_decl_type_union_field_length: udecl was null");
        return 0;
    }

    n = ic_pvector_length(&(udecl->fields));

    return n;
}

/* get field by number
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_field *ic_decl_type_union_field_get(struct ic_decl_type_union *udecl, unsigned int field_number) {
    struct ic_field *field = 0;

    if (!udecl) {
        puts("ic_decl_typefield_get: udecl was null");
        return 0;
    }

    field = ic_pvector_get(&(udecl->fields), field_number);
    if (!field) {
        puts("ic_decl_type_union_field_get: call to ic_pvector_get failed");
        return 0;
    }

    return field;
}

/* get the type of a field by name
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_decl_type *ic_decl_type_union_get_field_type(struct ic_decl_type_union *udecl, char *field_name) {
    struct ic_decl_type *type = 0;

    if (!udecl) {
        puts("ic_decl_type_union_get_field_type: udecl was null");
        return 0;
    }

    if (!field_name) {
        puts("ic_decl_type_union_get_field_type: field_name was null");
        return 0;
    }

    type = ic_dict_get(&(udecl->field_dict), field_name);
    if (!type) {
        printf("ic_decl_type_struct_get_field_type: failed to get type for field '%s' on type '%s'\n", field_name, ic_symbol_contents(&(udecl->name)));
        return 0;
    }

    return type;
}

/* get the offset of a field by name
 *
 * TODO FIXME no way to indicate failure
 *
 * returns n on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_union_get_field_offset(struct ic_decl_type_union *tdecl, char *field_name) {
    unsigned int offset = 0;

    unsigned int i = 0;
    unsigned int len = 0;
    struct ic_field *field = 0;

    struct ic_symbol *field_sym = 0;

    if (!tdecl) {
        puts("ic_decl_type_union_get_field_offset: tdecl was null");
        return 0;
    }

    if (!field_name) {
        puts("ic_decl_type_union_get_field_offset: field_name was null");
        return 0;
    }

    len = ic_decl_type_union_field_length(tdecl);

    for (i = 0; i < len; ++i) {
        field = ic_decl_type_union_field_get(tdecl, i);
        if (!field) {
            puts("ic_decl_type_union_get_field_offset: call to ic_decl_type_union_field_get failed");
            return 0;
        }

        field_sym = &(field->name);

        if (ic_symbol_equal_char(field_sym, field_name)) {
            offset = i;
            goto FOUND;
        }
    }

    puts("ic_decl_type_union_get_field_offset: ERROR: failed to field field name");
    printf("Was unable to find field_name '%s' in type '%s'\n", field_name, ic_symbol_contents(&(tdecl->name)));
    return 0;

FOUND:

    return offset;
}

/* add field to field_dict
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_union_add_field_type(struct ic_decl_type_union *udecl, char *field_name, struct ic_decl_type *type) {
    if (!udecl) {
        puts("ic_decl_type_union_add_field_type: udecl was null");
        return 0;
    }

    if (!field_name) {
        puts("ic_decl_type_union_add_field_type: field_name was null");
        return 0;
    }

    if (!type) {
        puts("ic_decl_type_union_add_field_type: type was null");
        return 0;
    }

    if (!ic_dict_insert(&(udecl->field_dict), field_name, type)) {
        printf("ic_decl_type_union_add_field_type: failed to insert type for field name '%s' into dict\n", field_name);
        return 0;
    }

    return 1;
}

/* compare types for equality
 *
 * returns 1 if equals
 * returns 0 otherwise
 */
unsigned int ic_decl_type_union_equal(struct ic_decl_type_union *a, struct ic_decl_type_union *b) {
    if (!a) {
        puts("ic_decl_type_union_equal: a was null");
        return 0;
    }

    if (!b) {
        puts("ic_decl_type_union_equal: b was null");
        return 0;
    }

    return a == b;
}

/* allocate and initialise a new ic_decl_type as a struct
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_decl_type *ic_decl_type_new_struct(char *name_src, unsigned int name_len) {
    struct ic_decl_type *tdecl = 0;

    if (!name_src) {
        puts("ic_decl_type_new_struct: name_src was null");
        return 0;
    }
    if (!name_len) {
        puts("ic_decl_type_new_struct: name_len was falsey");
        return 0;
    }

    tdecl = calloc(1, sizeof(struct ic_decl_type));
    if (!tdecl) {
        puts("ic_decl_type_new_struct: call to calloc failed");
        return 0;
    }

    if (!ic_decl_type_init_struct(tdecl, name_src, name_len)) {
        puts("ic_decl_type_new_struct: call to ic_decl_type_init_struct failed");
        return 0;
    }

    return tdecl;
}

/* initialise an existing ic_decl_type as a struct
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_init_struct(struct ic_decl_type *tdecl, char *name_src, unsigned int name_len) {
    if (!tdecl) {
        puts("ic_decl_type_init_struct: call to calloc failed");
        return 0;
    }

    if (!name_src) {
        puts("ic_decl_type_init_struct: name_src was null");
        return 0;
    }

    if (!name_len) {
        puts("ic_decl_type_init_struct: name_len was falsey");
        return 0;
    }

    tdecl->tag = ic_decl_type_tag_struct;
    tdecl->builtin = 0;

    if (!ic_decl_type_struct_init(&(tdecl->u.tstruct), name_src, name_len)) {
        puts("ic_decl_type_init_struct: call to ic_decl_type_struct_init failed");
        return 0;
    }

    return 1;
}

/* allocate and initialise a new ic_decl_type as a union
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_decl_type *ic_decl_type_new_union(char *name_src, unsigned int name_len) {
    struct ic_decl_type *tdecl = 0;

    if (!name_src) {
        puts("ic_decl_type_new_union: name_src was null");
        return 0;
    }
    if (!name_len) {
        puts("ic_decl_type_new_union: name_len was falsey");
        return 0;
    }

    tdecl = calloc(1, sizeof(struct ic_decl_type));
    if (!tdecl) {
        puts("ic_decl_type_new_union: call to calloc failed");
        return 0;
    }

    if (!ic_decl_type_init_union(tdecl, name_src, name_len)) {
        puts("ic_decl_type_new_union: call to ic_decl_type_init_union failed");
        return 0;
    }

    return tdecl;
}

/* initialise an existing ic_decl_type as a union
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_init_union(struct ic_decl_type *tdecl, char *name_src, unsigned int name_len) {
    if (!tdecl) {
        puts("ic_decl_type_init_union: call to calloc failed");
        return 0;
    }

    if (!name_src) {
        puts("ic_decl_type_init_union: name_src was null");
        return 0;
    }

    if (!name_len) {
        puts("ic_decl_type_init_union: name_len was falsey");
        return 0;
    }

    tdecl->tag = ic_decl_type_tag_union;
    tdecl->builtin = 0;

    if (!ic_decl_type_union_init(&(tdecl->u.tunion), name_src, name_len)) {
        puts("ic_decl_type_new_union: call to ic_decl_type_union_init failed");
        return 0;
    }

    return 1;
}

/* destroy an ic_decl_type
 *
 * will only free tdecl of free_tdectl is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_destroy(struct ic_decl_type *tdecl, unsigned int free_tdecl) {
    if (!tdecl) {
        puts("ic_decl_type_destroy: tdecl was null");
        return 0;
    }

    switch (tdecl->tag) {
        case ic_decl_type_tag_struct:
            /* do not free as member */
            if (!ic_decl_type_struct_destroy(&(tdecl->u.tstruct), 0)) {
                puts("ic_decl_type_destroy: call to ic_decl_type_struct_destroy failed");
                return 0;
            }
            break;

        case ic_decl_type_tag_union:
            /* do not free as member */
            if (!ic_decl_type_union_destroy(&(tdecl->u.tunion), 0)) {
                puts("ic_decl_type_destroy: call to ic_decl_type_union_destroy failed");
                return 0;
            }
            break;

        default:
            puts("ic_decl_type_destroy: unknown tag");
            return 0;
    }

    if (free_tdecl) {
        free(tdecl);
    }

    return 1;
}

/* mark this tdecl as being a builtin
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_mark_builtin(struct ic_decl_type *tdecl) {
    if (!tdecl) {
        puts("ic_decl_type_mark_builtin: tdecl was null");
        return 0;
    }

    /* to mark fdecl itself */
    tdecl->builtin = 1;

    return 1;
}

/* test if builtin
 *
 * returns 1 if builtin
 * returns 0 otherwise
 */
unsigned int ic_decl_type_isbuiltin(struct ic_decl_type *tdecl) {
    if (!tdecl) {
        puts("ic_decl_type_isbuiltin: tdecl was null");
        return 0;
    }

    return tdecl->builtin;
}

/* get symbol of name for this type
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_symbol *ic_decl_type_get_name(struct ic_decl_type *tdecl) {
    struct ic_symbol *sym = 0;

    if (!tdecl) {
        puts("ic_decl_type_get_name: tdecl was null");
        return 0;
    }

    switch (tdecl->tag) {
        case ic_decl_type_tag_struct:
            sym = ic_decl_type_struct_get_name(&(tdecl->u.tstruct));
            if (!sym) {
                puts("ic_decl_type_get_name: call to ic_decl_type_struct_get_name failed");
                return 0;
            }
            return sym;
            break;

        case ic_decl_type_tag_union:
            sym = ic_decl_type_union_get_name(&(tdecl->u.tunion));
            if (!sym) {
                puts("ic_decl_type_get_name: call to ic_decl_type_union_get_name failed");
                return 0;
            }
            return sym;
            break;

        default:
            puts("ic_decl_type_get_name: unknown tag");
            return 0;
            break;
    }
}

/* fetch a pointer to the internal decl_type_struct
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_decl_type_struct *ic_decl_type_get_struct(struct ic_decl_type *tdecl) {
    struct ic_decl_type_struct *tdecl_struct = 0;

    if (!tdecl) {
        puts("ic_decl_type_get_struct: tdecl was null");
        return 0;
    }

    if (tdecl->tag != ic_decl_type_tag_struct) {
        puts("ic_decl_type_get_struct: tdecl was not of type struct");
        return 0;
    }

    tdecl_struct = &(tdecl->u.tstruct);

    return tdecl_struct;
}

/* fetch a pointer to the internal decl_type_union
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_decl_type_union *ic_decl_type_get_union(struct ic_decl_type *tdecl) {
    struct ic_decl_type_union *tdecl_union = 0;

    if (!tdecl) {
        puts("ic_decl_type_get_union: tdecl was null");
        return 0;
    }

    if (tdecl->tag != ic_decl_type_tag_union) {
        puts("ic_decl_type_get_union: tdecl was not of type union");
        return 0;
    }

    tdecl_union = &(tdecl->u.tunion);

    return tdecl_union;
}

/* return the symbol name of this type
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_symbol *ic_decl_type_name(struct ic_decl_type *tdecl) {
    struct ic_symbol *sym = 0;
    if (!tdecl) {
        puts("ic_decl_type_name: tdecl was null");
        return 0;
    }

    switch (tdecl->tag) {
        case ic_decl_type_tag_struct:
            sym = ic_decl_type_struct_name(&(tdecl->u.tstruct));
            if (!sym) {
                puts("ic_decl_type_name: call to ic_decl_type_struct_name failed");
                return 0;
            }
            return sym;
            break;

        case ic_decl_type_tag_union:
            sym = ic_decl_type_union_name(&(tdecl->u.tunion));
            if (!sym) {
                puts("ic_decl_type_name: call to ic_decl_type_union_name failed");
                return 0;
            }
            return sym;
            break;

        default:
            puts("ic_decl_type_name: unknown tag");
            return 0;
            break;
    }
}

/* get the char * contents of the name
 *
 * returns char * on success
 * returns 0 on failure
 */
char *ic_decl_type_str(struct ic_decl_type *tdecl) {
    char *ch = 0;
    if (!tdecl) {
        puts("ic_decl_type_str: tdecl was null");
        return 0;
    }

    switch (tdecl->tag) {
        case ic_decl_type_tag_struct:
            ch = ic_decl_type_struct_str(&(tdecl->u.tstruct));
            if (!ch) {
                puts("ic_decl_type_str: call to ic_decl_type_struct_str failed");
                return 0;
            }
            return ch;
            break;

        case ic_decl_type_tag_union:
            ch = ic_decl_type_union_str(&(tdecl->u.tunion));
            if (!ch) {
                puts("ic_decl_type_str: call to ic_decl_type_union_str failed");
                return 0;
            }
            return ch;
            break;

        default:
            puts("ic_decl_type_str: unknown tag");
            return 0;
            break;
    }
}

/* add new type_param to decl
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_type_params_add(struct ic_decl_type *tdecl, struct ic_type_param *tparam) {
    if (!tdecl) {
        puts("ic_decl_type_type_params_add: tdecl was null");
        return 0;
    }

    if (!tparam) {
        puts("ic_decl_type_type_params_add: tparam was null");
        return 0;
    }

    switch (tdecl->tag) {
        case ic_decl_type_tag_struct:
            if (!ic_decl_type_struct_type_params_add(&(tdecl->u.tstruct), tparam)) {
                puts("ic_decl_type_type_params_add: call to ic_decl_type_struct_type_params_add failed");
                return 0;
            }

            return 1;
            break;

        case ic_decl_type_tag_union:
            if (!ic_decl_type_union_type_params_add(&(tdecl->u.tunion), tparam)) {
                puts("ic_decl_type_type_params_add: call to ic_decl_type_union_type_params_add failed");
                return 0;
            }

            return 1;
            break;

        default:
            puts("ic_decl_type_type_params_add: impossible tag");
            return 0;
            break;
    }
}

/* get length of type_params
 *
 * returns len on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_type_params_length(struct ic_decl_type *tdecl) {
    if (!tdecl) {
        puts("ic_decl_type_type_params_length: tdecl was null");
        return 0;
    }

    switch (tdecl->tag) {
        case ic_decl_type_tag_struct:
            return ic_decl_type_struct_type_params_length(&(tdecl->u.tstruct));
            break;

        case ic_decl_type_tag_union:
            return ic_decl_type_union_type_params_length(&(tdecl->u.tunion));
            break;

        default:
            puts("ic_decl_type_type_params_length: impossible tag");
            return 0;
            break;
    }
}

/* get type_param at i
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_type_param *ic_decl_type_type_params_get(struct ic_decl_type *tdecl, unsigned int i) {
    struct ic_type_param *tparam = 0;

    if (!tdecl) {
        puts("ic_decl_type_type_params_get: tdecl was null");
        return 0;
    }

    switch (tdecl->tag) {
        case ic_decl_type_tag_struct:
            tparam = ic_decl_type_struct_type_params_get(&(tdecl->u.tstruct), i);
            if (!tparam) {
                puts("ic_decl_type_type_params_get: call to ic_decl_type_struct_type_params_get failed");
                return 0;
            }

            return tparam;
            break;

        case ic_decl_type_tag_union:
            tparam = ic_decl_type_union_type_params_get(&(tdecl->u.tunion), i);
            if (!tparam) {
                puts("ic_decl_type_type_params_get: call to ic_decl_type_union_type_params_get failed");
                return 0;
            }

            return tparam;
            break;

        default:
            puts("ic_decl_type_type_params_get: impossible tag");
            return 0;
            break;
    }
}

/* add a new field to types list of fields
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_add_field(struct ic_decl_type *tdecl, struct ic_field *field) {
    if (!tdecl) {
        puts("ic_decl_type_add_field: tdecl was null");
        return 0;
    }

    switch (tdecl->tag) {
        case ic_decl_type_tag_struct:
            if (!ic_decl_type_struct_add_field(&(tdecl->u.tstruct), field)) {
                puts("ic_decl_type_add_field: call to ic_decl_type_struct_add_field failed");
                return 0;
            }
            return 1;
            break;

        case ic_decl_type_tag_union:
            if (!ic_decl_type_union_add_field(&(tdecl->u.tunion), field)) {
                puts("ic_decl_type_add_field: call to ic_decl_type_union_add_field failed");
                return 0;
            }
            return 1;
            break;

        default:
            puts("ic_decl_type_add_field: unknown tag");
            return 0;
            break;
    }
}

/* get number of fields
 *
 * returns number on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_field_length(struct ic_decl_type *tdecl) {
    if (!tdecl) {
        puts("ic_decl_type_field_length: tdecl was null");
        return 0;
    }

    switch (tdecl->tag) {
        case ic_decl_type_tag_struct:
            return ic_decl_type_struct_field_length(&(tdecl->u.tstruct));
            break;

        case ic_decl_type_tag_union:
            return ic_decl_type_union_field_length(&(tdecl->u.tunion));
            break;

        default:
            puts("ic_decl_type_field_length: unknown tag");
            return 0;
            break;
    }
}

/* get field by number
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_field *ic_decl_type_field_get(struct ic_decl_type *tdecl, unsigned int field_number) {
    struct ic_field *field = 0;
    if (!tdecl) {
        puts("ic_decl_type_field_get: tdecl was null");
        return 0;
    }

    switch (tdecl->tag) {
        case ic_decl_type_tag_struct:
            field = ic_decl_type_struct_field_get(&(tdecl->u.tstruct), field_number);
            if (!field) {
                puts("ic_decl_type_field_get: call to ic_decl_type_struct_field_get failed");
                return 0;
            }
            return field;
            break;

        case ic_decl_type_tag_union:
            field = ic_decl_type_union_field_get(&(tdecl->u.tunion), field_number);
            if (!field) {
                puts("ic_decl_type_field_get: call to ic_decl_type_union_field_get failed");
                return 0;
            }
            return field;
            break;

        default:
            puts("ic_decl_type_field_get: unknown tag");
            return 0;
            break;
    }
}

/* get the type of a field by name
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_decl_type *ic_decl_type_get_field_type(struct ic_decl_type *tdecl, char *field_name) {
    struct ic_decl_type *field_type = 0;

    if (!tdecl) {
        puts("ic_decl_type_field_type: tdecl was null");
        return 0;
    }

    if (!field_name) {
        puts("ic_decl_type_field_type: field_name was null");
        return 0;
    }

    switch (tdecl->tag) {
        case ic_decl_type_tag_struct:
            field_type = ic_decl_type_struct_get_field_type(&(tdecl->u.tstruct), field_name);
            if (!field_type) {
                puts("ic_decl_type_field_type: call to ic_decl_type_struct_get_field_type failed");
                return 0;
            }
            return field_type;
            break;

        case ic_decl_type_tag_union:
            field_type = ic_decl_type_union_get_field_type(&(tdecl->u.tunion), field_name);
            if (!field_type) {
                puts("ic_decl_type_field_type: call to ic_decl_type_union_get_field_type failed");
                return 0;
            }
            return field_type;
            break;

        default:
            puts("ic_decl_type_field_type: unknown tag");
            return 0;
            break;
    }
}

/* get the offset of a field by name
 *
 * TODO FIXME no way to indicate failure
 *
 * returns n on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_get_field_offset(struct ic_decl_type *tdecl, char *field_name) {
    unsigned int offset = 0;

    if (!tdecl) {
        puts("ic_decl_type_field_offset: tdecl was null");
        return 0;
    }

    if (!field_name) {
        puts("ic_decl_type_field_offset: field_name was null");
        return 0;
    }

    switch (tdecl->tag) {
        case ic_decl_type_tag_struct:
            offset = ic_decl_type_struct_get_field_offset(&(tdecl->u.tstruct), field_name);
            return offset;
            break;

        case ic_decl_type_tag_union:
            offset = ic_decl_type_union_get_field_offset(&(tdecl->u.tunion), field_name);
            return offset;
            break;

        default:
            puts("ic_decl_type_field_offset: unknown tag");
            return 0;
            break;
    }
}

/* add field to field_dict
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_add_field_type(struct ic_decl_type *tdecl, char *field_name, struct ic_decl_type *type) {
    if (!tdecl) {
        puts("ic_decl_type_add_field_type: tdecl was null");
        return 0;
    }

    if (!field_name) {
        puts("ic_decl_type_add_field_type: field_name was null");
        return 0;
    }

    if (!type) {
        puts("ic_decl_type_add_field_type: type was null");
        return 0;
    }

    switch (tdecl->tag) {
        case ic_decl_type_tag_struct:
            if (!ic_decl_type_struct_add_field_type(&(tdecl->u.tstruct), field_name, type)) {
                puts("ic_decl_type_add_field_type: call to ic_decl_type_struct_add_field_type failed");
                return 0;
            }
            return 1;
            break;

        case ic_decl_type_tag_union:
            if (!ic_decl_type_union_add_field_type(&(tdecl->u.tunion), field_name, type)) {
                puts("ic_decl_type_add_field_type: call to ic_decl_type_struct_add_field_type failed");
                return 0;
            }
            return 1;
            break;

        default:
            puts("ic_decl_type_add_field_type: unknown tag");
            return 0;
            break;
    }
}

/* the full signature for the constructor of this type
 *
 * returns * on success
 * returns 0 on failure
 */
char *ic_decl_type_sig_mangled_full(struct ic_decl_type *tdecl) {
    char *ch = 0;
    if (!tdecl) {
        puts("ic_decl_type_sig_mangled_full: tdecl was null");
        return 0;
    }

    switch (tdecl->tag) {
        case ic_decl_type_tag_struct:
            ch = ic_decl_type_struct_sig_mangled_full(&(tdecl->u.tstruct));
            if (!ch) {
                puts("ic_decl_type_sig_mangled_full: call to ic_decl_type_struct_sig_mangled_full failed");
                return 0;
            }
            return ch;
            break;

        case ic_decl_type_tag_union:
            puts("ic_decl_type_sig_mangled_full: union not yet supported");
            return 0;
            break;

        default:
            puts("ic_decl_type_sig_mangled_full: unknown tag");
            return 0;
            break;
    }
}

/* test if bool
 *
 * returns 1 if bool
 * returns 0 otherwise
 */
unsigned int ic_decl_type_isbool(struct ic_decl_type *tdecl) {
    if (!tdecl) {
        puts("ic_decl_type_isbool: tdecl was null");
        return 0;
    }

    switch (tdecl->tag) {
        case ic_decl_type_tag_struct:
            return ic_decl_type_struct_isbool(&(tdecl->u.tstruct));
            break;

        case ic_decl_type_tag_union:
            /* a union is not bool */
            return 0;
            break;

        default:
            puts("ic_decl_type_isbool: unknown tag");
            return 0;
            break;
    }
}

/* test if string
 *
 * returns 1 if string
 * returns 0 otherwise
 */
unsigned int ic_decl_type_isstring(struct ic_decl_type *tdecl) {
    if (!tdecl) {
        puts("ic_decl_type_isstring: tdecl was null");
        return 0;
    }

    switch (tdecl->tag) {
        case ic_decl_type_tag_struct:
            return ic_decl_type_struct_isstring(&(tdecl->u.tstruct));
            break;

        case ic_decl_type_tag_union:
            /* a union is not string */
            return 0;
            break;

        default:
            puts("ic_decl_type_isstring: unknown tag");
            return 0;
            break;
    }
}

/* test if uint
 *
 * returns 1 if uint
 * returns 0 otherwise
 */
unsigned int ic_decl_type_isuint(struct ic_decl_type *tdecl) {
    if (!tdecl) {
        puts("ic_decl_type_isuint: tdecl was null");
        return 0;
    }

    switch (tdecl->tag) {
        case ic_decl_type_tag_struct:
            return ic_decl_type_struct_isuint(&(tdecl->u.tstruct));
            break;

        case ic_decl_type_tag_union:
            /* a union is not uint */
            return 0;
            break;

        default:
            puts("ic_decl_type_isuint: unknown tag");
            return 0;
            break;
    }
}

/* test if sint
 *
 * returns 1 if sint
 * returns 0 otherwise
 */
unsigned int ic_decl_type_issint(struct ic_decl_type *tdecl) {
    if (!tdecl) {
        puts("ic_decl_type_issint: tdecl was null");
        return 0;
    }

    switch (tdecl->tag) {
        case ic_decl_type_tag_struct:
            return ic_decl_type_struct_issint(&(tdecl->u.tstruct));
            break;

        case ic_decl_type_tag_union:
            /* a union is not sint */
            return 0;
            break;

        default:
            puts("ic_decl_type_issint: unknown tag");
            return 0;
            break;
    }
}

/* test if void
 *
 * returns 1 if void
 * returns 0 otherwise
 */
unsigned int ic_decl_type_isvoid(struct ic_decl_type *tdecl) {
    if (!tdecl) {
        puts("ic_decl_type_isvoid: tdecl was null");
        return 0;
    }

    switch (tdecl->tag) {
        case ic_decl_type_tag_struct:
            return ic_decl_type_struct_isvoid(&(tdecl->u.tstruct));
            break;

        case ic_decl_type_tag_union:
            /* a union is not void */
            return 0;
            break;

        default:
            puts("ic_decl_type_isvoid: unknown tag");
            return 0;
            break;
    }
}

/* compare types for equality
 *
 * returns 1 if equals
 * returns 0 otherwise
 */
unsigned int ic_decl_type_equal(struct ic_decl_type *a, struct ic_decl_type *b) {
    if (!a) {
        puts("ic_decl_type_equal: a was null");
        return 0;
    }

    if (!b) {
        puts("ic_decl_type_equal: b was null");
        return 0;
    }

    if (a->tag != b->tag) {
        return 0;
    }

    switch (a->tag) {
        case ic_decl_type_tag_struct:
            return ic_decl_type_struct_equal(&(a->u.tstruct), &(b->u.tstruct));
            break;

        case ic_decl_type_tag_union:
            return ic_decl_type_union_equal(&(a->u.tunion), &(b->u.tunion));
            break;

        default:
            puts("ic_decl_type_equal: unknown tag");
            return 0;
    }
}

/* print the decl_type to provided fd */
void ic_decl_type_print(FILE *fd, struct ic_decl_type *tdecl, unsigned int *indent_level) {
    if (!tdecl) {
        puts("ic_decl_type_print: tdecl was null");
        return;
    }

    switch (tdecl->tag) {
        case ic_decl_type_tag_struct:
            ic_decl_type_struct_print(fd, &(tdecl->u.tstruct), indent_level);
            break;

        case ic_decl_type_tag_union:
            ic_decl_type_union_print(fd, &(tdecl->u.tunion), indent_level);
            break;

        default:
            puts("ic_decl_type_print: unknown tag");
            return;
            break;
    }
}

void ic_decl_type_print_header(FILE *fd, struct ic_decl_type *tdecl, unsigned int *indent_level) {
    if (!tdecl) {
        puts("ic_decl_type_print_header: tdecl was null");
        return;
    }

    switch (tdecl->tag) {
        case ic_decl_type_tag_struct:
            ic_decl_type_struct_print_header(fd, &(tdecl->u.tstruct), indent_level);
            break;

        case ic_decl_type_tag_union:
            ic_decl_type_union_print_header(fd, &(tdecl->u.tunion), indent_level);
            break;

        default:
            puts("ic_decl_type_print_header: unknown tag");
            return;
            break;
    }
}

void ic_decl_type_print_body(FILE *fd, struct ic_decl_type *tdecl, unsigned int *indent_level) {
    if (!tdecl) {
        puts("ic_decl_type_print_body: tdecl was null");
        return;
    }

    switch (tdecl->tag) {
        case ic_decl_type_tag_struct:
            ic_decl_type_struct_print_body(fd, &(tdecl->u.tstruct), indent_level);
            break;

        case ic_decl_type_tag_union:
            ic_decl_type_union_print_body(fd, &(tdecl->u.tunion), indent_level);
            break;

        default:
            puts("ic_decl_type_print_body: unknown tag");
            return;
            break;
    }
}

/* print debug information for this type */
void ic_decl_type_print_debug(FILE *fd, struct ic_decl_type *tdecl) {
    if (!tdecl) {
        puts("ic_decl_type_print_debug: tdecl was null");
        return;
    }

    switch (tdecl->tag) {
        case ic_decl_type_tag_struct:
            ic_decl_type_struct_print_debug(fd, &(tdecl->u.tstruct));
            break;

        case ic_decl_type_tag_union:
            ic_decl_type_union_print_debug(fd, &(tdecl->u.tunion));
            break;

        default:
            puts("ic_decl_type_print_debug: unknown tag");
            return;
            break;
    }
}

/* allocate and return a new decl_op
 *
 * returns new ic_field * on success
 * returns 0 on failure
 */
struct ic_decl_op *ic_decl_op_new(char *from_src, unsigned int from_len, char *to_src, unsigned int to_len) {
    struct ic_decl_op *op = 0;

    if (!from_src) {
        puts("ic_decl_op_new: from_src was null");
        return 0;
    }

    if (!from_len) {
        puts("ic_decl_op_new: from_len was 0");
        return 0;
    }

    if (!to_src) {
        puts("ic_decl_op_new: to_src was null");
        return 0;
    }

    if (!to_len) {
        puts("ic_decl_op_new: to_len was 0");
        return 0;
    }

    op = calloc(1, sizeof(struct ic_decl_op));
    if (!op) {
        puts("ic_decl_op_new: call to calloc failed");
        return 0;
    }

    if (!ic_decl_op_init(op, from_src, from_len, to_src, to_len)) {
        puts("ic_decl_op_new: call to ic_decl_op_init failed");
        return 0;
    }

    return op;
}

/* initialise an existing decl_op
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_op_init(struct ic_decl_op *op, char *from_src, unsigned int from_len, char *to_src, unsigned int to_len) {
    if (!op) {
        puts("ic_decl_op_init: top was null");
        return 0;
    }

    if (!from_src) {
        puts("ic_decl_op_init: from_src was null");
        return 0;
    }

    if (!from_len) {
        puts("ic_decl_op_init: from_len was 0");
        return 0;
    }

    if (!to_src) {
        puts("ic_decl_op_init: to_src was null");
        return 0;
    }

    if (!to_len) {
        puts("ic_decl_op_init: to_len was 0");
        return 0;
    }

    if (!ic_symbol_init(&(op->from), from_src, from_len)) {
        puts("ic_decl_op_init: call to ic_symbol_init for 'from' failed");
        return 0;
    }

    if (!ic_symbol_init(&(op->to), to_src, to_len)) {
        puts("ic_decl_op_init: call to ic_symbol_init for 'to' failed");
        return 0;
    }

    return 1;
}

/* calls destroy on every element within
 *
 * this will only free the op if `free_op` is truthy
 *
 * the caller must determine if it is appropriate
 * or not to call free(decl)
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_op_destroy(struct ic_decl_op *op, unsigned int free_op) {
    if (!op) {
        puts("ic_decl_op_destroy: op was null");
        return 0;
    }

    if (!ic_symbol_destroy(&(op->from), 0)) {
        puts("ic_decl_op_destroy: call to ic_symbol_destroy failed for 'from'");
        return 0;
    }

    if (!ic_symbol_destroy(&(op->to), 0)) {
        puts("ic_decl_op_destroy: call to ic_symbol_destroy failed for 'to'");
        return 0;
    }

    if (free_op) {
        free(op);
    }

    return 1;
}

/* print the decl_op to stdout */
void ic_decl_op_print(FILE *fd, struct ic_decl_op *op, unsigned int *indent_level) {
    if (!op) {
        puts("ic_decl_op_print: op was null");
        return;
    }
    if (!indent_level) {
        puts("ic_decl_op_print: indent_level was null");
        return;
    }

    fputs("op ", fd);

    ic_symbol_print(fd, &(op->from));

    fputs(" ", fd);

    ic_symbol_print(fd, &(op->from));

    /* trailing \n */
    fputs("\n", fd);
}

/* allocate and initialise a new ic_decl
 *
 * NOTE: this call will NOT initialise the contents of the union
 * this is left up to the caller
 *
 * returns new ic_decl on success
 * returns 0 on failure
 */
struct ic_decl *ic_decl_new(enum ic_decl_tag tag) {
    struct ic_decl *decl = 0;

    /* allocate space */
    decl = calloc(1, sizeof(struct ic_decl));
    if (!decl) {
        puts("ic_decl_new: call to calloc failed");
        return 0;
    }

    /* initialise */
    if (!ic_decl_init(decl, tag)) {
        puts("ic_decl_new: call to ic_decl_init failed");
        return 0;
    }

    return decl;
}

/* initialise an existing ic_decl
 *
 * NOTE: this call will NOT initialise the contents of the union
 * this is left up to the caller
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_init(struct ic_decl *decl, enum ic_decl_tag tag) {
    if (!decl) {
        puts("ic_decl_init: decl was null");
        return 0;
    }

    /* set type */
    decl->tag = tag;

    /* NOTE we do NOT initialise the contents
     * it is up to the caller
     *
     * FIXME: reconsider this interface
     */

    return 1;
}

/* calls destroy on every element within
 *
 * this will only free the decl if `free_decl` is truthy
 *
 * the caller must determine if it is appropriate
 * or not to call free(decl)
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_destroy(struct ic_decl *decl, unsigned int free_decl) {
    if (!decl) {
        puts("ic_decl_destroy: decl was null");
        return 0;
    }

    /* dispatch based on type
     *
     * note that in both calls to _destroy we set the second arg to 0
     * as both the fdecl and tdecl are elements on this ic_decl
     * so we have to handle the free bewlo
     */
    switch (decl->tag) {
        case ic_decl_tag_func:
        case ic_decl_tag_builtin_func:
            /* destroy all elements but not not (0) free fdecl itself */
            if (!ic_decl_func_destroy(&(decl->u.fdecl), 0)) {
                puts("ic_decl_destroy: call to ic_decl_func_destroy failed");
                return 0;
            }
            break;

        case ic_decl_tag_type:
        case ic_decl_tag_builtin_type:
            /* destroy all elements but not not (0) free tdecl itself */
            if (!ic_decl_type_destroy(&(decl->u.tdecl), 0)) {
                puts("ic_decl_destroy: call to ic_decl_type_destroy failed");
                return 0;
            }
            break;

        case ic_decl_tag_builtin_op:
            if (!ic_decl_op_destroy(&(decl->u.op), 0)) {
                puts("ic_decl_destroy: call to ic_decl_op_destroy failed");
                return 0;
            }
            break;

        default:
            puts("ic_decl_destroy: impossible decl type, aborting");
            return 0;
            break;
    }

    /* caller must determine if we are to free decl itself */
    if (free_decl) {
        free(decl);
    }

    /* success */
    return 1;
}

/* returns pointer to ic_decl_func element
 * this function will only success if the decl is of type decl_func
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_decl_func *ic_decl_get_fdecl(struct ic_decl *decl) {
    if (!decl) {
        puts("ic_decl_get_fdecl: decl was null");
        return 0;
    }

    /* check we are the right type */
    if (decl->tag != ic_decl_tag_func && decl->tag != ic_decl_tag_builtin_func) {
        return 0;
    }

    /* unbox */
    return &(decl->u.fdecl);
}

/* returns pointer to cf_decl_type element
 * this function will only success if the decl is of type decl_type
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_decl_type *ic_decl_get_tdecl(struct ic_decl *decl) {
    if (!decl) {
        puts("ic_decl_get_tdecl: decl was null");
        return 0;
    }

    /* check we are the right type */
    if (decl->tag != ic_decl_tag_type && decl->tag != ic_decl_tag_builtin_type) {
        return 0;
    }

    /* unbox */
    return &(decl->u.tdecl);
}

/* returns pointer to ic_decl_op element
 * this function will only success if the decl is of type decl_op
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_decl_op *ic_decl_get_op(struct ic_decl *decl) {
    if (!decl) {
        puts("ic_decl_get_op: decl was null");
        return 0;
    }

    /* check we are the right type */
    if (decl->tag != ic_decl_tag_builtin_op) {
        return 0;
    }

    /* unbox */
    return &(decl->u.op);
}

/* mark this decl as being a builtin
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_mark_builtin(struct ic_decl *decl) {
    if (!decl) {
        puts("ic_decl_mark_builtin: decl was null");
        return 0;
    }

    switch (decl->tag) {
        case ic_decl_tag_func:
            decl->tag = ic_decl_tag_builtin_func;
            if (!ic_decl_func_mark_builtin(&(decl->u.fdecl))) {
                puts("ic_decl_mark_builtin: call to ic_decl_func_mark_builtin failed");
                return 0;
            }
            break;

        case ic_decl_tag_type:
            decl->tag = ic_decl_tag_builtin_type;
            if (!ic_decl_type_mark_builtin(&(decl->u.tdecl))) {
                puts("ic_decl_mark_builtin: call to ic_decl_type_mark_builtin failed");
                return 0;
            }
            break;

        default:
            puts("ic_decl_mark_builtin: unknown tag");
            return 0;
    }

    return 1;
}

/* print contents of ic_decl */
void ic_decl_print(FILE *fd, struct ic_decl *decl, unsigned int *indent_level) {

    if (!decl) {
        puts("ic_decl_print: decl was null");
        return;
    }
    if (!indent_level) {
        puts("ic_decl_print: indent_level was null");
        return;
    }

    switch (decl->tag) {
        case ic_decl_tag_func:
            ic_decl_func_print(fd, ic_decl_get_fdecl(decl), indent_level);
            break;
        case ic_decl_tag_type:
            ic_decl_type_print(fd, ic_decl_get_tdecl(decl), indent_level);
            break;
        case ic_decl_tag_builtin_func:
            fputs("builtin ", stdout);
            ic_decl_func_print_header(fd, ic_decl_get_fdecl(decl), indent_level);
            break;
        case ic_decl_tag_builtin_type:
            fputs("builtin ", stdout);
            ic_decl_type_print_header(fd, ic_decl_get_tdecl(decl), indent_level);
            break;
        case ic_decl_tag_builtin_op:
            fputs("builtin ", stdout);
            ic_decl_op_print(fd, ic_decl_get_op(decl), indent_level);
            break;

        default:
            puts("ic_decl_print: impossible type!");
            break;
    }
}
