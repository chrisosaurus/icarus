#include <stdio.h> /* fopen, puts */
#include <stdlib.h> /* exit */
#include <string.h> /* strcmp */
#include <assert.h> /* assert */

#include "../../src/pvector.h"

int main(void){
    int i;
    struct ic_pvector *arr = ic_pvector_new(5);

    assert(arr);
    assert(arr->used == 0);
    assert(arr->cap == 5);

    /* check that fetching out of range is an error */
    assert(ic_pvector_get(arr, 0) == 0);

    /* a pvector should grow on append */
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


    /* also test default growth pattern */
    arr = ic_pvector_new(0);

    assert(arr);
    assert(arr->used == 0);
    assert(arr->cap == 0);

    for( i=0; i<8; ++i ){
        assert(ic_pvector_append(arr, arr) == i);
        assert(ic_pvector_get(arr, i) == arr);
    }

    assert(arr->used == 8);
    assert(arr->cap == 8);

    for( i=8; i<12; ++i ){
        assert(ic_pvector_append(arr, arr) == i);
        assert(ic_pvector_get(arr, i) == arr);
    }

    assert(arr->used == 12);
    assert(arr->cap == 16);

    for( i=12; i<18; ++i ){
        assert(ic_pvector_append(arr, arr) == i);
        assert(ic_pvector_get(arr, i) == arr);
    }

    assert(arr->used == 18);
    assert(arr->cap == 32);

    return 0;
}
