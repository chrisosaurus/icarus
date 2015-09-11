#ifndef ICARUS_EXPRESSION_H
#define ICARUS_EXPRESSION_H

#include "field.h"
#include "../../data/symbol.h"
#include "../../data/pvector.h"

struct ic_expr;

/* an application of a function
 */
struct ic_expr_func_call {
    struct ic_symbol fname;
    /* this is a pvector of ic_expr *
     */
    struct ic_pvector args;

    /* string representation of function call
     * generated by ic_expr_func_call_str
     * e.g. Foo(Int Int)
     */
    struct ic_string *string;

    /* the function we are calling
     * this is set at the analyse phase
     *
     * FIXME not currently used
     * FIXME eventually migrate to ic_func as this
     *      could be a call to an fdecl or a builtin
     */
    struct ic_func_decl *fdecl;
};

/* allocate and initialise a new func call
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr_func_call * ic_expr_func_call_new(char *name, unsigned int name_len);

/* intialise an existing func call
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_expr_func_call_init(struct ic_expr_func_call *fcall, char *name, unsigned int name_len);

/* destroy fcall
 *
 * only free fcall if `free_fcall` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_expr_func_call_destroy(struct ic_expr_func_call *fcall, unsigned int free_fcall);

/* set fdecl on fcall
 * must not already be set
 *
 * returns 1 on success
 * returns 1 on failure
 */
unsigned int ic_expr_func_call_set_fdecl(struct ic_expr_func_call *fcall, struct ic_func_decl *fdecl);

/* add a new argument to this function call
 *
 * returns 1 on success
 * returns 0 on failure
 */
int ic_expr_func_call_add_arg(struct ic_expr_func_call *fcall, struct ic_expr *expr);

/* get argument
 *
 * returns field at offset on success
 * returns 0 on failure
 */
struct ic_expr * ic_expr_func_call_get_arg(struct ic_expr_func_call *fcall, unsigned int i);

/* returns number of arguments on success
 * returns 0 on failure
 */
unsigned int ic_expr_func_call_length(struct ic_expr_func_call *fcall);

/* generate a string (char *) presentation of this function call
 * the signature generated will be consistent with the one generated
 * from a call to `ic_func_decl_str` so will include argument type information:
 *  foo(Int Int)
 *
 * return a string representation of this function call on success
 * returns 0 on failure
 */
char * ic_expr_func_call_str(struct ic_expr_func_call *fcall);

/* print this func call */
void ic_expr_func_call_print(struct ic_expr_func_call *fcall, unsigned int *indent_level);


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
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_expr_identifier_init(struct ic_expr_identifier * identifier, char *id, unsigned int id_len);

/* destroy identifier
 *
 * will free id if `free_id` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_expr_identifier_destroy(struct ic_expr_identifier * identifier, unsigned int free_id);

/* print this identifier */
void ic_expr_identifier_print(struct ic_expr_identifier * identifier, unsigned int *indent_level);

enum ic_expr_constant_type {
    ic_expr_constant_type_integer,
    ic_expr_constant_type_string
};

/* a constant
 * either a string or an int
 */
struct ic_expr_constant {
    enum ic_expr_constant_type type;
    union {
        long int integer;
        struct ic_string string;
    } u;
};

/* allocate and init a new constant
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr_constant * ic_expr_constant_new(enum ic_expr_constant_type type);

/* initialise an existing constant
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_expr_constant_destroy(struct ic_expr_constant *constant, enum ic_expr_constant_type type);

/* destroy const
 *
 * will free const if `free_const` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_expr_constant_init(struct ic_expr_constant *constant, unsigned int free_const);

/* return pointer to integer within,
 * will only succeed if constant is of the correct type
 *
 * returns pointers on success
 * returns 0 on failure
 */
long int * ic_expr_constant_get_integer(struct ic_expr_constant *constant);

/* return pointer to ic_string within,
 * will only succeed if constant is of the correct type
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_string * ic_expr_constant_get_string(struct ic_expr_constant *constant);

/* print this constant */
void ic_expr_constant_print(struct ic_expr_constant *constant, unsigned int *indent_level);

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
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_expr_operator_init(struct ic_expr_operator *operator, struct ic_expr *lexpr, struct ic_expr *rexpr, char *op, unsigned int op_len);

/* destroy operator
 *
 * will free op if `free_op` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_expr_operator_destroy(struct ic_expr_operator *op, unsigned int free_op);

/* print this operator */
void ic_expr_operator_print(struct ic_expr_operator *op, unsigned int *indent_level);


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
 * returns 1 on success
 * returns 0 on failure
 */
int ic_expr_init(struct ic_expr *expr, enum ic_expr_type type);

/* destroy expr
 *
 * will only free expr if `free_expr` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_expr_destroy(struct ic_expr *expr, unsigned int free_expr);

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
void ic_expr_print(struct ic_expr *expr, unsigned int *indent_level);


#endif
