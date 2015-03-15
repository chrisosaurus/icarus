#ifndef ICARUS_FIELD_H
#define ICARUS_FIELD_H

#include "symbol.h"
#include "pvector.h"

/* a field is a combination of a symbol and a type
 *  foo::Int
 * the symbol is `foo`
 * and the type is `Int
 */
struct ic_field {
    struct ic_symbol name;
    struct ic_symbol type;
};

/* allocate and return a new field
 * takes 2 tokens as char * and len pairs
 *
 * returns new ic_field * on success
 * returns 0 on failure
 */
struct ic_field * ic_field_new(char *name_src, unsigned int name_len, char *type_src, unsigned int type_len);

/* initialise an existing field
 * takes 2 tokens as char * and len pairs
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_field_init(struct ic_field *field, char *name_src, unsigned int name_len, char *type_src, unsigned int type_len);

/* print the field to stdout */
void ic_field_print(struct ic_field *field);

#endif
