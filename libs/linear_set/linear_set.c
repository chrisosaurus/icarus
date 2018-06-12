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

#include <stdio.h> /* puts, printf */
#include <limits.h> /* ULONG_MAX */

#include <stdlib.h> /* calloc, free */
#include <string.h> /* strcmp, strlen */
#include <stddef.h> /* size_t */

#include "linear_set.h"
#include "linear_set_internal.h"

/* default number of slots */
#define LS_DEFALT_SIZE  32

/* factor we grow the number of slots by each resize */
#define LS_SCALING_FACTOR 2

/* default loading factor (percentage) we resize at
 *
 * 1 through to 100 (inclusive)
 *
 * default is 60 so 60 %
 */
#define LS_DEFALT_THRESHOLD 60

/**********************************************
 **********************************************
 **********************************************
 ******** simple helper functions *************
 **********************************************
 **********************************************
 ***********************************************/

/* NOTE these helper functions are not exposed in our header
 * but are not static so as to allow easy unit testing
 * or extension
 */

/* logic for testing if the current entry is eq to the
 * provided hash, key_len and key
 * this is to centralise the once scattered logic
 */
unsigned int ls_entry_eq(const struct ls_entry *cur, unsigned long int hash, unsigned long int key_len, const char *key){
    if( ! cur ){
        puts("ls_entry_eq: cur was null");
        return 0;
    }
    if( ! key ){
        puts("ls_entry_eq: key was null");
        return 0;
    }

    if( cur->hash != hash ){
        return 0;
    }

    if( cur->key_len != key_len ){
        return 0;
    }

    if( strncmp(key, cur->key, key_len) ){
        return 0;
    }

    return 1;
}

/* internal strdup equivalent
 *
 * returns char* to new memory containing a strcpy on success
 * returns 0 on failure
 */
char * ls_strdupn(const char *str, size_t len){
    /* our new string */
    char *new_str = 0;

    if( ! str ){
        puts("ls_strdupn: str undef");
        return 0;
    }

    /* if len is 0 issue a warning and recalculate
     * note that if strlen is still 0 then all is well
     */
    if( len == 0 ){
        puts("ls_strdupn: provided len was 0, recalculating");
        len = strlen(str);
    }

    /* allocate our new string
     * len + 1 to fit null terminator
     */
    new_str = calloc(len + 1, sizeof(char));
    if( ! new_str ){
        puts("ls_strdupn: call to calloc failed");
        return 0;
    }

    /* perform copy */
    strncpy(new_str, str, len);

    /* ensure null terminator
     * do not rely on calloc as we may switch
     * to alt. alloc. later
     */
    new_str[len] = '\0';

    return new_str;
}

/* initialise an existing ls_entry
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ls_entry_init(struct ls_entry *entry,
                                       unsigned long int hash,
                                       const char *key,
                                       size_t key_len){

    if( ! entry ){
        puts("ls_entry_init: entry was null");
        return 0;
    }

    if( ! key ){
        puts("ls_entry_init: key was null");
        return 0;
    }

    /* we allow data to be null */

    /* we allow next to be null */

    /* if key_len is 0 we issue a warning and recalcualte */
    if( key_len == 0 ){
        puts("warning ls_entry_init: provided key_len was 0, recalcuating");
        key_len = strlen(key);
    }

    /* if hash is 0 we issue a warning and recalculate */
    if( hash == 0 ){
        puts("warning ls_entry_init: provided hash was 0, recalculating");
        hash = ls_hash(key, key_len);
    }

    /* setup our simple fields */
    entry->hash    = hash;
    entry->key_len = key_len;
    entry->state   = LS_ENTRY_OCCUPIED;

    /* we duplicate the string */
    entry->key = ls_strdupn(key, key_len);
    if( ! entry->key ){
        puts("ls_entry_init: call to ls_strdupn failed");
        return 0;
    }

    /* return success */
    return 1;
}

/* destroy entry
 *
 * will NOT free *next
 * will free all other values
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ls_entry_destroy(struct ls_entry *entry){
    if( ! entry ){
        puts("ls_entry_destroy: entry undef");
        return 0;
    }

    /* free key as strdup */
    free(entry->key);

    return 1;
}


/* find the ls_entry that should be holding this key
 *
 * returns a pointer to it on success
 * return 0 on failure
 */
struct ls_entry * ls_find_entry(const struct ls_set *set, const char *key){
    /* our cur entry */
    struct ls_entry *cur = 0;

    /* hash */
    unsigned long int hash = 0;
    /* position in hash table */
    size_t pos = 0;
    /* iterator through entries */
    size_t i = 0;
    /* cached strlen */
    size_t key_len = 0;


    if( ! set ){
        puts("ls_find_entry: set undef");
        return 0;
    }

    if( ! key ){
        puts("ls_find_entry: key undef");
        return 0;
    }

    /* cache strlen */
    key_len = strlen(key);

    /* calculate hash */
    hash = ls_hash(key, key_len);

    /* calculate pos
     * we know set is defined here
     * so ls_pos cannot fail
     */
    pos = ls_pos(hash, set->size);

    /* search pos..size */
    for( i=pos; i < set->size; ++i ){
        cur = &(set->entries[i]);

        /* if this is an empty then we stop */
        if( cur->state == LS_ENTRY_EMPTY ){
            /* failed to find element */
#ifdef DEBUG
            puts("ls_find_entry: failed to find key, encountered empty");
#endif
            return 0;
        }

        /* if this is a dummy then we skip but continue */
        if( cur->state == LS_ENTRY_DUMMY ){
            continue;
        }

        if( ! ls_entry_eq(cur, hash, key_len, key) ){
            continue;
        }

        return cur;
    }

    /* search 0..pos */
    for( i=0; i < pos; ++i ){
        cur = &(set->entries[i]);

        /* if this is an empty then we stop */
        if( cur->state == LS_ENTRY_EMPTY ){
            /* failed to find element */
#ifdef DEBUG
            puts("ls_find_entry: failed to find key, encountered empty");
#endif
            return 0;
        }

        /* if this is a dummy then we skip but continue */
        if( cur->state == LS_ENTRY_DUMMY ){
            continue;
        }

        if( ! ls_entry_eq(cur, hash, key_len, key) ){
            continue;
        }

        return cur;
    }

    /* failed to find element */
#ifdef DEBUG
    puts("ls_find_entry: failed to find key");
#endif
    return 0;
}



/**********************************************
 **********************************************
 **********************************************
 ******** linear_set.h implementation ********
 **********************************************
 **********************************************
 ***********************************************/

/* function to return number of elements
 *
 * returns number on success
 * returns 0 on failure
 */
unsigned int ls_nelems(const struct ls_set *set){
    if( ! set ){
        puts("ls_nelems: set was null");
        return 0;
    }

    return set->n_elems;
}

/* function to calculate load
 * (set->n_elems * 100) / set->size
 *
 * returns loading factor 0 -> 10 on success
 * returns 0 on failure
 */
unsigned int ls_load(const struct ls_set *set){
    if( ! set ){
        puts("ls_load: set was null");
        return 0;
    }

    return (set->n_elems * 100) / set->size;
}

/* set the load that we resize at
 * load is (set->n_elems * 100) / set->size
 *
 * this sets ls_set->threshold
 * this defaults to LS_DEFALT_THRESHOLD in linear_set.c
 * this is set to 60 (meaning 60% full) by default
 *
 * this will accept any value between 1 (1%) to 100 (100%)
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ls_tune_threshold(struct ls_set *set, unsigned int threshold){
    if( ! set ){
        puts("ls_tune_threshold: set was null");
        return 0;
    }

    if( threshold < 1 || threshold > 100 ){
        puts("ls_tune_threshold: threshold must be between 1 (1%) and 100 (100%) (inclusive)");
        return 0;
    }

    set->threshold = threshold;
    return 1;
}


/* takes a char* representing a string
 *
 * will recalculate key_len if 0
 *
 * returns an unsigned long integer hash value on success
 * returns 0 on failure
 */
unsigned long int ls_hash(const char *key, size_t key_len){
    /* our hash value */
    unsigned long int hash = 0;
    /* our iterator through the key */
    size_t i = 0;

    if( ! key ){
        puts("ls_hash: key undef");
        return 0;
    }

    /* we allow key_len to be 0
     * we issue a warning and then recalculate
     */
    if( ! key_len ){
        puts("ls_hash: key_len was 0, recalculating");
        key_len = strlen(key);
    }

#ifdef DEBUG
    printf("ls_hash: hashing string '%s'\n", key);
#endif

    /* hashing time */
    for( i=0; i < key_len; ++i ){

#ifdef DEBUG
    printf("ls_hash: looking at i '%zd', char '%c'\n", i, key[i]);
#endif

        /* we do not have to worry about overflow doing silly things:
         *
         * C99 section 6.2.5.9 page 34:
         * A computation involving unsigned operands can never overﬂow,
         * because a result that cannot be represented by the resulting
         * unsigned integer type is reduced modulo the number that is one
         * greater than the largest value that can be represented by the
         * resulting type.
         */

        /* hash this character
         * http://www.cse.yorku.ca/~oz/hash.html
         * djb2
         */
        hash = ((hash << 5) + hash) + key[i];
    }

#ifdef DEBUG
    printf("ls_hash: success for key '%s', hash value '%zd'\n", key, hash);
#endif
    return hash;
}

/* takes a set and a hash value
 *
 * returns the index into the set for this hash
 * returns 0 on failure (if set is null)
 *
 * note the error value is indistinguishable from the 0th bucket
 * this function can only error if set is null
 * so the caller can distinguish these 2 cases
 */
size_t ls_pos(unsigned long int hash, size_t set_size){

    /* force hash value into a bucket */
    return hash % set_size;
}

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
struct ls_set * ls_new(void){
    struct ls_set *sht = 0;

    /* alloc */
    sht = calloc(1, sizeof(struct ls_set));
    if( ! sht ){
        puts("ls_new: calloc failed");
        return 0;
    }

    /* init */
    if( ! ls_init(sht, LS_DEFALT_SIZE) ){
        puts("ls_new: call to ls_init failed");
        free(sht);
        return 0;
    }

    return sht;
}

/* free an existing ls_set
 * this will free all the sh entries stored
 * this will free all the keys (as they are strdup-ed)
 *
 * this will only free the *set pointer if `free_set` is set to 1
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ls_destroy(struct ls_set *set, unsigned int free_set){
    /* iterator through set */
    size_t i = 0;

    if( ! set ){
        puts("ls_destroy: set undef");
        return 0;
    }

    /* iterate through `entries` list
     * calling ls_entry_destroy on each
     */
    for( i=0; i < set->size; ++i ){
        if( ! ls_entry_destroy( &(set->entries[i]) ) ){
            puts("ls_destroy: call to ls_entry_destroy failed, continuing...");
        }
    }

    /* free entires set */
    free(set->entries);

    /* finally free set if asked to */
    if( free_set ){
        free(set);
    }

    return 1;
}

/* initialise an already allocated ls_set to size size
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ls_init(struct ls_set *set, size_t size){
    if( ! set ){
        puts("ls_init: set undef");
        return 0;
    }

    if( size == 0 ){
        puts("ls_init: specified size of 0, impossible");
        return 0;
    }

    set->size      = size;
    set->n_elems   = 0;
    set->threshold = LS_DEFALT_THRESHOLD;

    /* calloc our buckets (pointer to ls_entry) */
    set->entries = calloc(size, sizeof(struct ls_entry));
    if( ! set->entries ){
        puts("ls_init: calloc failed");
        return 0;
    }

    return 1;
}

/* resize an existing set to new_size
 * this will reshuffle all the buckets around
 *
 * you can use this to make a hash larger or smaller
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ls_resize(struct ls_set *set, size_t new_size){
    /* our new data area */
    struct ls_entry *new_entries = 0;
    /* the current entry we are copying across */
    struct ls_entry *cur = 0;
    /* our iterator through the old hash */
    size_t i = 0;
    /* our iterator through the new data */
    size_t j = 0;
    /* our new position for each element */
    size_t new_pos = 0;

    if( ! set ){
        puts("ls_resize: set was null");
        return 0;
    }

    if( new_size == 0 ){
        puts("ls_resize: asked for new_size of 0, impossible");
        return 0;
    }

    if( new_size <= set->n_elems ){
        puts("ls_resize: asked for new_size smaller than number of existing elements, impossible");
        return 0;
    }

    /* allocate an array of ls_entry */
    new_entries = calloc(new_size, sizeof(struct ls_entry));
    if( ! new_entries ){
        puts("ls_resize: call to calloc failed");
        return 0;
    }

    /* iterate through old data */
    for( i=0; i < set->size; ++i ){
        cur = &(set->entries[i]);

        /* if we are not occupied then skip */
        if( cur->state != LS_ENTRY_OCCUPIED ){
            continue;
        }

        /* our position within new entries */
        new_pos = ls_pos(cur->hash, new_size);

        for( j = new_pos; j < new_size; ++ j){
            /* skip if not empty */
            if( new_entries[j].state != LS_ENTRY_EMPTY ){
                continue;
            }
            goto LS_RESIZE_FOUND;
        }

        for( j = 0; j < new_pos; ++ j){
            /* skip if not empty */
            if( new_entries[j].state != LS_ENTRY_EMPTY ){
                continue;
            }
            goto LS_RESIZE_FOUND;
        }

        puts("ls_resize: failed to find spot for new element!");
        /* make sure to free our new_entries since we don't store them
         * no need to free items in as they are still held in our old elems
         */
        free(new_entries);
        return 0;

LS_RESIZE_FOUND:
        new_entries[j].hash    = cur->hash;
        new_entries[j].key     = cur->key;
        new_entries[j].key_len = cur->key_len;
        new_entries[j].state   = cur->state;
    }

    /* free old data */
    free(set->entries);

    /* swap */
    set->size = new_size;
    set->entries = new_entries;

    return 1;
}

/* check if the supplied key already exists in this hash
 *
 * returns 1 on success (key exists)
 * returns 0 if key doesn't exist or on failure
 */
unsigned int ls_exists(const struct ls_set *set, const char *key){
    struct ls_entry *she = 0;

    if( ! set ){
        puts("ls_exists: set undef");
        return 0;
    }

    if( ! key ){
        puts("ls_exists: key undef");
        return 0;
    }

#ifdef DEBUG
    printf("ls_exist: called with key '%s', dispatching to ls_find_entry\n", key);
#endif

    /* find entry */
    she = ls_find_entry(set, key);
    if( ! she ){
        /* not found */
        return 0;
    }

    /* found */
    return 1;
}

/* insert `key`
 * this will only success if !ls_exists(set, key)
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ls_insert(struct ls_set *set, const char *key){
    /* our new entry */
    struct ls_entry *she = 0;
    /* hash */
    unsigned long int hash = 0;
    /* position in hash table */
    size_t pos = 0;
    /* iterator through set */
    size_t i = 0;
    /* cached strlen */
    size_t key_len = 0;

    if( ! set ){
        puts("ls_insert: set undef");
        return 0;
    }

    if( ! key ){
        puts("ls_insert: key undef");
        return 0;
    }

#ifdef DEBUG
    printf("ls_insert: asked to insert for key '%s'\n", key);
#endif

    /* we allow data to be 0 */

#ifdef DEBUG
    puts("ls_insert: calling ls_exists");
#endif

    /* check for already existing key
     * insert only works if the key is not already present
     */
    if( ls_exists(set, key) ){
#ifdef DEBUG
        puts("ls_insert: key already exists in set");
#endif
        return 0;
    }

    /* determine if we have to resize
     * note we are checking the load before the insert
     */
    if( ls_load(set) >= set->threshold ){
        if( ! ls_resize(set, set->size * LS_SCALING_FACTOR) ){
            puts("ls_insert: call to ls_resize failed");
            return 0;
        }
    }

    /* cache strlen */
    key_len = strlen(key);

    /* calculate hash */
    hash = ls_hash(key, key_len);

    /* calculate pos
     * we know set is defined here
     * so ls_pos cannot fail
     */
    pos = ls_pos(hash, set->size);

#ifdef DEBUG
    printf("ls_insert: trying to insert key '%s', hash value '%zd', starting at pos '%zd'\n", key, hash, pos);
#endif

    /* iterate from pos to size */
    for( i=pos; i < set->size; ++i ){
        she = &(set->entries[i]);
        /* if taken keep searching */
        if( she->state == LS_ENTRY_OCCUPIED ){
            continue;
        }

        /* otherwise (empty or dummy) jump to found */
        goto LS_INSERT_FOUND;
    }

    /* iterate from 0 to pos */
    for( i=0; i < pos; ++i ){
        she = &(set->entries[i]);
        /* if taken keep searching */
        if( she->state == LS_ENTRY_OCCUPIED ){
            continue;
        }

        /* otherwise (empty or dummy) jump to found */
        goto LS_INSERT_FOUND;
    }

    /* no slot found */
    puts("ls_insert: unable to find insertion slot");
    return 0;

LS_INSERT_FOUND:
    /* she is already set! */

#ifdef DEBUG
    printf("ls_insert: inserting insert key '%s', hash value '%zd', starting at pos '%zd', into '%zd'\n", key, hash, pos, i);
#endif

    /* construct our new ls_entry
     * ls_entry_new(unsigned long int hash,
     *              char *key,
     *              size_t key_len,
     *              void *data,
     *              struct ls_entry *next){
     *
     * only key needs to be defined
     *
     */
    /*                 (entry, hash, key, key_len, data) */
    if( ! ls_entry_init(she,   hash, key, key_len) ){
        puts("ls_insert: call to ls_entry_init failed");
        return 0;
    }

    /* increment number of elements */
    ++set->n_elems;

    /* return success */
    return 1;
}


/* delete key `key`
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ls_delete(struct ls_set *set, const char *key){
    /* our cur entry */
    struct ls_entry *cur = 0;
    /* hash */
    unsigned long int hash = 0;
    /* position in hash table */
    size_t pos = 0;
    /* iterator through has set */
    size_t i =0;
    /* cached strlen */
    size_t key_len = 0;

    if( ! set ){
        puts("ls_delete: set undef");
        return 0;
    }

    if( ! key ){
        puts("ls_delete: key undef");
        return 0;
    }

    /* cache strlen */
    key_len = strlen(key);

    /* calculate hash */
    hash = ls_hash(key, key_len);

    /* calculate pos
     * we know set is defined here
     * so ls_pos cannot fail
     */
    pos = ls_pos(hash, set->size);


    /* starting at pos search for element
     * searches pos .. size
     */
    for( i = pos; i < set->size ; ++i ){
        cur = &(set->entries[i]);

        /* if this is an empty then we stop */
        if( cur->state == LS_ENTRY_EMPTY ){
            /* failed to find element */
#ifdef DEBUG
            puts("ls_delete: failed to find key, encountered empty");
#endif
            return 0;
        }

        /* if this is a dummy then we skip but continue */
        if( cur->state == LS_ENTRY_DUMMY ){
            continue;
        }

        if( ! ls_entry_eq(cur, hash, key_len, key) ){
            continue;
        }

        goto LS_DELETE_FOUND;
    }

    /* if we are here then we hit the end,
     * searches 0 .. pos
     */
    for( i = 0; i < pos; ++i ){
        cur = &(set->entries[i]);

        /* if this is an empty then we stop */
        if( cur->state == LS_ENTRY_EMPTY ){
            /* failed to find element */
#ifdef DEBUG
            puts("ls_delete: failed to find key, encountered empty");
#endif
            return 0;
        }

        /* if this is a dummy then we skip but continue */
        if( cur->state == LS_ENTRY_DUMMY ){
            continue;
        }

        if( ! ls_entry_eq(cur, hash, key_len, key) ){
            continue;
        }

        goto LS_DELETE_FOUND;
    }

    /* failed to find element */
    puts("ls_delete: failed to find key, both loops terminated");
    return 0;

LS_DELETE_FOUND:
        /* cur is already set! */

        /* clear out */
        cur->key = 0;
        cur->key_len = 0;
        cur->hash = 0;
        cur->state = LS_ENTRY_DUMMY;

        /* decrement number of elements */
        --set->n_elems;

        /* return old data */
        return 1;
}

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
unsigned int ls_iterate(struct ls_set *set, void *state, unsigned int (*each)(void *state, const char *key)){
    /* index into table we are considering */
    unsigned int i = 0;
    unsigned int len = 0;
    /* current entry we are considering */
    struct ls_entry *entry = 0;
    /* return value from user supplied function */
    unsigned int ret = 0;

    if( ! set ){
        puts("ls_iterate: set undef");
        return 0;
    }

    if( ! each ){
        puts("ls_iterate: each undef");
        return 0;
    }

    len = set->size;

    for( i=0; i<len; ++i ){
        entry = &(set->entries[i]);

        if( entry->state == LS_ENTRY_OCCUPIED ){
            ret = each(state, entry->key);

            /* if ret == 0 then user wants us to stop */
            if( ret == 0 ){
                return 1;
            }
        }
    }

    return 1;

}

