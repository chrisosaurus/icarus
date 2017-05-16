#include <stdio.h>  /* puts */
#include <stdlib.h> /* calloc, free */
#include <string.h> /* memset*/

#include "pvector.h"

#define PVECTOR_DEFAULT_SIZE 8

/* allocate a new pvector of capacity cap
 *
 * returns vector on success
 * returns 0 on failure
 */
struct ic_pvector *ic_pvector_new(unsigned int cap) {
    struct ic_pvector *arr;

    arr = calloc(1, sizeof(struct ic_pvector));
    if (!arr) {
        puts("ic_pvector_new: calloc of struct failed");
        return 0;
    }

    if (!ic_pvector_init(arr, cap)) {
        puts("ic_pvector_new: call to ic_pvector_init failed");
        free(arr);
        return 0;
    }

    return arr;
}

/* initialise existing pvector to specified cap
 *
 * will use a default cap of PVECTOR_DEFAULT_SIZE is no cap is supplied
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_pvector_init(struct ic_pvector *vec, unsigned int cap) {
    if (!vec) {
        puts("ic_pvector_init: vec was null");
        return 0;
    }

    /* default to PVECTOR_DEFAULT_SIZE if no cap is specified
     */
    if (!cap) {
        cap = PVECTOR_DEFAULT_SIZE;
    }

    vec->used = 0;
    vec->cap = cap;

    vec->contents = calloc(cap, sizeof(void *));
    if (!vec->contents) {
        puts("ic_pvector_init: calloc of contents failed");
        return 0;
    }

    return 1;
}

/* destroy pvector
 *
 * this will only free the pvecto if `free_pvector` is true
 *
 * takes an optional function which is called once for each argument stored in the pvector
 * it will be called with it's free argument set to true
 * this function is expected to return 0 on failure
 * FIXME consider allowing free_data param to ic_pvector_destroy that is passed to (*destroy_item) rather than defaulting to true
 *
 * this function will bail at the first error encountered
 *
 * note that this function signature prevents storing a pvector/parray inside a pvector
 * without the use of a shim as the destroy_item signature doesn't match ic_pvector_destroy
 *
 * sadly this does require a shim for each type as the following function pointers are not
 * compatible:
 *
 *  unsigned int (*destroy_item)(void *item, unsigned int free_item);
 *  unsigned int (*destroy_item)(struct foo *item, unsigned int free_item);
 *
 * so a simple shim is needed to convert between
 *  unsigned int shim(void *item, unsigned int free){
 *      return foo(item, free);
 *  }
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_pvector_destroy(struct ic_pvector *vec, unsigned int free_vec, unsigned int (*destroy_item)(void *item, unsigned int free)) {
    /* offset into pvector */
    unsigned int i = 0;
    /* cache of length */
    unsigned int len = 0;
    /* current item in vec */
    void *item = 0;

    if (!vec) {
        puts("ic_pvector_destroy: vec was null");
        return 0;
    }

    len = ic_pvector_length(vec);

    /* only iterate through elements if we have a destroy_item function */
    if (destroy_item) {
        /* iterate through each item calling the supplied
         * destroy_item function
         *
         * bail early at first sign of error
         */
        for (i = 0; i < len; ++i) {
            item = ic_pvector_get(vec, i);
            if (!item) {
                puts("ic_pvector_destroy: call to ic_pvector_get failed");
                return 0;
            }

            if (!destroy_item(item, 1)) {
                puts("ic_pvector_destroy: call to user provided destroy func failed");
                return 0;
            }
        }
    }

    /* if asked nicely */
    if (free_vec) {
        free(vec);
    }

    return 1;
}

/* simple default destroyer
 * calls free on every item
 *
 * cannot fail
 *
 * returns 1 on success
 */
unsigned int ic_pvector_destroyer_free(void *item, unsigned int free_item) {
    if (free_item) {
        free(item);
    }

    return 1;
}

/* get item at pos
 * bounds checked
 *
 * returns item on success
 * returns 0 on failure
 */
void *ic_pvector_get(struct ic_pvector *arr, unsigned int pos) {
    if (!arr) {
        return 0;
    }

    if (pos >= arr->used) {
        /* out of bounds */
        return 0;
    }

    return arr->contents[pos];
}

/* append a new item to the end of pvector
 * this will grow pvector if needed
 *
 * returns an integer representing the new item's index into the vector on success
 * returns -1 on failure
 */
int ic_pvector_append(struct ic_pvector *arr, void *data) {
    if (!arr) {
        puts("ic_pvector_append: supplied array was null");
        return -1;
    }

    /* if we are out of space */
    if (arr->used >= arr->cap) {
        /* resize */
        /* FIXME want to have a smarter growth pattern */
        if (!ic_pvector_ensure(arr, arr->cap * 2)) {
            puts("ic_pvector_append: call to ic_pvector_ensure failed");
            return -1;
        }
    }

    /* if after growing we still do not have enough space */
    if (arr->used >= arr->cap) {
        puts("ic_pvector_append: unknown error, failed to grow vector");
        return -1;
    }

    /* store data at current first empty position */
    arr->contents[arr->used] = data;
    /* increment used counter */
    ++arr->used;

    /* return position we stored data in */
    return arr->used - 1;
}

/* ensure pvector is at least as big as `new_cap`
 * this will not shrink the pvector
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_pvector_ensure(struct ic_pvector *arr, unsigned int new_cap) {
    if (!arr) {
        puts("ic_pvector_ensure: supplied array was null");
        return 0;
    }

    if (!new_cap) {
        new_cap = PVECTOR_DEFAULT_SIZE;
    }

    if (arr->cap >= new_cap) {
        /* nothing to do */
        return 1;
    }

    /* perform actual resizing
     * note that we only allocated enough room to store a void*
     * so we have to be careful about what we actually store here
     */
    arr->contents = realloc(arr->contents, sizeof(void *) * new_cap);
    if (!arr->contents) {
        puts("ic_pvector_ensure: realloc failed");
        return 0;
    }

    /* zero out new bytes */
    memset(&(arr->contents[arr->cap]), 0, (new_cap - arr->cap));

    arr->cap = new_cap;
    return 1;
}

/* returns the length of the used section of the vector
 *
 * returns 0 on failure
 */
unsigned int ic_pvector_length(struct ic_pvector *arr) {
    if (!arr) {
        puts("ic_pvector_length: arr was null");
        return 0;
    }

    return arr->used;
}
