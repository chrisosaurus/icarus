#include <stdio.h>  /* puts, printf */
#include <string.h> /* strncmp */

#include "parse.h"

struct ic_field *ic_parse_field(struct ic_token_list *token_list) {
    /* the field we build and return */
    struct ic_field *field = 0;
    /* name component char* and len*/
    char *name = 0;
    unsigned int name_len = 0;
    struct ic_type_ref *type = 0;

    /* our eventual field permissions */
    unsigned int permissions = 0;

    /* current token */
    struct ic_token *token = 0;

    if (!token_list) {
        puts("ic_parse_field: token_list was null");
        return 0;
    }

    permissions = ic_parse_permissions(token_list);

    /* capture field name
     * a::Signed
     * `a` is the name
     */
    token = ic_token_list_expect_important(token_list, IC_IDENTIFIER);
    if (!token) {
        puts("ic_parse_field: no name token found");
        return 0;
    }

    name = ic_token_get_string(token);
    name_len = ic_token_get_string_length(token);
    if (!name || !name_len) {
        puts("ic_parse_field: failed to extract name from token");
        return 0;
    }

    /* check for separator `::` */
    token = ic_token_list_expect_important(token_list, IC_DOUBLECOLON);
    if (!token) {
        puts("ic_parse_field: no doublcolon token found");
        return 0;
    }

    /* capture field type
     * a::Signed
     * `Signed` is the type
     */
    type = ic_parse_type_ref(token_list);
    if (!type) {
        puts("ic_parse_field: no type found");
        return 0;
    }

    field = ic_field_new(name, name_len, type, permissions);
    if (!field) {
        puts("ic_parse_field: call to ic_field_new failed");
        return 0;
    }

    return field;
}
