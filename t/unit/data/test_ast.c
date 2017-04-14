#include <assert.h> /* assert */
#include <stdio.h>  /* printf */
#include <string.h> /* strncmp */

#include "../../../src/parse/data/ast.h"
#include "../../../src/parse/permissions.h"

void basic(void) {
    struct ic_ast *ast;

    struct ic_decl *decl_1 = 0;
    struct ic_decl *decl_2 = 0;

    decl_1 = ic_decl_new(ic_decl_tag_func);
    assert(decl_1);

    decl_2 = ic_decl_new(ic_decl_tag_type);
    assert(decl_2);

    ast = ic_ast_new();
    assert(ast);

    /* check out initial length */
    assert(ic_ast_length(ast) == 0);

    /* check that out of bounds accesses fail */
    assert(ic_ast_get(ast, 0) == 0);

    /* append returns index of new item */
    assert(0 == ic_ast_append(ast, decl_1));
    assert(ic_ast_length(ast) == 1);
    assert(decl_1 == ic_ast_get(ast, 0));

    /* append returns index of new item */
    assert(1 == ic_ast_append(ast, decl_2));
    assert(ic_ast_length(ast) == 2);
    assert(decl_2 == ic_ast_get(ast, 1));

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
