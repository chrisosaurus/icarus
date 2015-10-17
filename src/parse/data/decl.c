#include <stdlib.h> /* calloc */
#include <stdio.h> /* puts, printf */
#include <string.h> /* strncpy */

#include "decl.h"
#include "field.h"
#include "../../data/pvector.h"
#include "stmt.h"
#include "../../data/symbol.h"
#include "../../data/string.h"
#include "../parse.h"

/* FIXME crutch for unused param */
#pragma GCC diagnostic ignored "-Wunused-parameter"

/* allocate and initialise a new decl_func
 *
 * returns new decl_func on success
 * returns 0 on failure
 */
struct ic_decl_func * ic_decl_func_new(char *name, unsigned int name_len){
    struct ic_decl_func *fdecl = 0;

    if( ! name ){
        puts("ic_decl_func_new: name was null");
        return 0;
    }

    /* allocate new fdecl */
    fdecl = calloc(1, sizeof(struct ic_decl_func));
    if( ! fdecl ){
        puts("ic_decl_func_new: calloc failed");
        return 0;
    }

    /* initialise */
    if( ! ic_decl_func_init(fdecl, name, name_len) ){
        puts("ic_decl_func_new: call to ic_decl_func_init failed");
        free(fdecl);
        return 0;
    }

    return fdecl;
}

/* initialise an existing decl_func
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_func_init(struct ic_decl_func *fdecl, char *name, unsigned int name_len){
    if( ! fdecl ){
        puts("ic_decl_func_init: fdecl was null");
        return 0;
    }

    if( ! name ){
        puts("ic_decl_func_init: name was null");
        return 0;
    }

    /* initialise name symbol */
    if( ! ic_symbol_init( &(fdecl->name), name, name_len ) ){
        puts("ic_decl_func_init: call to ic_symbol_init for name failed");
        return 0;
    }

    /* initialise empty args pvector */
    if( ! ic_pvector_init( &(fdecl->args), 0 ) ){
        puts("ic_decl_func_init: call to ic_pvector_init failed");
        return 0;
    }

    /* initialise empty string fdecl->string */
    if( ! ic_string_init_empty( &(fdecl->string) ) ){
        puts("ic_decl_func_init: call to ic_string_init_empty for string failed");
        return 0;
    }

    /* initialise return type to 0 (void) */
    fdecl->ret_type = 0;

    /* initialise our empty body */
    if( ! ic_body_init( &(fdecl->body) ) ){
        puts("ic_decl_func_init: call to ic_body_init failed");
        return 0;
    }

    return 1;
}

/* calls destroy on every element within
 *
 * this will only free the fdecl if `free_fdecl` is truthy
 *
 * the caller must determine if it is appropriate
 * or not to call free(decl)
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_func_destroy(struct ic_decl_func *fdecl, unsigned int free_fdecl){
    int i = 0;
    int len = 0;

    struct ic_field *field = 0;

    if( ! fdecl ){
        puts("ic_decl_type_destroy: fdecl was null");
        return 0;
    }

    /* free symbol contents but do not free symbol itself
     * since it is an element on fdecl
     */
    if( ! ic_symbol_destroy(&(fdecl->name), 0) ){
        puts("ic_decl_type_destroy: for name call to ic_symbol_destroy failed");
        return 0;
    }

    /* free string contents but do not free string itself
     * since it is an element on fdecl
     */
    if( ! ic_string_destroy(&(fdecl->string), 0) ){
        puts("ic_decl_type_destroy: for string call to ic_string_destroy failed");
        return 0;
    }

    len = ic_pvector_length( &(fdecl->args) );

    for( i=0; i<len; ++i ){
        field = ic_pvector_get( &(fdecl->args), i );
        if( ! field ){
            puts("ic_decl_type_destroy: call to ic_pvector_get failed");
            return 0;
        }

        /* dispatch to field destroy
         * free_field set to 1
         */
        if( ! ic_field_destroy(field, 1) ){
            puts("ic_decl_type_destroy: call to ic_field_destroy failed");
            return 0;
        }
    }

    /* only need to free if we have a ret_type */
    if( fdecl->ret_type ){
        /* free symbol contents and free symbol
         * as our ret type is always allocated
         *
         *    // ic_decl_func_set_return :
         *    fdecl->ret_type = ic_symbol_new(type, type_len);
         */
        if( ! ic_symbol_destroy(fdecl->ret_type, 1) ){
            puts("ic_decl_type_destroy: for ret_type call to ic_symbol_destroy failed");
            return 0;
        }
    }

    /* free body contents but do not free body itself
     * since it is an element on fdecl
     */
    if( ! ic_body_destroy(&(fdecl->body), 0) ){
        puts("ic_decl_type_destroy: for body call to ic_body_destroy failed");
        return 0;
    }

    /* only free if caller asked */
    if( free_fdecl ){
        free(fdecl);
    }

    /* success */
    return 1;
}

/* add new arg field to decl_func
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_func_add_arg(struct ic_decl_func *fdecl, struct ic_field *field){
    if( ! fdecl ){
        puts("ic_decl_func_add_arg: fdecl was null");
        return 0;
    }

    if( ! field ){
        puts("ic_decl_func_add_arg: field was null");
        return 0;
    }

    /* append field returns -1 on failure */
    if( -1 == ic_pvector_append( &(fdecl->args), field ) ){
        puts("ic_decl_func_add_arg: call to ic_pvector_append failed");
        return 0;
    }

    return 1;
}

/* set return type
 *
 * this function will fail if the return type is already set
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_func_set_return(struct ic_decl_func *fdecl, char *type, unsigned int type_len){
    if( ! fdecl ){
        puts("ic_decl_func_set_return: fdecl was null");
        return 0;
    }
    if( ! type ){
        puts("ic_decl_func_set_return: type was null");
        return 0;
    }
    if( ! type_len ){
        puts("ic_decl_func_set_return: type_len was 0");
        return 0;
    }

    /* check we haven't already set */
    if( fdecl->ret_type ){
        /* it is an error to re-set the return type */
        puts("ic_decl_func_set_return: attempt to re-set return type");
        return 0;
    }

    /* create our return type */
    fdecl->ret_type = ic_symbol_new(type, type_len);
    if( ! fdecl->ret_type ){
        puts("ic_decl_func_set_return: call to ic_symbol_new failed");
        return 0;
    }

    /* success */
    return 1;
}

/* add new stmt to the body
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_func_add_stmt(struct ic_decl_func *fdecl, struct ic_stmt *stmt){
    if( ! fdecl ){
        puts("ic_decl_func_add_stmt: fdecl was null");
        return 0;
    }
    if( ! stmt ){
        puts("ic_decl_func_add_stmt: stmt was null");
        return 0;
    }

    /* add our element */
    if( -1 == ic_body_append( &(fdecl->body), stmt ) ){
        puts("ic_decl_func_add_stmt: call to ic_body_append failed");
        return 0;
    }

    return 1;
}

/* print decl_func */
void ic_decl_func_print(struct ic_decl_func *fdecl, unsigned int *indent_level){
    /* offset into args
     * re-used as offset into body
     * */
    unsigned int i = 0;
    /* len of args
     * re-used as len of body
     */
    unsigned int len = 0;

    if( ! fdecl ){
        puts("ic_decl_func_print: fdecl was null");
        return;
    }

    if( ! indent_level ){
        puts("ic_decl_func_print: indent_level was null");
        return;
    }

    /* guarantee generation of function string */
    if( ! ic_decl_func_str(fdecl) ){
        puts("ERROR ERROR");
        puts("ic_decl_func_print: call to ic_decl_func_str failed");
        return;
    }

    /* print comment and then function decl string */
    fputs("# ", stdout);
    ic_string_print(&(fdecl->string));
    puts("");

    /* print `function`, name, and opening bracket */
    printf("fn %s(", ic_symbol_contents( &(fdecl->name) ));

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

    /* closing bracket and return type arrow */
    fputs(") -> ", stdout);

    /* print return type if we have one */
    if( fdecl->ret_type ){
        ic_symbol_print(fdecl->ret_type);
        /* trailing \n */
        puts("");
    } else {
        /* otherwise print Void */
        puts("Void");
    }

    /* get length of body */
    len = ic_body_length( &(fdecl->body) );


    /* print body
     * body will handle indent_level incr and decr for us
     */
    ic_body_print( &(fdecl->body), indent_level);

    /* print end\n */
    puts("end");
}

/* return a string representation of this function signature
 *
 * for a function signature
 *      fn foo(a::Int, b::Int) -> Int
 *
 * this function will return
 *      foo(Int Int)
 *
 * the char* returned is a string stored within fdecl,
 * this means the caller must not free or mutate this string
 *
 * returns char* on success
 * returns 0 on failure
 */
char * ic_decl_func_str(struct ic_decl_func *fdecl){
    /* offset into args pvector */
    unsigned int i = 0;
    /* cached len */
    unsigned int len = 0;
    /* cache of string */
    struct ic_string *fstr = 0;
    /* each field we consider in args */
    struct ic_field *field = 0;
    /* temporary symbol for current field type */
    struct ic_symbol *cur_type = 0;

    if( ! fdecl ){
        puts("ic_decl_func_str: fdecl was null");
        return 0;
    }

    /* cache string pointer */
    fstr = &(fdecl->string);

    /* if a non-zero length fecl->string is found then return it */
    if( ic_string_length(fstr) ){
        return ic_string_contents(fstr);
    }

    /* note that we do not check for length as a length of 0 is valid */
    len = ic_pvector_length(&(fdecl->args));

    /* fdecl->name */
    if( ! ic_string_append_symbol(fstr, &(fdecl->name)) ){
        puts("ic_decl_func_str: name: call to ic_string_append_symbol failed");
        return 0;
    }

    /* opening bracket */
    if( ! ic_string_append_char(fstr, "(", 1) ){
        puts("ic_decl_func_str: opening brace: call to ic_string_append_char failed");
        return 0;
    }

    /* iterate through args appending the type name to our string representation
     */
    for( i=0; i<len; ++i ){
        /* capture our field */
        field = ic_pvector_get( &(fdecl->args), i );
        if( ! field ){
            puts("ic_decl_func_str: arg: call to ic_pvector_get failed");
            return 0;
        }

        cur_type = ic_type_ref_get_symbol(&(field->type));
        if( ! cur_type ){
            puts("ic_decl_func_str: arg: call to ic_type_get_symbol failed");
            return 0;
        }

        if( ! ic_string_append_symbol(fstr, cur_type) ){
            puts("ic_decl_func_str: arg: call to ic_string_append_symbol failed");
            return 0;
        }
    }

    /* final bracket */
    if( ! ic_string_append_char(fstr, ")", 1) ){
        puts("ic_decl_func_str: closing brace: call to ic_string_append_char failed");
        return 0;
    }

    /* we rely on the fdecl storing the string */
    return ic_string_contents(&(fdecl->string));
}

/* allocate and return a new decl_type
 * only needs name and len
 * will also allocate an empty pvector for fields
 *
 * returns new ic_field * on success
 * returns 0 on failure
 */
struct ic_decl_type * ic_decl_type_new(char *name_src, unsigned int name_len){
    struct ic_decl_type * tdecl = 0;

    /* allocate decl_type */
    tdecl = calloc(1, sizeof(struct ic_decl_type));
    if( ! tdecl ){
        puts("ic_decl_type_new: calloc failed");
        return 0;
    }

    if( ! ic_decl_type_init(tdecl, name_src, name_len) ){
        puts("ic_decl_type_new: call to ic_decl_type_init failed");
        free(tdecl);
        return 0;
    }

    return tdecl;
}

/* initialise an existing decl_type
 * only needs name and len
 * will also allocate an empty pvector for fields
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_init(struct ic_decl_type *tdecl, char *name_src, unsigned int name_len){
    if( ! tdecl ){
        puts("ic_decl_type_init: tdecl was null");
        return 0;
    }

    if( ! name_src ){
        puts("ic_decl_type_init: name_src was null");
        return 0;
    }

    /* initialise name */
    if( ! ic_symbol_init( &(tdecl->name), name_src, name_len ) ){
        puts("ic_decl_type_init: call to ic_symbol_init for name failed");
        return 0;
    }

    /* init fields pvector */
    if( ! ic_pvector_init( &(tdecl->fields), 0 ) ){
        puts("ic_decl_type_init: call to ic_pvector_init for fields failed");
        return 0;
    }

    /* init field dict */
    if( ! ic_dict_init( &(tdecl->field_dict) ) ){
        puts("ic_decl_type_init: call to ic_dict_init for field_dict failed");
        return 0;
    }

    return 1;
}

/* calls destroy on every element within
 *
 * this will only free the tdecl if `free_tdecl` is truthy
 *
 * the caller must determine if it is appropriate
 * or not to call free(decl)
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_destroy(struct ic_decl_type *tdecl, unsigned int free_tdecl){
    int i = 0;
    int len = 0;
    struct ic_field *field = 0;

    if( ! tdecl ){
        puts("ic_decl_type_destroy: tdecl was null");
        return 0;
    }

    /* free symbol contents but do not free symbol itself
     * since it is an element on tdecl
     */
    if( ! ic_symbol_destroy(&(tdecl->name), 0) ){
        puts("ic_decl_type_destroy: call to ic_symbol_destroy failed");
        return 0;
    }

    len = ic_pvector_length( &(tdecl->fields) );
    if( len == -1 ){
        puts("ic_decl_type_destroy: call to ic_body_length failed");
        return 0;
    }

    /* loop through each item destroying */
    for( i=0; i<len; ++i ){
        field = ic_pvector_get( &(tdecl->fields), i );
        if( ! field ){
            puts("ic_decl_type_destroy: call to ic_pvector_get failed");
            return 0;
        }

        /* dispatch to field destroy
         * free_field set to 1
         */
        if( ! ic_field_destroy(field, 1) ){
            puts("ic_decl_type_destroy: call to ic_field_destroy failed");
            return 0;
        }
    }

    /* destroy field_dict */
    if( ! ic_dict_destroy( &(tdecl->field_dict), 0, 0 ) ){
        puts("ic_decl_type_destroy: call to ic_dict_destroy failed");
        return 0;
    }

    /* only free if caller asked */
    if( free_tdecl ){
        free(tdecl);
    }

    /* success */
    return 1;
}

/* add a new field to types list of fields
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_add_field(struct ic_decl_type *tdecl, struct ic_field *field){
    if( ! tdecl ){
        puts("ic_decl_type_add_field: tdecl was null");
        return 0;
    }

    if( ! field ){
        puts("ic_decl_type_add_field: field was null");
        return 0;
    }

    if( -1 == ic_pvector_append( &(tdecl->fields), field ) ){
        puts("ic_decl_type_add_field: call to ic_pvector_append failed");
        return 0;
    }

    return 1;
}

/* print the decl_type to stdout */
void ic_decl_type_print(struct ic_decl_type *tdecl, unsigned int *indent_level){
    unsigned int i = 0;

    if( ! tdecl ){
        puts("ic_decl_type_print: tdecl was null");
        return;
    }
    if( ! indent_level ){
        puts("ic_decl_type_print: indent_level was null");
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

/* get the char * contents of the name
 *
 * returns char * on success
 * returns 0 on failure
 */
char * ic_decl_type_str(struct ic_decl_type *tdecl){
    if( ! tdecl ){
        puts("ic_decl_type_str: tdecl was null");
        return 0;
    }

    return ic_symbol_contents(&(tdecl->name));
}

/* get the type of a field by name
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_type * ic_decl_type_get_field_type(struct ic_decl_type *tdecl, char * field_name){
    struct ic_type *type = 0;

    if( ! tdecl ){
        puts("ic_decl_type_get_field_type: tdecl was null");
        return 0;
    }

    if( ! field_name ){
        puts("ic_decl_type_get_field_type: field_name was null");
        return 0;
    }

    type = ic_dict_get( &(tdecl->field_dict), field_name );
    if( ! type ){
        printf("ic_decl_type_get_field_type: failed to get type for field name '%s' from dict\n", field_name);
        return 0;
    }

    return type;
}

/* add field to field_dict
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_add_field_type(struct ic_decl_type *tdecl, char * field_name, struct ic_type *type){
    if( ! tdecl ){
        puts("ic_decl_type_add_field_type: tdecl was null");
        return 0;
    }

    if( ! field_name ){
        puts("ic_decl_type_add_field_type: field_name was null");
        return 0;
    }

    if( ! type ){
        puts("ic_decl_type_add_field_type: type was null");
        return 0;
    }

    if( ! ic_dict_insert( &(tdecl->field_dict), field_name, type ) ){
        printf("ic_decl_type_add_field_type: failed to insert type for field name '%s' into dict\n", field_name);
        return 0;
    }

    return 1;
}

/* allocate and initialise a new ic_decl
 *
 * NOTE: this call will NOT initialise the contents of the union
 * this is left up to the caller
 *
 * returns new ic_decl on success
 * returns 0 on failure
 */
struct ic_decl * ic_decl_new(enum ic_decl_tag tag){
    struct ic_decl *decl = 0;

    /* allocate space */
    decl = calloc(1, sizeof(struct ic_decl));
    if( ! decl ){
        puts("ic_decl_new: call to calloc failed");
        return 0;
    }

    /* initialise */
    if( ! ic_decl_init(decl, tag) ){
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
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_init(struct ic_decl *decl, enum ic_decl_tag tag){
    if( ! decl ){
        puts("ic_decl_init: decl was null");
        return 0;
    }

    /* set type */
    decl->tag = tag;

    /* NOTE we do NOT initialise the contents
     * it is up to the caller
     *
     * FIXME: reconsider this interface
     */

    return 1;
}

/* calls destroy on every element within
 *
 * this will only free the decl if `free_decl` is truthy
 *
 * the caller must determine if it is appropriate
 * or not to call free(decl)
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_destroy(struct ic_decl *decl, unsigned int free_decl){
    if( ! decl ){
        puts("ic_decl_destroy: decl was null");
        return 0;
    }

    /* dispatch based on type
     *
     * note that in both calls to _destroy we set the second arg to 0
     * as both the fdecl and tdecl are elements on this ic_decl
     * so we have to handle the free bewlo
     */
    switch( decl->tag ){
        case ic_decl_decl_func:
            /* destroy all elements but not not (0) free fdecl itself */
            if( ! ic_decl_func_destroy(&(decl->u.fdecl), 0) ){
                puts("ic_decl_destroy: call to ic_decl_func_destroy failed");
                return 0;
            }
            break;

        case ic_decl_decl_type:
            /* destroy all elements but not not (0) free tdecl itself */
            if( ! ic_decl_type_destroy(&(decl->u.tdecl), 0) ){
                puts("ic_decl_destroy: call to ic_decl_type_destroy failed");
                return 0;
            }
            break;

        default:
            puts("ic_decl_destroy: impossible decl type, aborting");
            return 0;
            break;
    }

    /* caller must determine if we are to free decl itself */
    if( free_decl ){
        free(decl);
    }

    /* success */
    return 1;
}

/* returns pointer to ic_decl_func element
 * this function will only success if the decl is of type decl_func
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_decl_func * ic_decl_get_fdecl(struct ic_decl *decl){
    if( ! decl ){
        puts("ic_decl_get_fdecl: decl was null");
        return 0;
    }

    /* check we are the right type */
    if( decl->tag != ic_decl_decl_func ){
        return 0;
    }

    /* unbox */
    return &(decl->u.fdecl);
}

/* returns pointer to cf_decl_type element
 * this function will only success if the decl is of type decl_type
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_decl_type * ic_decl_get_tdecl(struct ic_decl *decl){
    if( ! decl ){
        puts("ic_decl_get_tdecl: decl was null");
        return 0;
    }

    /* check we are the right type */
    if( decl->tag != ic_decl_decl_type ){
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

    switch( decl->tag ){
        case ic_decl_decl_func:
            ic_decl_func_print( ic_decl_get_fdecl(decl), indent_level );
            break;
        case ic_decl_decl_type:
            ic_decl_type_print( ic_decl_get_tdecl(decl), indent_level );
            break;
        default:
            puts("ic_decl_print: impossible type!");
            break;
    }
}


