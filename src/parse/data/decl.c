#include <stdlib.h> /* calloc */
#include <stdio.h> /* puts, printf */
#include <string.h> /* strncpy */

#include "decl.h"
#include "field.h"
#include "../../data/pvector.h"
#include "statement.h"
#include "../../data/symbol.h"
#include "../../data/string.h"
#include "../parse.h"

/* FIXME crutch for unused param */
#pragma GCC diagnostic ignored "-Wunused-parameter"

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
        free(fdecl);
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
    if( ! ic_symbol_init( &(fdecl->name), name, name_len ) ){
        puts("ic_func_decl_init: call to ic_symbol_init for name failed");
        return 1;
    }

    /* initialise empty args pvector */
    if( ! ic_pvector_init( &(fdecl->args), 0 ) ){
        puts("ic_func_decl_init: call to ic_pvector_init failed");
        return 1;
    }

    /* initialise empty string fdecl->string */
    if( ! ic_string_init_empty( &(fdecl->string) ) ){
        puts("ic_func_decl_init: call to ic_string_init_empty for string failed");
        return 1;
    }

    /* initialise return type to 0 (void) */
    fdecl->ret_type = 0;

    /* initialise our empty body */
    if( ic_body_init( &(fdecl->body) ) ){
        puts("ic_func_decl_init: call to ic_body_init failed");
        return 1;
    }

    return 0;
}

/* calls destroy on every element within
 *
 * this will only free the fdecl if `free_fdecl` is truthy
 *
 * the caller must determine if it is appropriate
 * or not to call free(decl)
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_func_decl_destroy(struct ic_func_decl *fdecl, unsigned int free_fdecl){
    int i = 0;
    int len = 0;

    struct ic_field *field = 0;

    if( ! fdecl ){
        puts("ic_type_decl_destroy: fdecl was null");
        return 1;
    }

    /* free symbol contents but do not free symbol itself
     * since it is an element on fdecl
     */
    if( ! ic_symbol_destroy(&(fdecl->name), 0) ){
        puts("ic_type_decl_destroy: for name call to ic_symbol_destroy failed");
        return 1;
    }

    /* free string contents but do not free string itself
     * since it is an element on fdecl
     */
    if( ! ic_string_destroy(&(fdecl->string), 0) ){
        puts("ic_type_decl_destroy: for string call to ic_string_destroy failed");
        return 1;
    }

    len = ic_pvector_length( &(fdecl->args) );

    for( i=0; i<len; ++i ){
        field = ic_pvector_get( &(fdecl->args), i );
        if( ! field ){
            puts("ic_type_decl_destroy: call to ic_pvector_get failed");
            return 1;
        }

        /* dispatch to field destroy
         * free_field set to 1
         */
        if( ! ic_field_destroy(field, 1) ){
            puts("ic_type_decl_destroy: call to ic_field_destroy failed");
            return 1;
        }
    }

    /* only need to free if we have a ret_type */
    if( fdecl->ret_type ){
        /* free symbol contents and free symbol
         * as our ret type is always allocated
         *
         *    // ic_func_decl_set_return :
         *    fdecl->ret_type = ic_symbol_new(type, type_len);
         */
        if( ! ic_symbol_destroy(fdecl->ret_type, 1) ){
            puts("ic_type_decl_destroy: for ret_type call to ic_symbol_destroy failed");
            return 1;
        }
    }

    /* free body contents but do not free body itself
     * since it is an element on fdecl
     */
    if( ic_body_destroy(&(fdecl->body), 0) ){
        puts("ic_type_decl_destroy: for body call to ic_body_destroy failed");
        return 1;
    }

    /* only free if caller asked */
    if( free_fdecl ){
        free(fdecl);
    }

    /* success */
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

    /* append field returns -1 on failure */
    if( -1 == ic_pvector_append( &(fdecl->args), field ) ){
        puts("ic_func_decl_add_arg: call to ic_pvector_append failed");
        return 1;
    }

    return 0;
}

/* set return type
 *
 * this function will fail if the return type is already set
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_func_decl_set_return(struct ic_func_decl *fdecl, char *type, unsigned int type_len){
    if( ! fdecl ){
        puts("ic_func_decl_set_return: fdecl was null");
        return 1;
    }
    if( ! type ){
        puts("ic_func_decl_set_return: type was null");
        return 1;
    }
    if( ! type_len ){
        puts("ic_func_decl_set_return: type_len was 0");
        return 1;
    }

    /* check we haven't already set */
    if( fdecl->ret_type ){
        /* it is an error to re-set the return type */
        puts("ic_func_decl_set_return: attempt to re-set return type");
        return 1;
    }

    /* create our return type */
    fdecl->ret_type = ic_symbol_new(type, type_len);
    if( ! fdecl->ret_type ){
        puts("ic_func_decl_set_return: call to ic_symbol_new failed");
        return 1;
    }

    /* success */
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
    if( -1 == ic_body_append( &(fdecl->body), stmt ) ){
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

    /* guarantee generation of function string */
    if( ! ic_func_decl_str(fdecl) ){
        puts("ERROR ERROR");
        puts("ic_func_decl_print: call to ic_func_decl_str failed");
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
char * ic_func_decl_str(struct ic_func_decl *fdecl){
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
        puts("ic_func_decl_str: fdecl was null");
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
        puts("ic_func_decl_str: name: call to ic_string_append_symbol failed");
        return 0;
    }

    /* opening bracket */
    if( ! ic_string_append_char(fstr, "(", 1) ){
        puts("ic_func_decl_str: opening brace: call to ic_string_append_char failed");
        return 0;
    }

    /* iterate through args appending the type name to our string representation
     */
    for( i=0; i<len; ++i ){
        /* capture our field */
        field = ic_pvector_get( &(fdecl->args), i );
        if( ! field ){
            puts("ic_func_decl_str: arg: call to ic_pvector_get failed");
            return 0;
        }

        cur_type = ic_type_ref_get_symbol(&(field->type));
        if( ! cur_type ){
            puts("ic_func_decl_str: arg: call to ic_type_get_symbol failed");
            return 0;
        }

        if( ! ic_string_append_symbol(fstr, cur_type) ){
            puts("ic_func_decl_str: arg: call to ic_string_append_symbol failed");
            return 0;
        }
    }

    /* final bracket */
    if( ! ic_string_append_char(fstr, ")", 1) ){
        puts("ic_func_decl_str: closing brace: call to ic_string_append_char failed");
        return 0;
    }

    /* we rely on the fdecl storing the string */
    return ic_string_contents(&(fdecl->string));
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
        free(tdecl);
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
    if( ! ic_symbol_init( &(tdecl->name), name_src, name_len ) ){
        puts("ic_type_decl_init: call to ic_symbol_init for name failed");
        return 1;
    }

    /* init fields pvector */
    if( ! ic_pvector_init( &(tdecl->fields), 0 ) ){
        puts("ic_type_decl_init: call to ic_pvector_init for fields failed");
        return 1;
    }

    return 0;
}

/* calls destroy on every element within
 *
 * this will only free the tdecl if `free_tdecl` is truthy
 *
 * the caller must determine if it is appropriate
 * or not to call free(decl)
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_type_decl_destroy(struct ic_type_decl *tdecl, unsigned int free_tdecl){
    int i = 0;
    int len = 0;
    struct ic_field *field = 0;

    if( ! tdecl ){
        puts("ic_type_decl_destroy: tdecl was null");
        return 1;
    }

    /* free symbol contents but do not free symbol itself
     * since it is an element on tdecl
     */
    if( ! ic_symbol_destroy(&(tdecl->name), 0) ){
        puts("ic_type_decl_destroy: call to ic_symbol_destroy failed");
        return 1;
    }

    len = ic_pvector_length( &(tdecl->fields) );
    if( len == -1 ){
        puts("ic_type_decl_destroy: call to ic_body_length failed");
        return 1;
    }

    /* loop through each item destroying */
    for( i=0; i<len; ++i ){
        field = ic_pvector_get( &(tdecl->fields), i );
        if( ! field ){
            puts("ic_type_type_destroy: call to ic_pvector_get failed");
            return 1;
        }

        /* dispatch to field destroy
         * free_field set to 1
         */
        if( ! ic_field_destroy(field, 1) ){
            puts("ic_type_type_destroy: call to ic_field_destroy failed");
            return 1;
        }
    }

    /* only free if caller asked */
    if( free_tdecl ){
        free(tdecl);
    }

    /* success */
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

    if( -1 == ic_pvector_append( &(tdecl->fields), field ) ){
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

/* get the char * contents of the name
 *
 * returns char * on success
 * returns 0 on failure
 */
char * ic_type_decl_str(struct ic_type_decl *tdecl){
    if( ! tdecl ){
        puts("ic_type_decl_str: tdecl was null");
        return 0;
    }

    return ic_symbol_contents(&(tdecl->name));
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

/* calls destroy on every element within
 *
 * this will only free the decl if `free_decl` is truthy
 *
 * the caller must determine if it is appropriate
 * or not to call free(decl)
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_decl_destroy(struct ic_decl *decl, unsigned int free_decl){
    if( ! decl ){
        puts("ic_decl_destroy: decl was null");
        return 1;
    }

    /* dispatch based on type
     *
     * note that in both calls to _destroy we set the second arg to 0
     * as both the fdecl and tdecl are elements on this ic_decl
     * so we have to handle the free bewlo
     */
    switch( decl->type ){
        case ic_decl_func_decl:
            /* destroy all elements but not not (0) free fdecl itself */
            if( ic_func_decl_destroy(&(decl->u.fdecl), 0) ){
                puts("ic_decl_destroy: call to ic_func_decl_destroy failed");
                return 1;
            }
            break;

        case ic_decl_type_decl:
            /* destroy all elements but not not (0) free tdecl itself */
            if( ic_type_decl_destroy(&(decl->u.tdecl), 0) ){
                puts("ic_decl_destroy: call to ic_type_decl_destroy failed");
                return 1;
            }
            break;

        default:
            puts("ic_decl_destroy: impossible decl type, aborting");
            return 1;
            break;
    }

    /* caller must determine if we are to free decl itself */
    if( free_decl ){
        free(decl);
    }

    /* success */
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


