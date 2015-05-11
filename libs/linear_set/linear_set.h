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

#ifndef linear_hals_H
#define linear_hals_H

#include <stddef.h> /* size_t */

enum ls_entry_state {
    ls_ENTRY_EMPTY,
    ls_ENTRY_OCCUPIED,
    ls_ENTRY_DUMMY // was occupied but now delete
};

struct ls_entry {
    enum ls_entry_state state;
    /* hash value for this entry, output of ls_hash(key) */
    unsigned long int hash;
    /* string copied using ls_strdup (defined in linear_set.c) */
    char *key;
    /* strlen of key, simple cache */
    size_t key_len;
};

struct ls_set {
    /* number of slots in hash */
    size_t size;
    /* number of elements stored in hash */
    size_t n_elems;
    /* threshold that triggers an automatic resize */
    unsigned int threshold;
    /* array of ls_entry(s) */
    struct ls_entry *entries;
};

/* function to calculate load
 * (table->n_elems * 10) / table->size
 *
 * returns loading factor 0 -> 10 on success
 * returns 0 on failure
 */
unsigned int ls_load(struct ls_set *table);

/* set the load that we resize at
 * load is (table->n_elems * 10) / table->size
 *
 * this sets ls_set->threshold
 * this defaults to ls_DEFAULT_THRESHOLD in linear_set.c
 * this is set to 6 (meaning 60% full) by default
 *
 * this will accept any value between 1 (10%) to 10 (100%)
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ls_tune_threshold(struct ls_set *table, unsigned int threshold);

/* takes a char* representing a string
 * and a key_len of it's size
 *
 * will recalculate key_len if 0
 *
 * returns an unsigned long integer hash value on success
 * returns 0 on error
 */
unsigned long int ls_hash(char *key, size_t key_len);

/* takes a table and a hash value
 *
 * returns the index into the table for this hash
 * returns 0 on error (if table is null)
 *
 * note the error value is indistinguishable from the 0th bucket
 * this function can only error if table is null
 * so the caller can distinguish these 2 cases
 */
size_t ls_pos(unsigned long int hash, size_t table_size);

/* allocate and initialise a new ls_set
 *
 * will automatically assume a size of 32
 *
 * ls_set will automatically resize when a call to
 * ls_insert detects the load factor is over table->threshold
 *
 * returns pointer on success
 * returns 0 on error
 */
struct ls_set * ls_new(void);

/* free an existing ls_set
 * this will free all the sh entries stored
 * this will free all the keys (as they are strdup-ed)
 *
 * this will only free the *table pointer if `free_table` is set to 1
 *
 * returns 1 on success
 * returns 0 on error
 */
unsigned int ls_destroy(struct ls_set *table, unsigned int free_table);

/* initialise an already allocated ls_set to size size
 *
 * returns 1 on success
 * returns 0 on error
 */
unsigned int ls_init(struct ls_set *table, size_t size);

/* resize an existing table to new_size
 * this will reshuffle all the buckets around
 *
 * you can use this to make a hash larger or smaller
 *
 * you do not need to manually call this in the general case
 * as linear set will manage it's own size
 *
 * returns 1 on success
 * returns 0 on error
 */
unsigned int ls_resize(struct ls_set *table, size_t new_size);

/* check if the supplied key already exists in this hash
 *
 * returns 1 on success (key exists)
 * returns 0 if key doesn't exist or on error
 */
unsigned int ls_exists(struct ls_set *table, char *key);

/* insert `key`
 * this will only success if !ls_exists(table, key)
 *
 * returns 1 on success
 * returns 0 on error
 */
unsigned int ls_insert(struct ls_set *table, char *key);

/* delete key `key`
 *
 * returns 1 on success
 * returns 0 on error
 */
unsigned int ls_delete(struct ls_set *table, char *key);


#endif // ifndef linear_hals_H
