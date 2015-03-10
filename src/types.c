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


