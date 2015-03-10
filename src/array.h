#ifndef ICARUS_ARRAY_H
#define ICARUS_ARRAY_H

struct ic_array {
    unsigned int len;
    unsigned int cap;
    void ** contents;
};

/* allocate a new array of length len
 *
 * returns array on success
 * returns 0 on failure
 */
struct ic_array * ic_array_new(unsigned int len);

/* get item at pos
 * bounds checked
 *
 * returns item on success
 * return 0 on failure
 */
void * ic_array_get(struct ic_array *arr, unsigned int pos);
/* returns 0 on successful set
 * returns 1 on failure
 *
 * bounds checked
 */
int ic_array_set(struct ic_array *arr, unsigned int pos, void *val);

#endif
