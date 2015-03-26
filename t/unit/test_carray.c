#include <assert.h> /* assert */

#include "../../src/data/carray.h"

int main(void){
    int i;
    struct ic_carray *arr = ic_carray_new(0);

    assert(arr);
    assert(arr->len == 0);

    /* check that fetching out of range is an error */
    assert(ic_carray_get(arr, 0) == 0);

    /* check that assigning out of range is an error */
    assert(ic_carray_set(arr, 0, 'a') == 1);

    /* resize to hold 10 items, confirm the resize was a success*/
    assert(ic_carray_ensure(arr, 10) == 0);
    assert(arr->len == 10);

    /* check everything within range is settable and gettable */
    for(i=0; i<10; ++i){
        assert(ic_carray_set(arr, i, 'a') == 0);
        assert(ic_carray_get(arr, i) == 'a');
    }

    /* and check that out of range get and set are still errors */
    assert(ic_carray_get(arr, 10) == 0);
    assert(ic_carray_set(arr, 10, 'a') == 1);

    return 0;
}
