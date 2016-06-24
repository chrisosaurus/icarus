#include <stdlib.h>
#include <stdio.h>

#include "tir.h"

/* allocate and initialise a new let_literal
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_let_literal * ic_transform_ir_let_literal_new(void){
  struct ic_transform_ir_let_literal *let = 0;

  let = calloc(sizeof(struct ic_transform_ir_let_literal), 0);
  if( ! let ){
    puts("ir_transform_ir_let_literal_new: call to calloc failed");
    return 0;
  }

  if( ! ic_transform_ir_let_literal_init(let) ){
    puts("ir_transform_ir_let_literal_new: call to ic_transform_let_literal_init failed");
    return 0;
  }

  return let;
}

/* initialise an existing let_literal
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_let_literal_init(struct ic_transform_ir_let_literal *let){
  if( ! let ){
    puts("ic_transform_ir_let_literal_init: let was null");
    return 0;
  }

  let->name = 0;
  let->type = 0;
  let->literal = 0;

  return 1;
}

/* destroy let_literal
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free let if `free_let` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_let_literal_destroy(struct ic_transform_ir_let_literal *let, unsigned int free_let){
  if( ! let ){
    puts("ic_transform_ir_let_let_literal_destroy: let was null");
    return 0;
  }

  if( free_let ){
    free(let);
  }

  return 1;
}


/* allocate and initialise a new let_expr
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_let_expr * ic_transform_ir_let_expr_new(void){
  struct ic_transform_ir_let_expr *expr = 0;

  expr = calloc(sizeof(struct ic_transform_ir_let_expr), 0);
  if( ! expr ){
    puts("ir_transform_ir_let_expr_new: call to calloc failed");
    return 0;
  }

  if( ! ic_transform_ir_let_expr_init(expr) ){
    puts("ir_transform_ir_let_expr_new: call to ic_transform_let_expr_init failed");
    return 0;
  }

  return expr;
}

/* initialise an existing let_expr
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_let_expr_init(struct ic_transform_ir_let_expr *let){
  if( ! let ){
    puts("ic_transform_ir_let_expr_init: let was null");
    return 0;
  }

  let->name = 0;
  let->type = 0;
  let->expr = 0;

  return 1;
}

/* destroy let_expr
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free let if `free_let` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_let_expr_destroy(struct ic_transform_ir_let_expr *let, unsigned int free_let){
  if( ! let ){
    puts("ic_transform_ir_let_expr_destroy: let was null");
    return 0;
  }

  if( free_let ){
    free(let);
  }

  return 1;
}


/* allocate and initialise a new let
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_let * ic_transform_ir_let_new(enum ic_transform_ir_let_tag tag){
  struct ic_transform_ir_let *let = 0;

  let = calloc(sizeof(struct ic_transform_ir_let), 0);
  if( ! let ){
    puts("ir_transform_ir_let_new: call to calloc failed");
    return 0;
  }

  if( ! ic_transform_ir_let_init(let, tag) ){
    puts("ir_transform_ir_let_new: call to ic_transform_let_init failed");
    return 0;
  }

  return let;
}

/* initialise an existing let
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_let_init(struct ic_transform_ir_let *let, enum ic_transform_ir_let_tag tag){
  if( ! let ){
    puts("ic_transform_ir_let_init: let was null");
    return 0;
  }

  let->tag = tag;

  return 1;
}

/* destroy let
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free let if `free_let` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_let_destroy(struct ic_transform_ir_let *let, unsigned int free_let){
  if( ! let ){
    puts("ic_transform_ir_let_destroy: let was null");
    return 0;
  }

  if( free_let ){
    free(let);
  }

  return 1;
}

/* get pointer to internal let_literal
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_let_literal * ic_transform_ir_let_get_literal(struct ic_transform_ir_let *let){
  if( ! let ){
    puts("ic_transform_ir_let_get_literal: let was null");
    return 0;
  }

  if( let->tag != ic_transform_ir_let_type_literal ){
    puts("ic_transform_ir_let_get_literal: let was not of type literal");
    return 0;
  }

  return &(let->u.lit);
}

/* get pointer to internal let_expr
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_let_expr * ic_transform_ir_let_get_expr(struct ic_transform_ir_let *let){
  if( ! let ){
    puts("ic_transform_ir_let_get_expr: let was null");
    return 0;
  }

  if( let->tag != ic_transform_ir_let_type_fcall ){
    puts("ic_transform_ir_let_get_expr: let was not of type fcall");
    return 0;
  }

  return &(let->u.expr);
}


/* allocate and initialise a new assign
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_assign * ic_transform_ir_assign_new(void){
  struct ic_transform_ir_assign *assign = 0;

  assign = calloc(sizeof(struct ic_transform_ir_assign), 0);
  if( ! assign ){
    puts("ir_transform_ir_assign_new: call to calloc failed");
    return 0;
  }

  if( ! ic_transform_ir_assign_init(assign) ){
    puts("ir_transform_ir_assign_new: call to ic_transform_assign_init failed");
    return 0;
  }

  return assign;
}

/* initialise an existing assign
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_assign_init(struct ic_transform_ir_assign *assign){
  if( ! assign ){
    puts("ic_transform_ir_let_assign_init: assign was null");
    return 0;
  }

  assign->left = 0;
  assign->right = 0;

  return 1;
}

/* destroy assign
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free assign if `free_assign` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_assign_destroy(struct ic_transform_ir_assign *assign, unsigned int free_assign){
  if( ! assign ){
    puts("ic_transform_ir_let_assign_destroy: assign was null");
    return 0;
  }

  if( free_assign ){
    free(assign);
  }

  return 1;
}


/* allocate and initialise a new expr
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_expr * ic_transform_ir_expr_new(void){
  struct ic_transform_ir_expr *expr = 0;

  expr = calloc(sizeof(struct ic_transform_ir_expr), 0);
  if( ! expr ){
    puts("ir_transform_ir_expr_new: call to calloc failed");
    return 0;
  }

  if( ! ic_transform_ir_expr_init(expr) ){
    puts("ir_transform_ir_expr_new: call to ic_transform_expr_init failed");
    return 0;
  }

  return expr;
}

/* initialise an existing expr
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_expr_init(struct ic_transform_ir_expr *expr){
  if( ! expr ){
    puts("ic_transform_ir_expr_init: expr was null");
    return 0;
  }

  expr->fcall = 0;

  return 1;
}

/* destroy expr
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free expr if `free_expr` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_expr_destroy(struct ic_transform_ir_expr *expr, unsigned int free_expr){
  if( ! expr ){
    puts("ic_transform_ir_expr_destroy: expr was null");
    return 0;
  }

  if( free_expr ){
    free(expr);
  }

  return 1;
}


/* allocate and initialise a new ret
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_ret * ic_transform_ir_ret_new(void){
  struct ic_transform_ir_ret *ret = 0;

  ret = calloc(sizeof(struct ic_transform_ir_ret), 0);
  if( ! ret ){
    puts("ir_transform_ir_ret_new: call to calloc failed");
    return 0;
  }

  if( ! ic_transform_ir_ret_init(ret) ){
    puts("ir_transform_ir_ret_new: call to ic_transform_ret_init failed");
    return 0;
  }

  return ret;
}

/* initialise an existing ret
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_ret_init(struct ic_transform_ir_ret *ret){
  if( ! ret ){
    puts("ic_transform_ir_ret_init: ret was null");
    return 0;
  }

  ret->var = 0;

  return 1;
}

/* destroy ret
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free ret if `free_ret` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_ret_destroy(struct ic_transform_ir_ret *ret, unsigned int free_ret){
  if( ! ret ){
    puts("ic_transform_ir_ret_destroy: ret was null");
    return 0;
  }

  if( free_ret ){
    free(ret);
  }

  return 1;
}


/* allocate and initialise a new fcall
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_fcall * ic_transform_ir_fcall_new(void){
  struct ic_transform_ir_fcall *fcall = 0;

  fcall = calloc(sizeof(struct ic_transform_ir_fcall), 0);
  if( ! fcall ){
    puts("ir_transform_ir_fcall_new: call to calloc failed");
    return 0;
  }

  if( ! ic_transform_ir_fcall_init(fcall) ){
    puts("ir_transform_ir_fcall_new: call to ic_transform_fcall_init failed");
    return 0;
  }

  return fcall;
}

/* initialise an existing fcall
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_fcall_init(struct ic_transform_ir_fcall *fcall){
  if( ! fcall ){
    puts("ic_transform_ir_fcall_init: fcall was null");
    return 0;
  }

  fcall->fcall = 0;
  fcall->args = 0;

  return 1;
}

/* destroy fcall
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free fcall if `free_fcall` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_fcall_destroy(struct ic_transform_ir_fcall *fcall, unsigned int free_fcall){
  if( ! fcall ){
    puts("ic_transform_ir_fcall_destroy: fcall was null");
    return 0;
  }

  if( free_fcall ){
    free(fcall);
  }

  return 1;
}


/* allocate and initialise a new stmt
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_stmt * ic_transform_ir_stmt_new(enum ic_transform_ir_stmt_tag tag){
  struct ic_transform_ir_stmt *stmt = 0;

  stmt = calloc(sizeof(struct ic_transform_ir_stmt), 0);
  if( ! stmt ){
    puts("ir_transform_ir_stmt_new: call to calloc failed");
    return 0;
  }

  if( ! ic_transform_ir_stmt_init(stmt, tag) ){
    puts("ir_transform_ir_stmt_new: call to ic_transform_stmt_init failed");
    return 0;
  }

  return stmt;
}

/* initialise an existing stmt
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_stmt_init(struct ic_transform_ir_stmt *stmt, enum ic_transform_ir_stmt_tag tag){
  if( ! stmt ){
    puts("ic_transform_ir_stmt_init: stmt was null");
    return 0;
  }

  stmt->tag = tag;

  return 1;
}

/* destroy stmt
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free stmt if `free_stmt` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_stmt_destroy(struct ic_transform_ir_stmt *stmt, unsigned int free_stmt){
  if( ! stmt ){
    puts("ic_transform_ir_stmt_destroy: stmt was null");
    return 0;
  }

  if( free_stmt ){
    free(stmt);
  }

  return 1;
}

/* get pointer to internal expr
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_expr * ic_transform_ir_stmt_get_expr(struct ic_transform_ir_stmt *stmt){
  if( ! stmt ){
    puts("ic_transform_ir_stmt_get_expr: stmt was null");
    return 0;
  }

  if( stmt->tag != ic_transform_ir_type_expr){
    puts("ic_transform_ir_stmt_get_expr: stmt was not of type expr");
    return 0;
  }

  return &(stmt->u.expr);
}

/* get pointer to internal let
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_let * ic_transform_ir_stmt_get_let(struct ic_transform_ir_stmt *stmt){
  if( ! stmt ){
    puts("ic_transform_ir_stmt_get_let: stmt was null");
    return 0;
  }

  if( stmt->tag != ic_transform_ir_type_let){
    puts("ic_transform_ir_stmt_get_let: stmt was not of type let");
    return 0;
  }

  return &(stmt->u.let);
}

/* get pointer to internal ret
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_ret * ic_transform_ir_stmt_get_ret(struct ic_transform_ir_stmt *stmt){
  if( ! stmt ){
    puts("ic_transform_ir_stmt_get_ret: stmt was null");
    return 0;
  }

  if( stmt->tag != ic_transform_ir_type_ret){
    puts("ic_transform_ir_stmt_get_ret: stmt was not of type ret");
    return 0;
  }

  return &(stmt->u.ret);
}
