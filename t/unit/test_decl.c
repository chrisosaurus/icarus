#include <string.h> /* strncmp */
#include <assert.h> /* assert */
#include <stdio.h> /* printf */

#include "../../src/parse/data/decl.h"

int main(void){
    struct ic_field *field = 0;
    struct ic_decl *decl = 0;
    struct ic_type_decl *tdecl = 0;
    struct ic_func_decl *fdecl = 0;
    /* fake indent level */
    unsigned int fake_indent = 0;


    /* test type decl */
    decl = ic_decl_new(ic_decl_type_decl);
    assert(decl);
    /* check type */
    assert( decl->type == ic_decl_type_decl );

    /* check that trying to pull out the wrong type is an error */
    assert( ic_decl_get_fdecl(decl) == 0 );

    /* check that getting the right type out is fine */
    tdecl = ic_decl_get_tdecl(decl);
    assert(tdecl);

    /* initialise tdecl */
    assert( ic_type_decl_init(tdecl, "Foo", 3) == 0 );

    /* add a single field */
    field = ic_field_new("bar", 3, "Baz", 3);
    assert( ic_type_decl_add_field(tdecl, field) == 0 );

    /* test display */
    printf("Expected:\ntype Foo\n    bar::Baz\nend\n");
    /* print it out */
    puts("Output:");
    ic_decl_print(decl, &fake_indent);


    puts("");


    /* test func decl */
    decl = ic_decl_new(ic_decl_func_decl);
    assert(decl);
    /* check type */
    assert( decl->type == ic_decl_func_decl );

    /* check that trying to pull out the wrong type is an error */
    assert( ic_decl_get_tdecl(decl) == 0 );

    /* check that getting the right type out is fine */
    fdecl = ic_decl_get_fdecl(decl);
    assert(fdecl);

    /* initialise fdecl */
    assert( ic_func_decl_init(fdecl, "Foo", 3) == 0 );

    /* add a single field */
    field = ic_field_new("bar", 3, "Baz", 3);
    assert( ic_func_decl_add_arg(fdecl, field) == 0 );

    /* test display */
    printf("Expected:\n# Foo(Baz)\nfn Foo(bar::Baz) -> Void\nend\n");
    /* print it out */
    puts("Output:");
    ic_decl_print(decl, &fake_indent);


    return 0;
}
