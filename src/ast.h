#ifndef ICARUS_AST_H
#define ICARUS_AST_H

#include "types.h"
#include "array.h"

struct ic_func_decl {
    struct ic_symbol name;
    int nargs;
    struct ic_array arg_names;
    struct ic_array arg_types;
    /* FIXME how do we store a body ? */
    char *body;
};

struct ic_type_decl {
    struct ic_symbol name;
    int nfields;
    struct ic_array field_names;
    struct ic_array field_types;
};

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
    struct ic_array decls;
};

#endif
