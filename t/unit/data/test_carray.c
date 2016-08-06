#include <assert.h> /* assert */
#include <stdio.h>

#include "../../../src/data/carray.h"

void normal(void) {
    int i;
    struct ic_carray *arr = ic_carray_new(0);

    assert(arr);
    assert(arr->len == 0);

    /* check that fetching out of range is an error */
    assert(ic_carray_get(arr, 0) == 0);

    /* check that assigning out of range is an error */
    assert(ic_carray_set(arr, 0, 'a') == 0);

    /* resize to hold 10 items, confirm the resize was a success*/
    assert(ic_carray_ensure(arr, 10) == 1);
    assert(arr->len == 10);

    /* try to resize to a size that isn't bigger, will do nothing */
    assert(ic_carray_ensure(arr, 10) == 1);
    assert(arr->len == 10);

    /* same with smaller size */
    assert(ic_carray_ensure(arr, 4) == 1);
    assert(arr->len == 10);

    /* check everything within range is settable and gettable */
    for (i = 0; i < 10; ++i) {
        assert(ic_carray_set(arr, i, 'a') == 1);
        assert(ic_carray_get(arr, i) == 'a');
    }

    /* and check that out of range get and set are still errors */
    assert(ic_carray_get(arr, 10) == 0);
    assert(ic_carray_set(arr, 10, 'a') == 0);

    assert(1 == ic_carray_destroy(arr, 1));
}

void abnormal(void) {
    /* test null carry args */
    assert(0 == ic_carray_init(0, 0));
    assert(0 == ic_carray_get(0, 0));
    assert(0 == ic_carray_set(0, 0, 0));
    assert(0 == ic_carray_ensure(0, 0));
    assert(0 == ic_carray_destroy(0, 0));
}

int main(void) {
    normal();
    abnormal();

    puts("test_carray success");

    return 0;
}
