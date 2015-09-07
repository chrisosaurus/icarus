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

/* initialise existing pvector to specified cap
 *
 * will use a default cap of PVECTOR_DEFAULT_SIZE is no cap is supplied
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_pvector_init(struct ic_pvector *vec, unsigned int cap);

/* destroy pvector
 *
 * this will only free the pvecto if `free_pvector` is true
 *
 * takes an optional function which is called once for each argument stored in the pvector
 * it will be called with it's free argument set to true
 * FIXME consider allowing free_data param to ic_pvector_destroy that is passed to (*destroy_item) rather than defaulting to true
 *
 * this function will bail at the first error encountered
 *
 * note that this function signature prevents storing a pvector/parray inside a pvector
 * without the use of a shim as the destroy_item signature doesn't match ic_pvector_destroy
 *
 * sadly this does require a shim for each type as the following function pointers are not
 * compatible:
 *
 *  unsigned int (*destroy_item)(void *item, unsigned int free_item);
 *  unsigned int (*destroy_item)(struct foo *item, unsigned int free_item);
 *
 * so a simple shim is needed to convert between
 *  unsigned int shim(void *item, unsigned int free){
 *      return foo(item, free);
 *  }
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_pvector_destroy(struct ic_pvector *vec, unsigned int free_vec, unsigned int (*destroy_item)(void* item, unsigned int free));

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
 * returns an integer representing the new item's index into the vector on success
 * returns -1 on failure
 */
int ic_pvector_append(struct ic_pvector *arr, void *data);

/* ensure pvector is at least as big as `new_cap`
 * this will not shrink the pvector
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_pvector_ensure(struct ic_pvector *arr, unsigned int new_cap);

/* returns the length of the used section of the vector
 *
 * returns 0 on error
 */
unsigned int ic_pvector_length(struct ic_pvector *arr);

#endif
