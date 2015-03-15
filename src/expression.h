#ifndef ICARUS_EXPRESSION_H
#define ICARUS_EXPRESSION_H

#include "field.h"
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

/* allocate and initialise a new func call
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr_func_call * ic_expr_func_call_new(char *name, unsigned int name_len);

/* intialise an existing func call
 *
 * returns 0 on success
 * retunns 1 on failure
 */
unsigned int ic_expr_func_call_init(struct ic_expr_func_call *fcall, char *name, unsigned int name_len);

/* add a new argument to this function call
 *
 * returns offset of arg on success
 * returns -1 on failure
 */
int ic_expr_func_call_add_arg(struct ic_expr_func_call *fcall, struct ic_field *field);

/* get argument
 *
 * returns field at offset on success
 * returns 0 on failure
 */
struct ic_field * ic_expr_func_call_get_arg(struct ic_expr_func_call *fcall, unsigned int i);

/* returns number of arguments on success
 * returns 0 on failure
 */
unsigned int ic_expr_func_call_length(struct ic_expr_func_call *fcall);


/* a use of an identifier (variable)
 */
struct ic_expr_identifier{
    struct ic_symbol identifier;
};

/* allocate and initialise a new identifier
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr_identifier * ic_expr_identifier_new(char *id, unsigned int id_len);

/* initialise an existing identifier
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_expr_identifier_init(struct ic_expr_identifier * identifier, char *id, unsigned int id_len);


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
    ic_expr_type_func_call,
    ic_expr_type_identifier,
    ic_expr_type_constant,
    ic_expr_type_operator
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
