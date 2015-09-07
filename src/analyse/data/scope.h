#ifndef ICARUS_CLUDGE_H
#define ICARUS_CLUDGE_H

#include "../../data/dict.h"

/* this is a scope used in the analyse phase
 * this is not intended for runtime usage
 *
 * a scope is a dictionary with a pointer to the parent scope
 * (or null if no such scope exists)
 */

struct ic_scope {
    struct ic_scope *parent;

    /* map of variables in a scope
     * variable name (char*) -> contents
     * FIXME contents are undefined
     */
    struct ic_dict contents;
};

/* alloc and init a new scope
 *
 * parent may be null
 *
 * returns pointer on success
 * returns 0 on error
 */
struct ic_scope * ic_scope_new(struct ic_scope *parent);

/* init an existing scope
 *
 * parent may be null
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_scope_init(struct ic_scope *scope, struct ic_scope *parent);

/* destroy scope
 *
 * will only free scope if `free_scope` is truthy
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_scope_destroy(struct ic_scope *scope, unsigned int free_scope);

/* insert a new entry to this scope
 * this will insert into content, key must not already exist
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_scope_insert(struct ic_scope *scope, char *key, void *data);

/* retrieve contents by string
 *
 * this will first search the current scope
 * then, if a parent exists, it will continue search up parents
 *
 * returns * on success
 * returns 0 on failure
 */
void * ic_scope_get(struct ic_scope *scope, char *key);

/* retrieve contents by string
 *
 * this will first search the current scope
 * this will NOT consider parent scopes
 *
 * returns * on success
 * returns 0 on failure
 */
void * ic_scope_get_nofollow(struct ic_scope *scope, char *key);

/* add a new type decl to this scope
 * this will insert into dict_tname and also
 * into tdecls
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_scope_delete(struct ic_scope *scope, char *key);

#endif
