#ifndef IC_BACKEND_PANCAKE_CALL_INFO_H
#define IC_BACKEND_PANCAKE_CALL_INFO_H

struct ic_backend_pancake_call_info {
    unsigned int return_offset;
    unsigned int arg_count;
    unsigned int call_start_offset;
};

#endif /* IC_BACKEND_PANCAKE_CALL_INFO_H */
