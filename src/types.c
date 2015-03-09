#include <stdlib.h> /* calloc */
#include <stdio.h> /* puts */

#include "types.h"

/* allocate a new symbol of length len
 * containing contents
 *
 * returns symbol on success
 * returns 0 on failure
 */
struct ic_symbol * ic_symbol_new(unsigned int len, char *contents){
    struct ic_symbol *sym;

    sym = calloc(1, sizeof(struct ic_symbol));
    if( ! sym ){
        puts("ic_symbol_new: calloc failed");
        return 0;
    }

    sym->len = len;
    sym->sym = contents;

    return sym;
}

/* allocate a new string of length len
 * containing contents
 *
 * returns string on success
 * returns 0 on failure
 */
struct ic_string * ic_string_new(unsigned int len, char *contents){
    struct ic_string *str;

    str = calloc(1, sizeof(struct ic_string));
    if( ! str ){
        puts("ic_string_new: calloc failed");
        return 0;
    }

    str->len = len;
    str->cap = len;
    str->str = contents;

    return str;
}

/* allocate a new int containing value
 *
 * returns int on success
 * returns 0 on failure
 */
struct ic_int * ic_int_new(int value){
    struct ic_int *integer;

    integer = calloc(1, sizeof(struct ic_int));
    if( ! integer ){
        puts("ic_int_new: calloc failed");
        return 0;
    }

    integer->value = value;

    return integer;
}

/* allocate a new array of length len
 *
 * returns array on success
 * returns 0 on failure
 */
struct ic_array * ic_array_new(unsigned int len){
    struct ic_array *arr;

    arr = calloc(1, sizeof(struct ic_array));
    if( ! arr ){
        puts("ic_array_new: calloc of struct failed");
        return 0;
    }

    arr->len = len;
    arr->cap = len;

    arr->contents = calloc(len, sizeof(void*));
    if( ! arr->contents ){
        puts("ic_array_new: calloc of contents failed");
        return 0;
    }

    return arr;
}

/* get item at pos
 * bounds checked
 *
 * returns item on success
 * return 0 on failure
 */
void * ic_array_get(struct ic_array *arr, unsigned int pos){
    if( ! arr ){
        return 0;
    }

    if( pos >= arr->len ){
        /* out of bounds */
        return 0;
    }

    return arr->contents[pos];
}

/* returns 0 on successful set
 * returns 1 on failure
 */
int ic_array_set(struct ic_array *arr, unsigned int pos, void *val){
    if( ! arr ){
        return 1;
    }

    if( pos >= arr->len ){
        /* out of bounds */
        return 1;
    }

    arr->contents[pos] = val;
    return 0;
}


