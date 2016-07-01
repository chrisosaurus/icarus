#ifndef ICARUS_CLUDGE_H
#define ICARUS_CLUDGE_H

#include "../../data/dict.h"
#include "../../data/symbol.h"

struct ic_slot;

/* this is a scope used in the analyse phase
 * this is not intended for runtime usage
 *
 * a scope is a dictionary with a pointer to the parent scope
 * (or null if no such scope exists)
 */

struct ic_scope {
    struct ic_scope *parent;

    /* map of variables in a scope
     * variable name (char*) -> slot
     * FIXME contents are undefined
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
unsigned int ic_scope_insert(struct ic_scope *scope, char *key, struct ic_slot *data);

/* retrieve contents by string
 *
 * this will first search the current scope
 * then, if a parent exists, it will continue search up parents
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_slot *ic_scope_get(struct ic_scope *scope, char *key);

/* retrieve contents by symbol
 *
 * this will first search the current scope
 * then, if a parent exists, it will continue search up parents
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_slot *ic_scope_get_from_symbol(struct ic_scope *scope, struct ic_symbol *key);

/* retrieve contents by string
 *
 * this will first search the current scope
 * this will NOT consider parent scopes
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_slot *ic_scope_get_nofollow(struct ic_scope *scope, char *key);

/* add a new type decl to this scope
 * this will insert into dict_tname and also
 * into tdecls
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_scope_delete(struct ic_scope *scope, char *key);

#endif
