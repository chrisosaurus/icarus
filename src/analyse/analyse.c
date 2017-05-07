#include <stdio.h>
#include <stdlib.h> /* FIMXE exit */
#include <string.h> /* strcmp */

#include "../data/set.h"
#include "../parse/permissions.h"
#include "analyse.h"
#include "data/slot.h"
#include "helpers.h"

/* takes a kludge and performs analysis on it
 *
 * see kludge->errors for errors
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_analyse(struct ic_kludge *kludge) {
    /* our offset into various lists */
    unsigned int i = 0;
    /* cached len of various lists */
    unsigned int len = 0;

    /* the current type we are analysing */
    struct ic_decl_type *tdecl = 0;
    /* the current func we are analysing */
    struct ic_decl_func *fdecl = 0;

    /* steps:
     *      for each type call ic_analyse_decl_type
     *      for each func call ic_analyse_decl_func
     */

    if (!kludge) {
        puts("ic_analyse: kludge was null");
        return 0;
    }

    /* note that this analysis happens after kludge population
     * so all type and function names have been made public
     * this makes it safe for analyse_decl_type to generate functions
     * as long as it first checks they don't already exist
     */

    /* for each type call ic_analyse_decl_type */
    len = ic_pvector_length(&(kludge->tdecls));
    for (i = 0; i < len; ++i) {
        tdecl = ic_pvector_get(&(kludge->tdecls), i);
        if (!tdecl) {
            puts("ic_analyse: call to ic_pvector_get failed for tdecl");
            goto ERROR;
        }

        if (!ic_analyse_decl_type(kludge, tdecl)) {
            puts("ic_analyse: call to ic_analyse_decl_type failed");
            goto ERROR;
        }
    }

    /* for each func call analyse_decl_func */
    len = ic_pvector_length(&(kludge->fdecls));
    for (i = 0; i < len; ++i) {
        fdecl = ic_pvector_get(&(kludge->fdecls), i);
        if (!fdecl) {
            puts("ic_analyse: call to ic_pvector_get failed for fdecl");
            goto ERROR;
        }

        if (!ic_analyse_decl_func(kludge, fdecl)) {
            puts("ic_analyse: call to ic_analyse_decl_func failed");
            goto ERROR;
        }
    }

    /* check for main entry point
     * we must check last as this could have been added as part of an include
     *
     * FIXME may need to support multiple signatures for main
     */
    fdecl = ic_kludge_get_fdecl(kludge, "main()");
    if (!fdecl) {
        puts("ic_analyse: failed to find a main function");
        goto ERROR;
    }

    return 1;

ERROR:
    /* destroy kludge
     * free_kludge as allocated with new
     */
    if (!ic_kludge_destroy(kludge, 1)) {
        puts("ic_analyse: call to ic_kludge_destroy failed in error case");
    }

    return 0;
}

/* takes a decl_type and performs analysis
 *
 * FIXME need a way of signalling and passing errors
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_analyse_decl_type(struct ic_kludge *kludge, struct ic_decl_type *tdecl) {
    struct ic_decl_type_struct *tdecl_struct = 0;
    struct ic_decl_type_union *tdecl_union = 0;

    if (!kludge) {
        puts("ic_analyse_decl_type: kludge was null");
        return 0;
    }

    if (!tdecl) {
        puts("ic_analyse_decl_type: tdecl was null");
        return 0;
    }

    switch (tdecl->tag) {
        case ic_decl_type_tag_struct:
            tdecl_struct = ic_decl_type_get_struct(tdecl);
            if (!tdecl_struct) {
                puts("ic_analsye_decl_type: call to ic_decl_type_get_struct failed");
                return 0;
            }
            if (!ic_analyse_decl_type_struct(kludge, tdecl_struct)) {
                puts("ic_analsye_decl_type: call to ic_analyse_decl_type_struct failed");
                return 0;
            }
            break;

        case ic_decl_type_tag_union:
            tdecl_union = ic_decl_type_get_union(tdecl);
            if (!tdecl_union) {
                puts("ic_analsye_decl_type: call to ic_decl_type_get_union failed");
                return 0;
            }
            if (!ic_analyse_decl_type_union(kludge, tdecl_union)) {
                puts("ic_analsye_decl_type: call to ic_analyse_decl_type_union failed");
                return 0;
            }
            break;

        default:
            puts("ic_analyse_decl_type: unknown tag");
            return 0;
            break;
    }

    /* for non-builtin types we need to make sure that the backend generates
     * an appropriate default constructor for this type
     */
    if (!ic_decl_type_isbuiltin(tdecl)) {
        if (!ic_analyse_decl_type_generate_functions(kludge, tdecl)) {
            puts("ic_analyse_decl_type: call to ic_analyse_decl_type_generate_functions failed");
            return 0;
        }
    }

    return 1;
}

/* generate the needed function (s) for this type
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_analyse_decl_type_generate_functions(struct ic_kludge *kludge, struct ic_decl_type *tdecl) {
    struct ic_decl_type_struct *tdecl_struct = 0;
    struct ic_decl_type_union *tdecl_union = 0;

    if (!kludge) {
        puts("ic_analyse_decl_type_generate_functions: kludge was null");
        return 0;
    }

    if (!tdecl) {
        puts("ic_analyse_decl_type_generate_functions: tdecl was null");
        return 0;
    }

    if (tdecl->builtin != 0) {
        puts("ic_analyse_decl_type_generate_functions: tdecl was builtin");
        return 0;
    }

    switch (tdecl->tag) {
        case ic_decl_type_tag_struct:
            tdecl_struct = ic_decl_type_get_struct(tdecl);
            if (!tdecl_struct) {
                puts("ic_analyse_decl_type_generate_functions: call to ic_decl_type_get_struct failed");
                return 0;
            }
            if (!ic_analyse_decl_type_struct_generate_functions(kludge, tdecl, tdecl_struct)) {
                puts("ic_analyse_decl_type_generate_functions: call to ic_analyse_decl_type_struct_generate_functions failed");
                return 0;
            }
            return 1;
            break;

        case ic_decl_type_tag_union:
            tdecl_union = ic_decl_type_get_union(tdecl);
            if (!tdecl_union) {
                puts("ic_analyse_decl_type_generate_functions: call to ic_decl_type_get_union failed");
                return 0;
            }
            if (!ic_analyse_decl_type_union_generate_functions(kludge, tdecl, tdecl_union)) {
                puts("ic_analyse_decl_type_generate_functions: call to ic_analyse_decl_type_union_generate_functions failed");
                return 0;
            }
            return 1;
            break;

        default:
            puts("ic_analyse_decl_type_generate_functions: unknown tag");
            return 0;
            break;
    }
}

/* takes a decl_type_struct and performs analysis
 *
 * FIXME need a way of signalling and passing errors
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_analyse_decl_type_struct(struct ic_kludge *kludge, struct ic_decl_type_struct *tdecl_struct) {
    /* name of current type we are trying to declare */
    char *this_type = 0;

    /* current offset into field */
    unsigned int i = 0;
    /* cached len of field */
    unsigned int len = 0;
    /* current field */
    struct ic_field *field = 0;

    struct ic_symbol *type_sym = 0;
    char *type_str = 0;
    struct ic_decl_type *field_type = 0;

    if (!kludge) {
        puts("ic_analyse_decl_type_struct: kludge was null");
        return 0;
    }

    if (!tdecl_struct) {
        puts("ic_analyse_decl_type_struct: tdecl_struct was null");
        return 0;
    }

    this_type = ic_decl_type_struct_str(tdecl_struct);
    if (!this_type) {
        puts("ic_analyse_decl_type_struct: for this_type: call to ic_decl_type_struct_str failed");
        return 0;
    }

    /* check fields */
    if (!ic_analyse_field_list("type declaration", this_type, kludge, &(tdecl_struct->fields), this_type)) {
        puts("ic_analyse_decl_type_struct: call to ic_analyse_field_list for field validation failed");
        goto ERROR;
    }

    /* fill in field_dict
     * FIXME this is iterating through fields twice as we already iterate
     *       through once in ic_analyse_field_list
     * FIXME likewise it is also fetching type from string twice
     *       once here, and once in ic_analyse_field_list
     */
    len = ic_pvector_length(&(tdecl_struct->fields));
    for (i = 0; i < len; ++i) {
        field = ic_pvector_get(&(tdecl_struct->fields), i);
        if (!field) {
            puts("ic_analyse_decl_type_struct: call to ic_pvector_get failed");
            goto ERROR;
        }

        /* what we are really doing here is:
         *
         *  a) convert type to string representation
         *  b) checking that this is not a self-recursive type
         *  c) check that this field's type exists
         *
         *  FIXME check / consider this
         *  FIXME duplicated code from analyse_field_list
         *  FIXME do we allow co-recursive types
         *  FIXME what about N-level-recursive types?
         *        Foo includes Bar, Bar includes Baz, Bac includes Foo.
         */
        type_sym = ic_type_ref_get_symbol(&(field->type));
        if (!type_sym) {
            puts("ic_analyse_decl_type_struct: call to ic_type_ref_get_symbol_failed");
            goto ERROR;
        }

        type_str = ic_symbol_contents(type_sym);
        if (!type_str) {
            puts("ic_analyse_decl_type_struct: call to ic_symbol_contents failed");
            goto ERROR;
        }

        /* check that this field's type exists */
        field_type = ic_kludge_get_decl_type(kludge, type_str);
        if (!field_type) {
            puts("ic_analyse_decl_type_struct: call to ic_kludge_get_type failed");
            goto ERROR;
        }

        /* insert this type into field_dict */
        if (!ic_decl_type_struct_add_field_type(tdecl_struct, ic_symbol_contents(&(field->name)), field_type)) {
            puts("ic_analyse_decl_type_struct: call to ic_decl_type_struct_add_field_type failed");
            goto ERROR;
        }
    }

    return 1;

ERROR:

    puts("ic_analyse_decl_type_struct: error");
    return 0;
}

/* generate the needed function(s) for this type
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_analyse_decl_type_struct_generate_functions(struct ic_kludge *kludge, struct ic_decl_type *tdecl, struct ic_decl_type_struct *tdecl_struct) {
    struct ic_symbol *type_sym = 0;
    char *type_str = 0;
    /* length of type_str used for generating cons */
    int type_str_len = 0;

    /* current offset into field */
    unsigned int i = 0;
    /* cached len of field */
    unsigned int len = 0;
    /* current field */
    struct ic_field *field = 0;

    /* func decl for default type constructor */
    struct ic_decl_func *constructor_decl = 0;
    /* func decl for print function */
    struct ic_decl_func *print_decl = 0;
    /* func decl for println function */
    struct ic_decl_func *println_decl = 0;

    struct ic_generate *generate = 0;

    if (!kludge) {
        puts("ic_analyse_decl_type_struct_generate_functions: kludge was null");
        return 0;
    }

    if (!tdecl) {
        puts("ic_analyse_decl_type_struct_generate_functions: tdecl was null");
        return 0;
    }

    if (!tdecl_struct) {
        puts("ic_analyse_decl_type_struct_generate_functions: tdecl_struct was null");
        return 0;
    }

    /* not a builtin, so we need to ensure backend generates a default
     * constructor
     *
     * we also need to insert an appropriate function signature into
     * kludge->dict_fsig so that any calls to this constructor are typed correctly
     */

    type_sym = ic_decl_type_struct_get_name(tdecl_struct);
    if (!type_sym) {
        puts("ic_analyse_decl_type_struct_generate_functions: call to ic_decl_type_struct_get_name failed");
        goto ERROR;
    }

    type_str = ic_symbol_contents(type_sym);
    if (!type_str) {
        puts("ic_analyse_decl_type_struct_generate_functions: call to ic_symbol_contents failed");
        goto ERROR;
    }

    type_str_len = ic_symbol_length(type_sym);
    if (-1 == type_str_len) {
        puts("ic_analyse_decl_type_struct_generate_functions: call to ic_symbol_length failed");
        goto ERROR;
    }

    /* constructor */
    {
        constructor_decl = ic_decl_func_new(type_str, type_str_len);
        if (!constructor_decl) {
            puts("ic_analyse_decl_type_struct_generate_functions: call to ic_decl_func_new failed");
            goto ERROR;
        }

        /* associate constructor return value with type */
        if (!ic_decl_func_set_return(constructor_decl, type_str, ic_symbol_length(type_sym))) {
            puts("ic_analyse_decl_type_struct_generate_functions: call to ic_decl_func_set_return failed");
            goto ERROR;
        }

        /* populate func decl arguments....
         * TODO FIXME this now means that the fields are shared between the fdecl and tdecl
         * TODO FIXME iterating over the fields... once again (3rd time is the charm)
         * TODO FIXME backends do not take this into account
         */
        len = ic_pvector_length(&(tdecl_struct->fields));
        for (i = 0; i < len; ++i) {
            field = ic_pvector_get(&(tdecl_struct->fields), i);
            if (!field) {
                puts("ic_analyse_decl_type_struct_generate_functions: call to ic_pvector_get failed");
                goto ERROR;
            }

            /* note here we are sharing the field between the tdcel and the fdecl */

            if (!ic_decl_func_args_add(constructor_decl, field)) {
                puts("ic_analyse_decl_type_struct_generate_functions: call to ic_decl_func_args_add failed");
                goto ERROR;
            }
        }

        /* for each constructor we need
         * check if already exists (user defined)
         * if not, then add a generate to make it hapen
         * TODO FIXME check if exists before adding
         */

        generate = ic_generate_new(ic_generate_tag_cons_struct, constructor_decl, tdecl);
        if (!generate) {
            puts("ic_analyse_decl_type_struct_generate_functions: call to ic_generate_new failed");
            goto ERROR;
        }

        if (!ic_kludge_generates_add(kludge, generate)) {
            puts("ic_analyse_decl_type_struct_generate_functions: call to ic_kludge_generates_add failed");
            goto ERROR;
        }
    }

    /* field used by both print and println
     * TODO FIXME this means that some fdecls share fields and some own :/
     */
    field = ic_field_new("f", 1, type_str, type_str_len, ic_parse_perm_default());
    if (!field) {
        puts("ic_analyse_decl_type_struct_generate_functions: call to ic_field_new failed");
        goto ERROR;
    }

    /* print */
    {
        print_decl = ic_decl_func_new("print", 5);
        if (!print_decl) {
            puts("ic_analyse_decl_type_struct_generate_functions: call to ic_decl_func_new failed");
            goto ERROR;
        }

        if (!ic_decl_func_args_add(print_decl, field)) {
            puts("ic_analyse_decl_type_struct_generate_functions: call to ic_decl_func_args_add failed");
            goto ERROR;
        }

        if (!ic_decl_func_set_return(print_decl, "Void", 4)) {
            puts("ic_analyse_decl_type_struct_generate_functions: call to ic_decl_func_set_return failed");
            goto ERROR;
        }

        generate = ic_generate_new(ic_generate_tag_print, print_decl, tdecl);
        if (!generate) {
            puts("ic_analyse_decl_type_struct_generate_functions: call to ic_generate_new failed");
            goto ERROR;
        }

        if (!ic_kludge_generates_add(kludge, generate)) {
            puts("ic_analyse_decl_type_struct_generate_functions: call to ic_kludge_generates_add failed");
            goto ERROR;
        }
    }

    /* println */
    {
        println_decl = ic_decl_func_new("println", 7);
        if (!print_decl) {
            puts("ic_analyse_decl_type_struct_generate_functions: call to ic_decl_func_new failed");
            goto ERROR;
        }

        if (!ic_decl_func_args_add(println_decl, field)) {
            puts("ic_analyse_decl_type_struct_generate_functions: call to ic_decl_func_args_add failed");
            goto ERROR;
        }

        if (!ic_decl_func_set_return(println_decl, "Void", 4)) {
            puts("ic_analyse_decl_type_struct_generate_functions: call to ic_decl_func_set_return failed");
            goto ERROR;
        }

        generate = ic_generate_new(ic_generate_tag_println, println_decl, tdecl);
        if (!generate) {
            puts("ic_analyse_decl_type_struct_generate_functions: call to ic_generate_new failed");
            goto ERROR;
        }

        if (!ic_kludge_generates_add(kludge, generate)) {
            puts("ic_analyse_decl_type_struct_generate_functions: call to ic_kludge_generates_add failed");
            goto ERROR;
        }
    }

    /* success */
    return 1;

ERROR:

    if (generate) {
        if (!ic_generate_destroy(generate, 1)) {
            puts("ic_analyse_decl_type_struct_generate_functions: error handling: call to ic_generate_destroy failed");
        }
    }

    puts("ic_analyse_decl_type_struct_generate_functions: error");
    return 0;
}

/* takes a decl_type_union and performs analysis
 *
 * FIXME need a way of signalling and passing errors
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_analyse_decl_type_union(struct ic_kludge *kludge, struct ic_decl_type_union *tdecl_union) {
    /* name of current type we are trying to declare */
    char *this_type = 0;

    /* current offset into field */
    unsigned int i = 0;
    /* cached len of field */
    unsigned int len = 0;
    /* current field */
    struct ic_field *field = 0;

    struct ic_symbol *type_sym = 0;
    char *type_str = 0;
    struct ic_decl_type *field_type = 0;

    /* this is the only part that is different between type_union and
     * type_struct.
     *
     * use this set to check all field types are unique
     */
    struct ic_set *set = 0;

    if (!kludge) {
        puts("ic_analyse_decl_type_union: kludge was null");
        return 0;
    }

    if (!tdecl_union) {
        puts("ic_analyse_decl_type_union: tdecl_union was null");
        return 0;
    }

    this_type = ic_decl_type_union_str(tdecl_union);
    if (!this_type) {
        puts("ic_analyse_decl_type_union: for this_type: call to ic_decl_type_union_str failed");
        return 0;
    }

    /* check fields */
    if (!ic_analyse_field_list("type declaration", this_type, kludge, &(tdecl_union->fields), this_type)) {
        puts("ic_analyse_decl_type_union: call to ic_analyse_field_list for field validation failed");
        goto ERROR;
    }

    set = ic_set_new();
    if (!set) {
        puts("ic_analyse_decl_type_union: call to ic_set_new failed");
        goto ERROR;
    }

    /* fill in field_dict
     * FIXME this is iterating through fields twice as we already iterate
     *       through once in ic_analyse_field_list
     * FIXME likewise it is also fetching type from string twice
     *       once here, and once in ic_analyse_field_list
     */
    len = ic_pvector_length(&(tdecl_union->fields));
    for (i = 0; i < len; ++i) {
        field = ic_pvector_get(&(tdecl_union->fields), i);
        if (!field) {
            puts("ic_analyse_decl_type_union: call to ic_pvector_get failed");
            goto ERROR;
        }

        /* what we are really doing here is:
         *
         *  a) convert type to string representation
         *  b) checking that this is not a self-recursive type
         *  c) check that this field's type exists
         *  d) check that all field's types are unique
         *
         *  FIXME check / consider this
         *  FIXME duplicated code from analyse_field_list
         *  FIXME do we allow co-recursive types
         *  FIXME what about N-level-recursive types?
         *        Foo includes Bar, Bar includes Baz, Bac includes Foo.
         */
        type_sym = ic_type_ref_get_symbol(&(field->type));
        if (!type_sym) {
            puts("ic_analyse_decl_type_union: call to ic_type_ref_get_symbol_failed");
            goto ERROR;
        }

        type_str = ic_symbol_contents(type_sym);
        if (!type_str) {
            puts("ic_analyse_decl_type_union: call to ic_symbol_contents failed");
            goto ERROR;
        }

        if (ic_set_exists(set, type_str)) {
            printf("ic_analyse_decl_type_union: field with type '%s' already existed on this union\n", type_str);
            goto ERROR;
        }

        if (!ic_set_insert(set, type_str)) {
            puts("ic_analyse_decl_type_union: call to ic_set_insert failed");
            goto ERROR;
        }

        /* check that this field's type exists */
        field_type = ic_kludge_get_decl_type(kludge, type_str);
        if (!field_type) {
            puts("ic_analyse_decl_type_union: call to ic_kludge_get_type failed");
            goto ERROR;
        }

        /* insert this type into field_dict */
        if (!ic_decl_type_union_add_field_type(tdecl_union, ic_symbol_contents(&(field->name)), field_type)) {
            puts("ic_analyse_decl_type_union: call to ic_decl_type_struct_add_field_type failed");
            goto ERROR;
        }
    }

    if (set) {
        if (!ic_set_destroy(set, 1)) {
            puts("ic_analyse_decl_type_union: call to ic_set_destroy failed");
            goto ERROR;
        }
    }

    return 1;

ERROR:

    if (set) {
        if (!ic_set_destroy(set, 1)) {
            puts("ic_analyse_decl_type_union: error handler: call to ic_set_destroy failed");
        }
    }

    puts("ic_analyse_decl_type_union: error");
    return 0;
}

/* generate the constructor(s) for this type
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_analyse_decl_type_union_generate_functions(struct ic_kludge *kludge, struct ic_decl_type *tdecl, struct ic_decl_type_union *tdecl_union) {
    if (!kludge) {
        puts("ic_analyse_decl_type_union_generate_functions: kludge was null");
        return 0;
    }

    if (!tdecl) {
        puts("ic_analyse_decl_type_union_generate_functions: tdecl was null");
        return 0;
    }

    if (!tdecl_union) {
        puts("ic_analyse_decl_type_union_generate_functions: tdecl_union was null");
        return 0;
    }

    puts("ic_analyse_decl_type_union_generate_functions: not implemented yet");
    return 0;
}

/* takes a decl_func and performs analysis
 *
 * FIXME need a way of signalling and passing errors
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_analyse_decl_func(struct ic_kludge *kludge, struct ic_decl_func *fdecl) {
    /* name of current func we are trying to declare */
    char *this_func = 0;
    /* our scope */
    struct ic_scope *scope = 0;

    /* current arg */
    struct ic_field *arg = 0;
    /* index into args */
    unsigned int i = 0;
    /* length of args */
    unsigned int len = 0;
    /* arg type */
    struct ic_decl_type *arg_type = 0;
    /* slot created for arg */
    struct ic_slot *slot = 0;

    int ret = 0;

    if (!kludge) {
        puts("ic_analyse_decl_func: kludge was null");
        return 0;
    }

    if (!fdecl) {
        puts("ic_analyse_decl_func: fdecl was null");
        return 0;
    }

    /* name of this func, useful for error printing */
    this_func = ic_decl_func_sig_call(fdecl);
    if (!this_func) {
        puts("ic_analyse_decl_func: for this_type: call to ic_decl_func_sig_call failed");
        return 0;
    }

    /* check arg list */
    if (!ic_analyse_field_list("func declaration", this_func, kludge, &(fdecl->args), 0)) {
        puts("ic_analyse_decl_func: call to ic_analyse_field_list for validating argument list failed");
        goto ERROR;
    }

    /* new scope with no parent
     * FIXME this scope is currently leaked
     */
    scope = ic_scope_new(0);
    if (!scope) {
        puts("ic_analyse_decl_func: call to ic_scope_new failed");
        goto ERROR;
    }

    if (fdecl->body.scope) {
        puts("ic_analyse_decl_func: scope was already set on body");
        goto ERROR;
    }

    /* store scope
     * FIXME this scope is currently leaked
     */
    fdecl->body.scope = scope;

    /* insert each arg as a slot into a scope
     * FIXME this is gross as it means we are iterating through args twice
     * once above for ic_analyse_field_list
     * and again here
     */
    len = ic_pvector_length(&(fdecl->args));

    for (i = 0; i < len; ++i) {
        arg = ic_pvector_get(&(fdecl->args), i);
        if (!arg) {
            puts("ic_analyse_decl_func: call to ic_pvector_get failed");
            goto ERROR;
        }

        /* check each argument name does not shadow an existing identifier */
        ret = ic_kludge_identifier_exists_symbol(kludge, 0, &(arg->name));
        if (1 == ret) {
            printf("ic_analyse_decl_func: arg identifier already exists '%s'\n", ic_symbol_contents(&(arg->name)));
            return 0;
        }
        if (0 != ret) {
            printf("ic_analyse_decl_func: error checking for arg identifier existence '%s'\n", ic_symbol_contents(&(arg->name)));
            return 0;
        }

        /* get arg type */
        arg_type = ic_kludge_get_decl_type_from_typeref(kludge, &(arg->type));
        if (!arg_type) {
            puts("ic_analyse_decl_func: call to ic_kludge_get_decl_type_from_typeref failed");
            goto ERROR;
        }

        /* create slot
         * args are not mutable so always 0
         * references are not yet supported so 0 FIXME
         */
        slot = ic_slot_new(&(arg->name), arg_type, 0, 0, ic_slot_type_arg, arg);
        if (!slot) {
            puts("ic_analyse_decl_func: call to ic_slot_new failed");
            goto ERROR;
        }

        /* insert slot into scope */
        if (!ic_scope_insert_symbol(scope, &(arg->name), slot)) {
            puts("ic_analyse_decl_func: call to ic_scope_insert_symbol failed");
            goto ERROR;
        }

        slot = 0;
    }

    /* check body */
    if (!ic_analyse_body("func declaration", this_func, kludge, &(fdecl->body), fdecl)) {
        puts("ic_analyse_decl_func: call to ic_analyse_body for validating body failed");
        goto ERROR;
    }

    return 1;

ERROR:

    puts("ic_analyse_decl_func: error");
    return 0;
}
