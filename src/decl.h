#ifndef ICARUS_DECL_H
#define ICARUS_DECL_H

#include "body.h"
#include "field.h"
#include "symbol.h"
#include "pvector.h"

struct ic_func_decl {
    struct ic_symbol name;
    struct ic_pvector args;
    struct ic_body body;
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

/* add new arg field to func_decl
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_func_decl_add_arg(struct ic_func_decl *fdecl, struct ic_field *field);

/* add new stmt to the body
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_func_decl_add_stmt(struct ic_func_decl *fdecl, struct ic_stmt *stmt);

/* print func_decl */
void ic_func_decl_print(struct ic_func_decl *fdecl, unsigned int *indent_level);

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

/* add a new field to types list of fields
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_type_decl_add_field(struct ic_type_decl *tdecl, struct ic_field *field);

/* print the type_decl to stdout */
void ic_type_decl_print(struct ic_type_decl *tdecl, unsigned int *indent_level);

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
