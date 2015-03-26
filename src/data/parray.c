#include <stdio.h> /* puts */
#include <stdlib.h> /* calloc */
#include <string.h> /* memset*/

#include "parray.h"

/* allocate a new array of length len
 *
 * returns array on success
 * returns 0 on failure
 */
struct ic_parray * ic_parray_new(unsigned int len){
    struct ic_parray *arr;

    arr = calloc(1, sizeof(struct ic_parray));
    if( ! arr ){
        puts("ic_parray_new: calloc of struct failed");
        return 0;
    }

    if( ic_parray_init(arr, len) ){
        puts("ic_parray_new: clal to ic_parray_init failed");
        return 0;
    }

    return arr;
}

/* initialise an existing array to the specified len
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_parray_init(struct ic_parray *arr, unsigned int len){
    if( ! arr ){
        puts("ic_parray_init: called with null array");
        return 1;
    }

    /* set length */
    arr->len = len;

    /* allocate contents */
    arr->contents = calloc(len, sizeof(void*));
    if( ! arr->contents ){
        puts("ic_parray_init: calloc of contents failed");
        return 1;
    }

    return 0;
}

/* get item at pos
 * bounds checked
 *
 * returns item on success
 * returns 0 on failure
 */
void * ic_parray_get(struct ic_parray *arr, unsigned int pos){
    if( ! arr ){
        return 0;
    }

    if( pos >= arr->len ){
        /* out of bounds */
        return 0;
    }

    return arr->contents[pos];
}

/* returns 0 on successful set
 * returns 1 on failure
 */
unsigned int ic_parray_set(struct ic_parray *arr, unsigned int pos, void *val){
    if( ! arr ){
        return 1;
    }

    if( pos >= arr->len ){
        /* out of bounds */
        return 1;
    }

    arr->contents[pos] = val;
    return 0;
}

/* ensure array is at least as big as `new_cap`
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_parray_ensure(struct ic_parray *arr, unsigned int new_len){
    if( ! arr ){
        puts("ic_parray_ensure: supplied array was null");
        return 1;
    }

    if( arr->len >= new_len ){
        /* nothing to do */
        return 0;
    }

    /* perform actual resizing
     * note that we only allocated enough room to store a void*
     * so we have to be careful about what we actually store here
     */
    arr->contents = realloc(arr->contents, sizeof(void*) * new_len);
    if( ! arr->contents ){
        puts("ic_parray_ensure: realloc failed");
        return 1;
    }

    /* zero out new bytes */
    memset( &(arr->contents[arr->len]), 0, (new_len - arr->len) );

    arr->len = new_len;
    return 0;
}


