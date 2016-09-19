#include <stdio.h>
#include <stdlib.h>

#include "scope.h"

/* instantiate dict on scope on demand
 *
 * will create scope->locals
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_scope_instantiate_dict(struct ic_backend_pancake_scope *scope);

/* allocate and init a new scope
 *
 * taken optional parent
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_scope *ic_backend_pancake_scope_new(struct ic_backend_pancake_scope *parent) {
    struct ic_backend_pancake_scope *scope = 0;

    scope = calloc(1, sizeof(struct ic_backend_pancake_scope));
    if (!scope) {
        puts("ic_backend_pancake_scope_new: call to calloc failed");
        return 0;
    }

    if (!ic_backend_pancake_scope_init(scope, parent)) {
        puts("ic_backend_pancake_scope_new: call to calloc failed");
        return 0;
    }

    return scope;
}

/* init an existing scope
 *
 * taken optional parent
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_scope_init(struct ic_backend_pancake_scope *scope, struct ic_backend_pancake_scope *parent) {
    if (!scope) {
        puts("ic_backend_pancake_scope_init: scope was null");
        return 0;
    }

    scope->parent = parent;
    scope->locals = 0;

    return 1;
}

/* destroy an existing scope
 *
 * will free scope if `free_scope` is truhty
 *
 * will not touch parent
 * will call destroy on dict
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_scope_destroy(struct ic_backend_pancake_scope *scope, unsigned int free_scope) {
    if (!scope) {
        puts("ic_backend_pancake_scope_destroy: scope was null");
        return 0;
    }

    scope->parent = 0;

    if (scope->locals) {
        /* destroy dict
         * free dict
         * free values (all scope created in this scope)
         */
        if (!ic_dict_destroy(scope->locals, 1, 1)) {
            puts("ic_backend_pancake_scope_destroy: call to ic_backend_pancake_scope_destroy failed");
            return 0;
        }
    }

    if (free_scope) {
        free(scope);
    }

    return 1;
}

/* insert a local into the dict
 * must not already exist
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_scope_insert(struct ic_backend_pancake_scope *scope, char *key, struct ic_backend_pancake_local *local) {
    if (!scope) {
        puts("ic_backend_pancake_scope_insert: scope was null");
        return 0;
    }

    if (!key) {
        puts("ic_backend_pancake_scope_insert: key was null");
        return 0;
    }

    if (!local) {
        puts("ic_backend_pancake_scope_insert: local was null");
        return 0;
    }

    if (!(scope->locals)) {
        /* create on demand */
        if (!ic_backend_pancake_scope_instantiate_dict(scope)) {
            puts("ic_backend_pancake_scope_insert: call to ic_backend_pancake_scope_instantate_dict failed");
            return 0;
        }
    }

    if (!ic_dict_insert(scope->locals, key, local)) {
        puts("ic_backend_pancake_scope_insert: call to ic_dict_insert failed");
        return 0;
    }

    return 1;
}

/* get the object stored under a key from scope
 * if not found in this scope, will continue search up through parent scopes
 *
 * returns * on success
 * returns 0 on falure
 */
struct ic_backend_pancake_local *ic_backend_pancake_scope_get(struct ic_backend_pancake_scope *scope, char *key) {
    struct ic_backend_pancake_local *local = 0;

    if (!scope) {
        puts("ic_backend_pancake_scope_get: scope was null");
        return 0;
    }

    if (!key) {
        puts("ic_backend_pancake_scope_get: key was null");
        return 0;
    }

    if (!(scope->locals)) {
        /* create on demand */
        if (!ic_backend_pancake_scope_instantiate_dict(scope)) {
            puts("ic_backend_pancake_scope_get: call to ic_backend_pancake_scope_instantate_dict failed");
            return 0;
        }
    }

    local = ic_dict_get(scope->locals, key);
    if (local) {
        /* found it, return */
        return local;
    }

    /* otherwise we want to search up parent chain
   * check if we have a parent first
   */
    if (!scope->parent) {
        puts("ic_backend_pancake_scope_get: item not found, and no parent set");
        return 0;
    }

    /* otherwise recurse up parent chain */
    return ic_backend_pancake_scope_get(scope->parent, key);
}

/* get the object stored under a key from scope
 * will not search through parents
 *
 * returns * on success
 * returns 0 on falure
 */
struct ic_backend_pancake_local *ic_backend_pancake_scope_get_noparent(struct ic_backend_pancake_scope *scope, char *key) {
    struct ic_backend_pancake_local *local = 0;

    if (!scope) {
        puts("ic_backend_pancake_scope_get_noparent: scope was null");
        return 0;
    }

    if (!key) {
        puts("ic_backend_pancake_scope_get_noparent: key was null");
        return 0;
    }

    if (!(scope->locals)) {
        /* create on demand */
        if (!ic_backend_pancake_scope_instantiate_dict(scope)) {
            puts("ic_backend_pancake_scope_get_noparent: call to ic_backend_pancake_scope_instantate_dict failed");
            return 0;
        }
    }

    local = ic_dict_get(scope->locals, key);
    if (!local) {
        puts("ic_backend_pancake_scope_get_noparent: call to ic_dict_get failed");
        return 0;
    }

    return local;
}

/* instantiate dict on scope on demand
 *
 * will create scope->locals
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_scope_instantiate_dict(struct ic_backend_pancake_scope *scope) {
    if (!scope) {
        puts("ic_backend_pancake_scope_instantiate_dict: scope was null");
        return 0;
    }

    if (scope->locals) {
        puts("ic_backend_pancake_scope_instantiate_dict: scope->locals was already set");
        return 0;
    }

    scope->locals = ic_dict_new();
    if (!scope->locals) {
        puts("ic_backend_pancake_scope_instantiate_dict: call to ic_dict_new failed");
        return 0;
    }

    return 1;
}
