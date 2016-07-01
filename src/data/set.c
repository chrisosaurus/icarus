#include <stdio.h>  /* puts */
#include <stdlib.h> /* calloc */

#include "set.h"

#define IC_SET_DEFAULT_SIZE 32

/* allocate and init a new ic_set
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_set *ic_set_new(void) {
    struct ic_set *set = 0;

    set = calloc(1, sizeof(struct ic_set));
    if (!set) {
        puts("ic_set_new: call to calloc failed");
        return 0;
    }

    if (!ic_set_init(set)) {
        puts("ic_set_new: call to ic_set_init failed");
        free(set);
        return 0;
    }

    return set;
}

/* init an existing ic_set
 *
 * return 1 on success
 * return 0 on failure
 */
unsigned int ic_set_init(struct ic_set *set) {
    if (!set) {
        puts("ic_set_init: set was null");
        return 0;
    }

    return ls_init(&(set->lss), IC_SET_DEFAULT_SIZE);
}

/* insert new item
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_set_insert(struct ic_set *set, const char *item) {
    if (!set) {
        puts("ic_set_insert: set was null");
        return 0;
    }

    return ls_insert(&(set->lss), item);
}

/* check if key already exists in set
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_set_exists(const struct ic_set *set, const char *key) {
    if (!set) {
        puts("ic_set_exists: set was null");
        return 0;
    }

    return ls_exists(&(set->lss), key);
}

/* delete element at key
 * only works if key exists
 *
 * returns 1 success
 * returns 0 on failure
 */
unsigned int ic_set_delete(struct ic_set *set, const char *key) {
    if (!set) {
        puts("ic_set_delete: set was null");
        return 0;
    }

    return ls_delete(&(set->lss), key);
}

/* free an existing ic_set
 * this will free all memory internal to the hash
 *
 * if free_set is true then it will also free the ic_set *set
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_set_destroy(struct ic_set *set, unsigned int free_set) {
    unsigned int ret = 0;

    if (!set) {
        puts("ic_set_destroy: set was null");
        return 0;
    }

    /* do not set free_table as it is allocated
     * as part of ic_set
     */
    ret = ls_destroy(&(set->lss), 0);
    if (!ret) {
        puts("ic_set_destroy: call to ls_destroy failed");
        return 0;
    }

    if (free_set) {
        free(set);
    }

    return 1;
}
