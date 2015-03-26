#include <string.h> /* strncmp */
#include <assert.h> /* assert */
#include <stdio.h> /* printf */

#include "../../src/parse/data/ast.h"

int main(void){
    struct ic_field *field = 0;
    struct ic_type_decl *tdecl = 0;
    struct ic_ast *ast;
    /* fake indent level */
    unsigned int fake_indent = 0;


    /* test type_decl */
    tdecl = ic_type_decl_new("Foo", 3);
    assert(tdecl);

    /* check type name is correct */
    assert( ! strncmp( ic_symbol_contents(&(tdecl->name)), "Foo", 3) );

    /* check vector is zero used
     * FIXME should expose length methods
     */
    assert( tdecl->fields.used == 0 );


    /* add some fields */
    field = ic_field_new("a", 1, "Int", 3);
    assert(field);
    assert( ic_type_decl_add_field(tdecl, field) == 0 );

    field = ic_field_new("b", 1, "String", 6);
    assert(field);
    assert( ic_type_decl_add_field(tdecl, field) == 0 );

    printf("Should see:\ntype Foo\n  a::Int\n  b::String\nend\n");

    /* output type */
    ic_type_decl_print(tdecl, &fake_indent);


    /* test ast itself */
    ast = ic_ast_new();
    assert(ast);

    /* check out initial length */
    assert( ic_ast_length(ast) == 0 );

    /* check that out of bounds accesses fail */
    assert( ic_ast_get(ast, 0) == 0 );

    /* FIXME once we have working ic_decl we need to test ic_ast
     * more thoroughly
     */


    return 0;
}
