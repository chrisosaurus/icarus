#include <stdio.h> /* puts, fprintf */
#include <stdlib.h> /* calloc*/

#include "ast.h"
#include "pvector.h"
#include "symbol.h"

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

    /* init name symbol */
    if( ic_symbol_init( &(field->name), name_src, name_len ) ){
        puts("ic_field_new: call to ic_symbol_init for name failed");
        return 0;
    }

    /* init type symbol */
    if( ic_symbol_init( &(field->type), type_src, type_len ) ){
        puts("ic_field_new: call to ic_symbol_init for type failed");
        return 0;
    }

    return field;
}

/* print the field to stdout */
void ic_field_print(struct ic_field *field){
    if( ! field ){
        puts("ic_field_print: field was null");
        return;
    }
    printf("%s::%s",
           ic_symbol_contents(&(field->name))  ,
           ic_symbol_contents(&(field->type)) );

}

/* allocate and return a new type_decl
 * only needs name and len
 * will also allocate an empty pvector for fields
 *
 * returns new ic_field * on success
 * returns 0 on failure
 */
struct ic_type_decl * ic_type_decl_new(char *name_src, unsigned int name_len){
    struct ic_type_decl * tdecl = 0;

    /* allocate type_decl */
    tdecl = calloc(1, sizeof(struct ic_type_decl));
    if( ! tdecl ){
        puts("ic_type_decl_new: calloc failed");
        return 0;
    }

    /* initialise name */
    if( ic_symbol_init( &(tdecl->name), name_src, name_len ) ){
        puts("ic_type_decl_new: call to ic_symbol_init for name failed");
        return 0;
    }

    /* init fields pvector */
    if( ic_pvector_init( &(tdecl->fields), 0 ) ){
        puts("ic_type_decl_new: call to ic_pvector_init for fields failed");
        return 0;
    }

    return tdecl;
}

/* add a new field to types list of fields
 *
 * returns 0 on success
 * returns 1 on error
 */
int ic_type_decl_add_field(struct ic_type_decl *tdecl, struct ic_field *field){
    if( ! tdecl ){
        puts("ic_type_decl_add_field: tdecl was null");
        return 1;
    }

    if( ! field ){
        puts("ic_type_decl_add_field: field was null");
        return 1;
    }

    if( ic_pvector_append( &(tdecl->fields), field ) == -1 ){
        puts("ic_type_decl_add_field: call to ic_pvector_append failed");
        return 1;
    }

    return 0;
}

/* print the type_decl to stdout */
void ic_type_decl_print(struct ic_type_decl *tdecl){
    unsigned int i = 0;

    if( ! tdecl ){
        puts("ic_type_decl_print: tdecl was null");
        return;
    }

    /* print type and name */
    printf ("type %s\n", ic_symbol_contents(&(tdecl->name)) );

    /* iterate through pvector fields
     * prefix each field with a 2 spaces  and postfix each with a \n
     */
    for( i=0; i< ic_pvector_length(&(tdecl->fields)); ++i ){
        /* prefix 2 spaces */
        printf("  ");

        /* print field contents */
        ic_field_print( ic_pvector_get( &(tdecl->fields), i ) );

        /* postfix newline */
        puts("");
    }

    puts("end");
}


