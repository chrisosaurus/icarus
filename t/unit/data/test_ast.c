#include <assert.h> /* assert */
#include <stdio.h>  /* printf */
#include <string.h> /* strncmp */

#include "../../../src/parse/data/ast.h"
#include "../../../src/parse/permissions.h"

void basic(void) {
    struct ic_field *field = 0;
    struct ic_decl_type *tdecl = 0;
    struct ic_ast *ast;
    /* fake indent level */
    unsigned int fake_indent = 0;

    /* test decl_type */
    tdecl = ic_decl_type_new("Foo", 3);
    assert(tdecl);

    /* check type name is correct */
    assert(!strncmp(ic_symbol_contents(&(tdecl->name)), "Foo", 3));

    /* check vector is zero used
     * FIXME should expose length methods
     */
    assert(tdecl->fields.used == 0);

    /* add some fields */
    field = ic_field_new("a", 1, "Sint", 3, ic_parse_perm_default());
    assert(field);
    assert(1 == ic_decl_type_add_field(tdecl, field));

    field = ic_field_new("b", 1, "String", 6, ic_parse_perm_default());
    assert(field);
    assert(1 == ic_decl_type_add_field(tdecl, field));

    printf("Should see:\ntype Foo\n    a::Sint\n    b::String\nend\n");

    /* output type */
    ic_decl_type_print(stdout, tdecl, &fake_indent);

    /* test ast itself */
    ast = ic_ast_new();
    assert(ast);

    /* check out initial length */
    assert(ic_ast_length(ast) == 0);

    /* check that out of bounds accesses fail */
    assert(ic_ast_get(ast, 0) == 0);

    /* FIXME once we have working ic_decl we need to test ic_ast
     * more thoroughly
     */

    assert(1 == ic_decl_type_destroy(tdecl, 1));
    assert(1 == ic_ast_destroy(ast, 1));
}

void errors(void) {
    struct ic_ast ast;
    assert(0 == ic_ast_init(0));
    assert(0 == ic_ast_get(0, 0));
    assert(-1 == ic_ast_append(0, 0));
    assert(-1 == ic_ast_append(&ast, 0));
    assert(0 == ic_ast_length(0));
    assert(0 == ic_ast_destroy(0, 0));
}

int main(void) {
    basic();
    errors();

    return 0;
}
