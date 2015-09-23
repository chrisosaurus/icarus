#include <stdlib.h> /* calloc, free */
#include <stdio.h> /* puts */

#include "token.h"

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

    if( ! ic_pvector_append(&(list->tokens), token) ){
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

