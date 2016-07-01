#include <stdio.h>
#include <stdlib.h> /* FIMXE exit */
#include <string.h> /* strcmp */

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
    struct ic_type *field_type = 0;

    if (!kludge) {
        puts("ic_analyse_decl_type: kludge was null");
        return 0;
    }

    if (!tdecl) {
        puts("ic_analyse_decl_type: tdecl was null");
        return 0;
    }

    this_type = ic_decl_type_str(tdecl);
    if (!this_type) {
        puts("ic_analyse_decl_type: for this_type: call to ic_decl_type_str failed");
        return 0;
    }

    /* check fields */
    if (!ic_analyse_field_list("type declaration", this_type, kludge, &(tdecl->fields), this_type)) {
        puts("ic_analyse_decl_type: call to ic_analyse_field_list for field validation failed");
        goto ERROR;
    }

    /* fill in field_dict
     * FIXME this is iterating through fields twice
     * FIXME it is also fetching type from string twice
     */
    len = ic_pvector_length(&(tdecl->fields));
    for (i = 0; i < len; ++i) {
        field = ic_pvector_get(&(tdecl->fields), i);
        if (!field) {
            puts("ic_analyse_field_list: call to ic_pvector_get failed");
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
         */
        type_sym = ic_type_ref_get_symbol(&(field->type));
        if (!type_sym) {
            puts("ic_analyse_decl_type call to ic_type_ref_get_symbol_failed");
            goto ERROR;
        }

        type_str = ic_symbol_contents(type_sym);
        if (!type_str) {
            puts("ic_analyse_decl_type call to ic_symbol_contents failed");
            goto ERROR;
        }

        /* check that this field's type exists */
        field_type = ic_kludge_get_type(kludge, type_str);
        if (!field_type) {
            puts("ic_analyse_decl_type call to ic_kludge_get_type failed");
            goto ERROR;
        }

        /* insert this type into field_dict */
        if (!ic_decl_type_add_field_type(tdecl, ic_symbol_contents(&(field->name)), field_type)) {
            puts("ic_analyse_decl_type call to ic_decl_type_add_field_type failed");
            goto ERROR;
        }
    }

    return 1;

ERROR:

    puts("ic_analyse_decl_type: error");
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
    struct ic_type *arg_type = 0;
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
        arg_type = ic_kludge_get_type_from_typeref(kludge, &(arg->type));
        if (!arg_type) {
            puts("ic_analyse_decl_func: call to ic_kludge_get_type_from_typeref failed");
            goto ERROR;
        }

        /* create slot
         * args are not mutable so always 0
         * references are not yet supported so 0 FIXME
         */
        slot = ic_slot_new(&(arg->name), arg_type, 0, 0);
        if (!slot) {
            puts("ic_analyse_decl_func: call to ic_slot_new failed");
            goto ERROR;
        }

        /* insert slot into scope */
        if (!ic_scope_insert(scope, ic_symbol_contents(&(arg->name)), slot)) {
            puts("ic_analyse_decl_func: call to ic_scope_insert failed");
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
