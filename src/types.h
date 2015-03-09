#ifndef ICARUS_TYPES_H
#define ICARUS_TYPES_H

struct ic_symbol {
    unsigned int len;
    char *sym;
};

/* allocate a new symbol of length len
 * containing contents
 *
 * returns symbol on success
 * returns 0 on failure
 */
struct ic_symbol * ic_symbol_new(unsigned int len, char *contents);

struct ic_string {
    unsigned int len;
    unsigned int cap;
    char *str;
};

/* allocate a new string of length len
 * containing contents
 *
 * returns string on success
 * returns 0 on failure
 */
struct ic_string * ic_string_new(unsigned int len, char *contents);

struct ic_int {
    int value;
};

/* allocate a new int containing value
 *
 * returns int on success
 * returns 0 on failure
 */
struct ic_int * ic_int_new(int value);

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
