#ifndef ICARUS_TYPE_H
#define ICARUS_TYPE_H

#include "../../data/pvector.h"
#include "../../data/symbol.h"

enum ic_type_ref_tag {
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

    /* a type_param set during parse time
     * this is then resolved at analysis type to a concrete type per template
     * instantiation
     */
    ic_type_ref_param,

    /* we have found a concrete type for this
     * this is filled in during the analysis phase
     */
    ic_type_ref_resolved,

};

struct ic_type_ref {
    enum ic_type_ref_tag tag;
    union {
        /* no value for unknown */
        struct ic_symbol sym;         /* value for ic_type_ref_symbol */
        struct ic_type_param *tparam; /* value for ic_type_ref_param */
        struct ic_decl_type *tdecl;   /* value for ic_type_ref_resolved */
    } u;

    /* vector of type_refs which are type arguments given to this type
     * Maybe[Sint]
     * have a type_ref for Sint set here
     */
    struct ic_pvector type_args;
};

/* allocate and intialise a new type
 * this will set type.type to unknown
 *
 * returns new type on success
 * returns 0 on failure
 */
struct ic_type_ref *ic_type_ref_new(void);

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
struct ic_type_ref *ic_type_ref_symbol_new(char *type_str, unsigned int type_len);

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

/* perform a deep copy of a type_ref
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_type_ref *ic_type_ref_deep_copy(struct ic_type_ref *type);

/* perform a deep copy of a type_ref embedded within an object
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_ref_deep_copy_embedded(struct ic_type_ref *from, struct ic_type_ref *to);

/* set the sym on this type from the provided string
 * this will change type_ref.tag to sym
 *
 * this is an error if type_ref.tag was already resolved
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_ref_set_symbol(struct ic_type_ref *type, char *type_str, unsigned int type_len);

/* return a symbol representing this type
 *
 * if type_ref is unknown then 0 is returned
 * if type_ref is symbol then the symbol is returned
 * if type_ref is resolved then the symbol for that type is returned
 *
 * returns 0 on failure
 */
struct ic_symbol *ic_type_ref_get_symbol(struct ic_type_ref *type);

/* set the decl_type on this type_ref
 * this will change type.tag to resolved
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_ref_set_type_decl(struct ic_type_ref *type, struct ic_decl_type *tdecl);

/* return the underlying decl_type
 *
 * if type_ref.tag is not resolved then this is an error
 *
 * return * on success
 * returns 0 on failure
 */
struct ic_decl_type *ic_type_ref_get_type_decl(struct ic_type_ref *type);

/* set the decl_param on this type_ref
 * this will change type.tag to param
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_ref_set_type_param(struct ic_type_ref *type, struct ic_type_param *tparam);

/* return the underlying decl_param
 *
 * if type_ref.tag is not param then this is an error
 *
 * return * on success
 * returns 0 on failure
 */
struct ic_type_param *ic_type_ref_get_type_param(struct ic_type_ref *type);

/* check if this type_ref refers to a currently generic param
 *
 * returns 1 for yes
 * returns 0 for no
 */
unsigned int ic_type_ref_is_type_param(struct ic_type_ref *type);

/* add a type_arg
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_ref_type_args_add(struct ic_type_ref *type, struct ic_type_ref *type_arg);

/* get a type_arg
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_type_ref *ic_type_ref_type_args_get(struct ic_type_ref *type, unsigned int i);

/* get length of type_args
 *
 * returns num on success
 * returns 0 on failure
 */
unsigned int ic_type_ref_type_args_length(struct ic_type_ref *type);

/* print this this type */
void ic_type_ref_print(FILE *fd, struct ic_type_ref *type);

#endif
