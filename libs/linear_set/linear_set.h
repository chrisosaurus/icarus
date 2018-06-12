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

#ifndef LINEAR_SET_H
#define LINEAR_SET_H

#include <stddef.h> /* size_t */

enum ls_entry_state {
    LS_ENTRY_EMPTY,
    LS_ENTRY_OCCUPIED,
    LS_ENTRY_DUMMY /* was occupied but now delete */
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
    /* threshold percentage that triggers an automatic resize
     * 1 <= threshold >= 100
     * if (((n_elems * 100) / size) >= threshold) { resize }
     */
    unsigned int threshold;
    /* array of ls_entry(s) */
    struct ls_entry *entries;
};

/* function to return number of elements
 *
 * returns number on success
 * returns 0 on failure
 */
unsigned int ls_nelems(const struct ls_set *set);

/* function to calculate load
 * (set->n_elems * 100) / set->size
 *
 * returns loading factor 0 -> 100 on success
 * returns 0 on failure
 */
unsigned int ls_load(const struct ls_set *set);

/* set the load that we resize at
 * load is (set->n_elems * 10) / set->size
 *
 * this sets ls_set->threshold
 * this defaults to ls_DEFAULT_THRESHOLD in linear_set.c
 * this is set to 60 (meaning 60% full) by default
 *
 * this will accept any value between 1 (1%) to 100 (100%)
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ls_tune_threshold(struct ls_set *set, unsigned int threshold);

/* takes a char* representing a string
 * and a key_len of it's size
 *
 * will recalculate key_len if 0
 *
 * returns an unsigned long integer hash value on success
 * returns 0 on failure
 */
unsigned long int ls_hash(const char *key, size_t key_len);

/* takes a set and a hash value
 *
 * returns the index into the set for this hash
 * returns 0 on failure (if set is null)
 *
 * note the error value is indistinguishable from the 0th bucket
 * this function can only error if set is null
 * so the caller can distinguish these 2 cases
 */
size_t ls_pos(unsigned long int hash, size_t set_size);

/* allocate and initialise a new ls_set
 *
 * will automatically assume a size of 32
 *
 * ls_set will automatically resize when a call to
 * ls_insert detects the load factor is over set->threshold
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ls_set * ls_new(void);

/* free an existing ls_set
 * this will free all the sh entries stored
 * this will free all the keys (as they are strdup-ed)
 *
 * this will only free the *set pointer if `free_set` is set to 1
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ls_destroy(struct ls_set *set, unsigned int free_set);

/* initialise an already allocated ls_set to size size
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ls_init(struct ls_set *set, size_t size);

/* resize an existing set to new_size
 * this will reshuffle all the buckets around
 *
 * you can use this to make a hash larger or smaller
 *
 * you do not need to manually call this in the general case
 * as linear set will manage it's own size
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ls_resize(struct ls_set *set, size_t new_size);

/* check if the supplied key already exists in this hash
 *
 * returns 1 on success (key exists)
 * returns 0 if key doesn't exist or on failure
 */
unsigned int ls_exists(const struct ls_set *set, const char *key);

/* insert `key`
 * this will only success if !ls_exists(set, key)
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ls_insert(struct ls_set *set, const char *key);

/* delete key `key`
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ls_delete(struct ls_set *set, const char *key);

/* iterate through all keys in this set
 * calling the provided function on each key.
 *
 * the function cannot modify the key.
 * the function should not access the set in anyway including:
 *  modifying the set in anyway
 *  calling any other set functions
 *
 * the function will be given the value of the `state` pointer for each call,
 * this is useful for passing state between calls to the function as well as
 * for returning results
 *
 * the function should return
 *  1 if it wants the iteration to continue
 *  0 if it wants the iteration to stop
 *
 * returns 1 on success
 * returns 0 on success
 */
unsigned int ls_iterate(struct ls_set *set, void *state, unsigned int (*each)(void *state, const char *key));

#endif /* ifndef LINEAR_SET_H */

