#ifndef ICARUS_PARSE_H
#define ICARUS_PARSE_H

#include "lexer.h"
#include "types.h"
#include "ast.h"

struct ic_type_decl * ic_parse_type_decl(struct ic_tokens *tokens, unsigned int *i);

void * ic_parse(struct ic_tokens *tokens);

#endif
