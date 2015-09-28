#ifndef IC_LEXER_H
#define IC_LEXER_H

#include "data/token_list.h"

/* takes a character array of the source program
 *
 * returns a token_list on success
 * returns 0 on failure
 */
struct ic_token_list * ic_lex(char *source);

#endif
