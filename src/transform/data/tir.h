#ifndef ICARUS_TRANSFORM_IR_H
#define ICARUS_TRANSFOMR_IR_H

#include "../../data/symbol.h"
#include "../../data/pvector.h"
#include "../../analyse/data/type.h"

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
  struct ic_literal *literal;
};

/* allocate and initialise a new let_literal
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_let_literal * ic_transform_ir_let_literal_new(void);

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
struct ic_transform_ir_let_expr * ic_transform_ir_let_expr_new(void);

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


enum ic_transform_ir_let_tag {
  ic_transform_ir_let_type_literal,
  ic_transform_ir_let_type_fcall
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
struct ic_transform_ir_let * ic_transform_ir_let_new(void);

/* initialise an existing let
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_let_init(struct ic_transform_ir_let *let);

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

/* get pointer to internal let_literal
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_let_literal * ic_transform_ir_let_get_literal(struct ic_transform_ir_let *let);

/* get pointer to internal let_expr
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_let_expr * ic_transform_ir_let_get_expr(struct ic_transform_ir_let *let);


struct ic_transform_ir_assign {
  struct ic_symbol *left;
  struct ic_symbol *right;
};

/* allocate and initialise a new assign
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_assign * ic_transform_ir_assign_new(void);

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
struct ic_transform_ir_expr * ic_transform_ir_expr_new(void);

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
struct ic_transform_ir_ret * ic_transform_ir_ret_new(void);

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



struct ic_transform_ir_fcall {
  struct ic_expr_fcall *fcall;
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
struct ic_transform_ir_fcall * ic_transform_ir_fcall_new(void);

/* initialise an existing fcall
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_fcall_init(struct ic_transform_ir_fcall *fcall);

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



enum ic_transform_ir_stmt_tag {
  ic_transform_ir_type_expr,
  ic_transform_ir_type_let,
  ic_transform_ir_type_ret
};

struct ic_transform_ir_stmt {
  enum ic_transform_ir_stmt_tag tag;
  union {
    struct ic_transform_ir_expr expr;
    struct ic_transform_ir_let let;
    struct ic_transform_ir_ret ret;
  }u ;
};

/* allocate and initialise a new stmt
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_stmt * ic_transform_ir_stmt_new(void);

/* initialise an existing stmt
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_stmt_init(struct ic_transform_ir_stmt *stmt);

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

/* get pointer to internal expr
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_expr * ic_transform_ir_stmt_get_expr(struct ic_transform_ir_stmt *stmt);

/* get pointer to internal let
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_let * ic_transform_ir_stmt_get_let(struct ic_transform_ir_stmt *stmt);

/* get pointer to internal ret
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_ret * ic_transform_ir_stmt_get_ret(struct ic_transform_ir_stmt *stmt);

#endif /* ifndef ICARUS_TRANSFORM_IR_H */
