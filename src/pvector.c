#include <stdio.h> /* puts */
#include <stdlib.h> /* calloc */
#include <string.h> /* memset*/

#include "pvector.h"

#define PVECTOR_DEFAULT_GROWTH 8

/* allocate a new pvector of capacity cap
 *
 * returns vector on success
 * returns 0 on failure
 */
struct ic_pvector * ic_pvector_new(unsigned int cap){
    struct ic_pvector *arr;

    arr = calloc(1, sizeof(struct ic_pvector));
    if( ! arr ){
        puts("ic_pvector_new: calloc of struct failed");
        return 0;
    }

    arr->used = 0;
    arr->cap = cap;

    arr->contents = calloc(cap, sizeof(void*));
    if( ! arr->contents ){
        puts("ic_pvector_new: calloc of contents failed");
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
void * ic_pvector_get(struct ic_pvector *arr, unsigned int pos){
    if( ! arr ){
        return 0;
    }

    if( pos >= arr->used ){
        /* out of bounds */
        return 0;
    }

    return arr->contents[pos];
}

/* append a new item to the end of pvector
 * this will grow pvector if needed
 *
 * returns a positive integer representing the new item's index into the vector on success
 * returns -1 on failure
 */
int ic_pvector_append(struct ic_pvector *arr, void *data){
    if( ! arr ){
        puts("ic_pvector_append: supplied array was null");
        return -1;
    }

    /* if we are out of space */
    if( arr->used >= arr->cap ){
        /* resize */
        /* FIXME want to have a smarter growth pattern */
        if( ic_pvector_ensure(arr, arr->cap * 2) ){
            puts("ic_pvector_append: call to ic_pvector_ensure failed");
            return -1;
        }
    }

    /* if after growing we still do not have enough space */
    if( arr->used >= arr->cap ){
        puts("ic_pvector_append: unknown error, failed to grow vector");
        return -1;
    }

    /* store data at current first empty position */
    arr->contents[arr->used] = data;
    /* increment used counter */
    ++arr->used;

    /* return position we stored data in */
    return arr->used - 1 ;
}


/* ensure pvector is at least as big as `new_cap`
* this will not shrink the pvector
*
* returns 0 on success
* return 1 on failure
*/
int ic_pvector_ensure(struct ic_pvector *arr, unsigned int new_cap){
    if( ! arr ){
        puts("ic_pvector_ensure: supplied array was null");
        return 1;
    }

    if( ! new_cap ){
        new_cap = PVECTOR_DEFAULT_GROWTH;
    }

    if( arr->cap >= new_cap ){
        /* nothing to do */
        return 0;
    }

    /* perform actual resizing
     * note that we only allocated enough room to store a void*
     * so we have to be careful about what we actually store here
     */
    arr->contents = realloc(arr->contents, sizeof(void*) * new_cap);
    if( ! arr->contents ){
        puts("ic_pvector_ensure: realloc failed");
        return 1;
    }

    /* zero out new bytes */
    memset( &(arr->contents[arr->cap]), 0, (new_cap - arr->cap) );

    arr->cap = new_cap;
    return 0;
}


