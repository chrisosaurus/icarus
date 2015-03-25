#include "stdlib.h" /* calloc */
#include "stdio.h" /* puts, printf */

#include "decl.h"
#include "field.h"
#include "pvector.h"
#include "statement.h"
#include "symbol.h"
#include "parse.h"

/* allocate and initialise a new func_decl
 *
 * returns new func_decl on success
 * returns 0 on error
 */
struct ic_func_decl * ic_func_decl_new(char *name, unsigned int name_len){
    struct ic_func_decl *fdecl = 0;

    if( ! name ){
        puts("ic_func_decl_new: name was null");
        return 0;
    }

    /* allocate new fdecl */
    fdecl = calloc(1, sizeof(struct ic_func_decl));
    if( ! fdecl ){
        puts("ic_func_decl_new: calloc failed");
        return 0;
    }

    /* initialise */
    if( ic_func_decl_init(fdecl, name, name_len) ){
        puts("ic_func_decl_new: call to ic_func_decl_init failed");
        return 0;
    }

    return fdecl;
}

/* initialise an existing func_decl
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_func_decl_init(struct ic_func_decl *fdecl, char *name, unsigned int name_len){
    if( ! fdecl ){
        puts("ic_func_decl_init: fdecl was null");
        return 1;
    }

    if( ! name ){
        puts("ic_func_decl_init: name was null");
        return 1;
    }

    /* initialise name symbol */
    if( ic_symbol_init( &(fdecl->name), name, name_len ) ){
        puts("ic_func_decl_init: call to ic_symbol_init failed");
        return 1;
    }

    /* initialise empty args pvector */
    if( ic_pvector_init( &(fdecl->args), 0 ) ){
        puts("ic_func_decl_init: call to ic_pvector_init failed");
        return 1;
    }

    /* initialise our empty body */
    if( ic_body_init( &(fdecl->body) ) ){
        puts("ic_func_decl_init: call to ic_body_init failed");
        return 1;
    }

    return 0;
}

/* add new arg field to func_decl
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_func_decl_add_arg(struct ic_func_decl *fdecl, struct ic_field *field){
    if( ! fdecl ){
        puts("ic_func_decl_add_arg: fdecl was null");
        return 1;
    }

    if( ! field ){
        puts("ic_func_decl_add_arg: field was null");
        return 1;
    }

    /* append field
     * returns -1 on failure
     */
    if( ic_pvector_append( &(fdecl->args), field ) == -1 ){
        puts("ic_func_decl_add_arg: call to ic_pvector_append failed");
        return 1;
    }

    return 0;
}

/* add new stmt to the body
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_func_decl_add_stmt(struct ic_func_decl *fdecl, struct ic_stmt *stmt){
    if( ! fdecl ){
        puts("ic_func_decl_add_stmt: fdecl was null");
        return 1;
    }
    if( ! stmt ){
        puts("ic_func_decl_add_stmt: stmt was null");
        return 1;
    }

    /* add our element */
    if( ic_body_append( &(fdecl->body), stmt ) == -1 ){
        puts("ic_func_decl_add_stmt: call to ic_body_append failed");
        return 1;
    }

    return 0;
}

/* print func_decl */
void ic_func_decl_print(struct ic_func_decl *fdecl, unsigned int *indent_level){
    /* offset into args
     * re-used as offset into body
     * */
    unsigned int i = 0;
    /* len of args
     * re-used as len of body
     */
    unsigned int len = 0;

    if( ! fdecl ){
        puts("ic_func_decl_print: fdecl was null");
        return;
    }

    if( ! indent_level ){
        puts("ic_func_decl_print: indent_level was null");
        return;
    }

    /* print `function`, name, and opening bracket */
    printf("function %s(", ic_symbol_contents( &(fdecl->name) ));

    len = ic_pvector_length( &(fdecl->args) );

    /* print arguments */
    for( i=0; i < len; ++i ){
        ic_field_print( ic_pvector_get( &(fdecl->args), i ) );

        /* print a space between each arg
         * but only if we are not the last arg
         */
        if( i < (len - 1) ){
            printf(" ");
        }
    }

    /* closing bracket */
    puts(")");

    /* get length of body */
    len = ic_body_length( &(fdecl->body) );


    /* print body
     * body will handle indent_level incr and decr for us
     */
    ic_body_print( &(fdecl->body), indent_level);

    /* print end\n */
    puts("end");
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
unsigned int ic_type_decl_init(struct ic_type_decl *tdecl, char *name_src, unsigned int name_len){
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
unsigned int ic_type_decl_add_field(struct ic_type_decl *tdecl, struct ic_field *field){
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
void ic_type_decl_print(struct ic_type_decl *tdecl, unsigned int *indent_level){
    unsigned int i = 0;

    if( ! tdecl ){
        puts("ic_type_decl_print: tdecl was null");
        return;
    }
    if( ! indent_level ){
        puts("ic_type_decl_print: indent_level was null");
        return;
    }

    /* print type and name */
    printf ("type %s\n", ic_symbol_contents(&(tdecl->name)) );

    /* increment indent level before body */
    ++ *indent_level;

    /* iterate through pvector fields
     * prefix each field with a 2 spaces  and postfix each with a \n
     */
    for( i=0; i< ic_pvector_length(&(tdecl->fields)); ++i ){
        /* print indent */
        ic_parse_print_indent(*indent_level);

        /* print field contents */
        ic_field_print( ic_pvector_get( &(tdecl->fields), i ) );

        /* postfix newline */
        puts("");
    }

    /* decrement indent level after body */
    -- *indent_level;

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
unsigned int ic_decl_init(struct ic_decl *decl, enum ic_decl_type type){
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

/* returns pointer to ic_func_decl element
 * this function will only success if the decl is of type func_decl
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_func_decl * ic_decl_get_fdecl(struct ic_decl *decl){
    if( ! decl ){
        puts("ic_decl_get_fdecl: decl was null");
        return 0;
    }

    /* check we are the right type */
    if( decl->type != ic_decl_func_decl ){
        return 0;
    }

    /* unbox */
    return &(decl->u.fdecl);
}

/* returns pointer to cf_type_decl element
 * this function will only success if the decl is of type type_decl
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_type_decl * ic_decl_get_tdecl(struct ic_decl *decl){
    if( ! decl ){
        puts("ic_decl_get_tdecl: decl was null");
        return 0;
    }

    /* check we are the right type */
    if( decl->type != ic_decl_type_decl ){
        return 0;
    }

    /* unbox */
    return &(decl->u.tdecl);
}

/* print contents of ic_decl */
void ic_decl_print(struct ic_decl *decl, unsigned int *indent_level){
    if( ! decl ){
        puts("ic_decl_print: decl was null");
        return;
    }
    if( ! indent_level ){
        puts("ic_decl_print: indent_level was null");
        return;
    }

    switch( decl->type ){
        case ic_decl_func_decl:
            ic_func_decl_print( ic_decl_get_fdecl(decl), indent_level );
            break;
        case ic_decl_type_decl:
            ic_type_decl_print( ic_decl_get_tdecl(decl), indent_level );
            break;
        default:
            puts("ic_decl_print: impossible type!");
            break;
    }
}


