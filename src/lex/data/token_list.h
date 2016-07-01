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
struct ic_token_list *ic_token_list_new(void);

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
struct ic_token *ic_token_list_get(struct ic_token_list *list, unsigned int i);

/* get current length
 *
 * returns length on success
 * returns 0 on failure
 */
unsigned int ic_token_list_length(struct ic_token_list *list);

/* peek at next token
 *
 * will silently return 0 when there are no more tokens
 * returns * on success
 * returns 0 on failure
 */
struct ic_token *ic_token_list_peek(struct ic_token_list *list);

/* peek ahead and see if we are at the end of a line
 * note for this set we exclude all other whitespace
 *
 * if we are here
 *              v
 *  "hello world    \n"
 *
 * then we consider this the end of the line
 *
 * as the remaining characters until eol are not important
 *
 * returns 1 if this is the end of a line
 * returns 0 otherwise
 */
unsigned int ic_token_list_peek_iseol(struct ic_token_list *list);

/* peek at important next token
 * will skip all non-important tokens
 *
 * will silently return 0 when there are no more tokens
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_token *ic_token_list_peek_important(struct ic_token_list *list);

/* peek at next next token
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_token *ic_token_list_peek_ahead(struct ic_token_list *list);

/* peek at next next important token
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_token *ic_token_list_peek_ahead_important(struct ic_token_list *list);

/* consume next token and return
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_token *ic_token_list_next(struct ic_token_list *list);

/* consume up to and including next non-whitespace token and return
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_token *ic_token_list_next_important(struct ic_token_list *list);

/* peek at token, if it is of the type expected then
 * consume and return it
 *
 * otherwise do not consume and return 0
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_token *ic_token_list_expect(struct ic_token_list *list, enum ic_token_id id);

/* peek at token, if it is of the type expected then
 * consume and return it
 *
 * will skip over non-important
 *
 * otherwise do not consume and return 0
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_token *ic_token_list_expect_important(struct ic_token_list *list, enum ic_token_id id);

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

/* print all the tokens stored in this list
 * this will NOT use the token's iterator method
 * so no external side effects occur
 */
void ic_token_list_print(struct ic_token_list *list);

/* debug print all the tokens stored in this list
 * this will NOT use the token's iterator method
 * so no external side effects occur
 */
void ic_token_list_print_debug(struct ic_token_list *list);

#endif
