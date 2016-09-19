#ifndef IC_BACKEND_PANCAKE_SCOPE_H
#define IC_BACKEND_PANCAKE_SCOPE_H

#include "../../../data/dict.h"
#include "local.h"

struct ic_backend_pancake_scope {
    /* may or may not have a parent */
    struct ic_backend_pancake_scope *parent;
    /* lazily instantiated dictionary
   * if no locals exist, then locals will be null
   */
    struct ic_dict *locals;
};

/* allocate and init a new scope
 *
 * takes optional parent
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_scope *ic_backend_pancake_scope_new(struct ic_backend_pancake_scope *parent);

/* init an existing scope
 *
 * takes optional parent
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_scope_init(struct ic_backend_pancake_scope *scope, struct ic_backend_pancake_scope *parent);

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
unsigned int ic_backend_pancake_scope_destroy(struct ic_backend_pancake_scope *scope, unsigned int free_scope);

/* insert a local into the dict
 * must not already exist
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_scope_insert(struct ic_backend_pancake_scope *scope, char *key, struct ic_backend_pancake_local *local);

/* get the object stored under a key from scope
 * if not found in this scope, will continue search up through parent scopes
 *
 * returns * on success
 * returns 0 on falure
 */
struct ic_backend_pancake_local *ic_backend_pancake_scope_get(struct ic_backend_pancake_scope *scope, char *key);

/* get the object stored under a key from scope
 * will not search through parents
 *
 * returns * on success
 * returns 0 on falure
 */
struct ic_backend_pancake_local *ic_backend_pancake_scope_get_noparent(struct ic_backend_pancake_scope *scope, char *key);

#endif /* IC_BACKEND_PANCAKE_SCOPE_H */
