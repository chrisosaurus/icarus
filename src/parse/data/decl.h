#ifndef ICARUS_DECL_H
#define ICARUS_DECL_H

#include "body.h"
#include "field.h"
#include "../../data/symbol.h"
#include "../../data/pvector.h"

struct ic_func_decl {
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

/* allocate and initialise a new func_decl
 *
 * returns new func_decl on success
 * returns 0 on error
 */
struct ic_func_decl * ic_func_decl_new(char *name, unsigned int name_len);

/* initialise an existing func_decl
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_func_decl_init(struct ic_func_decl *fdecl, char *name, unsigned int name_len);

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
unsigned int ic_func_decl_destroy(struct ic_func_decl *fdecl, unsigned int free_fdecl);

/* add new arg field to func_decl
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_func_decl_add_arg(struct ic_func_decl *fdecl, struct ic_field *field);

/* set return type
 *
 * this function will fail if the return type is already set
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_func_decl_set_return(struct ic_func_decl *fdecl, char *type, unsigned int type_len);

/* add new stmt to the body
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_func_decl_add_stmt(struct ic_func_decl *fdecl, struct ic_stmt *stmt);

/* print func_decl */
void ic_func_decl_print(struct ic_func_decl *fdecl, unsigned int *indent_level);

/* return a string representation of this function signature
 *
 * for a function signature
 *      fn foo(a::Int b::Int) -> Int
 *
 * this function will return
 *      foo(Int Int)
 *
 * returns char* on success
 * returns 0 on failure
 */
char * ic_func_decl_str(struct ic_func_decl *fdecl);

/* a func declaration is a symbol and then a collection of fields
 *  type Foo
 *   a::Int
 *   b::String
 *  end
 *
 * gives the symbol 'Foo' and the
 * fields (a, Int) and (b, String)
 */
struct ic_type_decl {
    struct ic_symbol name;
    /* a pointer vector of field(s) */
    struct ic_pvector fields;
};

/* allocate and return a new type_decl
 * only needs name and len
 * will also allocate an empty pvector for fields
 *
 * returns new ic_field * on success
 * returns 0 on failure
 */
struct ic_type_decl * ic_type_decl_new(char *name_src, unsigned int name_len);

/* initialise an existing type_decl
 * only needs name and len
 * will also allocate an empty pvector for fields
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_type_decl_init(struct ic_type_decl *tdecl, char *name_src, unsigned int name_len);

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
unsigned int ic_type_decl_destroy(struct ic_type_decl *tdecl, unsigned int free_tdecl);

/* add a new field to types list of fields
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_type_decl_add_field(struct ic_type_decl *tdecl, struct ic_field *field);

/* print the type_decl to stdout */
void ic_type_decl_print(struct ic_type_decl *tdecl, unsigned int *indent_level);

/* get the char * contents of the name
 *
 * returns char * on success
 * returns 0 on failure
 */
char * ic_type_decl_str(struct ic_type_decl *tdecl);

enum ic_decl_type {
    ic_decl_func_decl,
    ic_decl_type_decl
};

struct ic_decl {
    enum ic_decl_type type;
    union {
        struct ic_func_decl fdecl;
        struct ic_type_decl tdecl;
    } u;
};

/* allocate and initialise a new ic_decl
 *
 * NOTE: this call will NOT initialise the contents of the union
 * this is left up to the caller
 *
 * returns new ic_decl on success
 * returns 0 on error
 */
struct ic_decl * ic_decl_new(enum ic_decl_type type);

/* initialise an existing ic_decl
 *
 * NOTE: this call will NOT initialise the contents of the union
 * this is left up to the caller
 *
 * returns 0 on sucess
 * returns 1 on error
 */
unsigned int ic_decl_init(struct ic_decl *decl, enum ic_decl_type type);

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
unsigned int ic_decl_destroy(struct ic_decl *decl, unsigned int free_decl);

/* returns pointer to ic_func_decl element
 * this function will only success if the decl is of type func_decl
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_func_decl * ic_decl_get_fdecl(struct ic_decl *decl);

/* returns pointer to ic_type_decl element
 * this function will only success if the decl is of type type_decl
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_type_decl * ic_decl_get_tdecl(struct ic_decl *decl);

/* print contents of ic_decl */
void ic_decl_print(struct ic_decl *decl, unsigned int *indent_level);

#endif
