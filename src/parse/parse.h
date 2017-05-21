#ifndef ICARUS_PARSE_H
#define ICARUS_PARSE_H

#include "../lex/lexer.h"
#include "data/ast.h"
#include "data/body.h"
#include "data/expr.h"
#include "data/stmt.h"

#define LENGTH(x) (sizeof(x) / sizeof(x[0]))

/* this is a shared header for all parse files
 * many of the following functions are not implemented in parse.c
 * and have instead been moved into src/parse/ source files
 * based on their return types
 */

/**** shared helper methods (parse.c) ****/

/* print levels worth of indent levels
 * an indent level is defined in parse.c
 */
void ic_parse_print_indent(FILE *fd, unsigned int levels);

/**** parse/field.c ****/

struct ic_field *ic_parse_field(struct ic_token_list *token_list);

/**** parse/expr.c ****/

struct ic_expr *ic_parse_expr(struct ic_token_list *token_list);

/**** parse/stmt.c ****/

struct ic_stmt *ic_parse_stmt(struct ic_token_list *token_list);

/**** parse/body.c ****/

/* parse a body
 * a body ends with an any unmatched:
 *  end
 *  else
 *  case
 * this ending token is only consumed if `consume_end` is truthy
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_body *ic_parse_body(struct ic_token_list *token_list, unsigned int consume_end);

/**** parse/decl.c ****/

unsigned int ic_parse_decl_optional_type_params(struct ic_token_list *token_list, struct ic_pvector *type_params);

struct ic_decl *ic_parse_decl_builtin(struct ic_token_list *token_list);

struct ic_decl *ic_parse_decl_func(struct ic_token_list *token_list);

struct ic_decl *ic_parse_decl_func_header(struct ic_token_list *token_list);

struct ic_decl *ic_parse_decl_func_body(struct ic_token_list *token_list, struct ic_decl *decl);

struct ic_decl *ic_parse_decl_type(struct ic_token_list *token_list);

struct ic_decl *ic_parse_decl_type_header(struct ic_token_list *token_list);

struct ic_decl *ic_parse_decl_type_body(struct ic_token_list *token_list, struct ic_decl *decl);

struct ic_decl *ic_parse_decl_enum(struct ic_token_list *token_list);

struct ic_decl *ic_parse_decl_union(struct ic_token_list *token_list);

struct ic_decl *ic_parse_decl_union_header(struct ic_token_list *token_list);

struct ic_decl *ic_parse_decl_union_body(struct ic_token_list *token_list, struct ic_decl *decl);

struct ic_decl *ic_parse_decl_op(struct ic_token_list *token_list);

/**** parse.c ****/

struct ic_ast *ic_parse(struct ic_token_list *token_list);

unsigned int ic_parse_permissions(struct ic_token_list *token_list);

#endif
