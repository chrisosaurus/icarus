#ifndef ICARUS_PARSE_H
#define ICARUS_PARSE_H

#include "lexer.h"
#include "ast.h"

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


/**** parse/field.c ****/

struct ic_field * ic_parse_field(struct ic_tokens *tokens, unsigned int *i);


/**** parse/decl.c ****/

struct ic_decl * ic_parse_type_decl(struct ic_tokens *tokens, unsigned int *i);

struct ic_decl * ic_parse_enum_decl(struct ic_tokens *tokens, unsigned int *i);

struct ic_decl * ic_parse_union_decl(struct ic_tokens *tokens, unsigned int *i);

struct ic_decl * ic_parse_func_decl(struct ic_tokens *tokens, unsigned int *i);


/**** parse.c ****/

struct ic_ast * ic_parse(struct ic_tokens *tokens);

#endif

