#include <stdio.h> /* puts */
#include <stdlib.h> /* calloc */

#include "scope.h"

/* ignored unused parameter
 * pending implementation
 */
#pragma GCC diagnostic ignored "-Wunused-parameter"

/* alloc and init a new scope
 *
 * parent may be null
 *
 * returns pointer on success
 * returns 0 on error
 */
struct ic_scope * ic_scope_new(struct ic_scope *parent){
    puts("ic_scope_new: unimplemented");
    return 0;
}

/* init an existing scope
 *
 * parent may be null
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_scope_init(struct ic_scope *scope, struct ic_scope *parent){
    puts("ic_scope_init: unimplemented");
    return 1;
}

/* destroy scope
 *
 * will only free scope if `free_scope` is truthy
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_scope_destroy(struct ic_scope *scope, unsigned int free_scope){
    puts("ic_scope_destroy: unimplemented");
    return 1;
}

/* insert a new entry to this scope
 * this will insert into content
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_scope_insert(struct ic_scope *scope, char *key, void *data){
    puts("ic_scope_insert: unimplemented");
    return 1;
}

/* retrieve contents by string
 *
 * this will first search the current scope
 * then, if a parent exists, it will continue search up parents
 *
 * returns * on success
 * returns 0 on failure
 */
void * ic_scope_get(struct ic_scope *scope, char *key){
    puts("ic_scope_get: unimplemented");
    return 0;
}

/* retrieve contents by string
 *
 * this will first search the current scope
 * this will NOT consider parent scopes
 *
 * returns * on success
 * returns 0 on failure
 */
void * ic_scope_get_nofollow(struct ic_scope *scope, char *key){
    puts("ic_scope_get_nofollow: unimplemented");
    return 0;
}

/* add a new type decl to this scope
 * this will insert into dict_tname and also
 * into tdecls
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_scope_delete(struct ic_scope *scope, char *key){
    puts("ic_scope_delete: unimplemented");
    return 1;
}


