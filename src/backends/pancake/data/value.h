#ifndef IC_BACKEND_PANCAKE_VALUE_H
#define IC_BACKEND_PANCAKE_VALUE_H

enum ic_backend_pancake_value_type {
    ic_backend_pancake_value_type_boolean,
    ic_backend_pancake_value_type_uint,
    ic_backend_pancake_value_type_sint,
    ic_backend_pancake_value_type_string,
    ic_backend_pancake_value_type_ref,
    ic_backend_pancake_value_type_unit,
};

union ic_backend_pancake_value_cell {
    unsigned int boolean;
    unsigned int uint;
    int sint;
    char *string;
    void *ref;
    char unit; /* only needed for READ and RESULT macro usage */
};

struct ic_backend_pancake_value {
    enum ic_backend_pancake_value_type tag;
    union ic_backend_pancake_value_cell u;
};

/* allocate a  blank value */
struct ic_backend_pancake_value *ic_backend_pancake_value_new(void);

/* copy all data from `value_from` into `value_to`
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_value_copy(struct ic_backend_pancake_value *value_from, struct ic_backend_pancake_value *value_to);

/* print value to provided file
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_value_print(FILE *fd, struct ic_backend_pancake_value *value);

#endif /* IC_BACKEND_PANCAKE_VALUE_H */
