#ifndef IC_BACKEND_PANCAKE_CALL_INFO_H
#define IC_BACKEND_PANCAKE_CALL_INFO_H

/* an instance of this struct is put onto the call_info_stack
 * for each icp_call
 *
 * for a tailcall this struct will still be inserted,
 * but after the struct from the caller is removed.
 */
struct ic_backend_pancake_call_info {
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
     * this is so we can address arguments
     */
    unsigned int arg_start;
};

#endif /* IC_BACKEND_PANCAKE_CALL_INFO_H */
