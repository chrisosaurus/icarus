#ifndef ICARUS_PARSE_H
#define ICARUS_PARSE_H

#include "ast.h"
#include "body.h"
#include "expression.h"
#include "lexer.h"
#include "statement.h"

#define LENGTH(x) (sizeof(x) / sizeof(x[0]))

/* this is a shared header for all parse files
 * many of the following functions are not implemented in parse.c
 * and have instead been moved into src/parse/ source files
 * based on their return types
 */


/**** shared helper methods (parse.c) ****/

/* returns length of token starting at source[i]
 * returns 0 on error
 */
unsigned int ic_parse_token_length(char *source, unsigned int i);

/* advance i by one token with token length dist */
void ic_parse_token_advance(unsigned int *i, unsigned int dist);

/* compare and consume un-important token
 *
 * this function will take an expected token-string
 * and compare it to the current token
 *
 * if they are equal then this function will consume that token
 * and return 0
 *
 * if thy are non-equal then this function will NOT consume that token
 * and just return 1
 *
 * returns 0 if token is as expected
 * returns 1 if token is not as expected
 */
unsigned int ic_parse_check_token(char *expected, unsigned int exp_len, char *source, unsigned int *i);


/**** parse/field.c ****/

struct ic_field * ic_parse_field(struct ic_tokens *tokens, unsigned int *i);


/**** parse/expr.c ****/

struct ic_expr * ic_parse_expr(struct ic_tokens *tokens, unsigned int *i);


/**** parse/stmt.c ****/

struct ic_stmt * ic_parse_stmt(struct ic_tokens *tokens, unsigned int *i);


/**** parse/body.c ****/

struct ic_body * ic_parse_body(struct ic_tokens *tokens, unsigned int *i);


/**** parse/decl.c ****/

struct ic_decl * ic_parse_type_decl(struct ic_tokens *tokens, unsigned int *i);

struct ic_decl * ic_parse_enum_decl(struct ic_tokens *tokens, unsigned int *i);

struct ic_decl * ic_parse_union_decl(struct ic_tokens *tokens, unsigned int *i);

struct ic_decl * ic_parse_func_decl(struct ic_tokens *tokens, unsigned int *i);


/**** parse.c ****/

struct ic_ast * ic_parse(struct ic_tokens *tokens);

/* check if we are looking at an `end` token
 *
 * this will also advance i past the token
 *
 * useful for continually iterating through tokens until we find `end
 *
 * example:
 *
 *      int ret = 0;
 *
 *      while( (ret = ic_parse_this_is_not_the_end(tokens, i)) > 0 ){
 *          ...
 *      }
 *
 *      if( ! ret )
 *          return true; // success, end token found
 *      return false; // other error occurred in this_is_not_the_end
 *
 * returns 1 if this is NOT an `end` token
 * returns 0 if this is an `end` token
 * returns -1 if an error occurs (if \0 is hit for example)
 */
int ic_parse_this_is_not_the_end(struct ic_tokens *tokens, unsigned int *i);


#endif

