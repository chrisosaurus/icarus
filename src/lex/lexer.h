#ifndef IC_LEXER_H
#define IC_LEXER_H

#include "data/token_list.h"
#include "data/lex_data.h"

/* takes a character array of the source program
 *
 * takes the filename for current source unit, this
 * is attached to each token
 *
 * returns a token_list on success
 * returns 0 on failure
 */
struct ic_token_list * ic_lex(char *filename, char *source);

#endif
