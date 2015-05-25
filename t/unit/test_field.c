#include <string.h> /* strncmp */
#include <assert.h> /* assert */
#include <stdio.h> /* printf */

#include "../../src/parse/data/field.h"

int main(void){
    struct ic_field *field = 0;
    struct ic_field init_me;

    /* test new */
    field = ic_field_new("b", 1, "String", 6);
    assert(field);

    assert( ! strncmp("b",
                ic_symbol_contents(&(field->name)),
                1) );

    assert( ! strncmp("String",
                ic_symbol_contents( ic_type_ref_get_symbol(&(field->type)) ),
                6) );

    /* test init */
    assert( ic_field_init(&init_me, "hello", 5, "Int", 3) == 0 );

    assert( ! strncmp("hello",
                ic_symbol_contents(&(init_me.name)),
                5) );

    assert( ! strncmp("Int",
                ic_symbol_contents( ic_type_ref_get_symbol(&(init_me.type)) ),
                3) );

    /* 'test' printing */
    printf("Should see:\nb::String\n");

    /* output field */
    puts("Output:");
    ic_field_print(field);
    puts("");

    return 0;
}
