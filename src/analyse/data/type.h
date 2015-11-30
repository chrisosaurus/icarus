#ifndef IC_TYPE_H
#define IC_TYPE_H

#include "../../parse/data/decl.h"

/* an instance of a type
 * ic_type has a single instance for each type within a kludge (FIXME consider type scoping)
 *
 * an ic_type can either be:
 *  - a builtin - builtin type
 *  - a user defined type - ic_decl_type
 */

enum ic_type_tag {
    ic_type_user,
    ic_type_builtin
};

struct ic_type {
    enum ic_type_tag tag;
    union {
        struct ic_decl_type *decl;
    } u;
};

/* alloc and init a new type representing a tdecl
 *
 * returns new type on success
 * returns 0 on failure
 */
struct ic_type * ic_type_new_tdecl(struct ic_decl_type *decl);

/* init an existing type representing a tdecl
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_init_tdecl(struct ic_type *type, struct ic_decl_type *decl);

/* destroy a type
 *
 * will only free the type if `free_type` is truthy
 *
 * will NOT free any decl objects
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_destroy(struct ic_type *type, unsigned int free_type);

/* get decl from type
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_decl_type * ic_type_get_decl(struct ic_type *type);

/* return a symbol representing the name of this type
 *
 * returns ic_symbol * on sucess
 * returns 0 on failure
 */
struct ic_symbol * ic_type_name(struct ic_type *type);

/* check if type is builtin
 *
 * returns 1 if builtin
 * returns 0 otherwise
 */
unsigned int ic_type_isbuiltin(struct ic_type *type);

/* is this type void
 *
 * returns 1 if provided type is Void
 * returns 0 if provided type is NOT void
 */
unsigned int ic_type_isvoid(struct ic_type *type);

/* is this type bool
 *
 * returns 1 if provided type is Bool
 * returns 0 if provided type is NOT Bool
 */
unsigned int ic_type_isbool(struct ic_type *type);

/* are these 2 types the equal
 *
 * returns 1 if they are equal
 * returns 0 of they are NOT equal
 */
unsigned int ic_type_equal(struct ic_type *a, struct ic_type *b);

/* print debug information about a type */
void ic_type_print(struct ic_type *type);

#endif
