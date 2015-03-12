#ifndef ICARUS_SYMBOL_H
#define ICARUS_SYMBOL_H

#include "string.h"

/* for now symbol is a synonym for string */

struct ic_symbol {
    struct ic_string internal;
};

/* build a new symbol from a char* and a length
 * this will allocate a new buffer and strncpy n
 * chars from source into the string
 * then it will add the null terminator
 *
 * returns new symbol on success
 * returns 0 on failure
 */
struct ic_symbol * ic_symbol_new(char *source, unsigned int len);

/* initialise an existing symbol from a char* and a length
 *
 * returns 0 on success
 * returns 1 on failure
 */
int ic_symbol_init(struct ic_symbol *sym, char *source, unsigned int len);


/* return backing character array
 * the caller is NOT allowed to mutate this character array directly
 *
 * returns a char * on success
 * return 0 on error
 */
char * ic_symbol_contents(struct ic_symbol *symbol);

/* get the strlen of the stores symbol
 * this length does NOT include the null terminator
 *
 * returns numbers of characters in symbol on success
 * returns -1 on error
 */
int ic_symbol_length(struct ic_symbol *symbol);

/* get a character from the symbol
 *
 * returns character on success
 * returns 0 on failure
 */
char ic_symbol_get(struct ic_symbol *symbol, unsigned int offset);



#endif

