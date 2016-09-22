#ifndef ICARUS_TRANSFORM_IR_H
#define ICARUS_TRANSFOMR_IR_H

#include "../../analyse/data/type.h"
#include "../../data/pvector.h"
#include "../../data/symbol.h"

/* IR:
 *  let name type literal
 *  let name type expr
 *  expr
 *  return name
 *  assign name name
 *
 * expr:
 *  fcall name...
 */

struct ic_transform_ir_let_literal {
    struct ic_symbol *name;
    struct ic_type *type;
    struct ic_expr_constant *literal;
};

/* allocate and initialise a new let_literal
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_let_literal *ic_transform_ir_let_literal_new(void);

/* initialise an existing let_literal
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_let_literal_init(struct ic_transform_ir_let_literal *let);

/* destroy let_literal
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free let if `free_let` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_let_literal_destroy(struct ic_transform_ir_let_literal *let, unsigned int free_let);

/* print let_literal
 *
 * returns 1 on success
 * return 0 on failure
 */
unsigned int ic_transform_ir_let_literal_print(FILE *fd, struct ic_transform_ir_let_literal *let, unsigned int *indent);

struct ic_transform_ir_let_expr {
    struct ic_symbol *name;
    struct ic_type *type;
    struct ic_transform_ir_expr *expr;
};

/* allocate and initialise a new let_expr
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_let_expr *ic_transform_ir_let_expr_new(void);

/* initialise an existing let_expr
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_let_expr_init(struct ic_transform_ir_let_expr *let);

/* destroy let_expr
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free let if `free_let` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_let_expr_destroy(struct ic_transform_ir_let_expr *let, unsigned int free_let);

/* print let_expr
 *
 * returns 1 on success
 * return 0 on failure
 */
unsigned int ic_transform_ir_let_expr_print(FILE *fd, struct ic_transform_ir_let_expr *let, unsigned int *indent);

enum ic_transform_ir_let_tag {
    ic_transform_ir_let_type_literal,
    ic_transform_ir_let_type_expr
};

struct ic_transform_ir_let {
    enum ic_transform_ir_let_tag tag;
    union {
        struct ic_transform_ir_let_literal lit;
        struct ic_transform_ir_let_expr expr;
    } u;
};

/* allocate and initialise a new let
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_let *ic_transform_ir_let_new(enum ic_transform_ir_let_tag tag);

/* initialise an existing let
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_let_init(struct ic_transform_ir_let *let, enum ic_transform_ir_let_tag tag);

/* destroy let
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free let if `free_let` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_let_destroy(struct ic_transform_ir_let *let, unsigned int free_let);

/* print let
 *
 * returns 1 on success
 * return 0 on failure
 */
unsigned int ic_transform_ir_let_print(FILE *fd, struct ic_transform_ir_let *let, unsigned int *indent);

/* get pointer to internal let_literal
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_let_literal *ic_transform_ir_let_get_literal(struct ic_transform_ir_let *let);

/* get pointer to internal let_expr
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_let_expr *ic_transform_ir_let_get_expr(struct ic_transform_ir_let *let);

/* FIXME TODO isn't very powerful yet, for example
 *  dict["key"] = value
 *  array[index] = value
 *  f.a = "foo"
 * are all currently not representable with this assign
 */
struct ic_transform_ir_assign {
    struct ic_symbol *left;
    struct ic_transform_ir_expr *right;
};

/* allocate and initialise a new assign
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_assign *ic_transform_ir_assign_new(void);

/* initialise an existing assign
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_assign_init(struct ic_transform_ir_assign *assign);

/* destroy assign
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free assign if `free_assign` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_assign_destroy(struct ic_transform_ir_assign *assign, unsigned int free_assign);

/* print assign
 *
 * returns 1 on success
 * return 0 on failure
 */
unsigned int ic_transform_ir_assign_print(FILE *fd, struct ic_transform_ir_assign *assign, unsigned int *indent);

struct ic_transform_ir_if {
    /* symbol pointing to let variables holding if-condition value */
    struct ic_symbol *cond;
    /* mandatory then tbody */
    struct ic_transform_body *then_tbody;
    /* optional else tbody */
    struct ic_transform_body *else_tbody;
};

/* allocate and initialise a new if
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_if *ic_transform_ir_if_new(struct ic_symbol *cond_sym);

/* initialise an existing if
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_if_init(struct ic_transform_ir_if *tif, struct ic_symbol *cond_sym);

/* destroy if
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free assign if `free_if` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_if_destroy(struct ic_transform_ir_if *tif, unsigned int free_if);

/* print if
 *
 * returns 1 on success
 * return 0 on failure
 */
unsigned int ic_transform_ir_if_print(FILE *fd, struct ic_transform_ir_if *tif, unsigned int *indent);

struct ic_transform_ir_expr {
    struct ic_transform_ir_fcall *fcall;
};

/* allocate and initialise a new expr
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_expr *ic_transform_ir_expr_new(void);

/* initialise an existing expr
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_expr_init(struct ic_transform_ir_expr *expr);

/* destroy expr
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free expr if `free_expr` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_expr_destroy(struct ic_transform_ir_expr *expr, unsigned int free_expr);

/* print expr
 *
 * returns 1 on success
 * return 0 on failure
 */
unsigned int ic_transform_ir_expr_print(FILE *fd, struct ic_transform_ir_expr *expr, unsigned int *indent);

struct ic_transform_ir_ret {
    struct ic_symbol *var;
};

/* allocate and initialise a new ret
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_ret *ic_transform_ir_ret_new(void);

/* initialise an existing ret
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_ret_init(struct ic_transform_ir_ret *ret);

/* destroy ret
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free ret if `free_ret` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_ret_destroy(struct ic_transform_ir_ret *ret, unsigned int free_ret);

/* print ret
 *
 * returns 1 on success
 * return 0 on failure
 */
unsigned int ic_transform_ir_ret_print(FILE *fd, struct ic_transform_ir_ret *ret, unsigned int *indent);

struct ic_transform_ir_fcall {
    struct ic_expr_func_call *fcall;
    /* pvector of symbols */
    struct ic_pvector *args;
};

/* allocate and initialise a new fcall
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_fcall *ic_transform_ir_fcall_new(struct ic_expr_func_call *fcall, struct ic_pvector *args);

/* initialise an existing fcall
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_fcall_init(struct ic_transform_ir_fcall *tir_fcall, struct ic_expr_func_call *fcall, struct ic_pvector *args);

/* destroy fcall
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free fcall if `free_fcall` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_fcall_destroy(struct ic_transform_ir_fcall *fcall, unsigned int free_fcall);

/* print fcall
 *
 * returns 1 on success
 * return 0 on failure
 */
unsigned int ic_transform_ir_fcall_print(FILE *fd, struct ic_transform_ir_fcall *fcall, unsigned int *indent);

/* get number of args
 *
 * returns number on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_fcall_length(struct ic_transform_ir_fcall *fcall);

/* get arg at offset i
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_symbol *ic_transform_ir_fcall_get_arg(struct ic_transform_ir_fcall *fcall, unsigned int i);

enum ic_transform_ir_stmt_tag {
    ic_transform_ir_stmt_type_expr,
    ic_transform_ir_stmt_type_let,
    ic_transform_ir_stmt_type_ret,
    ic_transform_ir_stmt_type_assign,
    ic_transform_ir_stmt_type_if
};

struct ic_transform_ir_stmt {
    enum ic_transform_ir_stmt_tag tag;
    union {
        struct ic_transform_ir_expr expr;
        struct ic_transform_ir_let let;
        struct ic_transform_ir_ret ret;
        struct ic_transform_ir_assign assign;
        struct ic_transform_ir_if sif;
    } u;
};

/* allocate and initialise a new stmt
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_stmt *ic_transform_ir_stmt_new(enum ic_transform_ir_stmt_tag tag);

/* initialise an existing stmt
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_stmt_init(struct ic_transform_ir_stmt *stmt, enum ic_transform_ir_stmt_tag tag);

/* destroy stmt
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free stmt if `free_stmt` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_stmt_destroy(struct ic_transform_ir_stmt *stmt, unsigned int free_stmt);

/* print stmt
 *
 * returns 1 on success
 * return 0 on failure
 */
unsigned int ic_transform_ir_stmt_print(FILE *fd, struct ic_transform_ir_stmt *stmt, unsigned int *indent);

/* get pointer to internal expr
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_expr *ic_transform_ir_stmt_get_expr(struct ic_transform_ir_stmt *stmt);

/* get pointer to internal let
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_let *ic_transform_ir_stmt_get_let(struct ic_transform_ir_stmt *stmt);

/* get pointer to internal ret
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_ret *ic_transform_ir_stmt_get_ret(struct ic_transform_ir_stmt *stmt);

/* get pointer to internal if
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_if *ic_transform_ir_stmt_get_if(struct ic_transform_ir_stmt *stmt);

/* get pointer to internal assign
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_assign *ic_transform_ir_stmt_get_assign(struct ic_transform_ir_stmt *stmt);

/* allocate and initialise a new stmt->let->literal
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_stmt *ic_transform_ir_stmt_let_literal_new(struct ic_symbol *name, struct ic_type *type, struct ic_expr_constant *literal);

/* allocate and initialise a new stmt->let->expr
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_stmt *ic_transform_ir_stmt_let_expr_new(struct ic_symbol *name, struct ic_type *type, struct ic_transform_ir_expr *expr);

/* allocate and initialise a new stmt->ret
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_stmt *ic_transform_ir_stmt_ret_new(struct ic_symbol *var);
/* allocate and initialise a new stmt->if
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_stmt *ic_transform_ir_stmt_if_new(struct ic_symbol *cond_sym);

#endif /* ifndef ICARUS_TRANSFORM_IR_H */
