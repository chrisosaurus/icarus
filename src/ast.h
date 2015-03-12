#ifndef ICARUS_AST_H
#define ICARUS_AST_H

#include "parray.h"

struct ic_func_decl {
#if 0
    struct ic_symbol name;
#endif
    int nargs;
    struct ic_parray arg_names;
    struct ic_parray arg_types;
    /* FIXME how do we store a body ? */
    char *body;
};

struct ic_type_decl {
#if 0
    struct ic_symbol name;
#endif
    int nfields;
    struct ic_parray field_names;
    struct ic_parray field_types;
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
    struct ic_parray decls;
};

#endif
