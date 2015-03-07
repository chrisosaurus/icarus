#ifndef ICARUS_LEXER_H
#define ICARUS_LEXER_H

typedef struct icarus_token {
    int len;
    char string[];
} icarus_token;

icarus_token * lex(char *filename);

#endif
