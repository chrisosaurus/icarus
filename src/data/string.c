#include <stdio.h>  /* puts, fprintf */
#include <stdlib.h> /* calloc */
#include <string.h> /* strncpy */

#include "string.h"
#include "symbol.h"
#include "strdup.h"

/* build a new string from a char* and a length
 * this will allocate a new buffer and strncpy n
 * chars from source into the string
 * then it will add the null terminator
 * returns new string on success
 * returns 0 on failure
 */
struct ic_string *ic_string_new(char *source, unsigned int len) {
    struct ic_string *str = calloc(1, sizeof(struct ic_string));
    if (!str) {
        puts("ic_string_new: call to calloc failed");
        return 0;
    }

    if (!ic_string_init(str, source, len)) {
        puts("ic_string_new: error in call to ic_string_init");
        free(str);
        return 0;
    }

    return str;
}

/* build a new empty string
 *
 * this will allocate a buffer to store the null terminator
 *
 * returns new string on success
 * returns 0 on failure
 */
struct ic_string *ic_string_new_empty(void) {
    return ic_string_new("", 0);
}

/* initalise an existing string from a char* and len
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_string_init(struct ic_string *str, char *source, unsigned int len) {
    if (!str) {
        puts("ic_string_init: pass in string was null");
        return 0;
    }

    /* our passed in len does not include the null terminator
     * but carray must take this into account
     */
    if (!ic_carray_init(&(str->backing), (len + 1))) {
        puts("ic_string_init: call to ic_carray_init failed");
        return 0;
    }

    if (!str->backing.contents) {
        puts("ic_string_init: results of ic_carray_init were suspicious");
        return 0;
    }

    /* strncpy source into string */
    strncpy(str->backing.contents, source, len);

    /* insert null terminator at end of string
     * NB: we allocated (len+1) so [len] is last valid offset
     */
    str->backing.contents[len] = '\0';

    /* set the number of used chars
     * NB: this does not include the null terminator
     */
    str->used = len;

    return 1;
}

/* initialise an existing symbol to empty
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_string_init_empty(struct ic_string *string) {
    return ic_string_init(string, "", 0);
}

/* destroy string
 *
 * this will only free this string is `free_str` is true
 *
 * the caller must determine if it is appropriate or not
 * to not to call free(str)
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_string_destroy(struct ic_string *str, unsigned int free_str) {
    if (!str) {
        puts("ic_string_destroy: passed in string was null");
        return 0;
    }

    /* dispatch to carray for dirty work
     * note we do not set free_carray as
     * it is an element on us
     */
    if (!ic_carray_destroy(&(str->backing), 0)) {
        puts("ic_string_destroy: call to ic_carray_destroy failed");
        return 0;
    }

    /* free string if asked */
    if (free_str) {
        free(str);
    }

    return 1;
}

/* perform deep copy of string
 *
 * returns * on success
 * returns 0 on error
 */
struct ic_string *ic_string_deep_copy(struct ic_string *str) {
    struct ic_string *new_str = 0;

    if (!str) {
        puts("ic_string_deep_copy: str was null");
        return 0;
    }

    new_str = calloc(1, sizeof(struct ic_string));
    if (!new_str) {
        puts("ic_string_deep_copy: call to calloc failed");
        return 0;
    }

    new_str->backing.contents = ic_strdup(str->backing.contents);
    if (!new_str->backing.contents) {
        puts("ic_string_deep_copy: call to ic_strdup failed");
        return 0;
    }

    new_str->used = str->used;
    new_str->backing.len = str->backing.len;

    return new_str;
}

/* returns backing character array
 * the caller is NOT allowed to mutate this character array directly
 *
 * returns a char * on success
 * returns 0 on failure
 */
char *ic_string_contents(struct ic_string *string) {
    if (!string) {
        puts("ic_string_contents: null string passed in");
        return 0;
    }

    return string->backing.contents;
}

/* get the strlen of the stores string
 * this length does NOT include the null terminator
 *
 * returns numbers of characters in string on success
 * returns -1 on failure
 */
int ic_string_length(struct ic_string *string) {
    if (!string) {
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
char ic_string_get(struct ic_string *string, unsigned int offset) {
    if (!string) {
        puts("ic_string_get: null string passed in");
        return 0;
    }

    /* used does not include null terminator
     * here we allow for equality as we allow the user
     * to fetch the null terminator
     */
    if (offset > string->used) {
        printf("ic_string_get: offset '%d' out of range '%d'\n", offset, string->used);
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
 * returns 1 on success
 * returns 0 on failure
 */
char ic_string_set(struct ic_string *string, unsigned int pos, char val) {
    if (!string) {
        puts("ic_string_set: null string passed in");
        return 0;
    }

    /* check we are within range
     * we do NOT allow the null terminator to be fiddled with through this */
    if (pos >= string->used) {
        puts("ic_string_set: requested pos was out of range");
        return 0;
    }

    /* we do also not allow val to be the null terminator
     * as this would screw with out string constraint
     */
    if (val == '\0') {
        puts("ic_string_set: provided val was null terminator");
        return 0;
    }

    if (!ic_carray_set(&(string->backing), pos, val)) {
        puts("ic_string_set: call to ic_carray_set failed");
        return 0;
    }

    return 1;
}

/* append the contents of `from` to `to`
 * this will resize `to` to guarantee there is enough space
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_string_append(struct ic_string *to, struct ic_string *from) {
    unsigned int len = 0;
    unsigned int from_len = 0;

    if (!to) {
        puts("ic_string_append: to string was null");
        return 0;
    }
    if (!from) {
        puts("ic_string_append: from string was null");
        return 0;
    }

    /* cache the result */
    from_len = ic_string_length(from);

    /* calculate our desired length */
    len = 1; /* null pointer */
    len += ic_string_length(to);
    len += from_len;

    /* make sure our carray is large enough */
    if (!ic_carray_ensure(&(to->backing), len)) {
        puts("ic_string_append: call to ic_carray_ensure failed");
        return 0;
    }

    /* store our string, note that we do not include the null terminator here */
    to->used = (len - 1);

    /* concat the strings together */
    strncat(ic_string_contents(to), ic_string_contents(from), from_len);

    /* ensure string */
    if (!ic_carray_set(&(to->backing), to->used, '\0')) {
        puts("ic_string_append: call to ic_carray_set failed");
        return 0;
    }

    /* success */
    return 1;
}

/* append the contents of `from` to `to`
 * this will resize `to` to guarantee there is enough space
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_string_append_symbol(struct ic_string *to, struct ic_symbol *from) {
    unsigned int len = 0;
    unsigned int from_len = 0;

    if (!to) {
        puts("ic_string_append_symbol: to string was null");
        return 0;
    }
    if (!from) {
        puts("ic_string_append_symbol: from symbol was null");
        return 0;
    }

    /* cache the result */
    from_len = ic_symbol_length(from);

    /* calculate our desired length */
    len = 1; /* null pointer */
    len += ic_string_length(to);
    len += from_len;

    /* make sure our carray is large enough */
    if (!ic_carray_ensure(&(to->backing), len)) {
        puts("ic_string_append_symbol: call to ic_carray_ensure failed");
        return 0;
    }

    /* store our string, note that we do not include the null terminator here */
    to->used = (len - 1);

    /* concat the strings together */
    strncat(ic_string_contents(to), ic_symbol_contents(from), from_len);

    /* ensure string */
    if (!ic_carray_set(&(to->backing), to->used, '\0')) {
        puts("ic_string_append_symbol: call to ic_carray_set failed");
        return 0;
    }

    /* success */
    return 1;
}

/* append the contents of `from` to `to`
 * copying over a maximum of from_len
 *
 * this will resize `to` to guarantee there is enough space
 *
 * this will also make sure the final character internally is a \0
 * even if the character at from[from_len - 1] != '\0'
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_string_append_char(struct ic_string *to, char *from, unsigned int from_len) {
    unsigned int len = 0;

    if (!to) {
        puts("ic_string_append_char: to string was null");
        return 0;
    }
    if (!from) {
        puts("ic_string_append_char: from string was null");
        return 0;
    }

    /* calculate our desired length */
    len = 1; /* null pointer */
    len += ic_string_length(to);
    len += from_len;

    /* make sure our carray is large enough */
    if (!ic_carray_ensure(&(to->backing), len)) {
        puts("ic_string_append_char: call to ic_carray_ensure failed");
        return 0;
    }

    /* store our string, note that we do not include the null terminator here */
    to->used = (len - 1);

    /* concat the strings together */
    strncat(ic_string_contents(to), from, from_len);

    /* ensure string */
    if (!ic_carray_set(&(to->backing), to->used, '\0')) {
        puts("ic_string_append_char: call to ic_carray_set failed");
        return 0;
    }

    /* success */
    return 1;
}

/* append the contents of `from` to `to`
 * caller must ensure `from` is a c-string (null terminated)
 *
 * this will resize `to` to guarantee there is enough space
 *
 * this will also make sure the final character internally is a \0
 * even if the character at from[from_len - 1] != '\0'
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_string_append_cstr(struct ic_string *to, char *from) {
    unsigned int len = 0;

    if (!to) {
        puts("ic_string_append_cstr: to was null");
        return 0;
    }

    if (!from) {
        puts("ic_string_append_cstr: from was null");
        return 0;
    }

    len = strlen(from);
    if (!len) {
        puts("ic_string_append_cstr: call to strlen failed or 0 length string given");
        return 0;
    }

    if (!ic_string_append_char(to, from, len)) {
        puts("ic_string_append_cstr: call to ic_string_append_char failed");
        return 0;
    }

    return 1;
}

/* print this string */
void ic_string_print(FILE *fd, struct ic_string *string) {
    char *con;

    if (!string) {
        puts("ic_string_print: string was null");
        return;
    }

    con = ic_string_contents(string);
    if (!con) {
        puts("ic_string_print: call to ic_string_contents failed");
        return;
    }

    fprintf(fd, "%s", con);
}

/* check for equality on string
 *
 * returns 1 on equal
 * returns 0 on failure or non-equal
 */
unsigned int ic_string_equal(struct ic_string *a, struct ic_string *b) {
    char *b_ch = 0;
    if (!a) {
        puts("ic_string_equal: a was null");
        return 0;
    }

    if (!b) {
        puts("ic_string_equal: b was null");
        return 0;
    }

    b_ch = ic_string_contents(b);
    if (!b_ch) {
        puts("ic_string_equal: call to ic_string_contents failed");
        return 0;
    }

    return ic_string_equal_char(a, b_ch);
}

/* check for equality on string
 *
 * returns 1 on equal
 * returns 0 on failure or non-equal
 */
unsigned int ic_string_equal_char(struct ic_string *a, char *b) {
    char *a_ch = 0;
    if (!a) {
        puts("ic_string_equal_char: a was null");
        return 0;
    }

    if (!b) {
        puts("ic_string_equal_char: b was null");
        return 0;
    }

    a_ch = ic_string_contents(a);
    if (!a_ch) {
        puts("ic_string_equal_char: call to ic_string_contents failed");
        return 0;
    }

    if (0 == strcmp(a_ch, b)) {
        /* equal */
        return 1;
    }

    /* non-equal */
    return 0;
}
