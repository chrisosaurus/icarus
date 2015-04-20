#include <assert.h> /* assert */

#include "../../src/data/parray.h"

void normal(void){
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

    /* ensure to smaller or same size should make no difference */
    assert(ic_parray_ensure(arr, 10) == 0);
    assert(arr->len == 10);
    assert(ic_parray_ensure(arr, 5) == 0);
    assert(arr->len == 10);

    /* check everything within range is settable and gettable */
    for(i=0; i<10; ++i){
        assert(ic_parray_set(arr, i, arr) == 0);
        assert(ic_parray_get(arr, i) == arr);
    }

    /* and check that out of range get and set are still errors */
    assert(ic_parray_get(arr, 10) == 0);
    assert(ic_parray_set(arr, 10, arr) == 1);
}

void abnormal(void){
    /* test null parrays */
    assert( 1 == ic_parray_init(0, 0) );
    assert( 0 == ic_parray_get(0, 0) );
    assert( 1 == ic_parray_set(0, 0, 0) );
    assert( 1 == ic_parray_ensure(0, 0) );
}

int main(void){
    normal();
    abnormal();

    return 0;
}
