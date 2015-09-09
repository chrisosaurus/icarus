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

    if( ! ic_parray_init(arr, len) ){
        puts("ic_parray_new: clal to ic_parray_init failed");
        free(arr);
        return 0;
    }

    return arr;
}

/* initialise an existing array to the specified len
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_parray_init(struct ic_parray *arr, unsigned int len){
    if( ! arr ){
        puts("ic_parray_init: called with null array");
        return 0;
    }

    /* set length */
    arr->len = len;

    /* allocate contents */
    arr->contents = calloc(len, sizeof(void*));
    if( ! arr->contents ){
        puts("ic_parray_init: calloc of contents failed");
        return 0;
    }

    return 1;
}

/* destroy this parray
 * will only free parray itself if free_parr is truthy
 *
 * this does NOT free the items in the parray, as parray does not store length,
 * this is the user's responsibility
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_parray_destroy(struct ic_parray *arr, unsigned int free_parr){
    if( ! arr ){
        puts("ic_parray_destroy: arr was null");
        return 0;
    }

    if( arr->contents ){
        free(arr->contents);
        arr->contents = 0;
    }

    if( free_parr ){
        free(arr);
    }

    return 1;
}

/* get item at pos
 *
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

/* set element at [pos] to val
 *
 * bounds checked
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_parray_set(struct ic_parray *arr, unsigned int pos, void *val){
    if( ! arr ){
        return 0;
    }

    if( pos >= arr->len ){
        /* out of bounds */
        return 0;
    }

    arr->contents[pos] = val;
    return 1;
}

/* ensure array is at least as big as `new_cap`
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_parray_ensure(struct ic_parray *arr, unsigned int new_len){
    if( ! arr ){
        puts("ic_parray_ensure: supplied array was null");
        return 0;
    }

    if( arr->len >= new_len ){
        /* nothing to do */
        return 1;
    }

    /* perform actual resizing
     * note that we only allocated enough room to store a void*
     * so we have to be careful about what we actually store here
     */
    arr->contents = realloc(arr->contents, sizeof(void*) * new_len);
    if( ! arr->contents ){
        puts("ic_parray_ensure: realloc failed");
        return 0;
    }

    /* zero out new bytes */
    memset( &(arr->contents[arr->len]), 0, (new_len - arr->len) );

    arr->len = new_len;
    return 1;
}


