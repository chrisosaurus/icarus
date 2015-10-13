#ifndef ICARUS_PARSE_H
#define ICARUS_PARSE_H

#include "data/ast.h"
#include "data/body.h"
#include "data/expression.h"
#include "../lex/lexer.h"
#include "data/statement.h"

#define LENGTH(x) (sizeof(x) / sizeof(x[0]))

/* this is a shared header for all parse files
 * many of the following functions are not implemented in parse.c
 * and have instead been moved into src/parse/ source files
 * based on their return types
 */


/**** shared helper methods (parse.c) ****/

/* check if the current token looks like an operator
 *
 * returns 1 for true
 * returns 0 for false
 */
int ic_parse_operatorish(struct ic_token_list *token_list);

/* print levels worth of indent levels
 * an indent level is defined in parse.c
 */
void ic_parse_print_indent(unsigned int levels);

/**** parse/field.c ****/

struct ic_field * ic_parse_field(struct ic_token_list *token_list);


/**** parse/expr.c ****/

struct ic_expr * ic_parse_expr(struct ic_token_list *token_list);


/**** parse/stmt.c ****/

struct ic_stmt * ic_parse_stmt(struct ic_token_list *token_list);


/**** parse/body.c ****/

/* parse a body
 * a body ends when an unmatched `end` is found, this will be consumed
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_body * ic_parse_body(struct ic_token_list *token_list);


/**** parse/decl.c ****/

struct ic_decl * ic_parse_func_decl(struct ic_token_list *token_list);

struct ic_decl * ic_parse_builtin_decl(struct ic_token_list *token_list);

struct ic_decl * ic_parse_type_decl(struct ic_token_list *token_list);

struct ic_decl * ic_parse_enum_decl(struct ic_token_list *token_list);

struct ic_decl * ic_parse_union_decl(struct ic_token_list *token_list);

/**** parse.c ****/

struct ic_ast * ic_parse(struct ic_token_list *token_list);

#endif

