#ifndef ICARUS_LEXER_H
#define ICARUS_LEXER_H

struct ic_tokens {
    /* length of char *tokens used */
    unsigned int len;
    /* length of char *tokens allocated */
    unsigned int cap;
    /* spare separated char array of tokens */
    char *tokens;
};

/* takes a character array of the source code as text
 * returns an icarus_tokens * containing the output from lexing
 *
 * returns 0 on failure
 */
struct ic_tokens * ic_lex(char *source);

#endif
