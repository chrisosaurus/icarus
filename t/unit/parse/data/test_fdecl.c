#include <string.h> /* strncmp */
#include <assert.h> /* assert */
#include <stdio.h> /* printf */

#include "../../../../src/parse/data/decl.h"

int main(void){
    struct ic_field *field = 0;
    struct ic_decl_func *fdecl = 0;
    /* fake indent level */
    unsigned int fake_indent = 0;


    /* test decl_func */
    fdecl = ic_decl_func_new("Foo", 3);
    assert(fdecl);

    /* check type name is correct */
    assert( ! strncmp( ic_symbol_contents(&(fdecl->name)), "Foo", 3) );

    /* check vector is zero used
     * FIXME expose length methods
     */
    assert( fdecl->args.used == 0 );


    /* add some fields */
    field = ic_field_new("a", 1, "Int", 3, 0);
    assert(field);
    assert( 1 == ic_decl_func_add_arg(fdecl, field) );

    field = ic_field_new("b", 1, "String", 6, 0);
    assert(field);
    assert( 1 == ic_decl_func_add_arg(fdecl, field) );

    printf("Should see:\n# Foo(int String)\nfn Foo(a::Int b::String) -> Void\nend\n");

    /* output type */
    puts("Output:");
    ic_decl_func_print(fdecl, &fake_indent);

    return 0;
}
