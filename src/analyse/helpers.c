#include <stdio.h>
#include <stdlib.h> /* FIMXE exit */
#include <string.h> /* strcmp */

#include "../data/set.h"
#include "../parse/data/stmt.h"
#include "../parse/permissions.h"
#include "data/slot.h"
#include "helpers.h"
#include "analyse.h"

/* mark this expr as being on the left-hand side of an assignment
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_analyse_mark_assigned(struct ic_scope *scope, struct ic_expr *expr) {
    struct ic_expr_identifier *id = 0;
    struct ic_symbol *sym = 0;
    struct ic_slot *slot = 0;
    enum ic_slot_assign_result assign_result;

    if (!scope) {
        puts("ic_analyse_mark_assigned: scope was null");
        return 0;
    }

    if (!expr) {
        puts("ic_analyse_mark_assigned: expr was null");
        return 0;
    }

    if (expr->tag != ic_expr_type_identifier) {
        puts("ic_analyse_mark_assigned: expr->tag was not identifier: unsupported assignment target");
        return 0;
    }

    id = ic_expr_get_identifier(expr);
    if (!id) {
        puts("ic_analyse_mark_assigned: call to ic_expr_get_identifier failed");
        return 0;
    }

    sym = &(id->identifier);

    slot = ic_scope_get_from_symbol(scope, sym);
    if (!slot) {
        puts("ic_analyse_mark_assigned: call to ic_scope_get_from_symbol failed");
        return 0;
    }

    assign_result = ic_slot_assign(slot);
    switch (assign_result) {
        case ic_slot_assign_result_internal_error:
            puts("ic_analyse_mark_assigned: assignment failed due to internal error");
            return 0;
            break;

        case ic_slot_assign_result_success:
            /* victory */
            return 1;

        case ic_slot_assign_result_permission_denied:
            puts("ic_analyse_mark_assigned: assignment failed due to permission denied");
            return 0;
            break;

        default:
            puts("ic_analyse_mark_assigned: assignment result was of unknown type");
            return 0;
            break;
    }
}

/* iterate through the field list checking:
 *  a) all field's names are unique within this list
 *  b) all field's types exist in this kludge
 *
 * `unit` and `unit_name` are used for error printing
 * it is always printed as '%s for %s error goes here'
 * e.g. unit of 'function declaration', name of 'Foo'
 *
 * returns 1 on success (all fields are valid as per the 3 rules)
 * returns 0 on failure
 */
unsigned int ic_analyse_field_list(char *unit, char *unit_name, struct ic_kludge *kludge, struct ic_pvector *type_params, struct ic_pvector *fields) {

    /* index into fields */
    unsigned int i = 0;
    /* len of fields */
    unsigned int len = 0;
    /* current field */
    struct ic_field *field = 0;
    /* current name from field */
    char *name = 0;
    /* current type from field */
    struct ic_symbol *type = 0;
    /* current type from field as string */
    char *type_str = 0;
    /* set of all field names used */
    struct ic_set *set = 0;
    /* for each field this captures the type we resolve it to */
    struct ic_decl_type *field_type = 0;
    /* type param we found when looking up generic argument */
    struct ic_type_param *type_param = 0;

    if (!unit) {
        puts("ic_analyse_field_list: unit was null");
        return 0;
    }

    if (!unit_name) {
        puts("ic_analyse_field_list: unit_name was null");
        return 0;
    }

    if (!kludge) {
        puts("ic_analyse_field_list: kludge was null");
        return 0;
    }

    /* type_params can be null */

    if (!fields) {
        puts("ic_analyse_field_list: field was null");
        return 0;
    }

    set = ic_set_new();
    if (!set) {
        puts("ic_analyse_field_list: call to ic_set_new failed");
        return 0;
    }

    len = ic_pvector_length(fields);
    for (i = 0; i < len; ++i) {
        field = ic_pvector_get(fields, i);
        if (!field) {
            puts("ic_analyse_field_list: call to ic_pvector_get failed");
            goto ERROR;
        }

        /* this is a char* to the inside of symbol
         * so we do not need to worry about free-ing this
         */
        name = ic_symbol_contents(&(field->name));
        if (!name) {
            puts("ic_analyse_field_list: call to ic_symbol_contents failed for name");
            goto ERROR;
        }

        /* check name is unique within this type decl
         * ic_set_insert makes a strdup copy
         */
        if (!ic_set_insert(set, name)) {
            printf("ic_analyse_field_list: field name '%s' it not unique within '%s' for '%s'\n",
                   name,
                   unit,
                   unit_name);
            goto ERROR;
        }

        /* what we are really doing here is:
         *  a) convert type to string representation
         *  b) checking that this is not a self-recursive type
         *  c) check that this field's type exists
         *  d) check this is not the void type
         *
         *  FIXME check / consider this
         */
        type = ic_type_ref_get_symbol(field->type);
        if (!type) {
            printf("ic_analyse_field_list: call to ic_type_ref_get_symbol failed for field '%s' in '%s' for '%s'\n",
                   name,
                   unit,
                   unit_name);
            goto ERROR;
        }

        type_str = ic_symbol_contents(type);
        if (!type_str) {
            printf("ic_analyse_field_list: call to ic_symbol_contents failed for field '%s' in '%s' for '%s'\n",
                   name,
                   unit,
                   unit_name);
            goto ERROR;
        }

        /* check that this field's type exists */
        /* if we have type_params then check there first */
        if (type_params && (type_param = ic_type_param_search(type_params, type_str))) {
            /* found
             *
             * if the found type_param is set (resolved),
             * then we also want to also set our type_ref
             * else (not resolved), we should leave it as we will
             *      later on instantiate this * AFTER performing a deep copy
             *      (which would invalidate any type_param *)
             */

            if (ic_type_param_check_set(type_param)) {
                field_type = ic_type_param_get(type_param);
                if (!field_type) {
                    puts("ic_analyse_field_list: call to ic_type_param_get failed");
                    goto ERROR;
                }

                if (!ic_type_ref_set_type_decl(field->type, field_type)) {
                    printf("ic_analyse_field_list: trying to store param for '%s' on field '%s' during '%s' for '%s' failed\n",
                           type_str,
                           name,
                           unit,
                           unit_name);
                    goto ERROR;
                }
            }

        } else {
            /* otherwise check in kludge */
            field_type = ic_kludge_get_decl_type(kludge, type_str);
            if (!field_type) {
                printf("ic_analyse_field_list: type '%s' mentioned in '%s' for '%s' does not exist within this kludge\n",
                       type_str,
                       unit,
                       unit_name);
                goto ERROR;
            }

            /* check this is not the void type */
            if (ic_decl_type_isvoid(field_type)) {
                puts("ic_analyse_field_list: void type used in field list");
                goto ERROR;
            }

            /* only store if type_ref is not already resolved
             * this happens for generic instantiated functions which have already
             * been analysed
             *
             * this double-handling might be a problem
             */
            if (field->type->tag != ic_type_ref_resolved) {
                /* store that type decl on the field (to save lookup costs again later)
                 * if field->type is already a tdecl this will blow up
                 */
                if (!ic_type_ref_set_type_decl(field->type, field_type)) {
                    printf("ic_analyse_field_list: trying to store tdecl for '%s' on field '%s' during '%s' for '%s' failed\n",
                           type_str,
                           name,
                           unit,
                           unit_name);
                    goto ERROR;
                }
            }
        }
    }

    /* call set destroy
     * free_set as we allocated above with new
     */
    if (!ic_set_destroy(set, 1)) {
        puts("ic_analyse_field_list: call to ic_set_destroy failed in error case");
    }

    return 1;

ERROR:

    /* call set destroy
     * free_set as we allocated above with new
     */
    if (!ic_set_destroy(set, 1)) {
        puts("ic_analyse_field_list: call to ic_set_destroy failed in error case");
    }

    return 0;
}

/* resolve a type_refs to type_decls
 *
 * if type_ref to a type_param then this link is used
 * if type_ref of a symbol, then it will be looked up in type_params
 * if both fail, it is an error
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_resolve_type_ref(char *unit, char *unit_name, struct ic_pvector *type_params, struct ic_type_ref *type_ref) {
    /* out field's type_param */
    struct ic_type_param *type_param = 0;
    /* our resolved type_decl */
    struct ic_decl_type *decl_type = 0;
    /* symbol on type_ref */
    struct ic_symbol *type_ref_symbol = 0;
    /* string for symbol on type_ref */
    char *type_ref_str = 0;

    if (!unit) {
        puts("ic_resolve_type_ref: unit was null");
        return 0;
    }

    if (!unit_name) {
        puts("ic_resolve_type_ref: unit_name was null");
        return 0;
    }

    if (!type_params) {
        puts("ic_resolve_type_ref: type_params was null");
        return 0;
    }

    if (!type_ref) {
        puts("ic_resolve_type_ref: type_ref was null");
        return 0;
    }

    switch (type_ref->tag) {
        case ic_type_ref_unknown:
            puts("ic_resolve_type_ref: field->type was unknown, internal error");
            return 0;
            break;

        /* encountering symbols at this point
         * we ideally want the binding to type_param to happen before here
         *
         * when can we link our type_ref to a type_param?
         * if we do it by pointer before deep_copy then we have problems
         *
         * for now we use our type_params list
         * maybe this work could be done better
         */
        case ic_type_ref_symbol:
            /* resolve our symbol to a type param */

            /* first get our symbol out */
            type_ref_symbol = ic_type_ref_get_symbol(type_ref);
            if (!type_ref_symbol) {
                puts("ic_resolve_type_ref: call to ic_type_ref_get_symbol failed");
                return 0;
            }

            /* convert symbol to char* */
            type_ref_str = ic_symbol_contents(type_ref_symbol);
            if (!type_ref_str) {
                puts("ic_resolve_type_ref: call to ic_symbol_contents failed");
                return 0;
            }

            /* find the matching type_param */
            type_param = ic_type_param_search(type_params, type_ref_str);
            if (!type_param) {
                puts("ic_resolve_type_ref: call to ic_type_param_search failed");
                return 0;
            }

            /* reuse type_param case to resolve our type_param to a decl_type */
            goto RESOLVE_LIST_PARAM;
            break;

        case ic_type_ref_param:
            /* get our type_param */
            type_param = ic_type_ref_get_type_param(type_ref);
            if (!type_param) {
                puts("ic_resolve_type_ref: call to ic_type_ref_get_type_param failed");
                return 0;
            }

RESOLVE_LIST_PARAM:

            /* check type param is already resolved */
            if (!ic_type_param_check_set(type_param)) {
                puts("ic_resolve_type_ref: type_param was not set, internal error");
                return 0;
            }

            /* get out type decl */
            decl_type = ic_type_param_get(type_param);
            if (!decl_type) {
                puts("ic_resolve_type_ref: call to ic_type_param_get failed");
                return 0;
            }

            /* set our type_ref to refer to decl_type directly */
            if (!ic_type_ref_set_type_decl(type_ref, decl_type)) {
                puts("ic_resolve_type_ref: call to ic_type_ref_set_type_decl failed");
                return 0;
            }

            /* victory */
            break;

        case ic_type_ref_resolved:
            /* nothing to do */
            break;

        default:
            puts("ic_resolve_type_ref: field->type was unknown tag, internal error");
            return 0;
            break;
    }

    return 1;
}

/* iterate through the field list resolving any type_refs to type_decls
 *
 * if a field is a type_ref to a type_param then this link is used
 * if a field is a type_ref of a symbol, then it will be looked up in type_params
 * if both fail, it is an error
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_resolve_field_list(char *unit, char *unit_name, struct ic_pvector *type_params, struct ic_pvector *fields) {
    /* index into fields */
    unsigned int i = 0;
    /* len of fields */
    unsigned int len = 0;
    /* current field */
    struct ic_field *field = 0;
    /* type_ref of current field */
    struct ic_type_ref *field_type_ref = 0;

    if (!unit) {
        puts("ic_resolve_field_list: unit was null");
        return 0;
    }

    if (!unit_name) {
        puts("ic_resolve_field_list: unit_name was null");
        return 0;
    }

    if (!type_params) {
        puts("ic_resolve_field_list: type_params was null");
        return 0;
    }

    if (!fields) {
        puts("ic_resolve_field_list: fields was null");
        return 0;
    }

    len = ic_pvector_length(fields);
    for (i=0; i<len; ++i) {
        field = ic_pvector_get(fields, i);
        if (!field) {
            puts("ic_resolve_field_list: call to ic_pvector_get failed");
            return 0;
        }

        field_type_ref = field->type;
        if (!field_type_ref) {
            puts("ic_resolve_field_list: field->type ref was null");
            return 0;
        }

        if (!ic_resolve_type_ref(unit, unit_name, type_params, field_type_ref)) {
            puts("ic_resolve_field_list: call to ic_resolve_type_ref failed");
            return 0;
        }

    }

    return 1;
}

/* perform analysis on body
 * this will iterate through each statement and perform analysis
 *
 * `unit` and `unit_name` are used for error printing
 * it is always printed as '%s for %s error goes here'
 * e.g. unit of 'function declaration', name of 'Foo'
 *
 * fdecl is the function containing this body
 *
 * returns 1 on success (pass)
 * returns 0 on failure
 */
unsigned int ic_analyse_body(char *unit, char *unit_name, struct ic_kludge *kludge, struct ic_body *body, struct ic_decl_func *fdecl) {
    /* index into body */
    unsigned int i_stmt = 0;
    /* len of body */
    unsigned int n_stmts = 0;
    /* current statement in body */
    struct ic_stmt *stmt = 0;
    /* expr from stmt */
    struct ic_expr *expr = 0;
    /* type  expression */
    struct ic_decl_type *type = 0;
    /* another type when we need to juggle */
    struct ic_decl_type *other_type = 0;
    /* ret from stmt */
    struct ic_stmt_ret *ret = 0;
    /* begin from stmt */
    struct ic_stmt_begin *begin = 0;
    /* if from stmt */
    struct ic_stmt_if *sif = 0;
    /* assign from stmt */
    struct ic_stmt_assign *assign = 0;
    /* new scope for body of if */
    struct ic_scope *if_scope = 0;
    /* new scope for body of begin */
    struct ic_scope *begin_scope = 0;
    /* match from stmt */
    struct ic_stmt_match *match = 0;
    /* set used to check match
     * name's of all the fields we match again
     */
    struct ic_set *field_set = 0;
    /* case from stmt */
    struct ic_stmt_case *scase = 0;
    /* field used in match */
    struct ic_field *field = 0;
    struct ic_symbol *field_name_sym = 0;
    char *field_name_char = 0;
    struct ic_decl_type *field_type = 0;
    struct ic_decl_type *fetched_field_type = 0;
    unsigned int match_cases = 0;
    struct ic_scope *match_scope = 0;
    struct ic_slot *slot = 0;
    /* index into cases */
    unsigned int i_case = 0;
    /* number of cases */
    unsigned int n_cases = 0;
    struct ic_symbol *ret_type_sym = 0;

    unsigned int fake_indent = 1;

    if (!unit) {
        puts("ic_analyse_body: unit was null");
        return 0;
    }

    if (!unit_name) {
        puts("ic_analyse_body: unit_name was null");
        return 0;
    }

    if (!kludge) {
        puts("ic_analyse_body: kludge was null");
        return 0;
    }

    if (!body) {
        puts("ic_analyse_body: body was null");
        return 0;
    }

    if (!body->scope) {
        puts("ic_analyse_body: body did not have a scope");
        return 0;
    }

    /* step through body checking each statement
     * FIXME
     */
    n_stmts = ic_body_length(body);
    for (i_stmt = 0; i_stmt < n_stmts; ++i_stmt) {
        stmt = ic_body_get(body, i_stmt);
        if (!stmt) {
            printf("ic_analyse_body: call to ic_body_get failed for i '%d' in '%s' for '%s'\n",
                   i_stmt,
                   unit,
                   unit_name);
            goto ERROR;
        }

        /* FIXME in all cases we must do some work:
         *  1) if fcall then check exists, and bind
         *  2) if variable used then check type, and bind
         */
        switch (stmt->tag) {
            case ic_stmt_type_ret:
                /* infer type of expression
                 * check returned value matches declared return type
                 */
                ret = ic_stmt_get_ret(stmt);
                if (!ret) {
                    puts("ic_analyse_body: call ot ic_stmt_get_ret failed");
                    goto ERROR;
                }

                expr = ret->ret;
                if (!expr) {
                    puts("ic_analyse_body: failed to pull out ret expr");
                    goto ERROR;
                }

                /* type actually returned */
                type = ic_analyse_infer(kludge, body->scope, expr);
                if (!type) {
                    /* Void is a concrete type so type being null is an error */
                    puts("ic_analyse_body: failed to infer returned type");
                    goto ERROR;
                }

                ret_type_sym = ic_type_ref_get_type_name(&(fdecl->ret_type));
                if (!ret_type_sym) {
                    puts("ic_analyse_body: call to ic_type_ref_get_type_name failed");
                    goto ERROR;
                }

                other_type = ic_kludge_get_decl_type_from_symbol(kludge, ret_type_sym);
                if (!other_type) {
                    puts("ic_analyse_body: call to ic_kludge_get_decl_from_symbol failed");
                    printf("ic_analyse_body: call to ic_kludge_get_decl_from_symbol failed for symbol '%s'\n", ic_symbol_contents(ret_type_sym));
                    printf("ic_analyse_body: for function: ");
                    ic_decl_func_print_header(stdout, fdecl, &fake_indent);
                    goto ERROR;
                }

                /* compare to declared return type */
                if (!ic_decl_type_equal(type, other_type)) {
                    /* Void is a concrete type so type being null is an error */
                    puts("ic_analyse_body: ret: returned type did not match declared");
                    goto ERROR;
                }

                break;

            case ic_stmt_type_let:
                /* infer type of init. expression
                 * check against declared let type (if declared)
                 * check all mentioned types exist
                 *
                 * check that this let isn't masking an existing slot in scope
                 * create a new slot for this let and insert it into the current scope
                 */
                if (!ic_analyse_let(unit, unit_name, kludge, body, ic_stmt_get_let(stmt))) {
                    puts("ic_analyse_body: call to ic_analyse_let failed");
                    goto ERROR;
                }

                break;

            case ic_stmt_type_begin:
                /* need to introduce new scope
                 * then analyse body
                 */

                /* pull out begin stmt */
                begin = ic_stmt_get_begin(stmt);
                if (!begin) {
                    puts("ic_analyse_body: begin: call to ic_stmt_get_begin failed");
                    return 0;
                }

                /* check begin body
                 * and if produces a new scope, so we must construct one and attach it
                 * FIXME this scope is leaked
                 */
                begin_scope = ic_scope_new(body->scope);
                if (!begin_scope) {
                    puts("ic_analyse_body: begin: call to ic_scope_new failed");
                    goto ERROR;
                }

                if (!begin->body) {
                    puts("ic_analyse_body: begin: begin statement had no body");
                    goto ERROR;
                }

                /* attach new scope to if body
                 * FIXME this scope is leaked */
                begin->body->scope = begin_scope;

                /* analyse body of if */
                if (!ic_analyse_body(unit, unit_name, kludge, begin->body, fdecl)) {
                    puts("ic_analyse_body: begin: ic_analyse_body failed");
                    goto ERROR;
                }

                break;

            case ic_stmt_type_if:
                /* need to validate expression
                 * need to the recurse to validate the body in
                 * each branch
                 */

                /* pull out if stmt */
                sif = ic_stmt_get_sif(stmt);
                if (!sif) {
                    puts("ic_analyse_body: if: call to ic_stmt_get_sif failed");
                    goto ERROR;
                }

                /* pull out if expr */
                expr = sif->expr;
                if (!expr) {
                    puts("ic_analyse_body: if: no expr found in if");
                    goto ERROR;
                }

                /* check expr and get type */
                type = ic_analyse_infer(kludge, body->scope, expr);
                if (!type) {
                    puts("ic_analyse_body: if: failed to infer type of condition expr");
                    goto ERROR;
                }

                /* check if this expression is void
                 * as a void has no boolean interpretation
                 */
                if (ic_decl_type_isvoid(type)) {
                    puts("ic_analyse_body: if: void expression used as if condition");
                    ic_decl_type_print_debug(stdout, type);
                    goto ERROR;
                }

                /* this expression must be of type bool */
                if (!ic_decl_type_isbool(type)) {
                    puts("ic_analyse_body: if: expression was not of type bool");
                    ic_decl_type_print_debug(stdout, type);
                    goto ERROR;
                }

                /* check if body
                 * and if produces a new scope, so we must construct one and attach it
                 * FIXME this scope is leaked
                 */
                if_scope = ic_scope_new(body->scope);
                if (!if_scope) {
                    puts("ic_analyse_body: if: call to ic_scope_new failed");
                    goto ERROR;
                }

                if (!sif->then_body) {
                    puts("ic_analyse_body: if: if statement had no body");
                    goto ERROR;
                }

                /* attach new scope to if body
                 * FIXME this scope is leaked */
                sif->then_body->scope = if_scope;

                /* analyse body of if */
                if (!ic_analyse_body(unit, unit_name, kludge, sif->then_body, fdecl)) {
                    puts("ic_analyse_body: if: ic_analyse_body failed");
                    goto ERROR;
                }

                /* else is optional */
                if (sif->else_body) {
                    /* attach new scope to if else body
                     * FIXME this scope is leaked
                     **/
                    if_scope = ic_scope_new(body->scope);
                    if (!if_scope) {
                        puts("ic_analyse_body: if: call to ic_scope_new failed");
                        goto ERROR;
                    }

                    sif->else_body->scope = if_scope;

                    /* analyse body of if */
                    if (!ic_analyse_body(unit, unit_name, kludge, sif->else_body, fdecl)) {
                        puts("ic_analyse_body: if: ic_analyse_body failed");
                        goto ERROR;
                    }
                }

                break;

            case ic_stmt_type_for:
                puts("ic_analyse_body: 'for' analysis is currently unsupported");
                goto ERROR;
                break;

            case ic_stmt_type_while:
                puts("ic_analyse_body: 'while' analysis is currently unsupported");
                goto ERROR;
                break;

            case ic_stmt_type_assign:
                /* a = b
                 * we need to know that both a and b are of the same type
                 *
                 * FIXME eventually we will need to take permissions into account
                 */

                /* pull out assignment stmt */
                assign = ic_stmt_get_assign(stmt);
                if (!assign) {
                    puts("ic_analyse_body: assign: call to ic_stmt_get_assign failed");
                    goto ERROR;
                }

                /* left expr */
                expr = ic_stmt_assign_get_left(assign);
                if (!expr) {
                    puts("ic_analyse_body: assign: call to ic_stmt_assign_get_left failed");
                    goto ERROR;
                }

                if (!ic_analyse_mark_assigned(body->scope, expr)) {
                    puts("ic_analyse_body: assign: call to ic_analyse_mark_assigned failed");
                    goto ERROR;
                }

                /* get type of left */
                type = ic_analyse_infer(kludge, body->scope, expr);
                if (!type) {
                    puts("ic_analyse_body: assign: call to ic_analyse_infer failed");
                    goto ERROR;
                }

                /* if either type is void then this is an error */
                if (ic_decl_type_isvoid(type)) {
                    puts("ic_analyse_body: assign: attempt to assign to void variable");
                    goto ERROR;
                }

                /* right expr */
                expr = ic_stmt_assign_get_right(assign);
                if (!expr) {
                    puts("ic_analyse_body: assign: call to ic_stmt_assign_get_right failed");
                    goto ERROR;
                }

                /* get type of right */
                other_type = ic_analyse_infer(kludge, body->scope, expr);
                if (!other_type) {
                    puts("ic_analyse_body: assign: call to ic_analyse_infer failed");
                    goto ERROR;
                }

                /* if either type is void then this is an error */
                if (ic_decl_type_isvoid(other_type)) {
                    puts("ic_analyse_body: assign: attempt to assign void value");
                    goto ERROR;
                }

                /* both types must be the same */
                if (!ic_decl_type_equal(type, other_type)) {
                    puts("ic_analyse_body: assign: assignment between invalid types");
                    goto ERROR;
                }

                break;

            case ic_stmt_type_expr:
                /* infer expr type */
                expr = ic_stmt_get_expr(stmt);
                if (!expr) {
                    puts("ic_analyse_body: expr: call to ic_stmt_get_expr failed");
                    goto ERROR;
                }

                type = ic_analyse_infer(kludge, body->scope, expr);
                if (!type) {
                    puts("ic_analyse_body: expr: call to ic_analyse_infer failed");
                    goto ERROR;
                }

                /* check if type is non-void so we can warn */
                if (!ic_decl_type_isvoid(type)) {
                    /* warn about non-void in void context
                     * FIXME make this more useful
                     */
                    puts("Warning: usage of non-void expression in void context");
                }

                break;

            case ic_stmt_type_match:
                match = ic_stmt_get_match(stmt);
                if (!match) {
                    puts("ic_analyse_body: match: call to ic_stmt_get_match failed");
                    goto ERROR;
                }

                expr = ic_stmt_match_get_expr(match);
                if (!expr) {
                    puts("ic_analyse_body: match: call to ic_stmt_match_get_expr failed");
                    goto ERROR;
                }

                type = ic_analyse_infer(kludge, body->scope, expr);
                if (!type) {
                    puts("ic_analyse_body: match: call to ic_analyse_infer failed");
                    goto ERROR;
                }

                if (type->tag != ic_decl_type_tag_union) {
                    puts("ic_analyse_body: match: illegal match on non-union");
                    goto ERROR;
                }

                field_set = ic_set_new();
                if (!field_set) {
                    puts("ic_analyse_body: match: call to ic_set_new failed");
                    goto ERROR;
                }

                /* need to check:
                 * 1) that every case used exists in the union
                 * 2) that every case isn't overlapping with another
                 * 3) that every possible filed of the union is dealt with OR there is an else
                 */

                n_cases = ic_stmt_match_cases_length(match);

                for (i_case = 0; i_case < n_cases; ++i_case) {
                    scase = ic_stmt_match_cases_get(match, i_case);
                    if (!scase) {
                        puts("ic_analyse_body: match: call to ic_stmt_match_cases_get failed");
                        goto ERROR;
                    }

                    field = ic_stmt_case_get_field(scase);
                    if (!field) {
                        puts("ic_analyse_body: match: call to ic_stmt_case_get_field failed");
                        goto ERROR;
                    }

                    field_name_sym = &(field->name);
                    field_name_char = ic_symbol_contents(field_name_sym);
                    if (!field_name_char) {
                        puts("ic_analyse_body: match: call to ic_symbol_contents failed");
                        goto ERROR;
                    }

                    if (ic_set_exists(field_set, field_name_char)) {
                        printf("ic_analyse_body: match: repeat of previously matched field '%s'\n", field_name_char);
                        puts("ic_analyse_body: match: repeat of previously matched field");
                        goto ERROR;
                    }

                    if (!ic_set_insert(field_set, field_name_char)) {
                        puts("ic_analyse_body: match: call to ic_set_insert failed");
                        goto ERROR;
                    }

                    match_cases += 1;

                    field_type = ic_kludge_get_decl_type_from_typeref(kludge, field->type);
                    if (!field_type) {
                        printf("ERROR: match case: unable to find type for field '%s' with declared type '", ic_symbol_contents(&(field->name)));
                        ic_type_ref_print(stdout, field->type);
                        puts("'");
                        puts("ic_analyse_body: match: call to ic_kludge_get_decl_type_from_typeref failed");
                        goto ERROR;
                    }

                    fetched_field_type = ic_decl_type_get_field_type(type, field_name_char);
                    if (!fetched_field_type) {
                        puts("ic_analyse_body: match: call to ic_decl_type_get_field_type failed");
                        goto ERROR;
                    }

                    if (!ic_decl_type_equal(field_type, fetched_field_type)) {
                        puts("ic_analyse_body: match: matched field case type did not match actual field type");
                        goto ERROR;
                    }

                    /* start new scope with field in */

                    if (!scase->body) {
                        puts("ic_analyse_body: match: scase->body was null");
                        goto ERROR;
                    }

                    if (scase->body->scope) {
                        puts("ic_analyse_body: match: scope already set on body");
                        goto ERROR;
                    }

                    match_scope = ic_scope_new(body->scope);
                    if (!match_scope) {
                        puts("ic_analyse_body: match: call to ic_scope_new failed");
                        goto ERROR;
                    }

                    slot = ic_slot_new(&(field->name), field_type, ic_parse_perm_default(), 0, ic_slot_type_case, scase);
                    if (!slot) {
                        puts("ic_analyse_body: match: call to ic_slot_new failed");
                        goto ERROR;
                    }

                    if (!ic_scope_insert_symbol(match_scope, &(field->name), slot)) {
                        puts("ic_analyse_body: match: call to ic_scope_insert_symbol failed");
                        goto ERROR;
                    }

                    scase->body->scope = match_scope;

                    /* check body */
                    /* analyse body of if */
                    if (!ic_analyse_body(unit, unit_name, kludge, scase->body, fdecl)) {
                        puts("ic_analyse_body: match: ic_analyse_body failed");
                        goto ERROR;
                    }
                }

                /* if an else exists, check it */
                if (match->else_body) {
                    match_scope = ic_scope_new(body->scope);
                    if (!match_scope) {
                        puts("ic_analyse_body: match: call to ic_scope_new failed");
                        goto ERROR;
                    }

                    match->else_body->scope = match_scope;
                    /* analyse body of if */
                    if (!ic_analyse_body(unit, unit_name, kludge, match->else_body, fdecl)) {
                        puts("ic_analyse_body: match: ic_analyse_body failed");
                        goto ERROR;
                    }
                } else {
                    /* otherwise, check we matched every possible case */
                    n_cases = ic_decl_type_field_length(type);

                    if (match_cases != n_cases) {
                        printf("ic_analyse_body: match: insufficient match_cases, expected '%d', found '%d'\n", n_cases, match_cases);
                        puts("ic_analyse_body: match: insufficient match_cases");
                        goto ERROR;
                    }
                }

                if (!ic_set_destroy(field_set, 1)) {
                    puts("ic_analyse_body: match: call to ic_set_destroy failed");
                    goto ERROR;
                }

                break;

            default:
                printf("ic_analyse_body: impossible stmt->tag '%d'\n", stmt->tag);
                goto ERROR;
                break;
        }

        /* FIXME */
    }

    return 1;

ERROR:

    puts("ic_analyse_body: unimplemented in error case");
    return 0;
}

struct ic_decl_type *ic_analyse_infer_fcall(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_expr_func_call *fcall) {
    /* our resulting type */
    struct ic_decl_type *type = 0;

    /* temporaries */
    char *ch = 0;
    struct ic_string *str = 0;
    struct ic_string *str_generic = 0;
    struct ic_decl_func *fdecl = 0;

    unsigned int fake_indent_level = 0;

    struct ic_pvector *type_refs = 0;

    if (!kludge) {
        puts("ic_analyse_infer_fcall: kludge was null");
        return 0;
    }

    if (!scope) {
        puts("ic_analyse_infer_fcall: scope was null");
        return 0;
    }

    if (!fcall) {
        puts("ic_analyse_infer_fcall: fcall was null");
        return 0;
    }

    /*
     *  infer addone(1) -> addone(Sint)->Sint -> Sint
     *  expr->tag == func_call
     *  fc = expr->u.fcall
     *  fstr = str(fc)
     *      TODO note that icarus fcalls are dependent on arg types
     *           so we need to infer arg types by calling ic_analyse_infer on the arguments
     *  fdecl = kludge get fdecl from fstr
     *  tstr = fdecl->ret_type
     *  type = kludge get tdecl from tstr
     *  return type
     */

    /*
     *  infer Foo(1,"hello") -> Foo(Sint,String) -> Foo
     *  expr->tag == func_call
     *  fc = expr->u.fcall
     *  fstr = str(fc)
     *      TODO note that icarus fcalls are dependent on arg types
     *           so we need to infer arg types by calling ic_analyse_infer on the arguments
     *  fdecl = kludge get fdecl from fstr
     *  tstr = fdecl->ret_type
     *  type = kludge get tdecl from tstr
     *  return type
     */

    /* first convert the fcall to an fdecl */

    /* FIXME bind fcall to fdecl
     * FIXME no appropriate field on fdecl
     */

    /* if fcall already has an fdecl set */
    if (ic_expr_func_call_has_fdecl(fcall)) {
        fdecl = ic_expr_func_call_get_fdecl(fcall);
        if (!fdecl) {
            puts("ic_analyse_infer_fcall: call to ic_expr_func_call_get_fdecl failed");
            return 0;
        }
        /* if fcall has generic arguments */
    } else if (ic_expr_func_call_type_refs_length(fcall) > 0) {
        /* first resolve all provided type args */
        if (!ic_analyse_type_ref_list(kludge, &(fcall->type_refs))) {
            puts("ic_analyse_infer_fcall: call to ic_analyse_type_ref_list failed");
            return 0;
        }

        /* otherwise work out the fdecl */
        str = ic_analyse_fcall_str(kludge, scope, fcall);
        if (!str) {
            puts("ic_analyse_infer_fcall: call to ic_analyse_fcall_str failed");
            return 0;
        }

        fdecl = ic_kludge_get_fdecl_from_string(kludge, str);
        if (fdecl) {
            goto INFER_FCALL_FOUND;
        }

        /* failed to find concrete type,
         * attempt to lookup generic
         */
        str_generic = ic_analyse_fcall_str_generic(kludge, scope, fcall);
        if (!str_generic) {
            puts("ic_analyse_infer_fcall: call to ic_analyse_fcall_str_generic failed");
            return 0;
        }

        fdecl = ic_kludge_get_fdecl_from_string(kludge, str_generic);
        if (fdecl) {
            goto INFER_FCALL_FOUND_GENERIC;
        }

        /* failed to find generic type
         * this is an error for a generic function
         * but if this is a constructor call to a generic type there is
         * still hope
         */

        /* 1) lookup fcall as type
         * this may trigger instantiation
         *
         * TODO FIXME need to trigger instantiation here
         * want a function we can also call from elsewhere with a type_ref
         */
        ch = ic_expr_func_call_get_name(fcall);
        if (!ch) {
            puts("ic_analyse_infer_fcall: call to ic_expr_func_call_get_name failed");
            return 0;
        }

        type_refs = &(fcall->type_refs);
        type = ic_analyse_type_decl_instantiate_generic(kludge, ch, type_refs);

        /* 2) if no tdecl, error */
        if (!type) {
            /* do not print error here */
            goto INFER_FCALL_NOT_FOUND;
        }

        /* 3) try concrete (str) lookup again */
        /* now that our type has been instantiated we try again */
        fdecl = ic_kludge_get_fdecl_from_string(kludge, str);
        if (fdecl) {
            goto INFER_FCALL_FOUND;
        }

INFER_FCALL_NOT_FOUND:

        /* failed 3 times, error */

        puts("ic_analyse_infer_fcall: error finding function declaration for function call:");
        fake_indent_level = 2;
        ic_expr_func_call_print(stdout, fcall, &fake_indent_level);
        printf("\n\n    I tried to lookup both:\n        ");
        ic_string_print(stdout, str);
        printf("\n        ");
        ic_string_print(stdout, str_generic);
        puts("\n    and failed to find either");
        printf("\n    I also tried to consider this as a constructor\n");
        printf("    but after attempting to instantiate this type, I still found no function matching the call:\n        ");
        ic_string_print(stdout, str);
        puts("\n");

        return 0;

INFER_FCALL_FOUND_GENERIC:

        fdecl = ic_analyse_func_decl_instantiate_generic(kludge, fdecl, fcall);
        if (!fdecl) {
            puts("ic_analyse_infer_fcall: call to ic_analyse_func_decl_instantiate_generic failed");
            printf("ic_analyse_infer_fcall: failed to instantiate for generic call '%s'\n", ic_string_contents(str_generic));
            return 0;
        }

INFER_FCALL_FOUND:

        /* record this found fdecl on the fcall */
        if (!ic_expr_func_call_set_fdecl(fcall, fdecl)) {
            puts("ic_analyse_infer_fcall: call to ic_expr_func_call_set_fdecl failed");
            return 0;
        }
    } else {
        /* otherwise work out the fdecl */
        str = ic_analyse_fcall_str(kludge, scope, fcall);
        if (!str) {
            puts("ic_analyse_infer_fcall: call to ic_analyse_fcall_str failed");
            return 0;
        }

        fdecl = ic_kludge_get_fdecl_from_string(kludge, str);
        if (!fdecl) {
            /* return type not found
             * need helpful error message
             */
            puts("ic_analyse_infer_fcall: error finding function declaration for function call:");
            fake_indent_level = 2;
            ic_expr_func_call_print(stdout, fcall, &fake_indent_level);
            printf("\n    I tried to lookup:\n        ");
            ic_string_print(stdout, str);
            puts("\n    and failed");
            return 0;
        }

        /* record this found fdecl on the fcall */
        if (!ic_expr_func_call_set_fdecl(fcall, fdecl)) {
            puts("ic_analyse_infer_fcall: call to ic_expr_func_call_set_fdecl failed");
            return 0;
        }
    }

    /* now convert the fdecl to a return type */

    /* now we have to get the return type for this func */

    /* get return type from this function we found */
    type = ic_kludge_get_decl_type_from_typeref(kludge, &(fdecl->ret_type));
    if (!type) {
        printf("ic_analyse_infer_fcall: could not find return type '%s'\n", ch);
        return 0;
    }

    return type;
}

static struct ic_decl_type *ic_analyse_infer_faccess(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_expr_faccess *faccess) {
    /* our resulting type */
    struct ic_decl_type *type = 0;
    /* temporary */
    struct ic_expr_identifier *id = 0;

    if (!kludge) {
        puts("ic_analyse_infer_faccess: kludge was null");
        return 0;
    }

    if (!scope) {
        puts("ic_analyse_infer_faccess: scope was null");
        return 0;
    }

    if (!faccess) {
        puts("ic_analyse_infer_faccess: faccess was null");
        return 0;
    }

    /* take the left and evaluate to a type */
    type = ic_analyse_infer(kludge, scope, faccess->left);
    if (!type) {
        puts("ic_analyse_infer_faccess: ic_analyse_infer failed");
        return 0;
    }

    /* take the thing on the right which is an identifier */
    id = ic_expr_get_identifier(faccess->right);
    if (!id) {
        puts("ic_analyse_infer_faccess: ic_expr_get_identifier failed");
        return 0;
    }

    /* get the type of this identifier as field on type
     * FIXME line is a bit too busy
     */
    type = ic_decl_type_get_field_type(type, ic_symbol_contents(ic_expr_identifier_symbol(id)));
    if (!type) {
        puts("ic_analyse_infer_faccess: ic_decl_type_get_field failed");
        return 0;
    }

    return type;
}

static struct ic_decl_type *ic_analyse_infer_identifier(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_expr_identifier *id) {
    /* our resulting type */
    struct ic_decl_type *type = 0;

    /* temporary intermediaries */
    char *ch = 0;
    struct ic_symbol *sym = 0;
    struct ic_slot *slot = 0;

    if (!kludge) {
        puts("ic_analyse_infer_identifier: kludge was null");
        return 0;
    }

    if (!scope) {
        puts("ic_analyse_infer_identifier: scope was null");
        return 0;
    }

    if (!id) {
        puts("ic_analyse_infer_identifier: id was null");
        return 0;
    }

    /*
     *  infer f -> typeof contents of f
     *  expr->tag = identifier
     *  id = expr->u.id
     *  name = str(id)
     *  variable = get variable within scope name
     *  type = get type of variable
     *  return type
     */

    sym = ic_expr_identifier_symbol(id);
    if (!sym) {
        puts("ic_analyse_infer_identifier: ic_expr_type_identifier: call to ic_expr_identifier_symbol failed");
        return 0;
    }

    /* char* representation */
    ch = ic_symbol_contents(sym);
    if (!ch) {
        puts("ic_analyse_infer_identifier: ic_expr_type_identifier: call to ic_symbol_contents failed");
        return 0;
    }

    /* get slot from identifier (variable) name */
    slot = ic_scope_get(scope, ch);
    if (!slot) {
        /* FIXME unknown identifier
         * need helpful output here
         */
        printf("ic_analyse_infer_identifier: unknown identifier '%s', not in scope\n", ch);
        return 0;
    }

    type = slot->type;
    if (!type) {
        printf("ic_analyse_infer_identifier: error fetching identifier '%s', no type found\n", ch);
        return 0;
    }

    /* return type */
    return type;
}

struct ic_decl_type *ic_analyse_infer_constant(struct ic_kludge *kludge, struct ic_expr_constant *cons) {
    /* our resulting type */
    struct ic_decl_type *type = 0;

    if (!kludge) {
        puts("ic_analyse_infer_constant: kludge was null");
        return 0;
    }

    if (!cons) {
        puts("ic_analyse_infer_constant: cons was null");
        return 0;
    }

    switch (cons->tag) {

        /*
         *  infer 1 -> Uint
         *  expr->tag == constant
         *  cons = expr->u.cons
         *  cons->type == integer
         *  return integer
         */
        case ic_expr_constant_type_unsigned_integer:
            /* FIXME decide on type case sensitivity */
            type = ic_kludge_get_decl_type(kludge, "Uint");
            if (!type) {
                puts("ic_analyse_infer_constant: Sint: call to ic_kludge_get_type failed");
                return 0;
            }
            return type;
            break;

        /*
         *  infer 1 -> Sint
         *  expr->tag == constant
         *  cons = expr->u.cons
         *  cons->type == integer
         *  return integer
         */
        case ic_expr_constant_type_signed_integer:
            /* FIXME decide on type case sensitivity */
            type = ic_kludge_get_decl_type(kludge, "Sint");
            if (!type) {
                puts("ic_analyse_infer_constant: Sint: call to ic_kludge_get_type failed");
                return 0;
            }
            return type;
            break;

        /*
         *  infer "hello" -> String
         *  expr->tag == constant
         *  cons = expr->u.cons
         *  cons->type == string
         *  return string
         */
        case ic_expr_constant_type_string:
            /* FIXME decide on type case sensitivity */
            type = ic_kludge_get_decl_type(kludge, "String");
            if (!type) {
                puts("ic_analyse_infer_constant: String: call to ic_kludge_get_type failed");
                return 0;
            }
            return type;
            break;

        case ic_expr_constant_type_boolean:
            /* FIXME decide on type case sensitivity */
            type = ic_kludge_get_decl_type(kludge, "Bool");
            if (!type) {
                puts("ic_analyse_infer_constant: Bool: call to ic_kludge_get_type failed");
                return 0;
            }
            return type;
            break;

        default:
            puts("ic_analyse_infer_constant: impossible and unexpected constant, neither integer not string");
            return 0;
            break;
    }

    puts("ic_analyse_infer_constant: impossible case");
    return 0;
}

static struct ic_decl_type *ic_analyse_infer_operator(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_expr_operator *op) {
    /* our resulting type */
    struct ic_decl_type *type = 0;

    /* temporaries */
    char *op_str = 0;
    struct ic_symbol *mapped_op_sym = 0;
    char *mapped_op_str = 0;
    unsigned int mapped_op_len = 0;
    struct ic_expr *expr = 0;
    struct ic_expr_identifier *id = 0;

    if (!kludge) {
        puts("ic_analyse_infer_operator: kludge was null");
        return 0;
    }

    if (!scope) {
        puts("ic_analyse_infer_operator: scope was null");
        return 0;
    }

    if (!op) {
        puts("ic_analyse_infer_operator: op was null");
        return 0;
    }

    if (!op->fcall) {
        /* infer operator
       * get operator str (e.g. '+')
       * get operator mapped symbol (e.g. 'plus')
       * build an identifier from this to satisfy the fcall constrctor
       * construct an fcall of this operator
       *      operator(1 + 2) => fcall(plus 1 2)
       * save this fcall on the op
       * call infer_fcall on this fcall
       * return that value
       */

        op_str = ic_token_get_representation(op->token);
        if (!op_str) {
            puts("ic_analyse_infer_operator: call to ic_token_get_representation failed");
            return 0;
        }

        mapped_op_sym = ic_kludge_get_operator(kludge, op_str);
        if (!mapped_op_sym) {
            puts("ic_analyse_infer_operator: call to ic_kludge_get_operator failed");
            return 0;
        }

        mapped_op_str = ic_symbol_contents(mapped_op_sym);
        if (!mapped_op_str) {
            puts("ic_analyse_infer_operator: call to ic_symbol_contents failed");
            return 0;
        }

        mapped_op_len = ic_symbol_length(mapped_op_sym);
        if (!mapped_op_len) {
            puts("ic_analyse_infer_operator: call to ic_symbol_length failed");
            return 0;
        }

        expr = ic_expr_new(ic_expr_type_identifier);
        if (!expr) {
            puts("ic_analyse_infer_operator: call to ic_expr_new failed");
            return 0;
        }

        id = ic_expr_get_identifier(expr);
        if (!id) {
            puts("ic_analyse_infer_operator: call to ic_expr_get_identifier failed");
            return 0;
        }

        /* relying on identifier_init to set sane default permissions */
        if (!ic_expr_identifier_init(id, mapped_op_str, mapped_op_len, 0)) {
            puts("ic_analyse_infer_operator: call to ic_expr_identifier_init failed");
            return 0;
        }

        op->fcall = ic_expr_func_call_new(expr);
        if (!op->fcall) {
            puts("ic_analyse_infer_operator: call to ic_expr_func_call_new failed");
            return 0;
        }

        if (!ic_expr_func_call_add_arg(op->fcall, op->first)) {
            puts("ic_analyse_infer_operator: call to ic_expr_func_call_add_arg failed for first");
            return 0;
        }

        if (!ic_expr_func_call_add_arg(op->fcall, op->second)) {
            puts("ic_analyse_infer_operator: call to ic_expr_func_call_add_arg failed for second");
            return 0;
        }
    }

    type = ic_analyse_infer_fcall(kludge, scope, op->fcall);
    if (!type) {
        puts("ic_analyse_infer_operator: call to ic_analyse_infer_fcall failed for second");
        return 0;
    }

    return type;
}

/* takes an expr and returns the inferred type
 *
 * FIXME need a way of signalling error and passing errors
 * possible suggestions:
 *  0 -> unrecoverable / internal error
 *  special ic_type_ref * -> syntax error -> ignore this statement, record error, keep going
 *  ic_type_ref * -> actual type inferred
 *
 * examples:
 *  infer 1 to Sint
 *  infer "hello" to String
 *  infer addone(1) which is a call to addone(Sint) -> Sint therefore Sint
 *  infer Foo(1,"hello") which is call to Foo(Sint,String) -> Foo therefore Foo
 *
 * returns ic_type * on success
 * returns 0 on failure
 */
struct ic_decl_type *ic_analyse_infer(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_expr *expr) {
    /* our resulting type */
    struct ic_decl_type *type = 0;

    /* unwrapped values */
    struct ic_expr_constant *cons = 0;
    struct ic_expr_operator *op = 0;
    struct ic_expr_identifier *id = 0;
    struct ic_expr_func_call *fcall = 0;
    struct ic_expr_faccess *faccess = 0;

    if (!kludge) {
        puts("ic_analyse_infer: kludge was null");
        return 0;
    }

    if (!scope) {
        puts("ic_analyse_infer: scope was null");
        return 0;
    }

    if (!expr) {
        puts("ic_analyse_infer: expr was null");
        return 0;
    }

    switch (expr->tag) {

        case ic_expr_type_func_call:
            /*
             *  infer addone(1) -> addone(Sint)->Sint -> Sint
             *  expr->tag == func_call
             *  fc = expr->u.fcall
             *  fstr = str(fc)
             *      TODO note that icarus fcalls are dependent on arg types
             *           so we need to infer arg types by calling ic_analyse_infer on the arguments
             *  fdecl = kludge get fdecl from fstr
             *  tstr = fdecl->ret_type
             *  type = kludge get tdecl from tstr
             *  return type
             */

            /*
             *  infer Foo(1,"hello") -> Foo(Sint,String) -> Foo
             *  expr->tag == func_call
             *  fc = expr->u.fcall
             *  fstr = str(fc)
             *      TODO note that icarus fcalls are dependent on arg types
             *           so we need to infer arg types by calling ic_analyse_infer on the arguments
             *  fdecl = kludge get fdecl from fstr
             *  tstr = fdecl->ret_type
             *  type = kludge get tdecl from tstr
             *  return type
             */

            fcall = ic_expr_get_fcall(expr);
            if (!fcall) {
                puts("ic_analyse_infer: call to ic_expr_get_fcall failed");
                return 0;
            }

            type = ic_analyse_infer_fcall(kludge, scope, fcall);
            if (!type) {
                puts("ic_analyse_infer: call to ic_analyse_infer_fcall failed");
                return 0;
            }

            return type;
            break;

        case ic_expr_type_identifier:
            /*
             *  infer f -> typeof contents of f
             *  expr->tag = identifier
             *  id = expr->u.id
             *  name = str(id)
             *  variable = get variable within scope name
             *  type = get type of variable
             *  return type
             */

            id = ic_expr_get_identifier(expr);
            if (!id) {
                puts("ic_analyse_infer: call to ic_expr_get_identifier failed");
                return 0;
            }

            type = ic_analyse_infer_identifier(kludge, scope, id);
            if (!type) {
                puts("ic_analyse_infer: call to ic_analyse_infer_identifier failed");
                return 0;
            }

            return type;
            break;

        case ic_expr_type_constant:

            cons = ic_expr_get_constant(expr);
            if (!cons) {
                puts("ic_analyse_infer: call to ic_expr_get_constant failed");
                return 0;
            }

            type = ic_analyse_infer_constant(kludge, cons);
            if (!type) {
                puts("ic_analyse_infer: call to ic_analyse_infer_constant failed");
                return 0;
            }

            return type;
            break;

        case ic_expr_type_operator:

            op = ic_expr_get_operator(expr);
            if (!op) {
                puts("ic_analyse_infer: call to ic_expr_get_operator failed");
                return 0;
            }

            type = ic_analyse_infer_operator(kludge, scope, op);
            if (!type) {
                puts("ic_analyse_infer: call to ic_analyse_infer_operator failed");
                return 0;
            }

            return type;
            break;

        case ic_expr_type_field_access:

            faccess = ic_expr_get_faccess(expr);
            if (!faccess) {
                puts("ic_analyse_infer: call to ic_expr_get_faccess failed");
                return 0;
            }

            type = ic_analyse_infer_faccess(kludge, scope, faccess);
            if (!type) {
                puts("ic_analyse_infer: call to ic_analyse_infer_faccess failed");
                return 0;
            }

            return type;
            break;

        default:
            printf("ic_analyse_infer: unknown expr->tag '%d'\n", expr->tag);
            return 0;
            break;
    }

    puts("ic_analyse_infer: unimplemented");
    return 0;
}

/* perform analyse of let statement in the provided body
 *
 * returns 1 for success
 * returns 0 on failure
 */
unsigned int ic_analyse_let(char *unit, char *unit_name, struct ic_kludge *kludge, struct ic_body *body, struct ic_stmt_let *let) {
    struct ic_slot *slot = 0;
    struct ic_decl_type *type = 0;

    struct ic_expr *init_expr = 0;
    struct ic_decl_type *init_type = 0;

    int ret = 0;

    if (!unit) {
        puts("ic_analyse_let: unit was null");
        return 0;
    }

    if (!unit_name) {
        puts("ic_analyse_let: unit_name was null");
        return 0;
    }

    if (!kludge) {
        puts("ic_analyse_let: kludge was null");
        return 0;
    }

    if (!body) {
        puts("ic_analyse_let: body was null");
        return 0;
    }

    if (!body->scope) {
        puts("ic_analyse_let: body is lacking scope");
        return 0;
    }

    if (!let) {
        puts("ic_analyse_let: let was null");
        return 0;
    }

    /* slot creation time
     * after gathering all the needed parts
     *
     * a slot needs:
     *  symbol *name
     *  ic_type *type
     *  bool mutable
     *  bool reference
     *
     * a let has:
     *  unsigned int mut
     *  symbol identifier
     *  symbol type
     *  ic_expr init
     *
     */

    /* check if identifier is already in use */
    ret = ic_kludge_identifier_exists_symbol(kludge, body->scope, &(let->identifier));
    if (1 == ret) {
        printf("ic_analyse_let: identifier already exists '%s'\n", ic_symbol_contents(&(let->identifier)));
        return 0;
    }
    if (0 != ret) {
        printf("ic_analyse_let: error checking for identifier existence '%s'\n", ic_symbol_contents(&(let->identifier)));
        return 0;
    }

    /* we have to get the init expr first as the let may not include a type declaration */

    /* infer the type of init expression if one exists */
    init_expr = let->init;
    /* FIXME eventually such an expr will be optional */
    if (!init_expr) {
        puts("ic_analyse_let: init_expr was null");
        return 0;
    }

    init_type = ic_analyse_infer(kludge, body->scope, init_expr);
    if (!init_type) {
        puts("ic_analyse_let: call to ic_analyse_infer on init_expr failed");
        return 0;
    }

    /* if let->type is set we must compare it to the init expr */
    switch (let->tref->tag) {
        case ic_type_ref_unknown:
            /* no declared let type, just use inferred type */
            type = init_type;

            /* set our tref */
            if (!ic_stmt_let_set_inferred_type(let, type)) {
                puts("ic_analyse_let: call to ic_stmt_let_inferred_type failed");
                return 0;
            }
            break;

        case ic_type_ref_symbol:
            /* type declared, compare */

            /* resolve symbol to tdecl
             * this will set let->tref for us
             */
            type = ic_kludge_get_decl_type_from_typeref(kludge, let->tref);
            if (!type) {
                puts("ic_analyse_let: failed to find type from typeref");
                printf("failed to find declared type '%s'\n", ic_symbol_contents(ic_type_ref_get_symbol(let->tref)));
                return 0;
            }

            /* if an init expression exists, verify it's type is the same as the declared */
            if (!ic_decl_type_equal(init_type, type)) {
                puts("ic_analyse_let: let init type did not match declared type");
                return 0;
            }
            break;

        case ic_type_ref_resolved:
            /* error, type shouldn't already be resolved */
            puts("ic_analyse_let: internal error: tref->tag was ic_type_ref_resolved");
            return 0;
            break;

        default:
            /* error, impossible tref.tag */
            puts("ic_analyse_let: impossible tref->tag");
            return 0;
            break;
    }

    /*
     * FIXME ref is hardcoded (0) to false as they are not
     * currently supported
     *
     *                (name, type, mut, ref)
     */
    slot = ic_slot_new(&(let->identifier), type, let->permissions, 0, ic_slot_type_let, let);
    if (!slot) {
        puts("ic_analyse_let: call to ic_slot_new failed");
        return 0;
    }

    /* need to store slot in body->scope */
    if (!ic_scope_insert_symbol(body->scope, &(let->identifier), slot)) {
        puts("ic_analyse_let: call to ic_scope_insert_symbol failed");
        return 0;
    }

    return 1;
}

/* create a function signature string from a function call
 *
 * this function must be compatible with the one produced
 * by `ic_decl_func_sig_call`
 *      foo(Sint,Sint)
 *
 * and
 *      bar(&Sint,String)
 *
 * for an fcall of the form
 *     id[Sint](6s)
 *
 * this will generate
 *     id[Sint](Sint)
 *
 * in order to generate
 *     id[_](_)
 * please see ic_analyse_fcall_str_generic
 *
 * returns ic_string * on success
 * returns 0 on failure
 */
struct ic_string *ic_analyse_fcall_str(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_expr_func_call *fcall) {
    /* resulting string, stored as fcall->string */
    struct ic_string *str = 0;
    /* offset into args pvector */
    unsigned int i = 0;
    /* len of args pvector */
    unsigned int len = 0;
    /* current argument */
    struct ic_expr *expr = 0;
    /* type of current argument */
    struct ic_decl_type *expr_type = 0;
    /* current arg type's name */
    struct ic_symbol *type_full_name = 0;
    /* type ref for generic function */
    struct ic_type_ref *type_ref = 0;
    /* symbol within a type-ref for generic function */
    struct ic_symbol *type_ref_full_name = 0;
    /* current arg's permissions */
    unsigned int arg_perms = 0;
    /* permission string for arg */
    char *arg_perm_str = 0;

    if (!kludge) {
        puts("ic_analyse_fcall_str: kludge was null");
        return 0;
    }

    if (!scope) {
        puts("ic_analyse_fcall_str: scope was null");
        return 0;
    }

    if (!fcall) {
        puts("ic_analyse_fcall_str: fcall was null");
        return 0;
    }

    if (fcall->string) {
        /* don't recalculate */
        goto EXIT;
    }

    /* construct a new string */
    str = ic_string_new_empty();
    if (!str) {
        puts("ic_analyse_fcall_str: call to ic_string_new_empty failed");
        goto ERROR;
    }

    /* append function name */
    if (!ic_string_append_symbol(str, ic_expr_func_call_get_symbol(fcall))) {
        puts("ic_analyse_fcall_str: call to ic_string_append_symbol for 'fname' failed");
        goto ERROR;
    }

    /* if generic, then insert our type_refs */
    len = ic_expr_func_call_type_refs_length(fcall);
    if (len > 0) {
        /* opening square bracket */
        if (!ic_string_append_char(str, "[", 1)) {
            puts("ic_analyse_fcall_str: call to ic_string_append_char for '[' failed");
            goto ERROR;
        }

        for (i = 0; i < len; ++i) {
            /* insert a comma between all type_refs */
            if (i != 0) {
                if (!ic_string_append_char(str, ",", 1)) {
                    puts("ic_analyse_fcall_str: call to ic_string_append_char for ',' failed");
                    goto ERROR;
                }
            }

            type_ref = ic_expr_func_call_get_type_ref(fcall, i);
            if (!type_ref) {
                puts("ic_analyse_fcall_str: call to ic_expr_func_call_get_type_ref failed");
                goto ERROR;
            }

            type_ref_full_name = ic_type_ref_full_name(type_ref);
            if (!type_ref_full_name) {
                puts("ic_analyse_fcall_str: call to ic_type_ref_full_name failed");
                goto ERROR;
            }

            /* append our type ref */
            if (!ic_string_append_symbol(str, type_ref_full_name)) {
                puts("ic_analyse_fcall_str: call to ic_string_append_symbol faild");
                goto ERROR;
            }
        }

        /* closing square bracket */
        if (!ic_string_append_char(str, "]", 1)) {
            puts("ic_analyse_fcall_str: call to ic_string_append_char for ']' failed");
            goto ERROR;
        }
    }

    /* opening bracket */
    if (!ic_string_append_char(str, "(", 1)) {
        puts("ic_analyse_fcall_str: call to ic_string_append_char for '(' failed");
        goto ERROR;
    }

    /* insert list of argument types */
    len = ic_pvector_length(&(fcall->args));
    for (i = 0; i < len; ++i) {
        /* insert a comma if this isn't the first arg */
        if (i != 0) {
            if (!ic_string_append_char(str, ",", 1)) {
                puts("ic_analyse_fcall_str: call to ic_string_append_char for ' ' failed");
                goto ERROR;
            }
        }

        /* current arg */
        expr = ic_pvector_get(&(fcall->args), i);
        if (!expr) {
            puts("ic_analyse_fcall_str: call to ic_pvector_get failed");
            goto ERROR;
        }

        /* if this argument is an identifier
         * then we must also check it's permissions
         */
        if (expr->tag == ic_expr_type_identifier) {
            /* get this argument's permissions */
            arg_perms = expr->u.id.permissions;

            /* if the permissions are not the default
             * then we also need to add them
             */
            if (!ic_parse_perm_is_default(arg_perms)) {
                arg_perm_str = ic_parse_perm_str(arg_perms);

                /* append our permissions */
                if (!ic_string_append_cstr(str, arg_perm_str)) {
                    printf("ic_analyse_fcall_str: call to ic_string_append_cstr for permissions failed for argument '%d'\n", i);
                    goto ERROR;
                }
            }
        }

        /* current arg's type */
        expr_type = ic_analyse_infer(kludge, scope, expr);
        if (!expr_type) {
            printf("ic_analyse_fcall_str: call to ic_analyse_infer failed for argument '%d'\n", i);
            goto ERROR;
        }

        /* current arg's type's name */
        type_full_name = ic_decl_type_full_name(expr_type);
        if (!type_full_name) {
            printf("ic_analyse_fcall_str: call to ic_type_full_name failed for argument '%d'\n", i);
            goto ERROR;
        }

        /* append our argument type */
        if (!ic_string_append_symbol(str, type_full_name)) {
            printf("ic_analyse_fcall_str: call to ic_string_append_symbol for 'arg' '%d' failed\n", i);
            goto ERROR;
        }
    }

    /* closing bracket */
    if (!ic_string_append_char(str, ")", 1)) {
        puts("ic_analyse_fcall_str: call to ic_string_append_char for ')' failed");
        goto ERROR;
    }

    /* store string on fcall */
    fcall->string = str;

    goto EXIT;

ERROR:
    /* cleanup */
    puts("ic_analyse_fcall_str: error occurred, destroying string");

    if (str) {
        if (!ic_string_destroy(str, 1)) {
            puts("ic_analyse_fcall_str: in ERROR cleanup: call to ic_string_destroy failed");
        }
    }

    return 0;

EXIT:
    return fcall->string;
}

/* create a generic function signature string from a function call
 *
 * for an fcall of the form
 *     id[Sint](6s)
 *
 * we will generate
 *     id[_](_)
 *
 * returns ic_string * on success
 * returns 0 on failure
 */
struct ic_string *ic_analyse_fcall_str_generic(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_expr_func_call *fcall) {
    /* resulting string, stored as fcall->string */
    struct ic_string *str = 0;
    /* offset into args pvector */
    unsigned int i = 0;
    /* len of args pvector */
    unsigned int len = 0;

    if (!kludge) {
        puts("ic_analyse_fcall_str_generic: kludge was null");
        return 0;
    }

    if (!scope) {
        puts("ic_analyse_fcall_str_generic: scope was null");
        return 0;
    }

    if (!fcall) {
        puts("ic_analyse_fcall_str_generic: fcall was null");
        return 0;
    }

    /* construct a new string */
    str = ic_string_new_empty();
    if (!str) {
        puts("ic_analyse_fcall_str_generic: call to ic_string_new_empty failed");
        goto ERROR;
    }

    /* append function name */
    if (!ic_string_append_symbol(str, ic_expr_func_call_get_symbol(fcall))) {
        puts("ic_analyse_fcall_str_generic: call to ic_string_append_symbol for 'fname' failed");
        goto ERROR;
    }

    /* if generic, then insert our _ for each type_refs */
    len = ic_expr_func_call_type_refs_length(fcall);
    if (len > 0) {
        /* opening square bracket */
        if (!ic_string_append_char(str, "[", 1)) {
            puts("ic_analyse_fcall_str: call to ic_string_append_char for '[' failed");
            goto ERROR;
        }

        for (i = 0; i < len; ++i) {
            /* insert a comma between all type_refs */
            if (i != 0) {
                if (!ic_string_append_char(str, ",", 1)) {
                    puts("ic_analyse_fcall_str_generic: call to ic_string_append_char for ',' failed");
                    goto ERROR;
                }
            }

            if (!ic_string_append_char(str, "_", 1)) {
                puts("ic_analyse_fcall_str_generic: call to ic_string_append_char for '_' failed");
                goto ERROR;
            }
        }

        /* closing square bracket */
        if (!ic_string_append_char(str, "]", 1)) {
            puts("ic_analyse_fcall_str_generic: call to ic_string_append_char for ']' failed");
            goto ERROR;
        }
    }

    /* opening bracket */
    if (!ic_string_append_char(str, "(", 1)) {
        puts("ic_analyse_fcall_str_generic: call to ic_string_append_char for '(' failed");
        goto ERROR;
    }

    /* insert list of _ for each argument types */
    len = ic_pvector_length(&(fcall->args));
    for (i = 0; i < len; ++i) {
        /* insert a comma if this isn't the first arg */
        if (i != 0) {
            if (!ic_string_append_char(str, ",", 1)) {
                puts("ic_analyse_fcall_str_generic: call to ic_string_append_char for ' ' failed");
                goto ERROR;
            }
        }

        /* append our argument type */
        if (!ic_string_append_char(str, "_", 1)) {
            puts("ic_analyse_fcall_str_generic: call to ic_string_append_char for '_' failed");
            goto ERROR;
        }
    }

    /* closing bracket */
    if (!ic_string_append_char(str, ")", 1)) {
        puts("ic_analyse_fcall_str_generic: call to ic_string_append_char for ')' failed");
        goto ERROR;
    }

    /* store string on fcall */
    fcall->string = str;

    goto EXIT;

ERROR:

    /* cleanup */
    puts("ic_analyse_fcall_str_generic: error occurred, destroying string");

    if (str) {
        if (!ic_string_destroy(str, 1)) {
            puts("ic_analyse_fcall_str_generic: in ERROR cleanup: call to ic_string_destroy failed");
        }
    }

    return 0;

EXIT:

    /* stored as fcall->string
     * destroyed as part of fcall destroy
     */
    return str;
}

/* trigger instantiate of this generic function decl to a concrete function decl
 *
 * creates new fdecl, inserts into kludge, returns * to new fdecl
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_decl_func *ic_analyse_func_decl_instantiate_generic(struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_expr_func_call *fcall) {
    struct ic_decl_func *new_fdecl = 0;
    unsigned int i = 0;
    unsigned int len = 0;
    struct ic_type_param *type_param = 0;
    struct ic_type_ref *type_arg = 0;
    struct ic_decl_type *type_arg_decl = 0;
    struct ic_type_ref *ret_type_ref = 0;

    if (!kludge) {
        puts("ic_analyse_func_decl_instantiate_generic: kludge was null");
        return 0;
    }

    if (!fdecl) {
        puts("ic_analyse_func_decl_instantiate_generic: fdecl was null");
        return 0;
    }

    if (!fcall) {
        puts("ic_analyse_func_decl_instantiate_generic: fcall was null");
        return 0;
    }

    /* 1) deep copy fdecl */
    new_fdecl = ic_decl_func_deep_copy(fdecl);
    if (!new_fdecl) {
        puts("ic_analyse_func_decl_instantiate_generic: call to ic_decl_func_deep_copy failed");
        return 0;
    }

    /* 2) bind each type_arg to matching type_param */
    len = ic_decl_func_type_params_length(new_fdecl);
    if (len != ic_expr_func_call_type_refs_length(fcall)) {
        puts("ic_analyse_func_decl_instantiate_generic: error: number of provided type arguments does not match number of expected type params");
        printf("ic_analyse_func_decl_instantiate_generic: I was given '%d' type arguments, I expected '%d' type parameters\n", ic_expr_func_call_type_refs_length(fcall), len);
        return 0;
    }

    for (i=0; i<len; ++i) {
        type_param = ic_decl_func_type_params_get(new_fdecl, i);
        if (!type_param) {
            puts("ic_analyse_func_decl_instantiate_generic: call to ic_decl_func_type_params_get failed");
            return 0;
        }

        type_arg = ic_expr_func_call_get_type_ref(fcall, i);
        if (!type_arg) {
            puts("ic_analyse_func_decl_instantiate_generic: call to ic_expr_func_call_get_type_ref failed");
            return 0;
        }

        type_arg_decl = ic_type_ref_get_type_decl(type_arg);
        if (!type_arg_decl) {
            puts("ic_analyse_func_decl_instantiate_generic: call to ic_type_ref_get_type_decl failed");
            return 0;
        }

        if (!ic_type_param_set(type_param, type_arg_decl)) {
            puts("ic_analyse_func_decl_instantiate_generic: call to ic_type_param_set failed");
            return 0;
        }
    }

    /* 3) resolve argument types (resolving type_arg through type_param) */
    if (!ic_resolve_field_list("generic function instantiation", ic_symbol_contents(&(fdecl->name)), &(new_fdecl->type_params), &(new_fdecl->args))) {
        puts("ic_analyse_func_decl_instantiate_generic: call to ic_resolve_field_list failed");
        return 0;
    }

    /* 4) infer return type if needed */
    ret_type_ref = ic_decl_func_get_return(new_fdecl);
    if (!ret_type_ref) {
        puts("ic_analyse_func_decl_instantiate_generic: call to ic_decl_func_get_return failed");
        return 0;
    }

    if (!ic_resolve_type_ref("generic function instantiation", ic_symbol_contents(&(fdecl->name)), &(new_fdecl->type_params), ret_type_ref)) {
        puts("ic_analyse_func_decl_instantiate_generic: call to ic_resolve_type_ref failed");
        return 0;
    }

    /* 5) mark instantiated */
    if (!ic_decl_func_mark_instantiated(new_fdecl)) {
        puts("ic_analyse_func_decl_instantiate_generic: call to ic_decl_func_mark_instantiated failed");
        return 0;
    }

    /* 6) store in kludge */
    if (!ic_kludge_add_fdecl(kludge, new_fdecl)) {
        puts("ic_analyse_func_decl_instantiate_generic: call to ic_kludge_add_fdecl failed");
        return 0;
    }

    /* 7) perform analysis on copy (resolving type_arg through type_params) */
    if (!ic_analyse_decl_func(kludge, new_fdecl)) {
        puts("ic_analyse_func_decl_instantiate_generic: call to ic_analyse_decl_func failed");
        return 0;
    }

    /* 8) return * */
    return new_fdecl;
}

/* trigger instantiation of this generic type decl to a concrete type decl
 *
 * creates new tdecl, inserts into kludge, returns * to new tdecl
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_decl_type *ic_analyse_type_decl_instantiate_generic(struct ic_kludge *kludge, char *type_name, struct ic_pvector *type_args) {
    struct ic_decl_type *decl_type = 0;

    unsigned int i = 0;
    unsigned int len = 0;

    struct ic_string *str = 0;
    struct ic_type_ref *type_ref = 0;
    struct ic_symbol *type_ref_name = 0;

    struct ic_type_param *type_param = 0;
    struct ic_type_ref *type_arg = 0;
    struct ic_decl_type *type_arg_decl = 0;

    if (!kludge) {
        puts("ic_analyse_type_decl_instantiate_generic: kludge was null");
        return 0;
    }

    if (!type_name) {
        puts("ic_analyse_type_decl_instantiate_generic: type_name was null");
        return 0;
    }

    if (!type_args) {
        puts("ic_analyse_type_decl_instantiate_generic: type_args was null");
        return 0;
    }

    len = ic_pvector_length(type_args);

    if (len == 0) {
        /* not generic
         * could trigger error
         * instead fail silently so that our caller can decide if this is an error or not
         */
        return 0;
    }

    str = ic_string_new_empty();
    if (!str) {
        puts("ic_analyse_type_decl_instantiate_generic: call to ic_string_new_empty failed");
        return 0;
    }

    /* TODO FIXME note that our matching func_decl_instantiate_generic DOESN'T check first
     * so our interfaces differ
     * both instantiate_generic functions should behave in a similar way
     */

    /* 1) generate signature of the form Maybe[Sint] */
    if (!ic_string_append_cstr(str, type_name)) {
        puts("ic_analyse_type_decl_instantiate_generic: call to ic_string_append_cstr failed");
        return 0;
    }

    if (!ic_string_append_char(str, "[", 1)) {
        puts("ic_analyse_type_decl_instantiate_generic: call to ic_string_append_cstr failed");
        return 0;
    }

    for (i=0; i<len; ++i) {

        if (i>0) {
            if (!ic_string_append_char(str, ",", 1)) {
                puts("ic_analyse_type_decl_instantiate_generic: call to ic_string_append_cstr failed");
                return 0;
            }
        }

        type_ref = ic_pvector_get(type_args, i);
        if (!type_ref) {
            puts("ic_analyse_type_decl_instantiate_generic: call to ic_pvector_get failed");
            return 0;
        }

        type_ref_name = ic_type_ref_get_type_name(type_ref);
        if (!type_ref_name) {
            puts("ic_analyse_type_decl_instantiate_generic: call to ic_type_ref_get_type_name failed");
            return 0;
        }

        if (!ic_string_append_symbol(str, type_ref_name)) {
            puts("ic_analyse_type_decl_instantiate_generic: call to ic_string_append_symbol failed");
            return 0;
        }
    }

    if (!ic_string_append_char(str, "]", 1)) {
        puts("ic_analyse_type_decl_instantiate_generic: call to ic_string_append_cstr failed");
        return 0;
    }


    /* 2) check if this exists
     *    if it does, look it up, and return it
     */
    decl_type = ic_kludge_get_decl_type_from_string(kludge, str);
    if (decl_type) {
        /* found type, freedom! */

        if (!ic_string_destroy(str, 1)) {
            puts("ic_analyse_type_decl_instantiate_generic: call to ic_string_destroy failed");
            return 0;
        }

        return decl_type;
    }

    /* 3) generate signature of the form Maybe[_] */

    /* clear our string so we can re-use */
    if (!ic_string_clear(str)) {
        puts("ic_analyse_type_decl_instantiate_generic: call to ic_string_clear failed");
        return 0;
    }

    if (!ic_string_append_cstr(str, type_name)) {
        puts("ic_analyse_type_decl_instantiate_generic: call to ic_string_append_cstr failed");
        return 0;
    }

    if (!ic_string_append_char(str, "[", 1)) {
        puts("ic_analyse_type_decl_instantiate_generic: call to ic_string_append_cstr failed");
        return 0;
    }

    for (i=0; i<len; ++i) {

        if (i==0) {
            if (!ic_string_append_char(str, "_", 1)) {
                puts("ic_analyse_type_decl_instantiate_generic: call to ic_string_append_cstr failed");
                return 0;
            }
        } else {
            if (!ic_string_append_char(str, ",_", 2)) {
                puts("ic_analyse_type_decl_instantiate_generic: call to ic_string_append_cstr failed");
                return 0;
            }
        }
    }

    if (!ic_string_append_char(str, "]", 1)) {
        puts("ic_analyse_type_decl_instantiate_generic: call to ic_string_append_cstr failed");
        return 0;
    }

    /* 4) check if this exists
     *    if it does not, give up - fail silently so user can decide if this is a problem
     */
    decl_type = ic_kludge_get_decl_type_from_string(kludge, str);
    if (!decl_type) {
        /* failed to find type, give up */

        /* clean up first */
        if (!ic_string_destroy(str, 1)) {
            puts("ic_analyse_type_decl_instantiate_generic: call to ic_string_destroy failed");
            return 0;
        }

        return 0;
    }

    /* clean up string */
    if (!ic_string_destroy(str, 1)) {
        puts("ic_analyse_type_decl_instantiate_generic: call to ic_string_destroy failed");
        return 0;
    }

    /* now instantiate found Maybe[_] with provided types [Sint] */
    /* 5) deep copy */
    decl_type = ic_decl_type_deep_copy(decl_type);
    if (!decl_type) {
        puts("ic_analyse_type_decl_instantiate_generic: call to ic_decl_type_deep_copy failed");
        return 0;
    }

    /* 6) bind each type_arg to matching type_param */
    len = ic_decl_type_type_params_length(decl_type);
    if (len != ic_pvector_length(type_args)) {
        puts("ic_analyse_type_decl_instantiate_generic: error: number of provided type arguments does not match number of expected type params");
        return 0;
    }

    for (i=0; i<len; ++i) {
        type_param = ic_decl_type_type_params_get(decl_type, i);
        if (!type_param) {
            puts("ic_analyse_type_decl_instantiate_generic: call to ic_decl_type_type_params_get failed");
            return 0;
        }

        type_arg = ic_pvector_get(type_args, i);
        if (!type_arg) {
            puts("ic_analyse_type_decl_instantiate_generic: call to ic_pvector_get failed");
            return 0;
        }

        type_arg_decl = ic_type_ref_get_type_decl(type_arg);
        if (!type_arg_decl) {
            puts("ic_analyse_type_decl_instantiate_generic: call to ic_type_ref_get_type_decl failed");
            return 0;
        }

        if (!ic_type_param_set(type_param, type_arg_decl)) {
            puts("ic_analyse_type_decl_instantiate_generic: call to ic_type_param_set failed");
            return 0;
        }
    }

    /* 6) mark instantiated */
    if (!ic_decl_type_mark_instantiated(decl_type)) {
        puts("ic_analyse_type_decl_instantiate_generic: call to ic_decl_type_mark_instantiated failed");
        return 0;
    }

    /* 7) perform analysis on copy (generating functions, populating field_dict, etc.) */
    if (!ic_analyse_decl_type(kludge, decl_type)) {
        puts("ic_analyse_type_decl_instantiate_generic: call to ic_analyse_decl_type failed");
        return 0;
    }


    /* 8) store in kludge */
    if (!ic_kludge_add_tdecl(kludge, decl_type)) {
        puts("ic_analyse_type_decl_instantiate_generic: call to ic_kludge_add_tdecl failed");
        return 0;
    }

    /* 9) return our new type */
    return decl_type;
}

/* iterate through a type_ref list resolving each type_ref to a decl_type
 *
 * returns 1 on success
 * returns 0 on error
 */
unsigned int ic_analyse_type_ref_list(struct ic_kludge *kludge, struct ic_pvector *type_refs) {
    unsigned int i = 0;
    unsigned int len = 0;
    struct ic_type_ref *tref = 0;
    struct ic_decl_type *tdecl = 0;

    if (!kludge) {
        puts("ic_anlyse_type_ref_list: kludge was null");
        return 0;
    }

    if (!type_refs) {
        puts("ic_anlyse_type_ref_list: type_refs was null");
        return 0;
    }

    /* FIXME TODO below we use kludge_get_decl_type
     * this will resolve every type_ref globally, rather than locally
     * this prevents our type_ref from referencing a local type_param
     * this will need to be fixed
     */

    len = ic_pvector_length(type_refs);
    for (i=0; i<len; ++i) {
        tref = ic_pvector_get(type_refs, i);
        if (!tref) {
            puts("ic_anlyse_type_ref_list: call to ic_pvector_get failed");
            return 0;
        }

        /* get tdecl
         * this will set tdecl on tref
         */
        tdecl = ic_kludge_get_decl_type_from_typeref(kludge, tref);
        if (!tdecl) {
            fputs("ic_anlyse_type_ref_list: call to ic_kludge_get_decl_type_from_typeref failed for typeref: ", stdout);
            ic_type_ref_print(stdout, tref);
            puts("");
            return 0;
        }
    }

    return 1;
}
