#include <stdio.h> /* puts, fprintf */
#include <stdlib.h> /* calloc */
#include <string.h> /* strncpy */

#include "string.h"

/* FIXME crutch for unused param */
#pragma GCC diagnostic ignored "-Wunused-parameter"

/* build a new string from a char* and a length
 * this will allocate a new buffer and strncpy n
 * chars from source into the string
 * then it will add the null terminator
 * returns new string on success
 * returns 0 on failure
 */
struct ic_string * ic_string_new(char *source, unsigned int len){
    struct ic_string *str = calloc(1, sizeof(struct ic_string));
    if( ! str ){
        puts("ic_string_new: call to calloc failed");
        return 0;
    }

    if( ic_string_init(str, source, len) ){
        puts("ic_string_new: error in call to ic_string_init");
        free(str);
        return 0;
    }

    return str;
}

/* initalise an existing string from a char* and len
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_string_init(struct ic_string *str, char *source, unsigned int len){
    if( ! str ){
        puts("ic_string_init: pass in string was null");
        return 1;
    }

    /* our passed in len does not include the null terminator
     * but carray must take this into account
     */
    if( ic_carray_init( &(str->backing), (len + 1) ) ){
        puts("ic_string_init: call to ic_carray_init failed");
        return 1;
    }

    if( ! str->backing.contents ){
        puts("ic_string_init: results of ic_carray_init were suspicious");
        return 1;
    }

    /* strncpy source into string */
    strncpy(str->backing.contents, source, len);

    /* insert null terminator */
    str->backing.contents[len+1] = '\0';

    /* set the number of used chars
     * NB: this does not include the null terminator
     */
    str->used = len;

    return 0;

}

/* destroy string
 *
 * this will only free this string is `free_str` is true
 *
 * the caller must determine if it is appropriate or not
 * to not to call free(str)
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_string_destroy(struct ic_string *str, unsigned int free_str){
    if( ! str ){
        puts("ic_string_destroy: passed in string was null");
        return 1;
    }

    /* dispatch to carray for dirty work
     * note we do not set free_carray as
     * it is an element on us
     */
    if( ic_carray_destroy( &(str->backing), 0 ) ){
        puts("ic_string_destroy: call to ic_carray_destroy failed");
        return 1;
    }

    /* free string if asked */
    if( free_str ){
        free(str);
    }

    return 0;
}

/* returns backing character array
 * the caller is NOT allowed to mutate this character array directly
 *
 * returns a char * on success
 * returns 0 on error
 */
char * ic_string_contents(struct ic_string *string){
    if( ! string ){
        puts("ic_string_contents: null string passed in");
        return 0;
    }

    return string->backing.contents;
}

/* get the strlen of the stores string
 * this length does NOT include the null terminator
 *
 * returns numbers of characters in string on success
 * returns -1 on error
 */
int ic_string_length(struct ic_string *string){
    if( ! string ){
        puts("ic_string_contents: null string passed in");
        return -1;
    }

    /* string->used is the strlen of the underlying string
     * this is the number of character slots used in the array
     * and does NOT include the null terminator
     */
    return string->used;
}

/* get a character from the string
 *
 * you may get any character up to and including the '\0'
 *
 * bounds checked
 *
 * returns character on success
 * returns 0 on failure
 */
char ic_string_get(struct ic_string *string, unsigned int offset){
    if( ! string ){
        puts("ic_string_get: null string passed in");
        return 0;
    }

    /* used does not include null terminator
     * here we allow for equality as we allow the user
     * to fetch the null terminator
     */
    if( offset > string->used ){
        printf("ic_string_get: offset '%d' out of range '%d'\n", offset, string->used );
        return 0;
    }

    return ic_carray_get(&(string->backing), offset);
}

/* set character at [pos] to val
 *
 * pos must not be the null terminator
 * pos must be < used
 *
 * val must not be '\0'
 *
 * bounds checked
 *
 * returns 0 on success
 * returns 1 on failure
 */
char ic_string_set(struct ic_string *string, unsigned int pos, char val){
    if( ! string ){
        puts("ic_string_set: null string passed in");
        return 1;
    }

    /* check we are within range
     * we do NOT allow the null terminator to be fiddled with through this */
    if( pos >= string->used ){
        puts("ic_string_set: requested pos was out of range");
        return 1;
    }

    /* we do also not allow val to be the null terminator
     * as this would screw with out string constraint
     */
    if( val == '\0' ){
        puts("ic_string_set: provided val was null terminator");
        return 1;
    }

    if( ic_carray_set(&(string->backing), pos, val) ){
        puts("ic_string_set: call to ic_carray_set failed");
        return 1;
    }

    return 0;

}

/* append the contents of `from` to `to`
 * this will resize `to` to guarantee there is enough space
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_string_append(struct ic_string *to, struct ic_string *from){
    unsigned int len = 0;
    unsigned int from_len = 0;

    if( ! to ){
        puts("ic_string_append: to string was null");
        return 1;
    }
    if( ! from ){
        puts("ic_string_append: from string was null");
        return 1;
    }

    /* cache the result */
    from_len = ic_string_length(from);

    /* calculate our desired length */
    len = 1; /* null pointer */
    len += ic_string_length(to);
    len += from_len;

    /* make sure our carray is large enough */
    if( ic_carray_ensure(&(to->backing), len) ){
        puts("ic_string_append: call to ic_carray_ensure failed");
        return 1;
    }

    /* store our string, note that we do not include the null terminator here */
    to->used = (len - 1);

    /* concat the strings together */
    strncat( ic_string_contents(to), ic_string_contents(from), ic_string_length(from) );

    /* ensure string */
    if( ic_carray_set( &(to->backing), to->used, '\0') ){
        puts("ic_string_append: call to ic_carray_set failed");
        return 1;
    }

    /* success */
    return 0;
}

/* append the contents of `from` to `to`
 * copying over a maximum of from_len
 *
 * this will resize `to` to guarantee there is enough space
 *
 * this will also make sure the final character internally is a \0
 * even if the character at from[from_len - 1] != '\0'
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_string_append_char(struct ic_string *to, char *from, unsigned int from_len){
    unsigned int len = 0;

    if( ! to ){
        puts("ic_string_append_char: to string was null");
        return 1;
    }
    if( ! from ){
        puts("ic_string_append_char: from string was null");
        return 1;
    }

    /* calculate our desired length */
    len = 1; /* null pointer */
    len += ic_string_length(to);
    len += from_len;

    /* make sure our carray is large enough */
    if( ic_carray_ensure(&(to->backing), len) ){
        puts("ic_string_append_char: call to ic_carray_ensure failed");
        return 1;
    }

    /* store our string, note that we do not include the null terminator here */
    to->used = (len - 1);

    /* concat the strings together */
    strncat( ic_string_contents(to), from, from_len );

    /* ensure string */
    if( ic_carray_set( &(to->backing), to->used, '\0') ){
        puts("ic_string_append_char: call to ic_carray_set failed");
        return 1;
    }

    /* success */
    return 0;

}


