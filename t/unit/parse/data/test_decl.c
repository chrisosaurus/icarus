#include <assert.h> /* assert */
#include <stdio.h>  /* printf */
#include <string.h> /* strncmp */

#include "../../../../src/parse/data/decl.h"
#include "../../../../src/parse/permissions.h"

int main(void) {
    struct ic_field *field = 0;
    struct ic_decl *decl = 0;
    struct ic_decl_type *tdecl = 0;
    struct ic_decl_func *fdecl = 0;
    /* fake indent level */
    unsigned int fake_indent = 0;

    /* test type decl */
    decl = ic_decl_new(ic_decl_tag_type);
    assert(decl);
    /* check type */
    assert(decl->tag == ic_decl_tag_type);

    /* check that trying to pull out the wrong type is an error */
    assert(0 == ic_decl_get_fdecl(decl));

    /* check that getting the right type out is fine */
    tdecl = ic_decl_get_tdecl(decl);
    assert(tdecl);

    /* initialise tdecl */
    assert(1 == ic_decl_type_init(tdecl, "Foo", 3));

    /* add a single field */
    field = ic_field_new("bar", 3, "Baz", 3, ic_parse_perm_default());
    assert(1 == ic_decl_type_add_field(tdecl, field));

    /* test display */
    printf("Expected:\ntype Foo\n    bar::Baz\nend\n");
    /* print it out */
    puts("Output:");
    ic_decl_print(stdout, decl, &fake_indent);

    puts("");

    /* test func decl */
    decl = ic_decl_new(ic_decl_tag_func);
    assert(decl);
    /* check type */
    assert(decl->tag == ic_decl_tag_func);

    /* check that trying to pull out the wrong type is an error */
    assert(0 == ic_decl_get_tdecl(decl));

    /* check that getting the right type out is fine */
    fdecl = ic_decl_get_fdecl(decl);
    assert(fdecl);

    /* initialise fdecl */
    assert(1 == ic_decl_func_init(fdecl, "Foo", 3));

    /* add a single field */
    field = ic_field_new("bar", 3, "Baz", 3, ic_parse_perm_default());
    assert(1 == ic_decl_func_add_arg(fdecl, field));

    /* test display */
    printf("Expected:\n# Foo(Baz)\nfn Foo(bar::Baz) -> Void\nend\n");
    /* print it out */
    puts("Output:");
    ic_decl_print(stdout, decl, &fake_indent);

    return 0;
}
