#include <stdlib.h> /* FIMXE exit */
#include <stdio.h>
#include <string.h> /* strcmp */

#include "../data/set.h"
#include "../parse/data/statement.h"
#include "helpers.h"
#include "data/slot.h"

/* ignored unused parameter and variables */
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"

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
unsigned int ic_analyse_field_list(char *unit, char *unit_name, struct ic_kludge *kludge, struct ic_pvector *fields, char *forbidden_type){

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
    struct ic_type *field_type = 0;

    if( ! unit ){
        puts("ic_analyse_field_list: unit was null");
        return 0;
    }

    if( ! unit_name ){
        puts("ic_analyse_field_list: unit_name was null");
        return 0;
    }

    if( ! kludge ){
        puts("ic_analyse_field_list: kludge was null");
        return 0;
    }

    if( ! fields ){
        puts("ic_analyse_field_list: field was null");
        return 0;
    }

    set = ic_set_new();
    if( ! set ){
        puts("ic_analyse_field_list: call to ic_set_new failed");
        return 0;
    }

    len = ic_pvector_length(fields);
    for( i=0; i<len; ++i ){
        field = ic_pvector_get(fields, i);
        if( ! field ){
            puts("ic_analyse_field_list: call to ic_pvector_get failed");
            goto ERROR;
        }

        /* this is a char* to the inside of symbol
         * so we do not need to worry about free-ing this
         */
        name = ic_symbol_contents(&(field->name));
        if( ! name ){
            puts("ic_analyse_field_list: call to ic_symbol_contents failed for name");
            goto ERROR;
        }

        /* check name is unique within this type decl
         * ic_set_insert makes a strdup copy
         */
        if( ! ic_set_insert(set, name) ){
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
         *
         *  FIXME check / consider this
         */
        type = ic_type_ref_get_symbol(&(field->type));
        if( ! type ){
            printf("ic_analyse_field_list: call to ic_type_ref_get_symbol failed for field '%s' in '%s' for '%s'\n",
                    name,
                    unit,
                    unit_name);
            goto ERROR;
        }

        type_str = ic_symbol_contents(type);
        if( ! type_str ){
            printf("ic_analyse_field_list: call to ic_symbol_contents failed for field '%s' in '%s' for '%s'\n",
                    name,
                    unit,
                    unit_name);
            goto ERROR;
        }

        if( forbidden_type ){
            /* check that the type used is not the same as the 'forbidden_type'
             * this is used to prevent recursive types during tdecl analyse
             */
            if( ! strcmp(type_str, forbidden_type) ){
                printf("ic_analyse_field_list: recursive type detected; '%s' used within '%s' for '%s'\n",
                        type_str,
                        unit,
                        unit_name);
                goto ERROR;
            }
        }

        /* check that this field's type exists */
        field_type = ic_kludge_get_type(kludge, type_str);
        if( ! field_type ){
            printf("ic_analyse_field_list: type '%s' mentioned in '%s' for '%s' does not exist within this kludge\n",
                    type_str,
                    unit,
                    unit_name);
            goto ERROR;
        }

        /* store that type decl on the field (to save lookup costs again later)
         * FIXME are we sure this is safe?
         * if field->type is already a tdecl this will blow up
         */
        /* FIXME deprecated code
        if( ! ic_type_ref_set_tdecl( &(field->type), field_tdecl ) ){
            printf("ic_analyse_field_list: trying to store tdecl for '%s' on field '%s' during '%s' for '%s' failed\n",
                    type_str,
                    name,
                    unit,
                    unit_name);
            goto ERROR;
        }
        */
    }

    /* call set destroy
     * free_set as we allocated above with new
     */
    if( ! ic_set_destroy(set, 1) ){
        puts("ic_analyse_field_list: call to ic_set_destroy failed in error case");
    }

    return 1;

ERROR:

    /* call set destroy
     * free_set as we allocated above with new
     */
    if( ! ic_set_destroy(set, 1) ){
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
 * returns 1 on success (pass)
 * returns 0 on failure
 */
unsigned int ic_analyse_body(char *unit, char *unit_name, struct ic_kludge *kludge, struct ic_body *body, struct ic_scope *parent_scope){
    /* index into body */
    unsigned int i = 0;
    /* len of body */
    unsigned int len = 0;
    /* current statement in body */
    struct ic_stmt *stmt = 0;

    if( ! unit ){
        puts("ic_analyse_body: unit was null");
        return 0;
    }

    if( ! unit_name ){
        puts("ic_analyse_body: unit_name was null");
        return 0;
    }

    if( ! kludge ){
        puts("ic_analyse_body: kludge was null");
        return 0;
    }

    if( ! body ){
        puts("ic_analyse_body: body was null");
        return 0;
    }

    if( body->scope ){
        puts("ic_analyse_body: body already had a scope, error");
        return 0;
    }

    /* FIXME this scope is currently leaked */
    body->scope = ic_scope_new(parent_scope);
    if( ! body->scope) {
        puts("ic_analyse_body: call to ic_body_new failed");
        return 0;
    }

    /* step through body checking each statement
     * FIXME
     */
    len = ic_body_length(body);
    for( i=0; i<len; ++i ){
        stmt = ic_body_get(body, i );
        if( ! stmt ){
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
        switch( stmt->type ){
            case ic_stmt_type_ret:
                /* infer type of expression
                 * check returned value matches declared return type
                 */
                puts("ic_analyse_body: unimplemented stmt->type ic_stmt_type_ret");
                goto ERROR;
                break;

            case ic_stmt_type_let:
                /* infer type of init. expression
                 * check against declared let type (if declared)
                 * check all mentioned types exist
                 *
                 * check that this let isn't masking an existing slot in scope
                 * create a new slot for this let and insert it into the current scope
                 */
                if( ! ic_analyse_let(unit, unit_name, kludge, body, ic_stmt_get_let(stmt)) ){
                    puts("ic_analyse_body: call to ic_analyse_let failed");
                    goto ERROR;
                }

                break;

            case ic_stmt_type_if:
                /* need to validate expression
                 * need to the recurse to validate the body in
                 * each branch
                 */
                puts("ic_analyse_body: unimplemented stmt->type ic_stmt_type_if");
                goto ERROR;
                break;

            case ic_stmt_type_expr:
                /* infer expr type
                 * warn if using non-void function in void context
                 */
                puts("ic_analyse_body: unimplemented stmt->type ic_stmt_type_expr");
                goto ERROR;
                break;

            default:
                printf("ic_analyse_body: impossible stmt->type '%d'\n", stmt->type);
                goto ERROR;
                break;
        }

        /* FIXME */
    }

    puts("ic_analyse_body: unimplemented");
    return 1;

ERROR:

    puts("ic_analyse_body: unimplemented in error case");
    return 0;
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
 *  infer 1 to Int
 *  infer "hello" to String
 *  infer addone(1) which is a call to addone(Int) -> Int therefore Int
 *  infer Foo(1 "hello") which is call to Foo(Int String) -> Foo therefore Foo
 *
 * returns ic_type * on success
 * returns 0 on failure
 */
struct ic_type * ic_analyse_infer(struct ic_kludge *kludge, struct ic_scope *scope, struct ic_expr *expr){
    struct ic_symbol *sym = 0;
    char *ch = 0;
    struct ic_slot *slot = 0;
    struct ic_type *type = 0;

    struct ic_expr_func_call *fcall = 0;
    struct ic_expr_identifier *id = 0;
    struct ic_expr_constant *cons  = 0;
    struct ic_expr_operator *op  = 0;

    if( ! kludge ){
        puts("ic_analyse_infer: kludge was null");
        return 0;
    }

    if( ! expr ){
        puts("ic_analyse_infer: expr was null");
        return 0;
    }

    switch( expr->type ){
        case ic_expr_type_func_call:
            /*
             *  infer addone(1) -> addone(Int)->Int -> Int
             *  expr->type == func_call
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
             *  infer Foo(1 "hello") -> Foo(Int String) -> Foo
             *  expr->type == func_call
             *  fc = expr->u.fcall
             *  fstr = str(fc)
             *      TODO note that icarus fcalls are dependent on arg types
             *           so we need to infer arg types by calling ic_analyse_infer on the arguments
             *  fdecl = kludge get fdecl from fstr
             *  tstr = fdecl->ret_type
             *  type = kludge get tdecl from tstr
             *  return type
             */

            puts("ic_analyse_infer: ic_expr_type_func_call unimplemented");
            return 0;

        case ic_expr_type_identifier:
            /*
             *  infer f -> typeof contents of f
             *  expr->type = identifier
             *  id = expr->u.id
             *  name = str(id)
             *  variable = get variable within scope name
             *  type = get type of variable
             *  return type
             */

            /* get symbol */
            id = ic_expr_get_identifier(expr);
            if( ! id ){
                puts("ic_analyse_infer: ic_expr_type_identifier: call to ic_expr_get_identifier failed");
                return 0;
            }

            sym = ic_expr_identifier_symbol(id);
            if( ! sym ){
                puts("ic_analyse_infer: ic_expr_type_identifier: call to ic_expr_identifier_symbol failed");
                return 0;
            }

            /* char* representation */
            ch = ic_symbol_contents(sym);
            if( ! ch ){
                puts("ic_analyse_infer: ic_expr_type_identifier: call to ic_symbol_contents failed");
                return 0;
            }

            /* get slot from identifier (variable) name */
            slot = ic_scope_get(scope, ch);
            if( ! slot ){
                /* FIXME unknown identifier
                 * need helpful output here
                 */
                printf("ic_analyse_infer: unknown identifier '%s', not in scope\n", ch);
                return 0;
            }

            type = slot->type;
            if( ! type ){
                printf("ic_analyse_infer: error fetching identifier '%s', no type found\n", ch);
                return 0;
            }

            /* return type */
            return type;

            puts("ic_analyse_infer: ic_expr_type_identifier unimplemented");
            return 0;

        case ic_expr_type_constant:
            cons = ic_expr_get_constant(expr);
            if( ! cons ){
                puts("ic_analyse_infer: constant: call to ic_expr_get_constant failed");
                return 0;
            }

            switch( cons->type ){
                /*
                 *  infer 1 -> Int
                 *  expr->type == constant
                 *  cons = expr->u.cons
                 *  cons->type == integer
                 *  return integer
                 */
                case ic_expr_constant_type_integer:
                    /* FIXME decide on type case sensitivity */
                    type = ic_scope_get(scope, "Int");
                    if( ! type ){
                        puts("ic_analyse_infer: constant: Int: call to ic_scope_get failed");
                        return 0;
                    }
                    return type;
                    break;

                /*
                 *  infer "hello" -> String
                 *  expr->type == constant
                 *  cons = expr->u.cons
                 *  cons->type == string
                 *  return string
                 */
                case ic_expr_constant_type_string:
                    /* FIXME decide on type case sensitivity */
                    type = ic_scope_get(scope, "String");
                    if( ! type ){
                        puts("ic_analyse_infer: constant: String: call to ic_scope_get failed");
                        return 0;
                    }
                    return type;
                    break;

                default:
                    puts("ic_analyse_infer: constant: impossible and unexpected constant, neither integer not string");
                    break;
            }

            puts("ic_analyse_infer: ic_expr_type_constant unimplemented");
            return 0;

        case ic_expr_type_operator:
            puts("ic_analyse_infer: ic_expr_type_operator unimplemented");
            return 0;

        default:
            printf("ic_analyse_infer: unknown expr->type '%d'\n", expr->type);
            return 0;
    }

    puts("ic_analyse_infer: unimplemented");
    return 0;
}

/* check a statement for validity
 *
 * FIXME need a way of signalling and passing errors
 *
 * examples:
 *  check let f::Foo = Foo(addone(1) "hello")
 *  check d(f)
 *  check print(s)
 *
 * returns 1 for success
 * returns 0 on failure
 */
unsigned int ic_analyse_check(struct ic_kludge *kludge, struct ic_stmt *stmt){
    puts("ic_analyse_check: unimplemented");
    exit(1);
}

/* perform analyse of let statement in the provided body
 *
 * returns 1 for success
 * returns 0 on failure
 */
unsigned int ic_analyse_let(char *unit, char *unit_name, struct ic_kludge *kludge, struct ic_body *body, struct ic_stmt_let *let){
    struct ic_slot *slot = 0;
    struct ic_type *type = 0;

    if( ! unit ){
        puts("ic_analyse_let: unit was null");
        return 0;
    }

    if( ! unit_name ){
        puts("ic_analyse_let: unit_name was null");
        return 0;
    }

    if( ! kludge ){
        puts("ic_analyse_let: kludge was null");
        return 0;
    }

    if( ! body ){
        puts("ic_analyse_let: body was null");
        return 0;
    }

    if( ! body->scope ){
        puts("ic_analyse_let: body is lacking scope");
        return 0;
    }

    if( ! let ){
        puts("ic_analyse_let: let was null");
        return 0;
    }

    /* FIXME slot creation time
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

    /* we need to take teh let->type (symbol) and convert it to a full
     * ic_type
     *
     * FIXME later on let->type may be null and we will instead
     * need to run inference against the init
     */
    if( ! ic_symbol_length(&(let->type)) ){
        puts("ic_analyse_let: let was lacking a type, inference not yet supported");
        return 0;
    }

    /* FIXME fetch type from kludge
     * not currently supported as here we want an ic_type
     * but the kludge is storing tdecls
     */
    type = 0;

    /*
     * FIXME ref is hardcoded (0) to false as they are not
     * currently supported
     *
     *                (name, type, mut, ref)
     */
    slot = ic_slot_new(&(let->identifier), type, let->mut, 0);
    if( ! slot ){
        puts("ic_analyse_let: call to ic_slot_new failed");
        return 0;
    }

    /* need to store slot in body->scope
     * FIXME this handling of symbols is a bit gross
     */
    if( ! ic_scope_insert( body->scope, ic_symbol_contents(&(let->identifier)), slot ) ){
        puts("ic_analyse_let: call to ic_scope_insert failed");
        return 0;
    }

    puts("ic_analyse_let: implementation pending");
    return 1;
}


