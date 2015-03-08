#ifndef ICARUS_LEXER_H
#define ICARUS_LEXER_H

typedef struct icarus_tokens {
    /* length of char *tokens used */
    int len;
    /* length of char *tokens allocated */
    int cap;
    /* spare separated char array of tokens */
    char *tokens;
} icarus_tokens;

/* takes a character array of the source code as text
 * returns an icarus_tokens * containing the output from lexing
 *
 * returns 0 on error
 */
icarus_tokens * lex(char *source);

#endif
