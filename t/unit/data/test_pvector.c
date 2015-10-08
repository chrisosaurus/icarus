#include <stdio.h> /* fopen, puts */
#include <stdlib.h> /* exit */
#include <string.h> /* strcmp */
#include <assert.h> /* assert */

#include "../../../src/data/pvector.h"
#include "../../../src/data/string.h"

void normal(void){
    int i;
    struct ic_pvector *arr = ic_pvector_new(5);
    struct ic_pvector *arr2 = 0;

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
    assert(1 == ic_pvector_ensure(arr, 20));

    /* this should not change used */
    assert(arr->used == 10);
    /* but should increase cap */
    assert(arr->cap == 20);


    /* also test default growth pattern
     * if we do not set an initial size then growth is based on
     * PVECTOR_DEFAULT_SIZE
     */
    arr = ic_pvector_new(0);

    assert(arr);
    assert(arr->used == 0);
    assert(arr->cap == 8);

    /* ensure to same or less size should do nothing */
    assert( 1 == ic_pvector_ensure(arr, 8) );
    assert(arr->used == 0);
    assert(arr->cap == 8);
    assert( 1 == ic_pvector_ensure(arr, 4) );
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


    /* force a small size */
    arr2 = ic_pvector_new(1);
    assert(arr2->used == 0);
    assert(arr2->cap == 1);

    /* here the ensure of 0 will default
     * to an ensure of PVECTOR_DEFAULT_SIZE
     */
    assert( 1 == ic_pvector_ensure(arr2, 0) );
    assert(arr2->used == 0);
    /* PVECTOR_DEFAULT_SIZE is 8 */
    assert(arr2->cap == 8);


}

void abnormal(void){
    struct ic_pvector *vec = ic_pvector_new(0);

    /* test null pvector args */
    assert( 0 == ic_pvector_init(0, 0) );
    assert( 0 == ic_pvector_get(0, 0) );
    assert( -1 == ic_pvector_append(0, 0) );
    assert( 0 == ic_pvector_ensure(0, 0) );
    assert( 0 == ic_pvector_length(0) );
    assert( 0 == ic_pvector_destroy(0, 0, 0) );

    /* actually destroy to clean up
     * nothing stored so a simple free
     * is sufficient
     */
    free(vec);
}

/* this is an example of why the c spec sucks */
unsigned int string_shim(void *str, unsigned int free){
    return ic_string_destroy(str, free);
}

void destroy(void){
    struct ic_pvector *vec = ic_pvector_new(0);

    /* insert some strings */
    assert( -1 != ic_pvector_append(vec, ic_string_new("hello", 5)) );
    assert( -1 != ic_pvector_append(vec, ic_string_new("world", 5)) );
    assert( -1 != ic_pvector_append(vec, ic_string_new("a", 1)) );
    assert( -1 != ic_pvector_append(vec, ic_string_new("dude", 4)) );

    assert( 1 == ic_pvector_destroy(vec, 1, string_shim) );
}

int main(void){
    normal();
    abnormal();
    destroy();

    return 0;
}
