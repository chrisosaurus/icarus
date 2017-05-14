#ifndef IC_BACKEND_PANCAKE_CALL_FRAME_H
#define IC_BACKEND_PANCAKE_CALL_FRAME_H

#include "../../../data/dict.h"
#include "value.h"

/* an instance of this struct is put onto the call_frame_stack
 * for each icp_call
 *
 * for a tailcall this struct will still be inserted,
 * but after the struct from the caller is removed.
 */
struct ic_backend_pancake_call_frame {
    /* offset of calling set
     * offset we return to upon icp_return_*
     */
    unsigned int return_offset;
    /* number of args we were called with */
    unsigned int arg_count;
    /* offset we jumped to
     * this is the start of the 'current' function
     */
    unsigned int call_start_offset;
    /* height of value stack when this call started
     * before any arguments were inserted
     *
     * this is also the point we reset back to on a call to `clean_stack`
     */
    unsigned int arg_start;
    /* map of char* to ic_backend_pancake_value which are used for the result
     * of runtime function calls
     *
     * e.g.
     *  let x = foo()
     *
     * will then insert the result of `foo()` under the key `x`
     */
    struct ic_dict *local_vars;
};

/* allocate and init a new call_frame
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_call_frame *ic_backend_pancake_call_frame_new(void);

/* init an existing call_frame
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_call_frame_init(struct ic_backend_pancake_call_frame *call_frame);

/* destroy a call_frame
 *
 * FIXME TODO clean up internal dict
 *
 * will only free call_frame is `free_call_frame` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_call_frame_destroy(struct ic_backend_pancake_call_frame *call_frame, unsigned int free_call_frame);

/* set the value stored under 'key' to 'value'
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_call_frame_set(struct ic_backend_pancake_call_frame *call_frame, char *key, struct ic_backend_pancake_value *value);

/* get the value stored under key 'x'
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_value *ic_backend_pancake_call_frame_get(struct ic_backend_pancake_call_frame *call_frame, char *key);

#endif /* IC_BACKEND_PANCAKE_CALL_FRAME_H */
