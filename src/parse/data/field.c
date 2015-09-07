#include <stdlib.h> /* calloc */
#include <stdio.h> /* puts, printf */

#include "field.h"

/* allocate and return a new field
 * takes 2 tokens as char * and len pairs
 *
 * returns new ic_field * on success
 * returns 0 on failure
 */
struct ic_field * ic_field_new(char *name_src, unsigned int name_len, char *type_src, unsigned int type_len){
    struct ic_field *field = 0;

    /* allocate space for our field */
    field = calloc(1, sizeof(struct ic_field));
    if( ! field ){
        puts("ic_field_new: calloc failed");
        return 0;
    }

    if( ic_field_init(field, name_src, name_len, type_src, type_len) ){
        puts("ic_field_new: call to ic_field_init failed");
        free(field);
        return 0;
    }

    return field;
}

/* initialise an existing field
 * takes 2 tokens as char * and len pairs
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_field_init(struct ic_field *field, char *name_src, unsigned int name_len, char *type_src, unsigned int type_len){
    if( ! field ){
        puts("ic_field_init: field was null");
        return 1;
    }

    if( ! name_src ){
        puts("ic_field_init: name_src was null");
        return 1;
    }

    if( ! type_src ){
        puts("ic_field_init: type_src was null");
        return 1;
    }

    /* init name symbol */
    if( ! ic_symbol_init( &(field->name), name_src, name_len ) ){
        puts("ic_field_init: call to ic_symbol_init for name failed");
        return 1;
    }

    /* init type symbol */
    if( ic_type_ref_symbol_init( &(field->type), type_src, type_len ) ){
        puts("ic_field_init: call to ic_type_symbol_init for type failed");
        return 1;
    }

    return 0;
}

/* destroy field
 *
 * will free field if `free_field` is truhty
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_field_destroy(struct ic_field *field, unsigned int free_field){
    if( ! field ){
        puts("ic_field_destroy: field was null");
        return 1;
    }

    /* dispatch to symbol destroy for name
     * note we do not ask it to destroy_symbol
     * as it is a member
     */
    if( ! ic_symbol_destroy( &(field->name), 0 ) ){
        puts("ic_field_destroy: name: call to ic_symbol_destroy failed");
        return 1;
    }

    /* dispatch to symbol destroy for type
     * note we do not ask it to destroy_symbol
     * as it is a member
     */
    if( ic_type_ref_destroy( &(field->type), 0 ) ){
        puts("ic_field_destroy: type: call to ic_type_destroy failed");
        return 1;
    }

    /* free field if we are asked nicely */
    if( free_field ){
        free(field);
    }

    return 0;
}


/* print the field to stdout */
void ic_field_print(struct ic_field *field){
    if( ! field ){
        puts("ic_field_print: field was null");
        return;
    }
    printf("%s::", ic_symbol_contents(&(field->name)) );
    ic_type_ref_print(&(field->type));
}

