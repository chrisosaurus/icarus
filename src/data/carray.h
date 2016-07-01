#ifndef ICARUS_CARRAY_H
#define ICARUS_CARRAY_H

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
struct ic_carray *ic_carray_new(unsigned int len);

/* initialise an existing array to the specified len
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_carray_init(struct ic_carray *arr, unsigned int len);

/* destroy carray
 *
 * this will only free this carray is `free_arr` is true
 *
 * the caller must determine if it is appropriate or not
 * to not to call free(arr)
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_carray_destroy(struct ic_carray *arr, unsigned int free_arr);

/* get item at pos
 *
 * bounds checked
 *
 * returns item on success
 * returns 0 on failure
 */
char ic_carray_get(struct ic_carray *arr, unsigned int pos);

/* set element at [pos] to val
 *
 * bounds checked
 *
 * returns 1 on successful set
 * returns 0 on failure
 */
unsigned int ic_carray_set(struct ic_carray *arr, unsigned int pos, char val);

/* ensure array is at least as big as `new_len`
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_carray_ensure(struct ic_carray *arr, unsigned int new_len);

#endif
