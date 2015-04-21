#include <string.h> /* strncmp */
#include <assert.h> /* assert */
#include <stdio.h> /* puts */

#include "../../src/data/string.h"

void normal(void){
    int i;
    struct ic_string *str = ic_string_new("hello", 5);
    struct ic_string *str2 = 0;

    puts("ignore any text output, testing error cases");

    assert(str);
    assert(ic_string_length(str) == 5);

    /* check the contents are what we expect */
    assert( ! strncmp("hello", ic_string_contents(str), 6) );

    /* test the get interface */
    assert(ic_string_get(str, 0) == 'h');
    assert(ic_string_get(str, 1) == 'e');
    assert(ic_string_get(str, 2) == 'l');
    assert(ic_string_get(str, 3) == 'l');
    assert(ic_string_get(str, 4) == 'o');

    /* check for out of bounds access */
    assert(ic_string_get(str, 5) == 0);
    assert(ic_string_get(str, 6) == 0);
    assert(ic_string_get(str, 7) == 0);

    /* create a second string */
    str2 = ic_string_new(" world", 6);
    assert(str2);
    assert(ic_string_length(str2) == 6);

    /* test append */
    assert( 0 == ic_string_append(str, str2) );
    assert(ic_string_length(str) == 11);
    assert( ! strncmp("hello world", ic_string_contents(str), 12) );

    /* test every detail */
    assert(ic_string_get(str,  0) == 'h');
    assert(ic_string_get(str,  1) == 'e');
    assert(ic_string_get(str,  2) == 'l');
    assert(ic_string_get(str,  3) == 'l');
    assert(ic_string_get(str,  4) == 'o');
    assert(ic_string_get(str,  5) == ' ');
    assert(ic_string_get(str,  6) == 'w');
    assert(ic_string_get(str,  7) == 'o');
    assert(ic_string_get(str,  8) == 'r');
    assert(ic_string_get(str,  9) == 'l');
    assert(ic_string_get(str, 10) == 'd');

    assert(ic_string_get(str, 11) == 0);
    assert(ic_string_get(str, 12) == 0);

    /* test append by char */
    assert( 0 == ic_string_append_char(str2, "ly", 2) );
    assert(ic_string_length(str2) == 8);
    assert( ! strncmp(" worldly", ic_string_contents(str2), 9) );

}

void abnormal(void){
    /* testing null string cases */
    assert( 1 == ic_string_init(0, 0, 0) );
    assert( 0 == ic_string_contents(0) );
    assert( 0 == ic_string_get(0, 0) );
    assert( -1 == ic_string_length(0) );
    assert( 1 == ic_string_append(0, 0) );
    assert( 1 == ic_string_append_char(0, 0, 0) );
}

int main(void){
    normal();
    abnormal();

    return 0;
}
