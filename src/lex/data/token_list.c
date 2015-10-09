#include <stdlib.h> /* calloc, free */
#include <stdio.h> /* puts */

#include "token_list.h"

/* allocate and init a new token list
 *
 * returns 1 on success
 * returns 0 on failure
 */
struct ic_token_list * ic_token_list_new(void){
    struct ic_token_list *list = 0;

    list = calloc(1, sizeof(struct ic_token_list));
    if( ! list ){
        puts("ic_token_list_new: call to calloc failed");
        return 0;
    }

    if( ! ic_token_list_init(list) ){
        puts("ic_token_list_new: call to ic_token_list_init failed");
        return 0;
    }

    return list;
}

/* initialise an existing token_list
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_token_list_init(struct ic_token_list *list){
    if( ! list ){
        puts("ic_token_list_init: list was null");
        return 0;
    }

    /* default capacity of 0 to force PVECTOR_DEFAULT_SIZE to be used */
    if( ! ic_pvector_init(&(list->tokens), 0) ){
        puts("ic_token_list_init: call to ic_pvector_init failed");
        return 0;
    }

    list->counter = 0;

    return 1;
}

/* destroy token_list
 *
 * will free token_list iff free_list is truthy
 * FIXME decide on tokens
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_token_list_destroy(struct ic_token_list *list, unsigned int free_list){
    if( ! list ){
        puts("ic_token_list_destroy: list was null");
        return 0;
    }

    /* vec, free_vec, destroy_item
     * do not destroy vector as it is a member
     *
     * FIXME will probably want to destroy tokens
     * FIXME currently leaking tokens
     */
    if( ! ic_pvector_destroy(&(list->tokens), 0, 0) ){
        puts("ic_token_list_destroy: call to ic_pvector_destroy failed");
        return 0;
    }

    if( free_list ){
        free(list);
    }

    return 1;
}

/* append token at end of token list
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_token_list_append(struct ic_token_list *list, struct ic_token *token){
    if( ! list ){
        puts("ic_token_list_append: list was null");
        return 0;
    }

    if( ! token ){
        puts("ic_token_list_append: token was null");
        return 0;
    }

    if( -1 == ic_pvector_append(&(list->tokens), token) ){
        puts("ic_token_list_append: call to ic_pvector_append failed");
        return 0;
    }

    return 1;
}

/* get a token at `i`
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_token * ic_token_list_get(struct ic_token_list *list, unsigned int i){
    struct ic_token *token = 0;

    if( ! list ){
        puts("ic_token_list_get: list was null");
        return 0;
    }

    token = ic_pvector_get(&(list->tokens), i);
    if( ! token ){
        printf("ic_token_list_get: failed to get token '%u'\n", i);
        return 0;
    }

    return token;
}

/* get current length
 *
 * returns length on success
 * returns 0 on failure
 */
unsigned int ic_token_list_length(struct ic_token_list *list){
    if( ! list ){
        puts("ic_token_list_length: list was null");
        return 0;
    }

    return ic_pvector_length(&(list->tokens));
}

/* peek at next token
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_token * ic_token_list_peek(struct ic_token_list *list){
    struct ic_token *token = 0;

    if( ! list ){
        puts("ic_token_list_peek: list was null");
        return 0;
    }

    token = ic_pvector_get(&(list->tokens), list->counter);
    if( ! token ){
        puts("ic_token_list_peek: call to ic_pvector_get failed");
        return 0;
    }

    ++list->counter;

    return token;
}

/* consume next token and return
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_token * ic_token_list_next(struct ic_token_list *list){
    struct ic_token *token = 0;

    if( ! list ){
        puts("ic_token_list_next: list was null");
        return 0;
    }

    token = ic_pvector_get(&(list->tokens), list->counter);
    if( ! token ){
        puts("ic_token_list_next: call to ic_pvector_get failed");
        return 0;
    }

    ++list->counter;

    return token;
}

/* reset token counter, moving back to start
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_token_list_reset(struct ic_token_list *list){
    if( ! list ){
        puts("ic_token_list_counter: list was null");
        return 0;
    }

    list->counter = 0;

    return 1;
}

/* get current value of counter
 * note that the counter is the NEXT token to consider
 * so a value of `0` means we are at the start
 * a value of len means we are finished
 *
 * returns count on success
 * returns 0 on failure
 */
unsigned int ic_token_list_counter(struct ic_token_list *list){
    if( ! list ){
        puts("ic_token_list_counter: list was null");
        return 0;
    }

    return list->counter;
}

/* print all the tokens stored in this list
 * this will NOT use the token's iterator method
 * so no external side effects occur
 */
void ic_token_list_print(struct ic_token_list *list){
    /* current token in list */
    struct ic_token *token = 0;
    /* current offset into token list */
    unsigned int i = 0;
    /* cached len of token_list */
    unsigned int len = 0;

    if( ! list ){
        puts("ic_token_list_print: ERROR list was null");
        return;
    }

    len = ic_token_list_length(list);

    for( i=0; i<len; ++i ){
        token = ic_token_list_get(list, i);

        if( ! token ){
            printf("ic_token_list_print: call to ic_token_list_get for i '%d' failed\n", i);
            return;
        }

        ic_token_print(token);
    }
}

/* debug print all the tokens stored in this list
 * this will NOT use the token's iterator method
 * so no external side effects occur
 */
void ic_token_list_print_debug(struct ic_token_list *list){
    /* current token in list */
    struct ic_token *token = 0;
    /* current offset into token list */
    unsigned int i = 0;
    /* cached len of token_list */
    unsigned int len = 0;

    if( ! list ){
        puts("ic_token_list_print: ERROR list was null");
        return;
    }

    len = ic_token_list_length(list);

    for( i=0; i<len; ++i ){
        token = ic_token_list_get(list, i);

        if( ! token ){
            printf("ic_token_list_print: call to ic_token_list_get for i '%d' failed\n", i);
            return;
        }

        if( i ){
            fputs(" ", stdout);
        }
        ic_token_print_debug(token);
    }
}

