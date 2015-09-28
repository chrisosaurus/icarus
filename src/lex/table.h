#ifndef ICARUS_TABLE_H
#define ICARUS_TABLE_H

#include "data/token.h"

struct ic_table_entry {
    char *str;
    unsigned int len;
    enum ic_token_id id;
};

/* a map of tokens to their ids */
struct ic_table_entry table[] {
    /* no entry for identifier */

    /* str,         len,    id */
    { "\n",         1,      IC_NEWLINE },
    { "end",        3,      IC_END },

    { "if",         2,      IC_IF },
    { "then",       4,      IC_THEN },
    { "return",     6,      IC_RETURN },
    { "let",        3,      IC_LET },

    { "type",       4,      IC_TYPE },
    { "fn",         2,      IC_FUNC },

    { "->",         2,      IC_ARROW },
    { "==",         2,      IC_EQUAL },
    { "=",          1,      IC_ASSIGN },
    { "::",         2,      IC_DOUBLECOLON },
    { ".",          1,      IC_PERIOD },
    { ",",          1,      IC_COMMA },

    { "(",          1,      IC_LRBRACKET },
    { ")",          1,      IC_RRBRACKET },

    { "",           1,      IC_TOKEN_LEN }
};

#endif
