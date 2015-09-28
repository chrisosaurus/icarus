#ifndef ICARUS_TOKEN_LIST_H
#define ICARUS_TOKEN_LIST_H

#include "../../data/pvector.h"
#include "token.h"

struct ic_token_list {
    /* pvector of tokens */
    struct ic_pvector tokens;
    /* ic_token_list will support an iterator interface
     * this is the next token to look at
     * (0 = no tokens read yet)
     * we stop once current == len
     */
    unsigned int counter;
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

/* get current length
 *
 * returns length on success
 * returns 0 on failure
 */
unsigned int ic_token_list_length(struct ic_token_list *list);

/* peek at next token
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_token * ic_token_list_peek(struct ic_token_list *list);

/* consume next token and return
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_token * ic_token_list_next(struct ic_token_list *list);

/* reset token counter, moving back to start
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_token_list_reset(struct ic_token_list *list);

/* get current value of counter
 * note that the counter is the NEXT token to consider
 * so a value of `0` means we are at the start
 * a value of len means we are finished
 *
 * returns count on success
 * returns 0 on failure
 */
unsigned int ic_token_list_counter(struct ic_token_list *list);

#endif
