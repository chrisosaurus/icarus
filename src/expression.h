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

/* print this func call */
void ic_expr_func_call_print(struct ic_expr_func_call *fcall);


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

/* print this identifier */
void ic_expr_identifier_print(struct ic_expr_identifier * identifier);

enum ic_expr_constant_type {
    ic_expr_constant_type_int,
    ic_expr_constant_type_string
};

/* a constant
 * either a string or an int
 */
struct ic_expr_constant {
    enum ic_expr_constant_type type;
    union {
        int integer;
        struct ic_string string;
    } u;
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

/* allocate and initialise a new op
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr_operator * ic_expr_operator_new(struct ic_expr *lexpr, struct ic_expr *rexpr, char *op, unsigned int op_len);

/* initialise an existing op
 *
 * returns 0 on sucess
 * returns 1 on failure
 */
unsigned int ic_expr_operator_init(struct ic_expr_operator *operator, struct ic_expr *lexpr, struct ic_expr *rexpr, char *op, unsigned int op_len);

/* print this operator */
void ic_expr_operator_print(struct ic_expr_operator *op);


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

/* allocate and initialise a new ic_expr
 * will not initialise union members
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr * ic_expr_new(enum ic_expr_type type);

/* initialise an existing ic_expr
 * will not initialise union members
 *
 * returns 0 on success
 * returns 1 on failure
 */
int ic_expr_init(struct ic_expr *expr, enum ic_expr_type type);

/* return pointer to fcall within,
 * will only succeed if expr is of the correct type
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_expr_func_call * ic_expr_get_fcall(struct ic_expr *expr);

/* return pointer to identifier within,
 * will only succeed if expr is of the correct type
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_expr_identifier * ic_expr_get_identifier(struct ic_expr *expr);

/* return pointer to constant within,
 * will only succeed if expr is of the correct type
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_expr_constant * ic_expr_get_constant(struct ic_expr *expr);

/* return pointer to operator within,
 * will only succeed if expr is of the correct type
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_expr_operator * ic_expr_get_operator(struct ic_expr *expr);

/* print this expr */
void ic_expr_print(struct ic_expr *expr);


#endif
