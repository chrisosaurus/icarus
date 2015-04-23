#ifndef ICARUS_STATEMENT_H
#define ICARUS_STATEMENT_H

#include "body.h"
#include "expression.h"
#include "../../data/symbol.h"

/* a return statement
 *  return expr
 */
struct ic_stmt_ret {
    struct ic_expr *ret;
};

/* allocate and initialise a new return
 * does not touch init ic_expr
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_stmt_ret * ic_stmt_ret_new(void);

/* initialise an existing return
 * does not touch the init expression
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_stmt_ret_init(struct ic_stmt_ret *ret);

/* destroy ret
 *
 * will only free ret if `free_ret` is truthy
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_stmt_ret_destroy(struct ic_stmt_ret *ret, unsigned int free_ret);

/* get the ic_expr * contained within
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr * ic_stmt_ret_get_expr(struct ic_stmt_ret *ret);

/* print this return */
void ic_stmt_ret_print(struct ic_stmt_ret *ret, unsigned int *indent_level);


/* a let statement
 *  let identifier::type = init
 */
struct ic_stmt_let {
    /* flag specifying if we have let a mutable variable
     *  let &a::Int = expr # mutable
     *  let b::Int = expr # immutable
     */
    unsigned int mut;
    struct ic_symbol identifier;
    struct ic_symbol type;
    /* FIXME making this an ic_expr *
     * to simplify interface between
     * parse stmt and parse expr
     */
    struct ic_expr *init;
};

/* allocate and initialise a new let
 * does not touch init ic_expr
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_stmt_let * ic_stmt_let_new(char *id_src, unsigned int id_len, char *type_src, unsigned int type_len);

/* initialise an existing let
 * does not touch the init expression
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_stmt_let_init(struct ic_stmt_let *let, char *id_src, unsigned int id_len, char *type_src, unsigned int type_len);

/* destroy let
 *
 * will only free let if `free_let` is truthy
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_stmt_let_destroy(struct ic_stmt_let *let, unsigned int free_let);

/* get the ic_expr * contained within
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr * ic_stmt_let_get_expr(struct ic_stmt_let *let);

/* print this let */
void ic_stmt_let_print(struct ic_stmt_let *let, unsigned int *indent_level);

/* an if statement
 *  if expr
 *      body
 *  end
 */
struct ic_stmt_if {
    /* FIXME making this an ic_expr *
     * to simplify interface between
     * parse stmt and parse expr
     */
    struct ic_expr *expr;
    struct ic_body body;
};

/* allocate and initialise a new ic_stmtm_if
 * this will initialise the body
 * but will NOT initialise the expression
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_stmt_if * ic_stmt_if_new(void);

/* initialise an existing new ic_stmtm_if
 * this will initialise the body
 * but will NOT initialise the expression
 *
 * returns 0 on sucess
 * returns 1 on failure
 */
unsigned int ic_stmt_if_init(struct ic_stmt_if *sif);

/* destroy if
 *
 * only frees stmt_if if `free_if` is truthy
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_stmt_if_destroy(struct ic_stmt_if *sif, unsigned int free_if);

/* returns pointer on success
 * returns 0 on failure
 */
struct ic_expr * ic_stmt_if_get_expr(struct ic_stmt_if *sif);

/* get statement of offset i within the body
 *
 * returns pointer to element on success
 * returns 0 on error
 */
struct ic_stmt * ic_stmt_if_get_stmt(struct ic_stmt_if *sif, unsigned int i);

/* get length of body
 *
 * returns length on sucess
 * returns 0 on failure
 */
unsigned int ic_stmt_if_length(struct ic_stmt_if *sif);

/* print this if */
void ic_stmt_if_print(struct ic_stmt_if *sif, unsigned int *indent_level);


enum ic_stmt_type {
    ic_stmt_type_ret,
    ic_stmt_type_let,
    ic_stmt_type_if,
    ic_stmt_type_expr
};

struct ic_stmt {
    enum ic_stmt_type type;
    union {
        struct ic_stmt_ret ret;
        struct ic_stmt_let let;
        struct ic_stmt_if sif;
        /* a statement can just be an expression in
         * void context
         *  foo(bar)
         *
         * FIXME making this an ic_expr *
         * to simplify interface between
         * parse stmt and parse expr
         */
        struct ic_expr *expr;
    } u;
};

/* allocate and initialise anew ic_stmt
 * will not initialise union members
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_stmt * ic_stmt_new(enum ic_stmt_type type);

/* initialise an existing ic_stmt
 *
 * returns 0 on success
 * returns 1 on failure
 */
int ic_stmt_init(struct ic_stmt *stmt, enum ic_stmt_type type);

/* destroy stmt
 *
 * will only free stmt if `free_stmt` is truthy
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_stmt_destroy(struct ic_stmt *stmt, unsigned int free_stmt);

/* get a pointer to the return within
 * will only succeed if ic_stmt is of the correct type
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_stmt_ret * ic_stmt_get_ret(struct ic_stmt *stmt);

/* get a pointer to the let within
 * will only succeed if ic_stmt is of the correct type
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_stmt_let * ic_stmt_get_let(struct ic_stmt *stmt);

/* get a pointer to the sif within
 * will only succeed if ic_stmt is of the correct type
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_stmt_if * ic_stmt_get_sif(struct ic_stmt *stmt);

/* get a pointer to the expr within
 * will only succeed if ic_stmt is of the correct type
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr * ic_stmt_get_expr(struct ic_stmt *stmt);

/* print this stmt */
void ic_stmt_print(struct ic_stmt *stmt, unsigned int *indent_level);

#endif
