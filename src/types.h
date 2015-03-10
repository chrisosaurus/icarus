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

#endif
