#ifndef ICARUS_TYPE_H
#define ICARUS_TYPE_H

#include "../../data/symbol.h"

enum ic_type_ref_type {
    /* a type field had 3 states: */

    /* unknown
     * this type must be inferred during analysis
     */
    ic_type_ref_unknown,

    /* a symbol found during parsing,
     * most likely user declared (`a::Int`)
     * this will need to resolved to an ic_type
     * during analysis
     */
    ic_type_ref_symbol,

    /* error type
     * this is NOT a runtime error
     *
     * this value is used by the analysis module to
     * indicate an error during inference such as
     * being unable to find the type mentioned
     *
     * FIXME it may be that errors are moved out of here
     */
    ic_type_ref_error
};

struct ic_type_ref {
    enum ic_type_ref_type type; /* heh */
    union {
        /* no value for unknown */
        struct ic_symbol sym; /* value for ic_type_symbol */
        /* FIXME need a structure for an error */
    } u ;
};

/* allocate and intialise a new type
 * this will set type.type to unknown
 *
 * returns new type on success
 * returns 0 on failure
 */
struct ic_type_ref * ic_type_ref_new(void);

/* intialise a type
 * this will set type.type to unknown
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_ref_init(struct ic_type_ref *type);

/* allocate and intialise a new type
 * as a symbol
 *
 * returns new type on success
 * returns 0 on failure
 */
struct ic_type_ref * ic_type_ref_symbol_new(char *type_str, unsigned int type_len);

/* intialise a type as a  symbol
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_ref_symbol_init(struct ic_type_ref *type, char *type_str, unsigned int type_len);

/* destroy type
 *
 * this will only free type if `free_type` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_ref_destroy(struct ic_type_ref *type, unsigned int free_type);

/* set the sym on this type from the provided string
 * this will change type.type to sym
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_ref_set_symbol(struct ic_type_ref *type, char *type_str, unsigned int type_len);

/* return a symbol representing this type
 *
 * if type is unknown then 0 is reuturned
 * if type is symbol then the symbol is returned
 *
 * returns 0 on failure
 */
struct ic_symbol * ic_type_ref_get_symbol(struct ic_type_ref *type);

/* print this this type */
void ic_type_ref_print(struct ic_type_ref *type);

#endif
