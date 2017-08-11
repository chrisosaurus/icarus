#ifndef ICARUS_STATEMENT_H
#define ICARUS_STATEMENT_H

#include <stdbool.h>

#include "../../data/symbol.h"
#include "body.h"
#include "expr.h"

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
struct ic_stmt_ret *ic_stmt_ret_new(void);

/* initialise an existing return
 * does not touch the init expression
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_ret_init(struct ic_stmt_ret *ret);

/* destroy ret
 *
 * will only free ret if `free_ret` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_ret_destroy(struct ic_stmt_ret *ret, unsigned int free_ret);

/* perform a deep copy of ret
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_stmt_ret *ic_stmt_ret_deep_copy(struct ic_stmt_ret *ret);

/* perform a deep copy of ret embedded within an object
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_ret_deep_copy_embedded(struct ic_stmt_ret *from, struct ic_stmt_ret *to);

/* get the ic_expr * contained within
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr *ic_stmt_ret_get_expr(struct ic_stmt_ret *ret);

/* print this return */
void ic_stmt_ret_print(FILE *fd, struct ic_stmt_ret *ret, unsigned int *indent_level);

/* a let statement
 *  let identifier::type = init
 */
struct ic_stmt_let {
    unsigned int permissions;

    struct ic_symbol identifier;

    /* may be set at parse time from user-declared types (symbol)
     * set at analysis time (decl_type) either from
     *  a) elevating the user-declared type
     *  b) inferring type from expr
     */
    struct ic_type_ref *tref;

    /* FIXME making this an ic_expr *
     * to simplify interface between
     * parse stmt and parse expr
     */
    struct ic_expr *init;

    /* if this let ever assigned to
     * or only read from
     *
     * used in transform phase to populate tlet->assigned_to
     */
    bool assigned_to;
};

/* allocate and initialise a new let
 * does not touch init ic_expr
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_stmt_let *ic_stmt_let_new(char *id_src, unsigned int id_len, unsigned int permissions);

/* initialise an existing let
 * does not touch the init expression
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_let_init(struct ic_stmt_let *let, char *id_src, unsigned int id_len, unsigned int permissions);

/* destroy let
 *
 * will only free let if `free_let` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_let_destroy(struct ic_stmt_let *let, unsigned int free_let);

/* set declared type on this let
 *
 * this is an error if the type has already been set
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_let_set_declared_type(struct ic_stmt_let *let, struct ic_type_ref *tref);

/* set inferred type on this let
 *
* this is set at analysis time
 *
 * this is an error if the type is already set
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_let_set_inferred_type(struct ic_stmt_let *let, struct ic_decl_type *type);

/* get the ic_expr * contained within
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr *ic_stmt_let_get_expr(struct ic_stmt_let *let);

/* print this let */
void ic_stmt_let_print(FILE *fd, struct ic_stmt_let *let, unsigned int *indent_level);

/* an assignment statement
 *  x = y
 *  FIXME need to have permissions on expressions
 */
struct ic_stmt_assign {
    struct ic_expr *left;
    struct ic_expr *right;
};

/* allocate and initialise a new assign
 * does not touch init ic_expr
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_stmt_assign *ic_stmt_assign_new(struct ic_expr *left, struct ic_expr *right);

/* initialise an existing assign
 * does not touch the init expression
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_assign_init(struct ic_stmt_assign *assign, struct ic_expr *left, struct ic_expr *right);

/* destroy assign
 *
 * will only free assign if `free_assign` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_assign_destroy(struct ic_stmt_assign *assign, unsigned int free_assign);

/* get the left ic_expr * contained within
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr *ic_stmt_assign_get_left(struct ic_stmt_assign *assign);

/* get the right ic_expr * contained within
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr *ic_stmt_assign_get_right(struct ic_stmt_assign *assign);

/* print this assign */
void ic_stmt_assign_print(FILE *fd, struct ic_stmt_assign *assign, unsigned int *indent_level);

/* a begin stmt
 *  begin
 *    body
 *  end
 */
struct ic_stmt_begin {
    struct ic_body *body;
};

/* allocate and initialise a new ic_stmt_begin
 * this will initialise the body
 * but will NOT initialise the expression
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_stmt_begin *ic_stmt_begin_new(void);

/* initialise an existing new ic_stmt_begin
 * this will initialise the body
 * but will NOT initialise the expression
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_begin_init(struct ic_stmt_begin *begin);

/* destroy begin
 *
 * only frees begin if `free_begin` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_begin_destroy(struct ic_stmt_begin *begin, unsigned int free_begin);

/* print this begin */
void ic_stmt_begin_print(FILE *fd, struct ic_stmt_begin *begin, unsigned int *indent_level);

/* an if statement
 *  if expr
 *      body
 *  end
 */
struct ic_stmt_if {
    struct ic_expr *expr;
    struct ic_body *then_body;
    struct ic_body *else_body;
};

/* allocate and initialise a new ic_stmt_if
 * this will initialise the body
 * but will NOT initialise the expression
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_stmt_if *ic_stmt_if_new(void);

/* initialise an existing new ic_stmt_if
 * this will initialise the body
 * but will NOT initialise the expression
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_if_init(struct ic_stmt_if *sif);

/* destroy if
 *
 * only frees stmt_if if `free_if` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_if_destroy(struct ic_stmt_if *sif, unsigned int free_if);

/* returns pointer on success
 * returns 0 on failure
 */
struct ic_expr *ic_stmt_if_get_expr(struct ic_stmt_if *sif);

/* print this if */
void ic_stmt_if_print(FILE *fd, struct ic_stmt_if *sif, unsigned int *indent_level);

/* a for statement
 *  for expr in iterator
 *      body
 *  end
 */
struct ic_stmt_for {
    struct ic_expr *expr;
    struct ic_expr *iterator;
    struct ic_body *body;
};

/* allocate and initialise a new ic_stmt_for
 * this will initialise the body
 * but will NOT initialise the expression OR the iterator
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_stmt_for *ic_stmt_for_new(void);

/* initialise an existing new ic_stmt_for
 * this will initialise the body
 * but will NOT initialise the expression
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_for_init(struct ic_stmt_for *sfor);

/* destroy for
 *
 * only frees stmt_for if `free_for` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_for_destroy(struct ic_stmt_for *sfor, unsigned int free_for);

/* returns pointer on success
 * returns 0 on failure
 */
struct ic_expr *ic_stmt_for_get_expr(struct ic_stmt_for *sfor);

/* returns pointer on success
 * returns 0 on failure
 */
struct ic_expr *ic_stmt_for_get_iterator(struct ic_stmt_for *sfor);

/* get statement of offset i within the body
 *
 * returns pointer to element on success
 * returns 0 on failure
 */
struct ic_stmt *ic_stmt_for_get_stmt(struct ic_stmt_for *sfor, unsigned int i);

/* get length of body
 *
 * returns length on success
 * returns 0 on failure
 */
unsigned int ic_stmt_for_length(struct ic_stmt_for *sfor);

/* print this for */
void ic_stmt_for_print(FILE *fd, struct ic_stmt_for *sfor, unsigned int *indent_level);

/* a while statement
 *  while expr
 *      body
 *  end
 */
struct ic_stmt_while {
    struct ic_expr *expr;
    struct ic_body *body;
};

/* allocate and initialise a new ic_stmt_while
 * this will initialise the body
 * but will NOT initialise the expression
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_stmt_while *ic_stmt_while_new(void);

/* initialise an existing new ic_stmt_while
 * this will initialise the body
 * but will NOT initialise the expression
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_while_init(struct ic_stmt_while *swhile);

/* destroy while
 *
 * only frees stmt_while if `free_while` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_while_destroy(struct ic_stmt_while *swhile, unsigned int free_while);

/* returns pointer on success
 * returns 0 on failure
 */
struct ic_expr *ic_stmt_while_get_expr(struct ic_stmt_while *swhile);

/* get statement of offset i within the body
 *
 * returns pointer to element on success
 * returns 0 on failure
 */
struct ic_stmt *ic_stmt_while_get_stmt(struct ic_stmt_while *swhile, unsigned int i);

/* get length of body
 *
 * returns length on success
 * returns 0 on failure
 */
unsigned int ic_stmt_while_length(struct ic_stmt_while *swhile);

/* print this if */
void ic_stmt_while_print(FILE *fd, struct ic_stmt_while *swhile, unsigned int *indent_level);

/* match expr
 *  case name::Type
 *    body
 *  case name::Type
 *    body
 *  else
 *    body
 *  end
 * end
 */
struct ic_stmt_match {
    /* expr we are matching on */
    struct ic_expr *expr;
    /* optional list of cases */
    struct ic_pvector cases;
    /* optional else body */
    struct ic_body *else_body;
};

/* allocate and initialise a new ic_stmt_match
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_stmt_match *ic_stmt_match_new(void);

/* initialise an existing new ic_stmt_match
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_match_init(struct ic_stmt_match *match);

/* destroy match
 *
 * only frees stmt_match if `free_match` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_match_destroy(struct ic_stmt_match *match, unsigned int free_match);

/* returns pointer on success
 * returns 0 on failure
 */
struct ic_expr *ic_stmt_match_get_expr(struct ic_stmt_match *match);

/* returns pointer on success
 * returns 0 on failure
 */
struct ic_body *ic_stmt_match_get_else_body(struct ic_stmt_match *match);

/* get stmt_case of offset i within cases
 *
 * returns pointer to element on success
 * returns 0 on failure
 */
struct ic_stmt_case *ic_stmt_match_cases_get(struct ic_stmt_match *match, unsigned int i);

/* get length of cases
 *
 * returns length on success
 * returns 0 on failure
 */
unsigned int ic_stmt_match_cases_length(struct ic_stmt_match *match);

/* print this if */
void ic_stmt_match_print(FILE *fd, struct ic_stmt_match *match, unsigned int *indent_level);

struct ic_stmt_case {
    /* field we are matching on */
    struct ic_field field;
    struct ic_body *body;
};

/* allocate and initialise a new ic_stmt_case
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_stmt_case *ic_stmt_case_new(char *id_ch, unsigned int id_len, char *type_ch, unsigned int type_len, struct ic_body *body);

/* initialise an existing new ic_stmt_case
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_case_init(struct ic_stmt_case *scase, char *id_ch, unsigned int id_len, char *type_ch, unsigned int type_len, struct ic_body *body);
/* destroy case
 *
 * only frees stmt_case if `free_case` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_case_destroy(struct ic_stmt_case *scase, unsigned int free_case);

/* returns pointer on success
 * returns 0 on failure
 */
struct ic_field *ic_stmt_case_get_field(struct ic_stmt_case *scase);

/* get statement of offset i within the body
 *
 * returns pointer to element on success
 * returns 0 on failure
 */
struct ic_stmt *ic_stmt_case_get_stmt(struct ic_stmt_case *scase, unsigned int i);

/* get length of body
 *
 * returns length on success
 * returns 0 on failure
 */
unsigned int ic_stmt_case_length(struct ic_stmt_case *scase);

/* print this if */
void ic_stmt_case_print(FILE *fd, struct ic_stmt_case *scase, unsigned int *indent_level);

enum ic_stmt_tag {
    ic_stmt_type_ret,
    ic_stmt_type_let,
    ic_stmt_type_assign,
    ic_stmt_type_begin,
    ic_stmt_type_if,
    ic_stmt_type_for,
    ic_stmt_type_while,
    ic_stmt_type_match,
    ic_stmt_type_expr
};

struct ic_stmt {
    enum ic_stmt_tag tag;
    union {
        struct ic_stmt_ret ret;
        struct ic_stmt_let let;
        struct ic_stmt_assign assign;
        struct ic_stmt_begin begin;
        struct ic_stmt_if sif;
        struct ic_stmt_for sfor;
        struct ic_stmt_while swhile;
        struct ic_stmt_match match;
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

/* allocate and initialise a new ic_stmt
 * will not initialise union members
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_stmt *ic_stmt_new(enum ic_stmt_tag tag);

/* initialise an existing ic_stmt
 *
 * returns 1 on success
 * returns 0 on failure
 */
int ic_stmt_init(struct ic_stmt *stmt, enum ic_stmt_tag tag);

/* destroy stmt
 *
 * will only free stmt if `free_stmt` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_destroy(struct ic_stmt *stmt, unsigned int free_stmt);

/* perform a deep copy of a stmt
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_stmt *ic_stmt_deep_copy(struct ic_stmt *stmt);

/* perform a deep copy of a stmt embedded within an object
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_stmt_deep_copy_embedded(struct ic_stmt *from, struct ic_stmt *to);

/* get a pointer to the return within
 * will only succeed if ic_stmt is of the correct type
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_stmt_ret *ic_stmt_get_ret(struct ic_stmt *stmt);

/* get a pointer to the let within
 * will only succeed if ic_stmt is of the correct type
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_stmt_let *ic_stmt_get_let(struct ic_stmt *stmt);

/* get a pointer to the assign within
 * will only succeed if ic_stmt is of the correct type
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_stmt_assign *ic_stmt_get_assign(struct ic_stmt *stmt);

/* get a pointer to the begin within
 * will only succeed if ic_stmt is of the correct type
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_stmt_begin *ic_stmt_get_begin(struct ic_stmt *stmt);

/* get a pointer to the sif within
 * will only succeed if ic_stmt is of the correct type
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_stmt_if *ic_stmt_get_sif(struct ic_stmt *stmt);

/* get a pointer to the sfor within
 * will only succeed if ic_stmt is of the correct type
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_stmt_for *ic_stmt_get_sfor(struct ic_stmt *stmt);

/* get a pointer to the swhile within
 * will only succeed if ic_stmt is of the correct type
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_stmt_while *ic_stmt_get_swhile(struct ic_stmt *stmt);

/* get a pointer to the match within
 * will only succeed if ic_stmt is of the correct type
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_stmt_match *ic_stmt_get_match(struct ic_stmt *stmt);

/* get a pointer to the expr within
 * will only succeed if ic_stmt is of the correct type
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr *ic_stmt_get_expr(struct ic_stmt *stmt);

/* print this stmt */
void ic_stmt_print(FILE *fd, struct ic_stmt *stmt, unsigned int *indent_level);

#endif
