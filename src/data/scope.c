#include <stdio.h>  /* puts */
#include <stdlib.h> /* calloc */

#include "scope.h"

/* alloc and init a new scope
 *
 * parent may be null
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_scope *ic_scope_new(struct ic_scope *parent) {
    struct ic_scope *scope = 0;

    scope = calloc(1, sizeof(struct ic_scope));
    if (!scope) {
        puts("ic_scope_new: call to calloc failed");
        return 0;
    }

    if (!ic_scope_init(scope, parent)) {
        puts("ic_scope_new: call to ic_scope_init failed");
        return 0;
    }

    return scope;
}

/* init an existing scope
 *
 * parent may be null
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_scope_init(struct ic_scope *scope, struct ic_scope *parent) {
    if (!scope) {
        puts("ic_scope_init: scope was null");
        return 0;
    }

    scope->parent = parent;

    if (!ic_dict_init(&(scope->contents))) {
        puts("ic_scope_init: call to ic_dict_init failed");
        return 0;
    }

    return 1;
}

/* destroy scope
 *
 * will only free scope if `free_scope` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_scope_destroy(struct ic_scope *scope, unsigned int free_scope) {
    if (!scope) {
        puts("ic_scope_destroy: scope was null");
        return 0;
    }

    /* Doesn't free parent */
    scope->parent = 0;

    /* do not destroy dict (as it is within the scope)
     * and do not destroy elements
     * FIXME consider element freeing
     */
    if (!ic_dict_destroy(&(scope->contents), 0, 0)) {
        puts("ic_scope_destroy: call ot ic_dict_destroy failed");
        return 0;
    }

    if (free_scope) {
        free(scope);
    }

    return 1;
}

/* insert a new entry to this scope
 * this will insert into content, key must not already exist
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_scope_insert(struct ic_scope *scope, char *key, void *data) {
    if (!scope) {
        puts("ic_scope_insert: scope was null");
        return 0;
    }

    if (!key) {
        puts("ic_scope_insert: key was null");
        return 0;
    }

    if (!data) {
        puts("ic_scope_insert: data was null");
        return 0;
    }

    if (!ic_dict_insert(&(scope->contents), key, data)) {
        puts("ic_scope_insert: call to ic_dict_insert failed");
        return 0;
    }

    return 1;
}

/* insert a new entry to this scope
 * this will insert into content, key must not already exist
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_scope_insert_symbol(struct ic_scope *scope, struct ic_symbol *key, void *data) {
    char *key_chars = 0;

    if (!scope) {
        puts("ic_scope_insert_symbol: scope was null");
        return 0;
    }

    if (!key) {
        puts("ic_scope_insert_symbol: key was null");
        return 0;
    }

    key_chars = ic_symbol_contents(key);
    if (!key_chars) {
        puts("ic_scope_insert_symbol: call to ic_symbol_contents failed");
        return 0;
    }

    return ic_scope_insert(scope, key_chars, data);
}

/* update an existing entry to this scope
 * this will insert into content, key must already exist
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_scope_update(struct ic_scope *scope, char *key, void *data) {
    if (!scope) {
        puts("ic_scope_update: scope was null");
        return 0;
    }

    if (!key) {
        puts("ic_scope_update: key was null");
        return 0;
    }

    if (!data) {
        puts("ic_scope_update: data was null");
        return 0;
    }

    if (!ic_dict_update(&(scope->contents), key, data)) {
        puts("ic_scope_update: call to ic_dict_update failed");
        return 0;
    }

    return 1;
}

/* update an existing entry to this scope
 * this will insert into content, key must already exist
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_scope_update_symbol(struct ic_scope *scope, struct ic_symbol *key, void *data) {
    char *key_chars = 0;

    if (!scope) {
        puts("ic_scope_update_symbol: scope was null");
        return 0;
    }

    if (!key) {
        puts("ic_scope_update_symbol: key was null");
        return 0;
    }

    key_chars = ic_symbol_contents(key);
    if (!key_chars) {
        puts("ic_scope_update_symbol: call to ic_symbol_contents failed");
        return 0;
    }

    return ic_scope_update(scope, key_chars, data);
}

/* set a key in this scope
 * this will insert into content, doesn't matter if key exists or not
 *
 * will perform either an insert or update, as appropriate
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_scope_set(struct ic_scope *scope, char *key, void *data) {
    if (!scope) {
        puts("ic_scope_set: scope was null");
        return 0;
    }

    if (!key) {
        puts("ic_scope_set: key was null");
        return 0;
    }

    if (!data) {
        puts("ic_scope_set: data was null");
        return 0;
    }

    if (!ic_dict_set(&(scope->contents), key, data)) {
        puts("ic_scope_set: call to ic_dict_set failed");
        return 0;
    }

    return 1;
}

/* set a key in this scope
 * this will insert into content, doesn't matter if key exists or not
 *
 * will perform either an insert or update, as appropriate
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_scope_set_symbol(struct ic_scope *scope, struct ic_symbol *key, void *data) {
    char *key_chars = 0;

    if (!scope) {
        puts("ic_scope_set_symbol: scope was null");
        return 0;
    }

    if (!key) {
        puts("ic_scope_set_symbol: key was null");
        return 0;
    }

    key_chars = ic_symbol_contents(key);
    if (!key_chars) {
        puts("ic_scope_set_symbol: call to ic_symbol_contents failed");
        return 0;
    }

    return ic_scope_update(scope, key_chars, data);
}

/* retrieve contents by string
 *
 * this will first search the current scope
 * then, if a parent exists, it will continue search up parents
 *
 * returns * on success
 * returns 0 on failure
 */
void *ic_scope_get(struct ic_scope *scope, char *key) {
    void *data = 0;

    if (!scope) {
        puts("ic_scope_get: scope was null");
        return 0;
    }

    if (!key) {
        puts("ic_scope_get: key was null");
        return 0;
    }

    data = ic_scope_get_nofollow(scope, key);

    if (data) {
        return data;
    }

    /* recurse up parent tree if that is an option */
    if (scope->parent) {
        return ic_scope_get(scope->parent, key);
    }

    return 0;
}

/* retrieve contents by symbol
 *
 * this will first search the current scope
 * then, if a parent exists, it will continue search up parents
 *
 * returns * on success
 * returns 0 on failure
 */
void *ic_scope_get_from_symbol(struct ic_scope *scope, struct ic_symbol *key) {
    char *key_chars = 0;

    if (!scope) {
        puts("ic_scope_get_from_symbol: scope was null");
        return 0;
    }

    if (!key) {
        puts("ic_scope_get_from_symbol: key was null");
        return 0;
    }

    key_chars = ic_symbol_contents(key);
    if (!key_chars) {
        puts("ic_scope_get_from_symbol: call to ic_symbol_contents failed");
        return 0;
    }

    return ic_scope_get(scope, key_chars);
}

/* retrieve contents by string
 *
 * this will first search the current scope
 * this will NOT consider parent scopes
 *
 * returns * on success
 * returns 0 on failure
 */
void *ic_scope_get_nofollow(struct ic_scope *scope, char *key) {
    if (!scope) {
        puts("ic_scope_get_nofollow: scope was null");
        return 0;
    }

    if (!key) {
        puts("ic_scope_get_nofollow: key was null");
        return 0;
    }

    return ic_dict_get(&(scope->contents), key);
}

/* check if specified key already exists in scope chain
 *
 * FIXME no way to indicate error
 *
 * this will first search the current scope
 * then, if a parent exists, it will continue search up parents
 *
 * returns 1 if key exists
 * returns 0 if key doesn't exist or on error
 */
unsigned int ic_scope_exists(struct ic_scope *scope, char *key) {
    if (!scope) {
        puts("ic_scope_exists: scope was null");
        return 0;
    }

    if (!key) {
        puts("ic_scope_exists: key was null");
        return 0;
    }

    if (ic_dict_exists(&(scope->contents), key)) {
        return 1;
    }

    if (scope->parent) {
        return ic_scope_exists(scope->parent, key);
    }

    return 0;
}

/* check if specified key already exists in this scope
 *
 * FIXME no way to indicate error
 *
 * returns 1 if key exists
 * returns 0 if key doesn't exist or on error
 */
unsigned int ic_scope_exists_nofollow(struct ic_scope *scope, char *key) {
    if (!scope) {
        puts("ic_scope_exists_nofollow: scope was null");
        return 0;
    }

    if (!key) {
        puts("ic_scope_exists_nofollow: key was null");
        return 0;
    }

    if (ic_dict_exists(&(scope->contents), key)) {
        return 1;
    }

    return 0;
}

/* delete the item stored under key
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_scope_delete(struct ic_scope *scope, char *key) {
    if (!scope) {
        puts("ic_scope_insert: scope was null");
        return 0;
    }

    if (!key) {
        puts("ic_scope_insert: key was null");
        return 0;
    }

    if (!ic_dict_delete(&(scope->contents), key)) {
        puts("ic_scope_insert: call to ic_dict_delete failed");
        return 0;
    }

    return 1;
}

/* pretty print scope contents */
void ic_scope_dump(struct ic_scope *scope) {
    if (!scope) {
        puts("ic_scope_dump: scope was null");
        return;
    }

    puts("------");
    ic_dict_dump(&(scope->contents));
    if (scope->parent) {
        ic_scope_dump(scope->parent);
    }
}
