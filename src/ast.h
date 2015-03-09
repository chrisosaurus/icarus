#ifndef ICARUS_AST_H
#define ICARUS_AST_H

#include "types.h"

/* predeclare to allow recursive types */
struct ic_expression;

/* a collection of expressions */
struct ic_begin {
    int len;
    struct ic_expression *expr;
};

struct ic_func_call {
    struct ic_symbol name;
    int nargs;
    struct ic_value * args;
};

struct ic_func_decl {
    struct ic_symbol name;
    int nargs;
    struct ic_array arg_names;
    struct ic_array arg_types;
    struct ic_begin body;
};

struct ic_type_decl {
    struct ic_symbol name;
    int nfields;
    struct ic_array field_names;
    struct ic_array field_types;
};

enum ic_value_type {
    symbol,
    string,
    integer,
    array
#if 0
    function
#endif
};

struct ic_value {
    enum ic_value_type type;
    union {
        struct ic_symbol sym;
        struct ic_string str;
        struct ic_int integer;
        struct ic_array array;
    } u;
};


enum ic_expr_type {
    value,
    func_call,
    func_decl,
    type_decl
};

struct ic_expression {
    enum ic_expr_type type;
    union {
        struct ic_value value;
        struct ic_func_call fcall;
        struct ic_func_decl fdecl;
        struct ic_type_decl tdecl;
    } u;
};

#endif
