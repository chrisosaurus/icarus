#include <assert.h> /* assert */
#include <stdio.h>  /* puts */
#include <string.h> /* strncmp */

#include "../../../src/data/scope.h"

void normal(void) {
    struct ic_scope *parent;
    struct ic_scope *scope;

    int data_1;
    int data_2;
    int data_3;

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

    puts("ignore any text output, testing error cases");
    /* check all the scope null cases */
    assert(0 == ic_scope_init(0, 0));
    assert(0 == ic_scope_get(0, 0));
    assert(0 == ic_scope_get_nofollow(0, 0));
    assert(0 == ic_scope_insert(0, 0, 0));
    assert(0 == ic_scope_delete(0, 0));
    assert(0 == ic_scope_destroy(0, 0));

    /* testing with null non-first args */
    scope = ic_scope_new(0);
    assert(scope);

    assert(0 == ic_scope_insert(scope, 0, 0));
    assert(0 == ic_scope_insert(scope, ch, 0));
    assert(0 == ic_scope_get(scope, 0));
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
