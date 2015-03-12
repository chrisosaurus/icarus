#include <string.h> /* strncmp */
#include <assert.h> /* assert */
#include <stdio.h> /* printf */

#include "../../src/ast.h"

int main(void){
    struct ic_field *field = 0;
    struct ic_type_decl *tdecl = ic_type_decl_new("Foo", 3);

    assert(tdecl);

    /* check type name is correct */
    assert( ! strncmp( ic_symbol_contents(&(tdecl->name)), "Foo", 3) );

    /* check vector is zero used and cap */
    assert( tdecl->fields.used == 0 );
    assert( tdecl->fields.cap == 0 );

    /* add some fields */
    field = ic_field_new("a", 1, "Int", 3);
    assert( ic_type_decl_add_field(tdecl, field) == 0 );

    field = ic_field_new("b", 1, "String", 6);
    assert( ic_type_decl_add_field(tdecl, field) == 0 );

    printf("Should see:\ntype Foo\n\ta::Int\n\tb::String\nend\n");

    /* output type */
    ic_type_decl_print(tdecl);

    return 0;
}
