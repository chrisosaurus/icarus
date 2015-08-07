#include <stdlib.h> /* FIMXE exit */
#include <stdio.h>
#include <string.h> /* strcmp */

#include "../data/set.h"
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


