#ifndef ICARUS_PARSE_H
#define ICARUS_PARSE_H

#include "lexer.h"
#include "ast.h"

struct ic_field * ic_parse_field(struct ic_tokens *tokens, unsigned int *i);

struct ic_decl * ic_parse_type_decl(struct ic_tokens *tokens, unsigned int *i);

struct ic_decl * ic_parse_enum_decl(struct ic_tokens *tokens, unsigned int *i);

struct ic_decl * ic_parse_union_decl(struct ic_tokens *tokens, unsigned int *i);

struct ic_decl * ic_parse_func_decl(struct ic_tokens *tokens, unsigned int *i);

struct ic_ast * ic_parse(struct ic_tokens *tokens);

#endif
