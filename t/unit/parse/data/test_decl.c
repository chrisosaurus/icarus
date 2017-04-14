#include <assert.h> /* assert */
#include <stdio.h>  /* printf */
#include <string.h> /* strncmp */

#include "../../../../src/parse/data/decl.h"
#include "../../../../src/parse/permissions.h"

void test_fdecl(void) {
    struct ic_field *field = 0;
    struct ic_decl *decl = 0;
    struct ic_decl_func *fdecl = 0;
    /* fake indent level */
    unsigned int fake_indent = 0;
    char *ch = 0;

    /* test func decl */
    decl = ic_decl_new(ic_decl_tag_func);
    assert(decl);
    /* check type */
    assert(decl->tag == ic_decl_tag_func);

    /* check that trying to pull out the wrong type is an error */
    assert(0 == ic_decl_get_tdecl(decl));
    assert(0 == ic_decl_get_op(decl));

    /* check that getting the right type out is fine */
    fdecl = ic_decl_get_fdecl(decl);
    assert(fdecl);

    /* initialise fdecl */
    assert(1 == ic_decl_func_init(fdecl, "Foo", 3));

    ch = ic_symbol_contents(&(fdecl->name));
    assert(ch);
    assert(0 == strcmp(ch, "Foo"));

    /* add a single field */
    field = ic_field_new("bar", 3, "Baz", 3, ic_parse_perm_default());
    assert(1 == ic_decl_func_add_arg(fdecl, field));

    assert(ic_decl_destroy(decl, 1));

    puts("Success");
}

void test_tdecl_struct(void) {
    struct ic_field *field = 0;
    struct ic_decl *decl = 0;
    struct ic_decl_type *tdecl = 0;
    /* fake indent level */
    unsigned int fake_indent = 0;
    char *ch = 0;

    /* test type decl */
    decl = ic_decl_new(ic_decl_tag_type);
    assert(decl);
    /* check type */
    assert(decl->tag == ic_decl_tag_type);

    /* check that trying to pull out the wrong type is an error */
    assert(0 == ic_decl_get_fdecl(decl));
    assert(0 == ic_decl_get_op(decl));

    /* check that getting the right type out is fine */
    tdecl = ic_decl_get_tdecl(decl);
    assert(tdecl);

    /* initialise tdecl */
    assert(1 == ic_decl_type_init_struct(tdecl, "Foo", 3));

    ch = ic_symbol_contents(ic_decl_type_get_name(tdecl));
    assert(ch);
    assert(0 == strcmp(ch, "Foo"));

    assert(0 == ic_decl_type_field_length(tdecl));
    /* add a single field */
    field = ic_field_new("bar", 3, "Baz", 3, ic_parse_perm_default());
    assert(1 == ic_decl_type_add_field(tdecl, field));
    assert(1 == ic_decl_type_field_length(tdecl));

    field = ic_decl_type_field_get(tdecl, 0);
    assert(field);

    ch = ic_symbol_contents(&(field->name));
    assert(ch);
    assert(0 == strcmp(ch, "bar"));

    assert(ic_decl_destroy(decl, 1));

    puts("Success");
}

void test_tdecl_union(void) {
    struct ic_field *field = 0;
    struct ic_decl *decl = 0;
    struct ic_decl_type *tdecl = 0;
    /* fake indent level */
    unsigned int fake_indent = 0;
    char *ch = 0;

    /* test type decl */
    decl = ic_decl_new(ic_decl_tag_type);
    assert(decl);
    /* check type */
    assert(decl->tag == ic_decl_tag_type);

    /* check that trying to pull out the wrong type is an error */
    assert(0 == ic_decl_get_fdecl(decl));
    assert(0 == ic_decl_get_op(decl));

    /* check that getting the right type out is fine */
    tdecl = ic_decl_get_tdecl(decl);
    assert(tdecl);

    /* initialise tdecl */
    assert(1 == ic_decl_type_init_union(tdecl, "Foo", 3));

    ch = ic_symbol_contents(ic_decl_type_get_name(tdecl));
    assert(ch);
    assert(0 == strcmp(ch, "Foo"));

    assert(0 == ic_decl_type_field_length(tdecl));
    /* add a single field */
    field = ic_field_new("bar", 3, "Baz", 3, ic_parse_perm_default());
    assert(1 == ic_decl_type_add_field(tdecl, field));
    assert(1 == ic_decl_type_field_length(tdecl));

    field = ic_decl_type_field_get(tdecl, 0);
    assert(field);

    ch = ic_symbol_contents(&(field->name));
    assert(ch);
    assert(0 == strcmp(ch, "bar"));

    assert(ic_decl_destroy(decl, 1));

    puts("Success");
}

void test_op(void) {
    struct ic_field *field = 0;
    struct ic_decl *decl = 0;
    struct ic_decl_op *op = 0;
    /* fake indent level */
    unsigned int fake_indent = 0;
    char *ch = 0;

    /* test type decl */
    decl = ic_decl_new(ic_decl_tag_builtin_op);
    assert(decl);
    /* check type */
    assert(decl->tag == ic_decl_tag_builtin_op);

    /* check that trying to pull out the wrong type is an error */
    assert(0 == ic_decl_get_fdecl(decl));
    assert(0 == ic_decl_get_tdecl(decl));

    /* check that getting the right type out is fine */
    op = ic_decl_get_op(decl);
    assert(op);

    /* initialise tdecl */
    assert(1 == ic_decl_op_init(op, "Foo", 3, "Barr", 4));

    ch = ic_symbol_contents(&(op->from));
    assert(ch);
    assert(0 == strcmp(ch, "Foo"));

    ch = ic_symbol_contents(&(op->to));
    assert(ch);
    assert(0 == strcmp(ch, "Barr"));

    assert(ic_decl_destroy(decl, 1));

    puts("Success");
}

int main(void) {
    test_fdecl();
    test_tdecl_struct();
    test_tdecl_union();
    test_op();

    puts("Success");

    return 0;
}
