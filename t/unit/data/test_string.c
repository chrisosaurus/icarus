#include <assert.h> /* assert */
#include <stdio.h>  /* puts */
#include <string.h> /* strncmp */

#include "../../../src/data/string.h"

void normal(void) {
    int i;
    struct ic_string *str = ic_string_new("hello", 5);
    struct ic_string *str2 = 0;
    struct ic_string empty;

    puts("ignore any text output, testing error cases");

    assert(str);
    assert(ic_string_length(str) == 5);

    /* check the contents are what we expect */
    assert(!strncmp("hello", ic_string_contents(str), 6));
    assert(1 == ic_string_equal_char(str, "hello"));

    /* test the get interface */
    assert(ic_string_get(str, 0) == 'h');
    assert(ic_string_get(str, 1) == 'e');
    assert(ic_string_get(str, 2) == 'l');
    assert(ic_string_get(str, 3) == 'l');
    assert(ic_string_get(str, 4) == 'o');
    assert(ic_string_get(str, 5) == '\0');

    /* check for out of bounds access */
    assert(ic_string_get(str, 6) == 0);
    assert(ic_string_get(str, 7) == 0);

    /* create a second string */
    str2 = ic_string_new(" world", 6);
    assert(str2);
    assert(ic_string_length(str2) == 6);

    /* test append */
    assert(1 == ic_string_append(str, str2));
    assert(ic_string_length(str) == 11);
    assert(!strncmp("hello world", ic_string_contents(str), 12));
    assert(1 == ic_string_equal_char(str, "hello world"));

    /* test every detail */
    assert(ic_string_get(str, 0) == 'h');
    assert(ic_string_get(str, 1) == 'e');
    assert(ic_string_get(str, 2) == 'l');
    assert(ic_string_get(str, 3) == 'l');
    assert(ic_string_get(str, 4) == 'o');
    assert(ic_string_get(str, 5) == ' ');
    assert(ic_string_get(str, 6) == 'w');
    assert(ic_string_get(str, 7) == 'o');
    assert(ic_string_get(str, 8) == 'r');
    assert(ic_string_get(str, 9) == 'l');
    assert(ic_string_get(str, 10) == 'd');
    assert(ic_string_get(str, 11) == '\0');

    /* out of bound */
    assert(ic_string_get(str, 12) == 0);

    /* test append by char */
    assert(1 == ic_string_append_char(str2, "ly", 2));
    assert(ic_string_length(str2) == 8);
    assert(!strncmp(" worldly", ic_string_contents(str2), 9));

    /* testing set */
    assert(1 == ic_string_set(str2, 0, '_'));
    assert('_' == ic_string_get(str2, 0));
    assert(ic_string_length(str2) == 8);
    assert(!strncmp("_worldly", ic_string_contents(str2), 9));

    assert(1 == ic_string_destroy(str, 1));
    assert(1 == ic_string_destroy(str2, 1));

    /* testing empty new and init */
    str = ic_string_new_empty();
    assert(str);
    assert(1 == ic_string_destroy(str, 1));

    assert(1 == ic_string_init_empty(&empty));
    assert(1 == ic_string_destroy(&empty, 0));
}

void abnormal(void) {
    struct ic_string *str = ic_string_new("hello", 5);

    /* testing null string cases */
    assert(0 == ic_string_init(0, 0, 0));
    assert(0 == ic_string_init_empty(0));
    assert(0 == ic_string_contents(0));
    assert(0 == ic_string_get(0, 0));
    assert(0 == ic_string_set(0, 0, 0));
    assert(-1 == ic_string_length(0));
    assert(0 == ic_string_append(0, 0));
    assert(0 == ic_string_append_char(0, 0, 0));
    assert(0 == ic_string_append_symbol(0, 0));
    assert(0 == ic_string_destroy(0, 0));

    /* testing cases with non-null string but other
     * null required arguments
     */
    assert(0 == ic_string_append(str, 0));
    assert(0 == ic_string_append_char(str, 0, 0));
    assert(0 == ic_string_append_symbol(str, 0));

    /* for string set we also do not allow for the pos to be out of range */
    assert(0 == ic_string_set(str, 5, 'c'));
    /* we also do not allow for val to be the '\0' */
    assert(0 == ic_string_set(str, 0, '\0'));

    assert(1 == ic_string_destroy(str, 1));
}

void empty_append(void) {
    struct ic_string str;
    struct ic_string *tstr = 0;

    assert(1 == ic_string_init_empty(&str));
    assert(0 == ic_string_length(&str));

    assert(1 == ic_string_append_char(&str, "hello", 5));
    assert(5 == ic_string_length(&str));

    tstr = ic_string_new(" happy", 6);
    assert(tstr);
    assert(6 == ic_string_length(tstr));
    assert(1 == ic_string_append(&str, tstr));
    assert(11 == ic_string_length(&str));

    assert(0 == ic_string_equal(0, 0));
    assert(0 == ic_string_equal(&str, 0));
    assert(0 == ic_string_equal_char(0, 0));
    assert(0 == ic_string_equal_char(&str, 0));

    assert(1 == ic_string_destroy(tstr, 1));
    assert(1 == ic_string_destroy(&str, 0));
}

void equality(void) {
    struct ic_string *str1 = 0;
    struct ic_string *str2 = 0;

    str1 = ic_string_new("Hello world", 11);
    assert(str1);

    str2 = ic_string_new_empty();
    assert(str2);
    assert(1 == ic_string_append_char(str2, "Hello world", 11));

    assert(1 == ic_string_equal(str1, str2));

    assert(1 == ic_string_append_char(str2, ".", 1));
    assert(0 == ic_string_equal(str1, str2));

    assert(1 == ic_string_destroy(str1, 1));
    assert(1 == ic_string_destroy(str2, 1));
}

void deep_copy(void) {
    struct ic_string *str1 = 0;
    struct ic_string *str2 = 0;

    str1 = ic_string_new("Hello world", 11);
    assert(str1);

    str2 = ic_string_deep_copy(str1);
    assert(str1);

    assert(str1 != str2);
    assert(str1->used == str2->used);
    assert(str1->backing.len == str2->backing.len);
    assert(str1->backing.contents != str2->backing.contents);
    assert(0 == strcmp(str1->backing.contents, str2->backing.contents));

    str1->backing.contents[0] = 'Q';
    assert(str1->backing.contents[0] == 'Q');
    assert(str2->backing.contents[0] == 'H');

    assert(1 == ic_string_destroy(str1, 1));
    assert(1 == ic_string_destroy(str2, 1));
}

int main(void) {
    normal();
    empty_append();
    equality();
    deep_copy();
    abnormal();

    return 0;
}
