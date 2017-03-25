#include <assert.h> /* assert */
#include <stdio.h>  /* printf */
#include <string.h> /* strncmp */

#include "../../../../src/parse/data/decl.h"
#include "../../../../src/parse/permissions.h"

int main(void) {
    struct ic_field *field = 0;
    struct ic_decl_op *op = 0;
    /* fake indent level */
    unsigned int fake_indent = 0;
    char *ch = 0;

    /* test decl_op */
    op = ic_decl_op_new("foo", 3, "barrr", 5);
    assert(op);

    ch = ic_symbol_contents(&(op->from));
    assert(ch);
    assert(0 == strcmp(ch, "foo"));

    ch = ic_symbol_contents(&(op->to));
    assert(ch);
    assert(0 == strcmp(ch, "barrr"));

    printf("Should see:\ntype Foo\n    a::Sint\n    b::String\nend\n");

    /* output type */
    puts("Output:");
    ic_decl_op_print(stdout, op, &fake_indent);

    assert(ic_decl_op_destroy(op, 1));

    return 0;
}
