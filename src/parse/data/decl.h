#ifndef ICARUS_DECL_H
#define ICARUS_DECL_H

#include "body.h"
#include "field.h"
#include "../../data/symbol.h"
#include "../../data/pvector.h"

struct ic_decl_func {
    /* fn name(args...) -> ret_type
     *      body ...
     * end
     *
     * fn name(args...)
     *      body ...
     * end
     *
     */
    struct ic_symbol name;

    /* a pointer vector of fields */
    struct ic_pvector args;

    /* return type is optional
     *  0    -> void
     *  else -> symbol for type
     */
    struct ic_symbol *ret_type;

    struct ic_body body;

    /* the string representation of this function decl */
    struct ic_string string;
};

/* allocate and initialise a new decl_func
 *
 * returns new decl_func on success
 * returns 0 on failure
 */
struct ic_decl_func * ic_decl_func_new(char *name, unsigned int name_len);

/* initialise an existing decl_func
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_func_init(struct ic_decl_func *fdecl, char *name, unsigned int name_len);

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
unsigned int ic_decl_func_destroy(struct ic_decl_func *fdecl, unsigned int free_fdecl);

/* add new arg field to decl_func
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_func_add_arg(struct ic_decl_func *fdecl, struct ic_field *field);

/* set return type
 *
 * this function will fail if the return type is already set
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_func_set_return(struct ic_decl_func *fdecl, char *type, unsigned int type_len);

/* add new stmt to the body
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_func_add_stmt(struct ic_decl_func *fdecl, struct ic_stmt *stmt);

/* print decl_func */
void ic_decl_func_print(struct ic_decl_func *fdecl, unsigned int *indent_level);

/* return a string representation of this function signature
 *
 * for a function signature
 *      fn foo(a::Int b::Int) -> Int
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
char * ic_decl_func_str(struct ic_decl_func *fdecl);

/* a func declaration is a symbol and then a collection of fields
 *  type Foo
 *   a::Int
 *   b::String
 *  end
 *
 * gives the symbol 'Foo' and the
 * fields (a, Int) and (b, String)
 */
struct ic_decl_type {
    struct ic_symbol name;
    /* a pointer vector of field(s) */
    struct ic_pvector fields;
};

/* allocate and return a new decl_type
 * only needs name and len
 * will also allocate an empty pvector for fields
 *
 * returns new ic_field * on success
 * returns 0 on failure
 */
struct ic_decl_type * ic_decl_type_new(char *name_src, unsigned int name_len);

/* initialise an existing decl_type
 * only needs name and len
 * will also allocate an empty pvector for fields
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_init(struct ic_decl_type *tdecl, char *name_src, unsigned int name_len);

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
unsigned int ic_decl_type_destroy(struct ic_decl_type *tdecl, unsigned int free_tdecl);

/* add a new field to types list of fields
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_type_add_field(struct ic_decl_type *tdecl, struct ic_field *field);

/* print the decl_type to stdout */
void ic_decl_type_print(struct ic_decl_type *tdecl, unsigned int *indent_level);

/* get the char * contents of the name
 *
 * returns char * on success
 * returns 0 on failure
 */
char * ic_decl_type_str(struct ic_decl_type *tdecl);

enum ic_decl_tag {
    ic_decl_decl_func,
    ic_decl_decl_type
};

struct ic_decl {
    enum ic_decl_tag tag;
    union {
        struct ic_decl_func fdecl;
        struct ic_decl_type tdecl;
    } u;
};

/* allocate and initialise a new ic_decl
 *
 * NOTE: this call will NOT initialise the contents of the union
 * this is left up to the caller
 *
 * returns new ic_decl on success
 * returns 0 on failure
 */
struct ic_decl * ic_decl_new(enum ic_decl_tag tag);

/* initialise an existing ic_decl
 *
 * NOTE: this call will NOT initialise the contents of the union
 * this is left up to the caller
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_decl_init(struct ic_decl *decl, enum ic_decl_tag tag);

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
unsigned int ic_decl_destroy(struct ic_decl *decl, unsigned int free_decl);

/* returns pointer to ic_decl_func element
 * this function will only success if the decl is of type decl_func
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_decl_func * ic_decl_get_fdecl(struct ic_decl *decl);

/* returns pointer to ic_decl_type element
 * this function will only success if the decl is of type decl_type
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_decl_type * ic_decl_get_tdecl(struct ic_decl *decl);

/* print contents of ic_decl */
void ic_decl_print(struct ic_decl *decl, unsigned int *indent_level);

#endif
