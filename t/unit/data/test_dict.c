#include <assert.h> /* assert */
#include <stdio.h>  /* puts */
#include <string.h> /* strncmp */

#include "../../../src/data/dict.h"

void normal(void) {
    struct ic_dict *dict = ic_dict_new();

    int data_1 = 1;
    int data_2 = 2;

    puts("ignore any text output, testing error cases");

    assert(dict);

    /* test we cannot pull anything out yet */
    assert(0 == ic_dict_get(dict, "hello"));

    /* test set doesn't yet work */
    assert(0 == ic_dict_set(dict, "hello", &data_1));

    /* add something */
    assert(ic_dict_insert(dict, "hello", &data_1));

    /* check it exists */
    assert(ic_dict_exists(dict, "hello"));

    /* check something doesn't exist */
    assert(0 == ic_dict_exists(dict, "world"));

    /* play around a bit */
    assert(&data_1 == ic_dict_get(dict, "hello"));
    assert(&data_1 == ic_dict_set(dict, "hello", &data_2));
    assert(&data_2 == ic_dict_get(dict, "hello"));
    assert(0 == ic_dict_get(dict, "world"));

    /* test delete */
    assert(&data_2 == ic_dict_delete(dict, "hello"));
    assert(0 == ic_dict_delete(dict, "hello"));
    assert(0 == ic_dict_delete(dict, "world"));

    /* finally cleanup */
    assert(ic_dict_destroy(dict, 1, 0));

    puts("success");
}

void abnormal(void) {
    /* check all the dict null cases */
    assert(0 == ic_dict_init(0));
    assert(0 == ic_dict_get(0, 0));
    assert(0 == ic_dict_set(0, 0, 0));
    assert(0 == ic_dict_insert(0, 0, 0));
    assert(0 == ic_dict_exists(0, 0));
    assert(0 == ic_dict_delete(0, 0));
    assert(0 == ic_dict_destroy(0, 0, 0));
}

int main(void) {
    normal();
    abnormal();

    return 0;
}
