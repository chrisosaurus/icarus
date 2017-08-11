#include <assert.h> /* assert */
#include <stdio.h>  /* puts */
#include <string.h> /* strncmp */

#include "../../../src/data/symbol.h"

void normal(void) {
    int i;
    struct ic_symbol *str = ic_symbol_new("hello", 5);

    puts("ignore any text output, testing error cases");

    assert(str);
    assert(ic_symbol_length(str) == 5);

    /* check the contents are what we expect */
    assert(!strncmp("hello", ic_symbol_contents(str), 5));

    /* test the get interface */
    assert(ic_symbol_get(str, 0) == 'h');
    assert(ic_symbol_get(str, 1) == 'e');
    assert(ic_symbol_get(str, 2) == 'l');
    assert(ic_symbol_get(str, 3) == 'l');
    assert(ic_symbol_get(str, 4) == 'o');

    /* check for out of bounds access */
    assert(ic_symbol_get(str, 5) == 0);
    assert(ic_symbol_get(str, 6) == 0);
    assert(ic_symbol_get(str, 7) == 0);

    /* cannot test output */
    ic_symbol_print(stdout, str);

    assert(1 == ic_symbol_destroy(str, 1));
}

void abnormal(void) {
    struct ic_symbol *sym = 0;

    /* test null symbol cases */
    assert(0 == ic_symbol_init(0, 0, 0));
    assert(0 == ic_symbol_contents(0));
    assert(0 == ic_symbol_get(0, 0));
    assert(-1 == ic_symbol_length(0));
    assert(0 == ic_symbol_destroy(0, 0));
    assert(0 == ic_symbol_equal(0, 0));
    assert(0 == ic_symbol_equal(sym, 0));
    assert(0 == ic_symbol_equal_str(0, 0));
    assert(0 == ic_symbol_equal_str(sym, 0));
    assert(0 == ic_symbol_equal_char(0, 0));
    assert(0 == ic_symbol_equal_char(sym, 0));

    /* cannot test output */
    ic_symbol_print(stdout, 0);
}

void equality(void) {
    struct ic_symbol *sym1 = 0;
    struct ic_symbol *sym2 = 0;

    sym1 = ic_symbol_new("Hello world", 11);
    assert(sym1);

    sym2 = ic_symbol_new("Hello world", 11);
    assert(sym2);

    assert(1 == ic_symbol_equal(sym1, sym2));
    assert(1 == ic_symbol_equal_char(sym1, "Hello world"));
    assert(0 == ic_symbol_equal_char(sym1, "Hello world."));

    assert(1 == ic_symbol_destroy(sym1, 1));
    assert(1 == ic_symbol_destroy(sym2, 1));
}

void deep_copy(void) {
    struct ic_symbol *sym1 = 0;
    struct ic_symbol *sym2 = 0;

    sym1 = ic_symbol_new("Hello world", 11);
    assert(sym1);

    sym2 = ic_symbol_deep_copy(sym1);
    assert(sym1);

    assert(sym1 != sym2);
    assert(sym1->internal.used == sym2->internal.used);
    assert(sym1->internal.backing.len == sym2->internal.backing.len);
    assert(sym1->internal.backing.contents != sym2->internal.backing.contents);
    assert(0 == strcmp(sym1->internal.backing.contents, sym2->internal.backing.contents));

    sym1->internal.backing.contents[0] = 'Q';
    assert(sym1->internal.backing.contents[0] == 'Q');
    assert(sym2->internal.backing.contents[0] == 'H');

    assert(1 == ic_symbol_destroy(sym1, 1));
    assert(1 == ic_symbol_destroy(sym2, 1));
}

int main(void) {
    normal();
    equality();
    deep_copy();
    abnormal();

    return 0;
}
