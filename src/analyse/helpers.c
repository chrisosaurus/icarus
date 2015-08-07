#include <stdlib.h> /* FIMXE exit */
#include <stdio.h>
#include <string.h> /* strcmp */

#include "../data/set.h"
#include "../parse/data/statement.h"
#include "helpers.h"

/* iterate through the field list checking:
 *  a) all field's names are unique within this list
 *  b) all field's types exist in this kludge
 *  c) (if provided) that no field's types eq forbidden_type
 *
 * `unit` and `unit_name` are used for error printing
 * it is always printed as '%s for %s error goes here'
 * e.g. unit of 'function declaration', name of 'Foo'
 *
 * returns 0 on success (all fields are valid as per the 3 rules)
 * returns 1 on failure
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
    struct ic_type_decl *field_tdecl = 0;

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
        return 1;
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
         * note that field->type may be in a few states,
         * if field->type.type is one of
         *  ic_type_ref_tdecl
         *  ic_type_ref_builtin
         * then checking the type exists (c) is wasted effort
         * as we already know the type exists
         *
         * at this point however every type should still be
         *  ic_type_ref_symbol
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
            /* check that the type used is not the same as the 'firbidden_type'
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
        field_tdecl = ic_kludge_get_tdecl(kludge, type_str);
        if( ! field_tdecl ){
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
        if( ic_type_ref_set_tdecl( &(field->type), field_tdecl ) ){
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
    if( ! ic_set_destroy(set, 1) ){
        puts("ic_analyse_field_list: call to ic_set_destroy failed in error case");
    }

    return 0;

ERROR:

    /* call set destroy
     * free_set as we allocated above with new
     */
    if( ! ic_set_destroy(set, 1) ){
        puts("ic_analyse_field_list: call to ic_set_destroy failed in error case");
    }


    return 1;
}

/* perform analysis on body
 * this will iterate through each statement and perform analysis
 *
 * `unit` and `unit_name` are used for error printing
 * it is always printed as '%s for %s error goes here'
 * e.g. unit of 'function declaration', name of 'Foo'
 * returns 0 on success (pass)
 * returns 1 on failure
 */
unsigned int ic_analyse_body(char *unit, char *unit_name, struct ic_kludge *kludge, struct ic_body *body){
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
                puts("ic_analyse_body: unimplemented stmt->type ic_stmt_type_ref");
                goto ERROR;

            case ic_stmt_type_let:
                /* infer type of init. expression
                 * check against declared let type (if declared)
                 * check all mentioned types exist
                 */
                puts("ic_analyse_body: unimplemented stmt->type ic_stmt_type_let");
                goto ERROR;

            case ic_stmt_type_if:
                /* need to validate expression
                 * need to the recurse to validate the body in
                 * each branch
                 */
                puts("ic_analyse_body: unimplemented stmt->type ic_stmt_type_if");
                goto ERROR;

            case ic_stmt_type_expr:
                /* infer expr type
                 * warn if using non-void function in void context
                 */
                puts("ic_analyse_body: unimplemented stmt->type ic_stmt_type_expr");
                goto ERROR;

            default:
                printf("ic_analyse_body: impossible stmt->type '%d'\n", stmt->type);
                goto ERROR;
        }

        /* FIXME */
    }

    puts("ic_analyse_body: unimplemented");
    return 1;

ERROR:

    puts("ic_analyse_body: unimplemented in error case");
    return 1;
}


