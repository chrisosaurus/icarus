#include <stdio.h>  /* puts */
#include <stdlib.h> /* calloc */

#include "string.h"
#include "symbol.h"

/* build a new symbol from a char* and a length
 * this will allocate a new buffer and strncpy n
 * chars from source into the string
 * then it will add the null terminator
 *
 * returns new symbol on success
 * returns 0 on failure
 */
struct ic_symbol *ic_symbol_new(char *source, unsigned int len) {
    struct ic_symbol *sym;

    sym = calloc(1, sizeof(struct ic_symbol));
    if (!sym) {
        puts("ic_symbol_new: call to calloc failed");
        return 0;
    }

    if (!ic_symbol_init(sym, source, len)) {
        puts("ic_symbol_new: error in call to ic_symbol_init");
        free(sym);
        return 0;
    }

    return sym;
}

/* initialise an existing symbol from a char* and a length
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_symbol_init(struct ic_symbol *sym, char *source, unsigned int len) {
    if (!sym) {
        puts("ic_symbol_init: passed in sym was null");
        return 0;
    }

    /* dispatch to handle initialisation of internal */
    if (!ic_string_init(&(sym->internal), source, len)) {
        puts("ic_symbol_new: error in call to ic_string_init");
        return 0;
    }

    return 1;
}

/* destroy symbol
 *
 * this will only free this symbol is `free_sym` is true
 *
 * the caller must determine if it is appropriate or not
 * to not to call free(sym)
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_symbol_destroy(struct ic_symbol *sym, unsigned int free_sym) {
    if (!sym) {
        puts("ic_symbol_destroy: passed in sym was null");
        return 0;
    }

    /* dispatch to string destroy for work
     * note that we do NOT ask it to free_str
     * as internal is an element on sym
     */
    if (!ic_string_destroy(&(sym->internal), 0)) {
        puts("ic_symbol_destroy: call to ic_string_destroy failed");
        return 0;
    }

    /* free symbol if asked */
    if (free_sym) {
        free(sym);
    }

    /* success */
    return 1;
}

/* returns backing character array
 * the caller is NOT allowed to mutate this character array directly
 *
 * returns a char * on success
 * returns 0 on failure
 */
char *ic_symbol_contents(struct ic_symbol *symbol) {
    if (!symbol) {
        puts("ic_symbol_contents: symbol was null");
        return 0;
    }
    return ic_string_contents(&(symbol->internal));
}

/* get the strlen of the stored symbol
 * this length does NOT include the null terminator
 *
 * returns numbers of characters in symbol on success
 * returns -1 on failure
 */
int ic_symbol_length(struct ic_symbol *symbol) {
    if (!symbol) {
        puts("ic_symbol_length: symbol was null");
        return -1;
    }
    return ic_string_length(&(symbol->internal));
}

/* get a character from the symbol
 *
 * returns character on success
 * returns 0 on failure
 */
char ic_symbol_get(struct ic_symbol *symbol, unsigned int offset) {
    if (!symbol) {
        puts("ic_symbol_get: symbol was null");
        return 0;
    }
    return ic_string_get(&(symbol->internal), offset);
}

/* print this symbol */
void ic_symbol_print(struct ic_symbol *symbol) {
    char *con;

    if (!symbol) {
        puts("ic_symbol_print: symbol was null");
        return;
    }

    con = ic_symbol_contents(symbol);
    if (!con) {
        puts("ic_symbol_print: call to ic_symbol_contents failed");
        return;
    }

    printf("%s", con);
}
