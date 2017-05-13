#include <stdio.h>
#include <stdlib.h> /* FIMXE exit */
#include <string.h> /* strcmp */

#include "../data/set.h"
#include "../parse/data/stmt.h"
#include "../parse/permissions.h"
#include "data/slot.h"
#include "helpers.h"

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
 *  c) (if provided) that no field's types eq forbidden_type
 *
 * `unit` and `unit_name` are used for error printing
 * it is always printed as '%s for %s error goes here'
 * e.g. unit of 'function declaration', name of 'Foo'
 *
 * returns 1 on success (all fields are valid as per the 3 rules)
 * returns 0 on failure
 */
unsigned int ic_analyse_field_list(char *unit, char *unit_name, struct ic_kludge *kludge, struct ic_pvector *fields, char *forbidden_type) {

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
        type = ic_type_ref_get_symbol(&(field->type));
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

        if (forbidden_type) {
            /* check that the type used is not the same as the 'forbidden_type'
             * this is used to prevent recursive types during tdecl analyse
             */
            if (!strcmp(type_str, forbidden_type)) {
                printf("ic_analyse_field_list: recursive type detected; '%s' used within '%s' for '%s'\n",
                       type_str,
                       unit,
                       unit_name);
                goto ERROR;
            }
        }

        /* check that this field's type exists */
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

        /* store that type decl on the field (to save lookup costs again later)
         * if field->type is already a tdecl this will blow up
         */
        if (!ic_type_ref_set_type_decl(&(field->type), field_type)) {
            printf("ic_analyse_field_list: trying to store tdecl for '%s' on field '%s' during '%s' for '%s' failed\n",
                   type_str,
                   name,
                   unit,
                   unit_name);
            goto ERROR;
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
    unsigned int i = 0;
    /* len of body */
    unsigned int len = 0;
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
    /* if from stmt */
    struct ic_stmt_if *sif = 0;
    /* assign from stmt */
    struct ic_stmt_assign *assign = 0;
    /* new scope for body of if */
    struct ic_scope *if_scope = 0;
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
    len = ic_body_length(body);
    for (i = 0; i < len; ++i) {
        stmt = ic_body_get(body, i);
        if (!stmt) {
            printf("ic_analyse_body: call to ic_body_get failed for i '%d' in '%s' for '%s'\n",
                   i,
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

                other_type = ic_kludge_get_decl_type_from_symbol(kludge, fdecl->ret_type);
                if (!other_type) {
                    puts("ic_analyse_body: call to ic_kludge_get_decl_from_symbol failed");
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
                    /* attach new scope to if body
                   * FIXME this scope is leaked */
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

                len = ic_stmt_match_cases_length(match);
                for (i = 0; i < len; ++i) {
                    scase = ic_stmt_match_cases_get(match, i);
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

                    field_type = ic_kludge_get_decl_type_from_typeref(kludge, &(field->type));
                    if (!field_type) {
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
                    len = ic_decl_type_field_length(type);

                    if (match_cases != len) {
                        printf("ic_analyse_body: match: insufficient match_cases, expected '%d', found '%d'\n", len, match_cases);
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
    struct ic_symbol *sym = 0;
    char *ch = 0;
    struct ic_decl_func *fdecl = 0;

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
    } else {
        /* otherwise work out the fdecl */
        ch = ic_analyse_fcall_str(kludge, scope, fcall);
        if (!ch) {
            puts("ic_analyse_infer_fcall: call to ic_analyse_fcall_str failed");
            return 0;
        }

        fdecl = ic_kludge_get_fdecl(kludge, ch);
        if (!fdecl) {
            /* FIXME return type not found
           * need helpful error message
           */
            printf("ic_analyse_infer_fcall: error finding fdecl for fcall '%s'\n", ch);
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
    sym = fdecl->ret_type;
    if (!sym) {
        puts("ic_analyse_infer_fcall: failed to get fdecl->ret_type");
        return 0;
    }

    ch = ic_symbol_contents(sym);
    if (!ch) {
        puts("ic_analyse_infer_fcall: call to ic_symbol_contents failed for ret_type");
        return 0;
    }

    /* get return type from this function we found */
    type = ic_kludge_get_decl_type(kludge, ch);
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
         *  infer 1 -> Sint
         *  expr->tag == constant
         *  cons = expr->u.cons
         *  cons->type == integer
         *  return integer
         */
        case ic_expr_constant_type_integer:
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

    if (op->fcall) {
        puts("ic_analyse_infer_operator: this operator already had an fcall set");
        return 0;
    }

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
    switch (let->tref.tag) {
        case ic_type_ref_unknown:
            /* no declared let type, just use inferred type */
            type = init_type;
            break;

        case ic_type_ref_symbol:
            /* type declared, compare */
            type = ic_kludge_get_decl_type_from_typeref(kludge, &(let->tref));
            if (!type) {
                puts("ic_analyse_let: failed to find type from typeref");
                printf("failed to find declared type '%s'\n", ic_symbol_contents(ic_type_ref_get_symbol(&(let->tref))));
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
            puts("ic_analyse_let: internal error: tref.tag was ic_type_ref_resolved");
            return 0;
            break;

        default:
            /* error, impossible tref.tag */
            puts("ic_analyse_let: impossible tref.tag");
            return 0;
            break;
    }

    /* either way we set the type we now have for this let */
    if (!ic_stmt_let_set_inferred_type(let, type)) {
        puts("ic_analyse_let: call to ic_stmt_let_inferred_type failed");
        return 0;
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
 * returns char * on success
 * returns 0 on failure
 */
char *ic_analyse_fcall_str(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_expr_func_call *fcall) {
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
    struct ic_symbol *type_name = 0;
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
        type_name = ic_decl_type_name(expr_type);
        if (!type_name) {
            printf("ic_analyse_fcall_str: call to ic_type_name failed for argument '%d'\n", i);
            goto ERROR;
        }

        /* append our argument type */
        if (!ic_string_append_symbol(str, type_name)) {
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
    /* we rely on fcall->string holding onto the ic_string
     * otherwise we would have to clean up
     * this is freed as part of ic_expr_func_call_destroy
     */
    return ic_string_contents(fcall->string);
}
