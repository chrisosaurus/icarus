#ifndef ICARUS_DICT_H
#define ICARUS_DICT_H

#include "../../libs/linear_hash/linear_hash.h"

/* this dictionary type maps strings to void *
 * this type is a wrapper around
 *  linear_hash.h:struct lh_table
 */
struct ic_dict{
    struct lh_table lht;
};

/* allocate and init a new ic_dict
 *
 * returns pointer on success
 * returns 0 on error
 */
struct ic_dict * ic_dict_new(void);

/* init an existing ic_dict
 *
 * return 0 on success
 * return 1 on error
 */
unsigned int ic_dict_init(struct ic_dict *dict);

/* get item stored in dict at key
 *
 * returns void *data on success
 * return 0 on error
 */
void * ic_dict_get(struct ic_dict *dict, char *key);

/* set data in dict at key
 * set only works if the key already exists
 *  set will NOT insert new keys
 *
 * returns old data on success
 * returns 0 on error
 */
void * ic_dict_set(struct ic_dict *dict, char *key, void *data);

/* insert data into dict under key
 *
 * returns 1 on success
 * returns 0 on error
 */
unsigned int ic_dict_insert(struct ic_dict *dict, char *key, void *data);

/* check if key already exists in dict
 *
 * returns 1 on success
 * returns 0 on error
 */
unsigned int ic_dict_exists(struct ic_dict *dict, char *key);

/* delete element at key
 * only works if key exists
 *
 * returns old data on success
 * returns 0 on error
 */
void * ic_dict_delete(struct ic_dict *dict, char *key);

/* free an existing ic_dict
 * this will free all memory internal to the hash
 *
 * if free_dict is true then it will also free the ic_dict *dict
 * if free_data is true then it will also free all the void *data
 *
 * returns 1 on success
 * returns 0 on error
 */
unsigned int ic_dict_destroy(struct ic_dict *dict, unsigned int free_dict, unsigned int free_data);

#endif
