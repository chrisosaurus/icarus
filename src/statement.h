#ifndef ICARUS_STATEMENT_H
#define ICARUS_STATEMENT_H

#include "body.h"
#include "expression.h"
#include "symbol.h"
#include "pvector.h"

struct ic_stmt;

/* a let statement
 *  let identifier::type = init
 */
struct ic_stmt_let {
    struct ic_symbol identifier;
    struct ic_symbol type;
    struct ic_expr init;
};

/* an if statement
 *  if expr
 *      body
 *  end
 */
struct ic_stmt_if {
    struct ic_expr expr;
    struct ic_body body;
};

enum ic_stmt_type {
    ic_stmt_type_let,
    ic_stmt_type_if,
    ic_stmt_type_expr
};

struct ic_stmt {
    enum ic_stmt_type type;
    union {
        struct ic_stmt_let let;
        struct ic_stmt_if sif;
        /* a statement can just be an expression in
         * void context
         *  foo(bar)
         */
        struct ic_expr expr;
    } u;
};

#endif
