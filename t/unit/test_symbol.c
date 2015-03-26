#include <string.h> /* strncmp */
#include <assert.h> /* assert */

#include "../../src/data/symbol.h"

int main(void){
    int i;
    struct ic_symbol *str = ic_symbol_new("hello", 5);

    assert(str);
    assert(ic_symbol_length(str) == 5);

    /* check the contents are what we expect */
    assert( ! strncmp("hello", ic_symbol_contents(str), 5) );

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

    return 0;
}
