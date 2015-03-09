#ifndef ICARUS_PARSE_H
#define ICARUS_PARSE_H

#include "lexer.h"
#include "types.h"
#include "ast.h"

void * ic_parse(struct ic_tokens *tokens);

#endif
