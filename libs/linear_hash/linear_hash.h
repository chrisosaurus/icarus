/* The MIT License (MIT)
 *
 * Author: Chris Hall <followingthepath at gmail dot c0m>
 *
 * Copyright (c) 2015 Chris Hall (cjh)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef LINEAR_HASH_H
#define LINEAR_HASH_H

#include <stddef.h> /* size_t */

enum lh_entry_state {
    LH_ENTRY_EMPTY,
    LH_ENTRY_OCCUPIED,
    LH_ENTRY_DUMMY // was occupied but now delete
};

struct lh_entry {
    enum lh_entry_state state;
    /* hash value for this entry, output of lh_hash(key) */
    unsigned long int hash;
    /* string copied using lh_strdup (defined in linear_hash.c) */
    char *key;
    /* strlen of key, simple cache */
    size_t key_len;
    /* data pointer */
    void *data;
};

struct lh_table {
    /* number of slots in hash */
    size_t size;
    /* number of elements stored in hash */
    size_t n_elems;
    /* threshold that triggers an automatic resize */
    unsigned int threshold;
    /* array of lh_entry(s) */
    struct lh_entry *entries;
};

/* function to return number of elements
 *
 * returns number on success
 * returns 0 on error
 */
unsigned int lh_nelems(const struct lh_table *table);

/* function to calculate load
 * (table->n_elems * 10) / table->size
 *
 * returns loading factor 0 -> 10 on success
 * returns 0 on failure
 */
unsigned int lh_load(const struct lh_table *table);

/* set the load that we resize at
 * load is (table->n_elems * 10) / table->size
 *
 * this sets lh_table->threshold
 * this defaults to LH_DEFAULT_THRESHOLD in linear_hash.c
 * this is set to 6 (meaning 60% full) by default
 *
 * this will accept any value between 1 (10%) to 10 (100%)
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int lh_tune_threshold(struct lh_table *table, unsigned int threshold);

/* takes a char* representing a string
 * and a key_len of it's size
 *
 * will recalculate key_len if 0
 *
 * returns an unsigned long integer hash value on success
 * returns 0 on error
 */
unsigned long int lh_hash(const char *key, size_t key_len);

/* takes a table and a hash value
 *
 * returns the index into the table for this hash
 * returns 0 on error (if table is null)
 *
 * note the error value is indistinguishable from the 0th bucket
 * this function can only error if table is null
 * so the caller can distinguish these 2 cases
 */
size_t lh_pos(unsigned long int hash, size_t table_size);

/* allocate and initialise a new lh_table
 *
 * will automatically assume a size of 32
 *
 * lh_table will automatically resize when a call to
 * lh_insert detects the load factor is over table->threshold
 *
 * returns pointer on success
 * returns 0 on error
 */
struct lh_table * lh_new(void);

/* free an existing lh_table
 * this will free all the sh entries stored
 * this will free all the keys (as they are strdup-ed)
 *
 * this will only free the *table pointer if `free_table` is set to 1
 * this will only free the *data pointers if `free_data` is set to 1
 *
 * returns 1 on success
 * returns 0 on error
 */
unsigned int lh_destroy(struct lh_table *table, unsigned int free_table, unsigned int free_data);

/* initialise an already allocated lh_table to size size
 *
 * returns 1 on success
 * returns 0 on error
 */
unsigned int lh_init(struct lh_table *table, size_t size);

/* resize an existing table to new_size
 * this will reshuffle all the buckets around
 *
 * you can use this to make a hash larger or smaller
 *
 * returns 1 on success
 * returns 0 on error
 */
unsigned int lh_resize(struct lh_table *table, size_t new_size);

/* check if the supplied key already exists in this hash
 *
 * returns 1 on success (key exists)
 * returns 0 if key doesn't exist or on error
 */
unsigned int lh_exists(const struct lh_table *table, const char *key);

/* insert `data` under `key`
 * this will only success if !lh_exists(table, key)
 *
 * returns 1 on success
 * returns 0 on error
 */
unsigned int lh_insert(struct lh_table *table, const char *key, void *data);

/* set `data` under `key`
 * this will only succeed if lh_exists(table, key)
 *
 * returns old data on success
 * returns 0 on error
 */
void * lh_set(struct lh_table *table, const char *key, void *data);

/* get `data` stored under `key`
 *
 * returns data on success
 * returns 0 on error
 */
void * lh_get(const struct lh_table *table, const char *key);

/* delete entry stored under `key`
 *
 * returns data on success
 * returns 0 on error
 */
void *  lh_delete(struct lh_table *table, const char *key);

#endif // ifndef LINEAR_HASH_H

