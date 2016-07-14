#ifndef IC_BACKEND_PANCAKE_VALUE_H
#define IC_BACKEND_PANCAKE_VALUE_H

enum ic_backend_pancake_value_type {
    ic_backend_pancake_value_type_bool,
    ic_backend_pancake_value_type_uint,
    ic_backend_pancake_value_type_int,
    ic_backend_pancake_value_type_string,
    ic_backend_pancake_value_type_ref,
};

struct ic_backend_pancake_value {
    enum ic_backend_pancake_value_type tag;
    union {
        unsigned int boolean;

        unsigned int uinteger;

        int integer;

        /* FIXME */
        void *string;

        /* FIXME */
        void *ref;
    } u;
};

#endif /* IC_BACKEND_PANCAKE_VALUE_H */
