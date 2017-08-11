#include <assert.h> /* assert */
#include <stdio.h>  /* printf */
#include <string.h> /* strncmp */

#include "../../../../src/parse/data/field.h"
#include "../../../../src/parse/data/type_ref.h"
#include "../../../../src/parse/permissions.h"

void basic(void) {
    struct ic_field *field = 0;
    struct ic_field init_me;
    struct ic_type_ref *type = 0;

    type = ic_type_ref_symbol_new("String", 6);
    assert(type);

    /* test new */
    field = ic_field_new("b", 1, type, ic_parse_perm_default());
    assert(field);

    assert(!strncmp("b",
                    ic_symbol_contents(&(field->name)),
                    1));

    assert(!strncmp("String",
                    ic_symbol_contents(ic_type_ref_get_symbol(field->type)),
                    6));

    assert(field->permissions == ic_parse_perm_default());

    /* test init */
    type = ic_type_ref_symbol_new("Sint", 4);
    assert(type);

    assert(ic_field_init(&init_me, "hello", 5, type, 2));

    assert(!strncmp("hello",
                    ic_symbol_contents(&(init_me.name)),
                    5));

    assert(!strncmp("Sint",
                    ic_symbol_contents(ic_type_ref_get_symbol(init_me.type)),
                    4));

    /* NB: okay to use int here, as we only set so we can later on assert
     * that it hasn't silently changed under us
     */
    assert(init_me.permissions == 2);

    /* 'test' printing */
    printf("Should see:\nb::String\n");

    /* output field */
    puts("Output:");
    ic_field_print(stdout, field);
    puts("");
}

int main(void) {
    basic();

    return 0;
}
