#include <stdio.h>
#include <stdlib.h>

#include "call_frame.h"

/* allocate and init a new call_frame
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_call_frame *ic_backend_pancake_call_frame_new(void) {
    struct ic_backend_pancake_call_frame *call_frame = 0;

    call_frame = calloc(1, sizeof(struct ic_backend_pancake_call_frame));
    if (!call_frame) {
        puts("ic_backend_pancake_call_frame_new: call to calloc failed");
        return 0;
    }

    if (!ic_backend_pancake_call_frame_init(call_frame)) {
        puts("ic_backend_pancake_call_frame_new: call to ic_backend_pancake_call_frame_init failed");
        return 0;
    }

    return call_frame;
}

/* init an existing call_frame
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_call_frame_init(struct ic_backend_pancake_call_frame *call_frame) {
    if (!call_frame) {
        puts("ic_backend_pancake_call_frame_init: call_frame was null");
        return 0;
    }

    call_frame->return_offset = 0;
    call_frame->arg_count = 0;
    call_frame->call_start_offset = 0;
    call_frame->arg_start = 0;
    call_frame->local_vars = 0;

    return 1;
}

/* destroy a call_frame
 *
 * FIXME TODO clean up internal dict
 *
 * will only free call_frame is `free_call_frame` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_call_frame_destroy(struct ic_backend_pancake_call_frame *call_frame, unsigned int free_call_frame) {
    if (!call_frame) {
        puts("ic_backend_pancake_call_frame_destroy: call_frame was null");
        return 0;
    }

    if (free_call_frame) {
        free(call_frame);
    }

    return 1;
}

/* set the value stored under 'key' to 'value'
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_call_frame_set(struct ic_backend_pancake_call_frame *call_frame, char *key, struct ic_backend_pancake_value *value) {
    if (!call_frame) {
        puts("ic_backend_pancake_call_frame_set: call_frame was null");
        return 0;
    }

    if (!key) {
        puts("ic_backend_pancake_call_frame_set: key was null");
        return 0;
    }

    if (!value) {
        puts("ic_backend_pancake_call_frame_set: value was null");
        return 0;
    }

    if (!call_frame->local_vars) {
        call_frame->local_vars = ic_dict_new();
        if (!call_frame->local_vars) {
            puts("ic_backend_pancake_call_frame_set: call to ic_dict_new failed");
            return 0;
        }
    }

    if (ic_dict_exists(call_frame->local_vars, key)) {
        if (!ic_dict_set(call_frame->local_vars, key, value)) {
            puts("ic_backend_pancake_call_frame_set: call to ic_dict_set failed");
            return 0;
        }
    } else {
        if (!ic_dict_insert(call_frame->local_vars, key, value)) {
            puts("ic_backend_pancake_call_frame_set: call to ic_dict_insert failed");
            return 0;
        }
    }

    return 1;
}

/* get the value stored under key 'x'
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_value *ic_backend_pancake_call_frame_get(struct ic_backend_pancake_call_frame *call_frame, char *key) {
    struct ic_backend_pancake_value *value = 0;

    if (!call_frame) {
        puts("ic_backend_pancake_call_frame_get: call_frame was null");
        return 0;
    }

    if (!key) {
        puts("ic_backend_pancake_call_frame_get: key was null");
        return 0;
    }

    value = ic_dict_get(call_frame->local_vars, key);
    return value;
}
