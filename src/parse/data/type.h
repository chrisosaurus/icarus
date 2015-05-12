#ifndef ICARUS_TYPE_H
#define ICARUS_TYPE_H

#include "../../data/symbol.h"

enum ic_type_type {
    /* a type field had 5 states: */

    /* unknown
     * this type must be inferred during analysis
     */
    ic_type_unknown,

    /* a symbol found during parsing,
     * most likely user declared (`a::Int`)
     * this will need to resolved to a tdecl
     * during analysis
     */
    ic_type_symbol,

    /* a fully fledged type
     */
    ic_type_tdecl,

    /* a builtin type
     * Int
     * String
     * ...
     */
    ic_type_builtin,

    /* error type
     * this is NOT a runtime error
     *
     * this value is used by the analysis module to
     * indicate an error during inference such as
     * being unable to find the type mentioned
     */
    ic_type_error
};

struct ic_type {
    enum ic_type_type type; /* heh */
    union {
        /* no value for unknown */
        struct ic_symbol sym; /* value for ic_type_symbol */
        struct ic_type_decl *tdecl; /* value for ic_type_tdecl */
        /* FIXME what is the value for an builtin? */
        /* FIXME what is the value for an error? */
    } u ;
};

/* allocate and intialise a new type
 * this will set type.type to unknown
 *
 * returns new type on success
 * returns 0 on failure
 */
struct ic_type * ic_type_new(void);

/* intialise a type
 * this will set type.type to unknown
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_type_init(struct ic_type *type);

/* allocate and intialise a new type
 * as a symbol
 *
 * returns new type on success
 * returns 0 on failure
 */
struct ic_type * ic_type_symbol_new(char *type_str, unsigned int type_len);

/* intialise a type as a  symbol
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_type_symbol_init(struct ic_type *type, char *type_str, unsigned int type_len);

/* destroy type
 *
 * this will only free type if `free_type` is truthy
 *
 * returns 0 on sucess
 * returns 1 on error
 */
unsigned int ic_type_destroy(struct ic_type *type, unsigned int free_type);

/* set the sym on this type from the provided string
 * this will change type.type to sym
 *
 * Note that this is ONLY allowed if a tdecl hasn't already been set
 *
 * if type.type is tdecl then calling this function is an error
 * as that would be going 'backwards'
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_type_set_symbol(struct ic_type *type, char *type_str, unsigned int type_len);

/* set the *tdecl on this type
 * this will change type.type to tdecl
 *
 * this is only allowed it the type is NOT already set to tdecl
 * if type is already a symbol then the symbol will first be destroyed
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_type_set_tdecl(struct ic_type *type, struct ic_type_decl *tdecl);

/* return a symbol representing this type
 *
 * if type is unknown then 0 is reuturned
 * if type is symbol then the symbol is returned
 * if type is tdecl then the symbol on that tdecl is returedn
 *
 * returns 0 on failure
 */
struct ic_symbol * ic_type_get_symbol(struct ic_type *type);

/* print this this type */
void ic_type_print(struct ic_type *type);

#endif
