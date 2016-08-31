#include <stdio.h>

#include "value.h"

/* copy all data from `value_from` into `value_to`
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_value_copy(struct ic_backend_pancake_value *value_from, struct ic_backend_pancake_value *value_to) {
    if (!value_from) {
        puts("ic_backend_pancake_value_copy: value_from was null");
        return 0;
    }

    if (!value_to) {
        puts("ic_backend_pancake_value_copy: value_to was null");
        return 0;
    }

    value_to->tag = value_from->tag;

    switch (value_from->tag) {
        case ic_backend_pancake_value_type_bool:
            value_to->u.boolean = value_from->u.boolean;
            break;

        case ic_backend_pancake_value_type_uint:
            value_to->u.uint = value_from->u.uint;
            break;

        case ic_backend_pancake_value_type_sint:
            value_to->u.sint = value_from->u.sint;
            break;

        case ic_backend_pancake_value_type_string:
            value_to->u.string = value_from->u.string;
            break;

        case ic_backend_pancake_value_type_ref:
            value_to->u.ref = value_from->u.ref;
            break;

        default:
            puts("ic_backend_pancake_value_copy: impossible value_from->tag");
            return 0;
    }

    return 1;
}
