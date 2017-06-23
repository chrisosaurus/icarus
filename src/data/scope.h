#ifndef ICARUS_SCOPE_H
#define ICARUS_SCOPE_H

#include "dict.h"
#include "symbol.h"

/* this is a scope used in the analyse phase
 * this is not intended for runtime usage
 *
 * a scope is a dictionary with a pointer to the parent scope
 * (or null if no such scope exists)
 */

struct ic_scope {
    struct ic_scope *parent;

    /* map of char* to void* contents
     * FIXME ownership undefined
     */
    struct ic_dict contents;
};

/* alloc and init a new scope
 *
 * parent may be null
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_scope *ic_scope_new(struct ic_scope *parent);

/* init an existing scope
 *
 * parent may be null
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_scope_init(struct ic_scope *scope, struct ic_scope *parent);

/* destroy scope
 *
 * will only free scope if `free_scope` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_scope_destroy(struct ic_scope *scope, unsigned int free_scope);

/* insert a new entry to this scope
 * this will insert into content, key must not already exist
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_scope_insert(struct ic_scope *scope, char *key, void *data);

/* insert a new entry to this scope
 * this will insert into content, key must not already exist
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_scope_insert_symbol(struct ic_scope *scope, struct ic_symbol *key, void *data);

/* update an existing entry to this scope
 * this will insert into content, key must already exist
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_scope_update(struct ic_scope *scope, char *key, void *data);

/* update an existing entry to this scope
 * this will insert into content, key must already exist
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_scope_update_symbol(struct ic_scope *scope, struct ic_symbol *key, void *data);

/* set a key in this scope
 * this will insert into content, doesn't matter if key exists or not
 *
 * will perform either an insert or update, as appropriate
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_scope_set(struct ic_scope *scope, char *key, void *data);

/* set a key in this scope
 * this will insert into content, doesn't matter if key exists or not
 *
 * will perform either an insert or update, as appropriate
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_scope_set_symbol(struct ic_scope *scope, struct ic_symbol *key, void *data);

/* retrieve contents by string
 *
 * this will first search the current scope
 * then, if a parent exists, it will continue search up parents
 *
 * returns * on success
 * returns 0 on failure
 */
void *ic_scope_get(struct ic_scope *scope, char *key);

/* retrieve contents by symbol
 *
 * this will first search the current scope
 * then, if a parent exists, it will continue search up parents
 *
 * returns * on success
 * returns 0 on failure
 */
void *ic_scope_get_from_symbol(struct ic_scope *scope, struct ic_symbol *key);

/* retrieve contents by string
 *
 * this will first search the current scope
 * this will NOT consider parent scopes
 *
 * returns * on success
 * returns 0 on failure
 */
void *ic_scope_get_nofollow(struct ic_scope *scope, char *key);

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
unsigned int ic_scope_exists(struct ic_scope *scope, char *key);

/* check if specified key already exists in this scope
 *
 * FIXME no way to indicate error
 *
 * returns 1 if key exists
 * returns 0 if key doesn't exist or on error
 */
unsigned int ic_scope_exists_nofollow(struct ic_scope *scope, char *key);

/* delete the item stored under key
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_scope_delete(struct ic_scope *scope, char *key);

/* pretty print scope contents */
void ic_scope_dump(struct ic_scope *scope);

#endif
