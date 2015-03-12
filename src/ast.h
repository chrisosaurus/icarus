#ifndef ICARUS_AST_H
#define ICARUS_AST_H

#include "symbol.h"
#include "parray.h"
#include "pvector.h"

struct ic_func_decl {
    struct ic_symbol name;
    int nargs;
    struct ic_parray arg_names;
    struct ic_parray arg_types;
    /* FIXME how do we store a body ? */
    char *body;
};

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

/* print the field to stdout */
void ic_field_print(struct ic_field *field);

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

struct ic_ast {
    struct ic_parray decls;
};

#endif
