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

    if( ic_field_init(field, name_src, name_len, type_src, type_len) ){
        puts("ic_field_new: call to ic_field_init failed");
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
int ic_field_init(struct ic_field *field, char *name_src, unsigned int name_len, char *type_src, unsigned int type_len){
    if( ! field ){
        puts("ic_filed_init: field was null");
        return 1;
    }

    if( ! name_src ){
        puts("ic_filed_init: name_src was null");
        return 1;
    }

    if( ! type_src ){
        puts("ic_filed_init: type_src was null");
        return 1;
    }

    /* init name symbol */
    if( ic_symbol_init( &(field->name), name_src, name_len ) ){
        puts("ic_field_init: call to ic_symbol_init for name failed");
        return 1;
    }

    /* init type symbol */
    if( ic_symbol_init( &(field->type), type_src, type_len ) ){
        puts("ic_field_init: call to ic_symbol_init for type failed");
        return 1;
    }

    return 0;
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

    if( ic_type_decl_init(tdecl, name_src, name_len) ){
        puts("ic_type_decl_new: call to ic_type_decl_init failed");
        return 0;
    }

    return tdecl;
}

/* initialise an existing type_decl
 * only needs name and len
 * will also allocate an empty pvector for fields
 *
 * returns 0 on success
 * returns 1 on error
 */
int ic_type_decl_init(struct ic_type_decl *tdecl, char *name_src, unsigned int name_len){
    if( ! tdecl ){
        puts("ic_type_decl_init: tdecl was null");
        return 1;
    }

    if( ! name_src ){
        puts("ic_type_decl_init: name_src was null");
        return 1;
    }

    /* initialise name */
    if( ic_symbol_init( &(tdecl->name), name_src, name_len ) ){
        puts("ic_type_decl_init: call to ic_symbol_init for name failed");
        return 1;
    }

    /* init fields pvector */
    if( ic_pvector_init( &(tdecl->fields), 0 ) ){
        puts("ic_type_decl_init: call to ic_pvector_init for fields failed");
        return 1;
    }

    return 0;
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

/* allocate and initialise a new ic_decl
 *
 * NOTE: this call will NOT initialise the contents of the union
 * this is left up to the caller
 *
 * returns new ic_decl on success
 * returns 0 on error
 */
struct ic_decl * ic_decl_new(enum ic_decl_type type){
    struct ic_decl *decl = 0;

    /* allocate space */
    decl = calloc(1, sizeof(struct ic_decl));
    if( ! decl ){
        puts("ic_decl_new: call to calloc failed");
        return 0;
    }

    /* initialise */
    if( ic_decl_init(decl, type) ){
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
 * returns 0 on sucess
 * returns 1 on error
 */
int ic_decl_init(struct ic_decl *decl, enum ic_decl_type type){
    if( ! decl ){
        puts("ic_decl_init: decl was null");
        return 1;
    }

    /* set type */
    decl->type = type;

    /* NOTE we do NOT initialise the contents
     * it is up to the caller
     *
     * FIXME: reconsider this interface
     */

    return 0;
}

/* return pointer to ic_func_decl element
 * this function will only success if the decl is of type func_decl
 *
 * returns pointer on success
 * return 0 on failure
 */
struct ic_func_decl * ic_decl_get_fdecl(struct ic_decl *decl){
    if( ! decl ){
        puts("ic_decl_get_fdecl: decl was null");
        return 0;
    }

    /* check we are the right type */
    if( decl->type != type_decl ){
        return 0;
    }

    /* unbox */
    return &(decl->u.fdecl);
}

/* return pointer to cf_type_decl element
 * this function will only success if the decl is of type type_decl
 *
 * returns pointer on success
 * return 0 on failure
 */
struct ic_type_decl * ic_decl_get_tdecl(struct ic_decl *decl){
    if( ! decl ){
        puts("ic_decl_get_tdecl: decl was null");
        return 0;
    }

    /* check we are the right type */
    if( decl->type != type_decl ){
        return 0;
    }

    /* unbox */
    return &(decl->u.tdecl);
}

/* allocate and initialise a new ast
 *
 * returns pointer to new ast on success
 * returns 0 on failure
 */
struct ic_ast * ic_ast_new(void){
    struct ic_ast *ast = 0;

    /* allocate new ast */
    ast = calloc(1, sizeof(struct ic_ast));
    if( ! ast ){
        puts("ic_ast_new: call to calloc failed");
        return 0;
    }

    /* initialise ast */
    if( ic_ast_init(ast) ){
        puts("ic_ast_new: call to ic_ast_init fialed");
        return 0;
    }

    return ast;
}

/* initialise a pre-existing ast
 *
 * returns 0 on success
 * returns 1 on failure
 */
int ic_ast_init(struct ic_ast *ast){
    if( ! ast ){
        puts("ic_ast_init: ast was null");
        return 1;
    }

    /* initialise pvector decls to 0 cap */
    if( ic_pvector_init( &(ast->decls), 0 ) ){
        puts("ic_ast_init: call to ic_pvector_init failed");
        return 1;
    }

    return 0;
}

/* get item stores at index i
 *
 * returns pointer to item on success
 * returns 0 on failure
 */
struct ic_decl * ic_ast_get(struct ic_ast *ast, unsigned int i){
    if( ! ast ){
        puts("ic_ast_get: ast was null");
        return 0;
    }

    /* dispatch to pvector */
    return ic_pvector_get( &(ast->decls), i );
}

/* append decl to ast
 * ast will resize to make room
 *
 * returns index of item on success
 * returns -1 on failure
 */
int ic_ast_append(struct ic_ast *ast, struct ic_decl *decl){
    if( ! ast ){
        puts("ic_ast_append: ast was null");
        return -1;
    }

    if( ! decl ){
        puts("ic_ast_append: decl was null");
        return -1;
    }

    /* dispatch to pvector */
    return ic_pvector_append( &(ast->decls), decl );
}

/* returns length on success
 * returns -1 on failure
 */
int ic_ast_length(struct ic_ast *ast){
    if( ! ast ){
        puts("ic_ast_length: ast was null");
        return -1;
    }

    /* dispatch to pvector */
    return ic_pvector_length( &(ast->decls) );
}


