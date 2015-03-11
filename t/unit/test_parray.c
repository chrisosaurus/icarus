#include <stdio.h> /* fopen, puts */
#include <stdlib.h> /* exit */
#include <string.h> /* strcmp */
#include <assert.h> /* assert */

#include "../../src/parray.h"

int main(void){
    int i;
    struct ic_parray *arr = ic_parray_new(0);

    assert(arr);
    assert(arr->len == 0);

    /* check that fetching out of range is an error */
    assert(ic_parray_get(arr, 0) == 0);

    /* check that assigning out of range is an error */
    assert(ic_parray_set(arr, 0, arr) == 1);

    /* resize to hold 10 items, confirm the resize was a success*/
    assert(ic_parray_ensure(arr, 10) == 0);
    assert(arr->len == 10);

    /* check everything within range is settable and gettable */
    for(i=0; i<10; ++i){
        assert(ic_parray_set(arr, i, arr) == 0);
        assert(ic_parray_get(arr, i) == arr);
    }

    /* and check that out of range get and set are still errors */
    assert(ic_parray_get(arr, 10) == 0);
    assert(ic_parray_set(arr, 10, arr) == 1);

    return 0;
}
