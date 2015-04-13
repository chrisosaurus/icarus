#include <string.h> /* strncmp */
#include <assert.h> /* assert */
#include <stdio.h> /* puts */

#include "../../src/data/dict.h"

int main(void){
    struct ic_dict *dict = ic_dict_new();

    int data_1 = 1;
    int data_2 = 2;

    puts("ignore any text output, testing error cases");

    assert(dict);

    /* test we cannot pull anything out yet */
    assert( 0 == ic_dict_get(dict, "hello") );

    /* test set doesn't yet work */
    assert( 0 ==  ic_dict_set(dict, "hello", &data_1) );

    /* add something */
    assert( ic_dict_insert(dict, "hello", &data_1) );

    /* play around a bit */
    assert( &data_1 == ic_dict_get(dict, "hello") );
    assert( &data_1 == ic_dict_set(dict, "hello", &data_2) );
    assert( &data_2 == ic_dict_get(dict, "hello") );
    assert( 0 == ic_dict_get(dict, "world") );

    /* test delete */
    assert( &data_2 == ic_dict_delete(dict, "hello") );
    assert( 0 == ic_dict_delete(dict, "hello") );
    assert( 0 == ic_dict_delete(dict, "world") );

    /* finally cleanup */
    assert( ic_dict_destroy(dict, 1, 0) );

    puts("success");

    return 0;
}
