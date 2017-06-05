#include <stdio.h>  /* puts, printf */
#include <stdlib.h> /* calloc */

#include "../permissions.h"
#include "field.h"

/* allocate and return a new field
 * takes 2 tokens as char * and len pairs
 *
 * returns new ic_field * on success
 * returns 0 on failure
 */
struct ic_field *ic_field_new(char *name_src, unsigned int name_len, struct ic_type_ref *type, unsigned int permissions) {
    struct ic_field *field = 0;

    /* allocate space for our field */
    field = calloc(1, sizeof(struct ic_field));
    if (!field) {
        puts("ic_field_new: calloc failed");
        return 0;
    }

    if (!ic_field_init(field, name_src, name_len, type, permissions)) {
        puts("ic_field_new: call to ic_field_init failed");
        free(field);
        return 0;
    }

    return field;
}

/* initialise an existing field
 * takes 2 tokens as char * and len pairs
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_field_init(struct ic_field *field, char *name_src, unsigned int name_len, struct ic_type_ref *type, unsigned int permissions) {
    if (!field) {
        puts("ic_field_init: field was null");
        return 0;
    }

    if (!name_src) {
        puts("ic_field_init: name_src was null");
        return 0;
    }

    if (!type) {
        puts("ic_field_init: type was null");
        return 0;
    }

    /* init name symbol */
    if (!ic_symbol_init(&(field->name), name_src, name_len)) {
        puts("ic_field_init: call to ic_symbol_init for name failed");
        return 0;
    }

    field->type = type;

    field->permissions = permissions;
    field->assigned_to = 0;

    return 1;
}

/* destroy field
 *
 * will free field if `free_field` is truhty
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_field_destroy(struct ic_field *field, unsigned int free_field) {
    if (!field) {
        puts("ic_field_destroy: field was null");
        return 0;
    }

    /* dispatch to symbol destroy for name
     * note we do not ask it to destroy_symbol
     * as it is a member
     */
    if (!ic_symbol_destroy(&(field->name), 0)) {
        puts("ic_field_destroy: name: call to ic_symbol_destroy failed");
        return 0;
    }

    /* destroy type as given ownership during parse time */
    if (!ic_type_ref_destroy(field->type, 1)) {
        puts("ic_field_destroy: type: call to ic_type_destroy failed");
        return 0;
    }

    /* free field if we are asked nicely */
    if (free_field) {
        free(field);
    }

    return 1;
}

/* print the field to stdout */
void ic_field_print(FILE *fd, struct ic_field *field) {
    char *perm_str;

    if (!field) {
        puts("ic_field_print: field was null");
        return;
    }

    perm_str = ic_parse_perm_str(field->permissions);
    if (!perm_str) {
        puts("ic_field_print: call to ic_parse_perm_str failed");
        return;
    }

    fprintf(fd, "%s%s::", perm_str, ic_symbol_contents(&(field->name)));
    ic_type_ref_print(fd, field->type);
}
