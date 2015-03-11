#include <stdio.h> /* puts */
#include <stdlib.h> /* calloc */
#include <string.h> /* memset*/

#include "carray.h"

/* allocate a new array of length len
 *
 * returns array on success
 * returns 0 on failure
 */
struct ic_carray * ic_carray_new(unsigned int len){
    struct ic_carray *arr;

    arr = calloc(1, sizeof(struct ic_carray));
    if( ! arr ){
        puts("ic_carray_new: calloc of struct failed");
        return 0;
    }

    arr->len = len;

    arr->contents = calloc(len, sizeof(void*));
    if( ! arr->contents ){
        puts("ic_carray_new: calloc of contents failed");
        return 0;
    }

    return arr;
}

/* get item at pos
 * bounds checked
 *
 * returns item on success
 * return 0 on failure
 */
char ic_carray_get(struct ic_carray *arr, unsigned int pos){
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
int ic_carray_set(struct ic_carray *arr, unsigned int pos, char val){
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
 * return 1 on failure
 */
int ic_carray_ensure(struct ic_carray *arr, unsigned int new_len){
    if( ! arr ){
        puts("ic_carray_ensure: supplied array was null");
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
        puts("ic_carray_ensure: realloc failed");
        return 1;
    }

    /* zero out new bytes */
    memset( &(arr->contents[arr->len]), 0, (new_len - arr->len) );

    arr->len = new_len;
    return 0;
}


