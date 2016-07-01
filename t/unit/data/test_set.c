#include <assert.h> /* assert */
#include <stdio.h>  /* puts */
#include <string.h> /* strncmp */

#include "../../../src/data/set.h"

void normal(void) {
    struct ic_set *set = ic_set_new();

    puts("ignore any text output, testing error cases");

    assert(set);

    /* test we cannot pull anything out yet */
    assert(0 == ic_set_exists(set, "hello"));

    /* add something */
    assert(ic_set_insert(set, "hello"));

    /* check it exists */
    assert(ic_set_exists(set, "hello"));

    /* check something doesn't exist */
    assert(0 == ic_set_exists(set, "world"));

    /* test delete */
    assert(ic_set_delete(set, "hello"));
    assert(0 == ic_set_delete(set, "hello"));
    assert(0 == ic_set_delete(set, "world"));

    /* finally cleanup */
    assert(ic_set_destroy(set, 1));

    puts("success");
}

void abnormal(void) {
    /* check all the set null cases */
    assert(0 == ic_set_init(0));
    assert(0 == ic_set_insert(0, 0));
    assert(0 == ic_set_exists(0, 0));
    assert(0 == ic_set_delete(0, 0));
    assert(0 == ic_set_destroy(0, 0));
}

int main(void) {
    normal();
    abnormal();

    return 0;
}
