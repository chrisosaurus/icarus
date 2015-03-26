#ifndef ICARUS_PVECTOR_H
#define ICARUS_PVECTOR_H

/* pointer vector
 * a dynamic bounds checked vector
 * stores an array of void pointers
 * caller is responsible for managing the contents
 *
 * a vector does not offer a set but instead has an append
 * a vector will grow automatically to store more object
 */
struct ic_pvector {
    unsigned int used;
    unsigned int cap;
    /* note that we store an array of void*
     * so we must be careful what we actually store here
     */
    void ** contents;
};

/* allocate a new vector of capacity cap
 *
 * returns pvector on success
 * returns 0 on failure
 */
struct ic_pvector * ic_pvector_new(unsigned int cap);

/* initialise existing pvector
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_pvector_init(struct ic_pvector *vec, unsigned int cap);

/* get item at pos
 * bounds checked
 *
 * returns item on success
 * returns 0 on failure
 */
void * ic_pvector_get(struct ic_pvector *arr, unsigned int pos);

/* append a new item to the end of pvector
 * this will grow pvector if needed
 *
 * returns a positive integer representing the new item's index into the vector on success
 * returns -1 on failure
 */
int ic_pvector_append(struct ic_pvector *arr, void *data);

/* ensure pvector is at least as big as `new_cap`
 * this will not shrink the pvector
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_pvector_ensure(struct ic_pvector *arr, unsigned int new_cap);

/* returns the length of the used section of the vector
 *
 * returns 0 on error
 */
unsigned int ic_pvector_length(struct ic_pvector *arr);

#endif
