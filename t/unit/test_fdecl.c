#include <string.h> /* strncmp */
#include <assert.h> /* assert */
#include <stdio.h> /* printf */

#include "../../src/parse/data/decl.h"

int main(void){
    struct ic_field *field = 0;
    struct ic_func_decl *fdecl = 0;
    /* fake indent level */
    unsigned int fake_indent = 0;


    /* test func_decl */
    fdecl = ic_func_decl_new("Foo", 3);
    assert(fdecl);

    /* check type name is correct */
    assert( ! strncmp( ic_symbol_contents(&(fdecl->name)), "Foo", 3) );

    /* check vector is zero used
     * FIXME expose length methods
     */
    assert( fdecl->args.used == 0 );


    /* add some fields */
    field = ic_field_new("a", 1, "Int", 3);
    assert(field);
    assert( ic_func_decl_add_arg(fdecl, field) == 0 );

    field = ic_field_new("b", 1, "String", 6);
    assert(field);
    assert( ic_func_decl_add_arg(fdecl, field) == 0 );

    printf("Should see:\nfn Foo(a::Int b::String) -> Void\nend\n");

    /* output type */
    puts("Output:");
    ic_func_decl_print(fdecl, &fake_indent);

    return 0;
}
