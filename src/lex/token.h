#ifndef ICARUS_LEX_H
#define ICARUS_LEX_H

#include "../data/pvector.h"

enum ic_token_id {
    IC_NEWLINE,
    IC_END,

    IC_IF,
    IC_ELSE,
    IC_THEN,
    IC_RETURN,
    IC_LET,

    IC_TYPE,
    IC_FUNC,
    IC_IDENTIFIER,

    IC_ARROW,
    IC_ASSIGN,
    IC_EQUAL,
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

struct ic_token_list {
    /* pvector of tokens */
    struct ic_pvector tokens;
};

/* allocate and init a new token list
 *
 * returns 1 on success
 * returns 0 on failure
 */
struct ic_token_list * ic_token_list_new(void);

/* initialise an existing token_list
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_token_list_init(struct ic_token_list *list);

/* destroy token_list
 *
 * will free token_list iff free_list is truthy
 * FIXME decide on tokens
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_token_list_destroy(struct ic_token_list *list, unsigned int free_list);

/* append token at end of token list
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_token_list_append(struct ic_token_list *list, struct ic_token *token);

/* get a token at `i`
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_token * ic_token_list_get(struct ic_token_list *list, unsigned int i);

#endif
