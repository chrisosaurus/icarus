#include <assert.h> /* assert */
#include <stdio.h>  /* printf */
#include <string.h> /* strncmp */

#include "../../../../src/parse/data/decl.h"
#include "../../../../src/parse/permissions.h"

int main(void) {
    struct ic_field *field = 0;
    struct ic_decl_union *udecl = 0;
    /* fake indent level */
    unsigned int fake_indent = 0;

    /* test decl_union */
    udecl = ic_decl_union_new("Foo", 3);
    assert(udecl);

    /* check type name is correct */
    assert(!strncmp(ic_symbol_contents(&(udecl->name)), "Foo", 3));

    /* check vector is zero used
     * FIXME expose lenght methods
     */
    assert(udecl->fields.used == 0);

    /* add some fields */
    field = ic_field_new("a", 1, "Sint", 3, ic_parse_perm_default());
    assert(field);
    assert(1 == ic_decl_union_add_field(udecl, field));

    field = ic_field_new("b", 1, "String", 6, ic_parse_perm_default());
    assert(field);
    assert(1 == ic_decl_union_add_field(udecl, field));

    printf("Should see:\ntype Foo\n    a::Sint\n    b::String\nend\n");

    /* output type */
    puts("Output:");
    ic_decl_union_print(stdout, udecl, &fake_indent);

    assert(ic_decl_union_destroy(udecl, 1));

    return 0;
}
