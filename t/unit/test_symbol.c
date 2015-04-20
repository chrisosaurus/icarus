#include <string.h> /* strncmp */
#include <assert.h> /* assert */
#include <stdio.h> /* puts */

#include "../../src/data/symbol.h"

void normal(void){
    int i;
    struct ic_symbol *str = ic_symbol_new("hello", 5);

    puts("ignore any text output, testing error cases");

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

    /* cannot test output */
    ic_symbol_print(str);
}

void abnormal(void){
    /* test null symbol cases */
    assert( 1 == ic_symbol_init(0, 0, 0) );
    assert( 0 == ic_symbol_contents(0) );
    assert( 0 == ic_symbol_get(0, 0) );
    assert( -1 == ic_symbol_length(0) );
    /* cannot test output */
    ic_symbol_print(0);
}


int main(void){
    normal();
    abnormal();

    return 0;
}
