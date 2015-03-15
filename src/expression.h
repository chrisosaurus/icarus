#ifndef ICARUS_EXPRESSION_H
#define ICARUS_EXPRESSION_H

#include "symbol.h"
#include "pvector.h"

struct ic_expr;

/* an application of a function
 */
struct ic_expr_func_call {
    struct ic_symbol fname;
    /* FIXME may want to change from pvector */
    struct ic_pvector args;
};

/* a use of an identifier (variable)
 */
struct ic_expr_identifier{
    struct ic_symbol identifier;
};

/* a constant
 */
struct ic_expr_constant {
    /* FIXME need a value type */
    void * value;
};

/* an application of an operator to 2
 * sub expressions
 *
 * maps to a function call
 */
struct ic_expr_operator {
    struct ic_expr * lexpr;
    struct ic_expr * rexpr;
    struct ic_symbol op;
};


enum ic_expr_type {
    ic_expr_func_call,
    ic_expr_identifier_use,
    ic_expr_constant,
    ic_expr_operator_application
};

struct ic_expr{
    enum ic_expr_type type;
    union {
        struct ic_expr_func_call fcall;
        struct ic_expr_identifier id;
        struct ic_expr_constant cons;
        struct ic_expr_operator op;
    } u;
};

#endif
