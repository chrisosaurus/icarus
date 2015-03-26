#ifndef ICARUS_carray_H
#define ICARUS_carray_H

/* character array
 * a dynamic bounds checked array
 * stores an array of char pointers
 * caller is responsible for managing the contents
 */
struct ic_carray {
    unsigned int len;
    char *contents;
};

/* allocate a new array of length len
 *
 * returns array on success
 * returns 0 on failure
 */
struct ic_carray * ic_carray_new(unsigned int len);

/* initialise an existing array to the specified len
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_carray_init(struct ic_carray *arr, unsigned int len);

/* get item at pos
 * bounds checked
 *
 * returns item on success
 * returns 0 on failure
 */
char ic_carray_get(struct ic_carray *arr, unsigned int pos);

/* returns 0 on successful set
 * returns 1 on failure
 *
 * bounds checked
 */
unsigned int ic_carray_set(struct ic_carray *arr, unsigned int pos, char val);

/* ensure array is at least as big as `new_len`
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_carray_ensure(struct ic_carray *arr, unsigned int new_len);

#endif
