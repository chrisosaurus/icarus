#ifndef IC_BACKEND_PANCAKE_VALUE_H
#define IC_BACKEND_PANCAKE_VALUE_H

enum ic_backend_pancake_value_type {
    ic_backend_pancake_value_type_boolean,
    ic_backend_pancake_value_type_uint,
    ic_backend_pancake_value_type_sint,
    ic_backend_pancake_value_type_string,
    ic_backend_pancake_value_type_ref,
};

struct ic_backend_pancake_value {
    enum ic_backend_pancake_value_type tag;
    union {
        unsigned int boolean;

        unsigned int uint;

        int sint;

        char *string;

        /* FIXME */
        void *ref;
    } u;
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
