#include <stdio.h> /* fopen, puts */
#include <stdlib.h> /* exit */
#include <string.h> /* strcmp */
#include <assert.h> /* assert */

#include "../../src/data/pvector.h"

int main(void){
    int i;
    struct ic_pvector *arr = ic_pvector_new(5);

    assert(arr);
    assert(arr->used == 0);
    assert(arr->cap == 5);

    /* check that fetching out of range is an error */
    assert(ic_pvector_get(arr, 0) == 0);

    /* a pvector should grow on append
     * since our starting size is 5 the next growth should be
     * to 10
     */
    for(i=0; i<10; ++i){
        assert(ic_pvector_append(arr, arr) == i);
        assert(ic_pvector_get(arr, i) == arr);
    }

    assert(arr->used == 10);
    assert(arr->cap == 10);

    /* we can also artifically grow a pvector */
    assert(ic_pvector_ensure(arr, 20) == 0);

    /* this should not change used */
    assert(arr->used == 10);
    /* but should increase cap */
    assert(arr->cap == 20);


    /* also test default growth pattern
     * if we do not set an initial size then growth is based on
     * PVECTOR_DEFAULT_GROWTH
     */
    arr = ic_pvector_new(0);

    assert(arr);
    assert(arr->used == 0);
    assert(arr->cap == 8);

    /* push over current 0 capacity
     * this will cause our first resize
     */
    for( i=0; i<8; ++i ){
        assert(ic_pvector_append(arr, arr) == i);
        assert(ic_pvector_get(arr, i) == arr);
    }

    assert(arr->used == 8);
    assert(arr->cap == 8);

    /* push over current 8 capacity */
    for( i=8; i<12; ++i ){
        assert(ic_pvector_append(arr, arr) == i);
        assert(ic_pvector_get(arr, i) == arr);
    }

    assert(arr->used == 12);
    assert(arr->cap == 16);

    /* push over current 16 capacity */
    for( i=12; i<18; ++i ){
        assert(ic_pvector_append(arr, arr) == i);
        assert(ic_pvector_get(arr, i) == arr);
    }

    assert(arr->used == 18);
    assert(arr->cap == 32);

    return 0;
}
