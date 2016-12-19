#include <stdio.h>  /* puts */
#include <stdlib.h> /* calloc */

#include "dict.h"

#define IC_DICT_DEFAULT_SIZE 32

/* allocate and init a new ic_dict
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_dict *ic_dict_new(void) {
    struct ic_dict *dict = 0;

    dict = calloc(1, sizeof(struct ic_dict));
    if (!dict) {
        puts("ic_dict_new: call to calloc failed");
        return 0;
    }

    if (!ic_dict_init(dict)) {
        puts("ic_dict_new: call to ic_dict_init failed");
        free(dict);
        return 0;
    }

    return dict;
}

/* init an existing ic_dict
 *
 * return 1 on success
 * return 0 on failure
 */
unsigned int ic_dict_init(struct ic_dict *dict) {
    if (!dict) {
        puts("ic_dict_init: dict was null");
        return 0;
    }

    return lh_init(&(dict->lht), IC_DICT_DEFAULT_SIZE);
}

/* get item stored in dict at key
 *
 * returns void *data on success
 * return 0 on failure
 */
void *ic_dict_get(struct ic_dict *dict, char *key) {
    if (!dict) {
        puts("ic_dict_get: dict was null");
        return 0;
    }

    return lh_get(&(dict->lht), key);
}

/* update data in dict at key
 * update only works if the key already exists
 *  update will NOT insert new keys
 *
 * returns old data on success
 * returns 0 on failure
 */
void *ic_dict_update(struct ic_dict *dict, char *key, void *data) {
    if (!dict) {
        puts("ic_dict_update: dict was null");
        return 0;
    }

    return lh_update(&(dict->lht), key, data);
}

/* set data under key in dict
 *
 * will perform either an insert or an update
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_dict_set(struct ic_dict *dict, char *key, void *data) {
    if (!dict) {
        puts("ic_dict_set: dict was null");
        return 0;
    }

    return lh_set(&(dict->lht), key, data);
}

/* insert data into dict under key
 * insert will only work if key doesn't already exist
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_dict_insert(struct ic_dict *dict, char *key, void *data) {
    if (!dict) {
        puts("ic_dict_insert: dict was null");
        return 0;
    }

    return lh_insert(&(dict->lht), key, data);
}

/* check if key already exists in dict
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_dict_exists(struct ic_dict *dict, char *key) {
    if (!dict) {
        puts("ic_dict_exists: dict was null");
        return 0;
    }

    return lh_exists(&(dict->lht), key);
}

/* delete element at key
 * only works if key exists
 *
 * returns old data on success
 * returns 0 on failure
 */
void *ic_dict_delete(struct ic_dict *dict, char *key) {
    if (!dict) {
        puts("ic_dict_delete: dict was null");
        return 0;
    }

    return lh_delete(&(dict->lht), key);
}

/* free an existing ic_dict
 * this will free all memory internal to the hash
 *
 * if free_dict is true then it will also free the ic_dict *dict
 * if free_data is true then it will also free all the void *data
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_dict_destroy(struct ic_dict *dict, unsigned int free_dict, unsigned int free_data) {
    unsigned int ret = 0;

    if (!dict) {
        puts("ic_dict_destroy: dict was null");
        return 0;
    }

    /* do not set free_table as it is allocated
     * as part of ic_dict
     */
    ret = lh_destroy(&(dict->lht), 0, free_data);
    if (!ret) {
        puts("ic_dict_destroy: call to lh_destroy failed");
        return 0;
    }

    if (free_dict) {
        free(dict);
    }

    return 1;
}
