#ifndef ICARUS_FIELD_H
#define ICARUS_FIELD_H

#include "../../data/pvector.h"
#include "../../data/symbol.h"
#include "type_ref.h"

/* a field is a combination of a symbol, a type, and a permission
 *  foo::Int
 * the symbol is `foo`
 * and the type is `Int`
 * the permission is [default]
 *
 * &bar::String
 * the symbol is `bar`
 * the type is `String`
 * the permission is [ampersand]
 *
 * FIXME the above permissions are not fixed yet
 */
struct ic_field {
    struct ic_symbol name;
    struct ic_type_ref type;
    unsigned int permissions;
};

/* allocate and return a new field
 * takes 2 tokens as char * and len pairs
 *
 * returns new ic_field * on success
 * returns 0 on failure
 */
struct ic_field *ic_field_new(char *name_src, unsigned int name_len, char *type_src, unsigned int type_len, unsigned int permissions);

/* initialise an existing field
 * takes 2 tokens as char * and len pairs
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_field_init(struct ic_field *field, char *name_src, unsigned int name_len, char *type_src, unsigned int type_len, unsigned int permissions);

/* destroy field
 *
 * will free field if `free_field` is truhty
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_field_destroy(struct ic_field *field, unsigned int free_field);

/* print the field to stdout */
void ic_field_print(FILE *fd, struct ic_field *field);

#endif
