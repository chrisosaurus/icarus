#ifndef ICARUS_AST_H
#define ICARUS_AST_H

#include "symbol.h"
#include "pvector.h"

/* a field is a combination of a symbol and a type
 *  foo::Int
 * the symbol is `foo`
 * and the type is `Int
 */
struct ic_field {
    struct ic_symbol name;
    struct ic_symbol type;
};

/* allocate and return a new field
 * takes 2 tokens as char * and len pairs
 *
 * returns new ic_field * on success
 * returns 0 on failure
 */
struct ic_field * ic_field_new(char *name_src, unsigned int name_len, char *type_src, unsigned int type_len);

/* initialise an existing field
 * takes 2 tokens as char * and len pairs
 *
 * returns 0 on success
 * returns 1 on error
 */
int ic_field_init(struct ic_field *field, char *name_src, unsigned int name_len, char *type_src, unsigned int type_len);

/* print the field to stdout */
void ic_field_print(struct ic_field *field);

struct ic_func_decl {
    struct ic_symbol name;
    struct ic_pvector args;
    /* FIXME how do we store a body ? */
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
int ic_func_decl_init(struct ic_func_decl *fdecl, char *name, unsigned int name_len);

/* add new arg field to func_decl
 *
 * returns 0 on success
 * returns 1 on error
 */
int ic_func_decl_add_arg(struct ic_func_decl *fdecl, struct ic_field *field);

/* print func_decl */
void ic_func_decl_print(struct ic_func_decl *fdecl);

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
int ic_type_decl_init(struct ic_type_decl *tdecl, char *name_src, unsigned int name_len);

/* add a new field to types list of fields
 *
 * returns 0 on success
 * returns 1 on error
 */
int ic_type_decl_add_field(struct ic_type_decl *tdecl, struct ic_field *field);

/* print the type_decl to stdout */
void ic_type_decl_print(struct ic_type_decl *tdecl);

enum ic_decl_type {
    func_decl,
    type_decl
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
int ic_decl_init(struct ic_decl *decl, enum ic_decl_type type);

/* return pointer to ic_func_decl element
 * this function will only success if the decl is of type func_decl
 *
 * returns pointer on success
 * return 0 on failure
 */
struct ic_func_decl * ic_decl_get_fdecl(struct ic_decl *decl);

/* return pointer to ic_type_decl element
 * this function will only success if the decl is of type type_decl
 *
 * returns pointer on success
 * return 0 on failure
 */
struct ic_type_decl * ic_decl_get_tdecl(struct ic_decl *decl);

/* print contents of ic_decl */
void ic_decl_print(struct ic_decl *decl);

struct ic_ast {
    struct ic_pvector decls;
};

/* allocate and initialise a new ast
 *
 * returns pointer to new ast on success
 * returns 0 on failure
 */
struct ic_ast * ic_ast_new(void);

/* initialise a pre-existing ast
 *
 * returns 0 on success
 * returns 1 on failure
 */
int ic_ast_init(struct ic_ast *ast);

/* get item stores at index i
 *
 * returns pointer to item on success
 * returns 0 on failure
 */
struct ic_decl * ic_ast_get(struct ic_ast *ast, unsigned int i);

/* append decl to ast
 * ast will resize to make room
 *
 * returns index of item on success
 * returns -1 on failure
 */
int ic_ast_append(struct ic_ast *ast, struct ic_decl *decl);

/* returns length on success
 * returns 0 on failure
 */
unsigned int ic_ast_length(struct ic_ast *ast);

/* calls print on all elements within ast */
void ic_ast_print(struct ic_ast *ast);

#endif
