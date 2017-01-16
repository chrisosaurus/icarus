#include <assert.h> /* assert */
#include <stdio.h>  /* puts */
#include <string.h> /* strncmp */

#include "../../../src/data/scope.h"

void normal(void) {
    struct ic_scope *parent;
    struct ic_scope *scope;

    /* data to insert */
    int data_1 = 1;
    int data_2 = 2;
    int data_3 = 3;

    /* fetched data */
    int *data = 0;

    puts("testing scope");

    puts("testing creation");

    parent = ic_scope_new(0);
    assert(parent);

    scope = ic_scope_new(parent);
    assert(scope);
    assert(parent == scope->parent);

    puts("testing simple insert and get");

    /* test we cannot pull anything out yet */
    assert(0 == ic_scope_get(scope, "hello"));
    assert(0 == ic_scope_exists(scope, "hello"));

    /* add something */
    assert(1 == ic_scope_insert(scope, "hello", &data_1));

    /* check that the insert was actually a success */
    assert(1 == ic_scope_exists(scope, "hello"));
    assert(&data_1 == ic_scope_get(scope, "hello"));

    /* check something doesn't exist */
    assert(0 == ic_scope_get(scope, "world"));

    puts("testing symbol insert and get");
    {
        /* insert completely new key via normal insert
       * fetch by get_from_symbol
       */
        char *sym_char_1 = "unique symbol key 1";
        struct ic_symbol *sym_1 = 0;
        sym_1 = ic_symbol_new(sym_char_1, strlen(sym_char_1));
        assert(sym_1);

        assert(0 == ic_scope_get(scope, sym_char_1));
        assert(1 == ic_scope_insert(scope, sym_char_1, &data_1));
        data = ic_scope_get_from_symbol(scope, sym_1);
        assert(data);
        assert(data == &data_1);

        assert(1 == ic_symbol_destroy(sym_1, 1));
    }
    {
        /* insert completely new key via insert_symbol
       * fetch via normal insert
       */
        char *sym_char_2 = "unique symbol key 2";
        struct ic_symbol *sym_2 = 0;
        sym_2 = ic_symbol_new(sym_char_2, strlen(sym_char_2));
        assert(sym_2);

        assert(0 == ic_scope_get(scope, sym_char_2));
        assert(1 == ic_scope_insert_symbol(scope, sym_2, &data_2));
        data = ic_scope_get(scope, sym_char_2);
        assert(data);
        assert(data == &data_2);

        assert(1 == ic_symbol_destroy(sym_2, 1));
    }

    puts("testing update and set");

    /* update an existing key to new value */
    assert(1 == ic_scope_exists(scope, "hello"));
    assert(1 == ic_scope_update(scope, "hello", &data_2));
    assert(&data_2 == ic_scope_get(scope, "hello"));

    /* set an existing key to new value */
    assert(1 == ic_scope_exists(scope, "hello"));
    assert(1 == ic_scope_set(scope, "hello", &data_1));
    assert(&data_1 == ic_scope_get(scope, "hello"));

    /* set a new key */
    assert(0 == ic_scope_exists(scope, "completely unique key"));
    assert(1 == ic_scope_set(scope, "completely unique key", &data_1));
    assert(&data_1 == ic_scope_get(scope, "completely unique key"));

    puts("testing symbol update and set");
    {
        /* set new key using set_symbol
       * check using get_from_symbol
       * then update using update_symbol
       * check using normal get
       */

        char *sym_char_3 = "unique symbol key 3";
        struct ic_symbol *sym_3 = 0;
        sym_3 = ic_symbol_new(sym_char_3, strlen(sym_char_3));
        assert(sym_3);

        assert(0 == ic_scope_get(scope, sym_char_3));
        assert(1 == ic_scope_insert_symbol(scope, sym_3, &data_1));
        data = ic_scope_get_from_symbol(scope, sym_3);
        assert(data);
        assert(data == &data_1);

        assert(1 == ic_scope_exists(scope, sym_char_3));
        assert(data);
        assert(data == &data_1);
        assert(1 == ic_scope_update_symbol(scope, sym_3, &data_1));
        data = ic_scope_get(scope, sym_char_3);
        assert(data);
        assert(data == &data_1);

        assert(1 == ic_symbol_destroy(sym_3, 1));
    }

    puts("testing parent functionality");

    /* test insertion into parent */
    /* test we cannot pull anything out yet */
    assert(0 == ic_scope_get(parent, "world"));

    /* add something */
    assert(1 == ic_scope_insert(parent, "world", &data_2));

    /* check that the insert was actually a success */
    assert(&data_2 == ic_scope_get(parent, "world"));

    /* test fetch through child */
    assert(&data_2 == ic_scope_get(scope, "world"));

    /* test exists through child */
    assert(1 == ic_scope_exists(scope, "world"));

    /* test exists_nofollow through child */
    assert(0 == ic_scope_exists_nofollow(scope, "world"));

    /* test that child cannot see through _nofollow */
    assert(0 == ic_scope_get_nofollow(scope, "world"));

    /* test masking of parent value by child */
    assert(1 == ic_scope_insert(scope, "world", &data_3));
    assert(&data_2 == ic_scope_get(parent, "world"));
    assert(&data_3 == ic_scope_get(scope, "world"));

    puts("testing delete");

    /* test delete */
    /* successful delete */
    assert(1 == ic_scope_delete(scope, "hello"));
    /* unsuccessful delete */
    assert(0 == ic_scope_delete(scope, "hello"));
    /* successful delete */
    assert(1 == ic_scope_delete(parent, "world"));
    /* successful delete */
    assert(1 == ic_scope_delete(scope, "world"));

    puts("testing destroy");

    /* finally cleanup
     * this will also cleanup parent
     */
    assert(1 == ic_scope_destroy(scope, 1));

    puts("success");
}

void abnormal(void) {
    struct ic_scope *scope = 0;
    char *ch = "hello";
    struct ic_symbol sym;

    puts("ignore any text output, testing error cases");
    /* check all the scope null cases */
    assert(0 == ic_scope_init(0, 0));
    assert(0 == ic_scope_get(0, 0));
    assert(0 == ic_scope_get_from_symbol(0, 0));
    assert(0 == ic_scope_get_nofollow(0, 0));
    assert(0 == ic_scope_insert(0, 0, 0));
    assert(0 == ic_scope_insert_symbol(0, 0, 0));
    assert(0 == ic_scope_update(0, 0, 0));
    assert(0 == ic_scope_update_symbol(0, 0, 0));
    assert(0 == ic_scope_set(0, 0, 0));
    assert(0 == ic_scope_set_symbol(0, 0, 0));
    assert(0 == ic_scope_delete(0, 0));
    assert(0 == ic_scope_destroy(0, 0));

    /* testing with null non-first args */
    scope = ic_scope_new(0);
    assert(scope);

    assert(0 == ic_scope_insert(scope, 0, 0));
    assert(0 == ic_scope_insert(scope, ch, 0));
    assert(0 == ic_scope_insert_symbol(scope, 0, 0));
    assert(0 == ic_scope_insert_symbol(scope, &sym, 0));
    assert(0 == ic_scope_update(scope, 0, 0));
    assert(0 == ic_scope_update(scope, ch, 0));
    assert(0 == ic_scope_update_symbol(scope, 0, 0));
    assert(0 == ic_scope_update_symbol(scope, &sym, 0));
    assert(0 == ic_scope_set(scope, 0, 0));
    assert(0 == ic_scope_set(scope, ch, 0));
    assert(0 == ic_scope_set_symbol(scope, 0, 0));
    assert(0 == ic_scope_set_symbol(scope, &sym, 0));
    assert(0 == ic_scope_get(scope, 0));
    assert(0 == ic_scope_get_from_symbol(scope, 0));
    assert(0 == ic_scope_get_nofollow(scope, 0));
    assert(0 == ic_scope_delete(scope, 0));

    assert(1 == ic_scope_destroy(scope, 1));

    puts("success");
}

int main(void) {
    normal();
    abnormal();

    return 0;
}
