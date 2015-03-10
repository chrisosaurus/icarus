#include <stdio.h> /* puts */
#include <stdlib.h> /* calloc */

#include "array.h"

/* allocate a new array of length len
 *
 * returns array on success
 * returns 0 on failure
 */
struct ic_array * ic_array_new(unsigned int len){
    struct ic_array *arr;

    arr = calloc(1, sizeof(struct ic_array));
    if( ! arr ){
        puts("ic_array_new: calloc of struct failed");
        return 0;
    }

    arr->len = len;
    arr->cap = len;

    arr->contents = calloc(len, sizeof(void*));
    if( ! arr->contents ){
        puts("ic_array_new: calloc of contents failed");
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
void * ic_array_get(struct ic_array *arr, unsigned int pos){
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
int ic_array_set(struct ic_array *arr, unsigned int pos, void *val){
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


