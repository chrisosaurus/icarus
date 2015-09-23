#ifndef ICARUS_TOKEN_H
#define ICARUS_TOKEN_H

#include "../../data/pvector.h"

enum ic_token_id {
    IC_IDENTIFIER, /* no entry in table[] */

    IC_NEWLINE,
    IC_END,

    IC_IF,
    IC_ELSE,
    IC_THEN,
    IC_RETURN,
    IC_LET,

    IC_TYPE,
    IC_FUNC,

    IC_ARROW,
    IC_EQUAL, /* == */
    IC_ASSIGN, /* = */
    IC_DOUBLECOLON, /* :: double colon */
    IC_PERIOD,
    IC_COMMA,

    IC_LRBRACKET, /* ( left round bracket */
    IC_RRBRACKET, /* ) right round backet */

    IC_TOKEN_LEN /* final token */
};

struct ic_token {
    /* this token type */
    enum ic_token_id id;

    /* location of token in source */
    /* source line */
    char *line;
    /* offset into line */
    unsigned int offset;
    /* filename */
    char *file;
    /* line number in file */
    unsigned int line_num;

    /* token data */
    union {
        struct {
            char *string;
            unsigned int len;
        } str;
        int integer;
        float floating;
    } u;
};

/* alloc and init a new token
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_token * ic_token_new(void);

/* init an existing token
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_token_init(struct ic_token *token);

/* destroy
 *
 * will only free token if free_token is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_token_destroy(struct ic_token *token, unsigned int free_token);

#endif
