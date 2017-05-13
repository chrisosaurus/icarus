#include <stdio.h>
#include <stdlib.h>

#include "data/runtime_data.h"
#include "pancake.h"

#include "runtime/builtins.h"

#define PANCAKE_INTERPRET_DEBUG 0

/* interpret bytecode in runtime_data
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_interpret(struct ic_backend_pancake_runtime_data *runtime_data) {
    struct ic_backend_pancake_bytecode *instruction = 0;
    struct ic_backend_pancake_instructions *instructions = 0;
    struct ic_backend_pancake_call_frame_stack *call_frame_stack = 0;
    struct ic_backend_pancake_value_stack *value_stack = 0;

    /* values inside our instructions we may want to unpack */
    char *str = 0;
    int sint = 0;
    unsigned int uint = 0;
    bool boolean = false;
    void *ref = 0;
    union ic_backend_pancake_value_cell *ref_array = 0;
    union ic_backend_pancake_value_cell *ref_slot = 0;

    /* new offset */
    unsigned int new_offset = 0;
    /* current offset */
    unsigned int cur_offset = 0;

    /* offset into value_stack */
    unsigned int value_stack_offset = 0;

    /* current values we are working with */
    struct ic_backend_pancake_value *value = 0;
    struct ic_backend_pancake_value *value2 = 0;
    /* value for copyarg to copy into */
    struct ic_backend_pancake_value *to_value = 0;

    /* current call_frame we are working with */
    struct ic_backend_pancake_call_frame *call_frame = 0;

    /* builtin function we are calling */
    ic_backend_function_sig builtin_func = 0;

    /* saved value for returning */
    struct ic_backend_pancake_value saved_return_value;

    if (!runtime_data) {
        puts("ic_backend_pancake_interpret: runtime_data was null");
        return 0;
    }

    instructions = runtime_data->instructions;
    call_frame_stack = runtime_data->call_frame_stack;
    value_stack = runtime_data->value_stack;

    for (;;) {
        instruction = ic_backend_pancake_instructions_get_current(instructions);
        if (!instruction) {
            puts("ic_backend_pancake_interpret: ic_backend_pancake_instructions_get failed");
            return 0;
        }

#if PANCAKE_INTERPRET_DEBUG
        fputs("ic_backend_pancake_interpret: DEBUG looking at: ", stdout);
        ic_backend_pancake_bytecode_print(instruction, stdout);
        puts("");
#endif

        switch (instruction->tag) {
            /* fdecl_label fdecl_sig_call */
            case icp_label:
                /* noop advance */
                break;

            /* panic desc::str */
            case icp_panic:
                str = ic_backend_pancake_bytecode_arg1_get_char(instruction);
                if (!str) {
                    puts("ic_backend_pancake_interpret: ic_backend_pancake_bytecode_arg1_get_char failed");
                    return 0;
                }
                printf("runtime panic: %s\n", str);
                exit(1);
                break;

            /* exit_success */
            case icp_exit:
                return 1;
                break;

            /* push_bool bool */
            case icp_pushbool:
                boolean = ic_backend_pancake_bytecode_arg1_get_bool(instruction);

                value = ic_backend_pancake_value_stack_push(value_stack);
                if (!value) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_push failed");
                    return 0;
                }

                value->tag = ic_backend_pancake_value_type_boolean;
                value->u.boolean = boolean;

                break;

            /* push_uint uint */
            case icp_pushuint:
                uint = ic_backend_pancake_bytecode_arg1_get_uint(instruction);
                value = ic_backend_pancake_value_stack_push(value_stack);
                if (!value) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_push failed");
                    return 0;
                }

                value->tag = ic_backend_pancake_value_type_uint;
                value->u.uint = uint;

                break;

            /* push_int int */
            case icp_pushint:
                sint = ic_backend_pancake_bytecode_arg1_get_sint(instruction);
                value = ic_backend_pancake_value_stack_push(value_stack);
                if (!value) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_push failed");
                    return 0;
                }

                value->tag = ic_backend_pancake_value_type_sint;
                value->u.sint = sint;

                break;

            /* push_str string */
            case icp_pushstr:
                str = ic_backend_pancake_bytecode_arg1_get_char(instruction);
                if (!str) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_bytecode_arg1_get_char failed");
                    return 0;
                }

                value = ic_backend_pancake_value_stack_push(value_stack);
                if (!value) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_push failed");
                    return 0;
                }

                value->tag = ic_backend_pancake_value_type_string;
                value->u.string = str;

                break;

            /* jmp addr::uint */
            case icp_jmp:
                uint = ic_backend_pancake_bytecode_arg1_get_uint(instruction);
                if (!ic_backend_pancake_instructions_set_offset(instructions, uint)) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_instructions_set_offset failed");
                    return 0;
                }

                /* do not advance this round */
                continue;

                break;

            /* jif addr::uint */
            case icp_jif:
                uint = ic_backend_pancake_bytecode_arg1_get_uint(instruction);

                /* get value */
                value = ic_backend_pancake_value_stack_peek(value_stack);

                if (!value) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_peek failed");
                    return 0;
                }

                if (value->tag != ic_backend_pancake_value_type_boolean) {
                    puts("ic_backend_pancake_interpret: value->tag was not of type boolean");
                    return 0;
                }

                boolean = value->u.boolean;

                /* consume value */
                if (!ic_backend_pancake_value_stack_pop(value_stack)) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_pop failed");
                    return 0;
                }

                if (boolean) {
                    if (!ic_backend_pancake_instructions_set_offset(instructions, uint)) {
                        puts("ic_backend_pancake_interpret: call to ic_backend_pancake_instructions_set_offset failed");
                        return 0;
                    }
                }

                /* do not advance this round */
                continue;

                break;

            /* jnif addr::uint */
            case icp_jnif:
                uint = ic_backend_pancake_bytecode_arg1_get_uint(instruction);

                /* get value */
                value = ic_backend_pancake_value_stack_peek(value_stack);

                if (!value) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_peek failed");
                    return 0;
                }

                if (value->tag != ic_backend_pancake_value_type_boolean) {
                    puts("ic_backend_pancake_interpret: value->tag was not of type boolean");
                    return 0;
                }

                boolean = value->u.boolean;

                /* consume value */
                if (!ic_backend_pancake_value_stack_pop(value_stack)) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_pop failed");
                    return 0;
                }

                if (!boolean) {
                    if (!ic_backend_pancake_instructions_set_offset(instructions, uint)) {
                        puts("ic_backend_pancake_interpret: call to ic_backend_pancake_instructions_set_offset failed");
                        return 0;
                    }
                }

                /* do not advance this round */
                continue;

                break;

            /* jmp_label label::string */
            case icp_jmp_label:
                str = ic_backend_pancake_bytecode_arg1_get_char(instruction);
                if (!str) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_bytecode_arg1_get_char failed");
                    return 0;
                }

                /* offset to jump to
                 * NB: this cannot be 0 as 0 is always our entry point and not a valid function
                 * address
                 */
                new_offset = ic_backend_pancake_instructions_get_label(instructions, str);

                if (!new_offset) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_instructions_get_label failed");
                    printf("Couldn't find address of function '%s' called at offset '%u'\n", str, cur_offset);
                    return 0;
                }

                /* jump to offset */
                if (!ic_backend_pancake_instructions_set_offset(instructions, new_offset)) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_instructions_set_offset failed");
                    return 0;
                }

                /* do not advance this round */
                continue;
                break;

            /* jif_label label::string */
            case icp_jif_label:
                /* inspect top of stack
                 * if true then jmp
                 */
                /* get value */
                value = ic_backend_pancake_value_stack_peek(value_stack);

                if (!value) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_peek failed");
                    return 0;
                }

                if (value->tag != ic_backend_pancake_value_type_boolean) {
                    puts("ic_backend_pancake_interpret: value->tag was not of type boolean");
                    return 0;
                }

                boolean = value->u.boolean;

                /* consume value */
                if (!ic_backend_pancake_value_stack_pop(value_stack)) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_pop failed");
                    return 0;
                }

                /* if false, then break and do nothing */
                if (!boolean) {
                    break;
                }

                str = ic_backend_pancake_bytecode_arg1_get_char(instruction);
                if (!str) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_bytecode_arg1_get_char failed");
                    return 0;
                }

                /* offset to jump to
                 * NB: this cannot be 0 as 0 is always our entry point and not a valid function
                 * address
                 */
                new_offset = ic_backend_pancake_instructions_get_label(instructions, str);

                if (!new_offset) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_instructions_get_label failed");
                    printf("Couldn't find address of function '%s' called at offset '%u'\n", str, cur_offset);
                    return 0;
                }

                /* jump to offset */
                if (!ic_backend_pancake_instructions_set_offset(instructions, new_offset)) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_instructions_set_offset failed");
                    return 0;
                }

                /* do not advance this round */
                continue;
                break;

            /* jnif_label label::string */
            case icp_jnif_label:
                /* FIXME TODO inspect top of stack
                 * if false then jmp
                 */

                /* get value */
                value = ic_backend_pancake_value_stack_peek(value_stack);

                if (!value) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_peek failed");
                    return 0;
                }

                if (value->tag != ic_backend_pancake_value_type_boolean) {
                    puts("ic_backend_pancake_interpret: value->tag was not of type boolean");
                    return 0;
                }

                boolean = value->u.boolean;

                /* consume value */
                if (!ic_backend_pancake_value_stack_pop(value_stack)) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_pop failed");
                    return 0;
                }

                /* if true, then break and do nothing */
                if (boolean) {
                    break;
                }

                str = ic_backend_pancake_bytecode_arg1_get_char(instruction);
                if (!str) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_bytecode_arg1_get_char failed");
                    return 0;
                }

                /* offset to jump to
                 * NB: this cannot be 0 as 0 is always our entry point and not a valid function
                 * address
                 */
                new_offset = ic_backend_pancake_instructions_get_label(instructions, str);

                if (!new_offset) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_instructions_get_label failed");
                    printf("Couldn't find address of function '%s' called at offset '%u'\n", str, cur_offset);
                    return 0;
                }

                /* jump to offset */
                if (!ic_backend_pancake_instructions_set_offset(instructions, new_offset)) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_instructions_set_offset failed");
                    return 0;
                }

                /* do not advance this round */
                continue;
                break;

            /* pop n::uint */
            case icp_pop:
                uint = ic_backend_pancake_bytecode_arg1_get_uint(instruction);
                /* pop uint items from the value_stack */
                for (; uint > 0; --uint) {
                    if (!ic_backend_pancake_value_stack_pop(value_stack)) {
                        puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_pop failed");
                        return 0;
                    }
                }
                break;

            /* call fname::string argn::uint */
            case icp_call:
                /* fdecl sig call */
                str = ic_backend_pancake_bytecode_arg1_get_char(instruction);
                if (!str) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_bytecode_arg1_get_char failed");
                    return 0;
                }

                /* n args */
                uint = ic_backend_pancake_bytecode_arg2_get_uint(instruction);

                /* get current offset
                 * NB: fine to return to this value
                 * as the for loop will then advance past it
                 */
                cur_offset = ic_backend_pancake_instructions_get_offset(instructions);

                /* make new element on top of call_frame_stack */
                call_frame = ic_backend_pancake_call_frame_stack_push(call_frame_stack);
                if (!call_frame) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_call_frame_stack_push failed");
                    return 0;
                }

                /* height of value stack */
                value_stack_offset = ic_backend_pancake_value_stack_height(value_stack);

                /* set appropriate call_frame values */
                call_frame->return_offset = cur_offset;
                call_frame->arg_count = uint;
                call_frame->call_start_offset = new_offset;
                call_frame->arg_start = value_stack_offset;

                /* FIXME TODO ignoring uint */

                /* FIXME TODO ignoring arg cleanup details */

                /* offset to jump to
                 * NB: this cannot be 0 as 0 is always our entry point and not a valid function
                 * address
                 */
                new_offset = ic_backend_pancake_instructions_get_label(instructions, str);

                if (!new_offset) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_instructions_get_label failed");
                    printf("Couldn't find address of function '%s' called at offset '%u'\n", str, cur_offset);
                    return 0;
                }

                /* jump to offset */
                if (!ic_backend_pancake_instructions_set_offset(instructions, new_offset)) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_instructions_set_offset failed");
                    return 0;
                }

#if PANCAKE_INTERPRET_DEBUG
                /* DEBUG */
                printf("ic_backend_pancake_interpret: DEBUG icp_call: going to %s at offset %u\n", str, new_offset);
#endif

                /* do not advance this round */
                continue;

                break;

            /* return_value */
            case icp_return_value:
            /* return_void */
            case icp_return_void:
                /* no difference in behavior between return_value and return-void */
                call_frame = ic_backend_pancake_call_frame_stack_peek(call_frame_stack);
                if (!call_frame) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_call_frame_stack_peek failed");
                    return 0;
                }

                new_offset = call_frame->return_offset;

                /* remove value at top of return stack */
                if (!ic_backend_pancake_call_frame_stack_pop(call_frame_stack)) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_call_frame_stack_pop failed");
                    return 0;
                }

                /* jump to offset */
                if (!ic_backend_pancake_instructions_set_offset(instructions, new_offset)) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_instructions_set_offset failed");
                    return 0;
                }

                break;

            /* copyarg argn::uint
             * copyarg at offset argn onto stack
             * Note: arguments to functions are pushed in order
             *       a function call pushes the arguments, and then records the stack
             *       position when the function call starts
             *       `copyarg 0` will refer to the 0th argument before the fcall started
             *       which is the *last* argument
             *       if you had 3 arguments then,
             *       copyarg 0 - 3rd argument
             *       copyarg 1 - 2nd argument
             *       copyarg 2 - 1st argument
             */
            case icp_copyarg:
                call_frame = ic_backend_pancake_call_frame_stack_peek(call_frame_stack);
                if (!call_frame) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_call_frame_stack_peek failed");
                    return 0;
                }

                /* offset for the start of this function */
                value_stack_offset = call_frame->arg_start;

                /* our arg numbers */
                uint = ic_backend_pancake_bytecode_arg1_get_uint(instruction);

                /* the offset of our arg into the value_stack */
                value_stack_offset -= uint;

                /* value to copy onto head of stack */
                value = ic_backend_pancake_value_stack_get_offset(value_stack, value_stack_offset);

                /* create new value on value_stack to copy into */
                to_value = ic_backend_pancake_value_stack_push(value_stack);
                if (!to_value) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_push failed");
                    return 0;
                }

                /* perform copy */
                if (!ic_backend_pancake_value_copy(value, to_value)) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_copy failed");
                    return 0;
                }

                break;

            /* call_builtin fname::string argn::uint */
            case icp_call_builtin:
                /* fdecl sig call */
                str = ic_backend_pancake_bytecode_arg1_get_char(instruction);
                if (!str) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_bytecode_arg1_get_char failed");
                    return 0;
                }

                cur_offset = ic_backend_pancake_instructions_get_offset(instructions);

                /* FIXME TODO ignoring uint */

                /* FIXME TODO should we create a call_frame frame ? */

                /* try fetch our builtin function */
                builtin_func = ic_backend_pancake_builtins_table_get(str);
                if (!builtin_func) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_builtins_table_get failed");
                    printf("tried to fetch builtin func '%s' at offset '%u'\n", str, cur_offset);
                    return 0;
                }

                /* call func */
                if (!builtin_func(value_stack)) {
                    puts("runtime panic: call to builtin func failed");
                    printf("builtin func '%s' called at offset '%u'\n", str, cur_offset);
                    return 0;
                }
                break;

            /* save current top of stack to restore later
             * NB: save will overwrite any previously saved value
             */
            case icp_save:
                /* get value */
                value = ic_backend_pancake_value_stack_peek(value_stack);

                if (!value) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_peek failed");
                    return 0;
                }

                if (!ic_backend_pancake_value_copy(value, &saved_return_value)) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_copy failed");
                    return 0;
                }

                /* consume value */
                if (!ic_backend_pancake_value_stack_pop(value_stack)) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_pop failed");
                    return 0;
                }

                break;

            /* restore previously saved item to top of stack
             * NB: error if no previous item was saved
             */
            case icp_restore:
                value = ic_backend_pancake_value_stack_push(value_stack);

                if (!value) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_push failed");
                    return 0;
                }

                if (!ic_backend_pancake_value_copy(&saved_return_value, value)) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_copy failed");
                    return 0;
                }

                break;

            /* clean_stack
             * resets stack-frame back to how it was before caller inserted args
             */
            case icp_clean_stack:
                /* call_frame contains
                 *  arg_start
                 *
                 * which we know is the height of the value_stack from before
                 * our args were inserted
                 *
                 * so we can reset the value stack back to this height to
                 * cleanup
                 */

                call_frame = ic_backend_pancake_call_frame_stack_peek(call_frame_stack);
                if (!call_frame) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_call_frame_stack_peek failed");
                    return 0;
                }

                value_stack_offset = call_frame->arg_start;

                /* reset value_stack back to this point */
                if (!ic_backend_pancake_value_stack_reset(value_stack, value_stack_offset)) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_offset failed");
                    return 0;
                }

                break;

            /* store key::string
             * stores current top of stack under key
             */
            case icp_store:
                /* key we will store under */
                str = ic_backend_pancake_bytecode_arg1_get_char(instruction);
                if (!str) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_bytecode_arg1_get_char failed for icp_store");
                    return 0;
                }

                /* value from top of stack to store */
                value = ic_backend_pancake_value_stack_peek(value_stack);
                if (!value) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_peek failed for icp_store");
                    return 0;
                }

                /* make a new value to copy into */
                to_value = ic_backend_pancake_value_new();
                if (!to_value) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_new failed for icp_store");
                    return 0;
                }

                /* perform copy of value */
                if (!ic_backend_pancake_value_copy(value, to_value)) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_copy failed for icp_store");
                    return 0;
                }

                /* get our current call_frame */
                call_frame = ic_backend_pancake_call_frame_stack_peek(call_frame_stack);
                if (!call_frame) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_call_frame_stack_peek failed for icp_store");
                    return 0;
                }

                /* store to_value under str on call_frame */
                if (!ic_backend_pancake_call_frame_set(call_frame, str, to_value)) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_call_frame_set failed for icp_store");
                    return 0;
                }

                /* consume value on top of value_stack */
                if (!ic_backend_pancake_value_stack_pop(value_stack)) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_pop failed for icp_store");
                    return 0;
                }

                break;

            /* load key::string
             * loads from key onto under
             */
            case icp_load:
                /* key we will load from */
                str = ic_backend_pancake_bytecode_arg1_get_char(instruction);
                if (!str) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_bytecode_arg1_get_char failed for icp_load");
                    return 0;
                }

                /* get our current call_frame */
                call_frame = ic_backend_pancake_call_frame_stack_peek(call_frame_stack);
                if (!call_frame) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_call_frame_stack_peek failed for icp_load");
                    return 0;
                }

                /* get out our value */
                value = ic_backend_pancake_call_frame_get(call_frame, str);
                if (!value) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_call_frame_get failed for icp_load");
                    return 0;
                }

                /* create new value from top of value stack to store */
                to_value = ic_backend_pancake_value_stack_push(value_stack);
                if (!to_value) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_push failed for icp_load");
                    return 0;
                }

                /* perform copy of value */
                if (!ic_backend_pancake_value_copy(value, to_value)) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_copy failed for icp_load");
                    return 0;
                }

                break;

            /* tailcall_void fname::string argn::uint */
            case icp_tailcall_void:
                puts("ic_backend_pancake_interpret: unimplemented bytecode instruction: icp_tailcall_void");
                return 0;
                break;

            /* tailcall_value fname::string argn::uint */
            case icp_tailcall_value:
                puts("ic_backend_pancake_interpret: unimplemented bytecode instruction: icp_tailcall_value");
                return 0;
                break;

            /* alloc slots::uint
             * allocate a new object with this many slots
             *
             * a slot size is implementation defined, but most be large
             * enough to hold any value
             * this is likely 32 or 64 bits
             */
            case icp_alloc:
                uint = ic_backend_pancake_bytecode_arg1_get_uint(instruction);
                if (!uint) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_bytecode_arg1_get_uint failed, zero-length allocs not supported");
                    return 0;
                }

                /* uint from user is specified as 'number of slots'
                 * convert to units of sizeof(union ic_backend_pancake_value_cell)
                 */
                uint = uint * sizeof(union ic_backend_pancake_value_cell);

                ref = ic_alloc(uint);

                value = ic_backend_pancake_value_stack_push(value_stack);
                if (!value) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_push failed");
                    return 0;
                }

                value->tag = ic_backend_pancake_value_type_ref;
                value->u.ref = ref;

                break;

            /* store_offset slot::uint
             * let value = pop()
             * let object = peek()
             * store value at offset `slot` within object
             */
            case icp_store_offset:
                uint = ic_backend_pancake_bytecode_arg1_get_uint(instruction);

                /* get value */
                value = ic_backend_pancake_value_stack_peek(value_stack);

                if (!value) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_peek failed");
                    return 0;
                }

                /* consume value */
                if (!ic_backend_pancake_value_stack_pop(value_stack)) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_pop failed");
                    return 0;
                }

                /* get value DO NOT POP */
                value2 = ic_backend_pancake_value_stack_peek(value_stack);

                if (!value2) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_peek failed");
                    return 0;
                }

                if (value2->tag != ic_backend_pancake_value_type_ref) {
                    puts("ic_backend_pancake_interpret: value2->tag was not of type ref");
                    return 0;
                }

                /* move to offset within ref */
                ref = value2->u.ref;
                ref_array = ref;
                ref_slot = &(ref_array[uint]);

                /* perform write */
                switch (value->tag) {
                    case ic_backend_pancake_value_type_boolean:
                        ref_slot->boolean = value->u.boolean;
                        break;

                    case ic_backend_pancake_value_type_uint:
                        ref_slot->uint = value->u.uint;
                        break;

                    case ic_backend_pancake_value_type_sint:
                        ref_slot->sint = value->u.sint;
                        break;

                    case ic_backend_pancake_value_type_string:
                        ref_slot->string = value->u.string;
                        break;

                    case ic_backend_pancake_value_type_ref:
                        ref_slot->ref = value->u.ref;
                        break;

                    default:
                        puts("ic_backend_pancake_interpret: impossible value->tag");
                        return 0;
                        break;
                }

                break;

            /* load_offset_bool slot::uint
             * let object = peek()
             * load value at offset `slot` within object and push onto stack as bool
             */
            case icp_load_offset_bool:
                uint = ic_backend_pancake_bytecode_arg1_get_uint(instruction);

                /* get value DO NOT POP */
                value = ic_backend_pancake_value_stack_peek(value_stack);

                if (!value) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_peek failed");
                    return 0;
                }

                /* create new value from top of value stack to store */
                to_value = ic_backend_pancake_value_stack_push(value_stack);
                if (!to_value) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_push failed for icp_load");
                    return 0;
                }

                if (value->tag != ic_backend_pancake_value_type_ref) {
                    puts("ic_backend_pancake_interpret: value->tag was not of type ref");
                    return 0;
                }

                ref = value->u.ref;
                ref_array = ref;
                ref_slot = &(ref_array[uint]);

                to_value->tag = ic_backend_pancake_value_type_boolean;
                to_value->u.boolean = ref_slot->boolean;

                /* success */
                break;

            /* load_offset_str slot::uint
             * let object = peek()
             * load value at offset `slot` within object and push onto stack as str
             */
            case icp_load_offset_str:
                uint = ic_backend_pancake_bytecode_arg1_get_uint(instruction);

                /* get value DO NOT POP */
                value = ic_backend_pancake_value_stack_peek(value_stack);

                if (!value) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_peek failed");
                    return 0;
                }

                /* create new value from top of value stack to store */
                to_value = ic_backend_pancake_value_stack_push(value_stack);
                if (!to_value) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_push failed for icp_load");
                    return 0;
                }

                if (value->tag != ic_backend_pancake_value_type_ref) {
                    puts("ic_backend_pancake_interpret: value->tag was not of type ref");
                    return 0;
                }

                ref = value->u.ref;
                ref_array = ref;
                ref_slot = &(ref_array[uint]);

                to_value->tag = ic_backend_pancake_value_type_string;
                to_value->u.string = ref_slot->string;

                /* success */
                break;

            /* load_offset_uint slot::uint
             * let object = peek()
             * load value at offset `slot` within object and push onto stack as uint
             */
            case icp_load_offset_uint:
                uint = ic_backend_pancake_bytecode_arg1_get_uint(instruction);

                /* get value DO NOT POP */
                value = ic_backend_pancake_value_stack_peek(value_stack);

                if (!value) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_peek failed");
                    return 0;
                }

                /* create new value from top of value stack to store */
                to_value = ic_backend_pancake_value_stack_push(value_stack);
                if (!to_value) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_push failed for icp_load");
                    return 0;
                }

                if (value->tag != ic_backend_pancake_value_type_ref) {
                    puts("ic_backend_pancake_interpret: value->tag was not of type ref");
                    return 0;
                }

                ref = value->u.ref;
                ref_array = ref;
                ref_slot = &(ref_array[uint]);

                to_value->tag = ic_backend_pancake_value_type_uint;
                to_value->u.uint = ref_slot->uint;

                /* success */
                break;

            /* load_offset_sint slot::uint
             * let object = peek()
             * load value at offset `slot` within object and push onto stack as sint
             */
            case icp_load_offset_sint:
                uint = ic_backend_pancake_bytecode_arg1_get_uint(instruction);

                /* get value DO NOT POP */
                value = ic_backend_pancake_value_stack_peek(value_stack);

                if (!value) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_peek failed");
                    return 0;
                }

                /* create new value from top of value stack to store */
                to_value = ic_backend_pancake_value_stack_push(value_stack);
                if (!to_value) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_push failed for icp_load");
                    return 0;
                }

                if (value->tag != ic_backend_pancake_value_type_ref) {
                    puts("ic_backend_pancake_interpret: value->tag was not of type ref");
                    return 0;
                }

                ref = value->u.ref;
                ref_array = ref;
                ref_slot = &(ref_array[uint]);

                to_value->tag = ic_backend_pancake_value_type_sint;
                to_value->u.sint = ref_slot->sint;

                /* sucess */
                break;

            /* load_offset_ref slot::uint
             * let object = peek()
             * load value at offset `slot` within object and push onto stack as ref
             */
            case icp_load_offset_ref:
                uint = ic_backend_pancake_bytecode_arg1_get_uint(instruction);

                /* get value DO NOT POP */
                value = ic_backend_pancake_value_stack_peek(value_stack);

                if (!value) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_peek failed");
                    return 0;
                }

                /* create new value from top of value stack to store */
                to_value = ic_backend_pancake_value_stack_push(value_stack);
                if (!to_value) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_push failed for icp_load");
                    return 0;
                }

                if (value->tag != ic_backend_pancake_value_type_ref) {
                    puts("ic_backend_pancake_interpret: value->tag was not of type ref");
                    return 0;
                }

                ref = value->u.ref;
                ref_array = ref;
                ref_slot = &(ref_array[uint]);

                to_value->tag = ic_backend_pancake_value_type_ref;
                to_value->u.ref = ref_slot->ref;

                /* success */
                break;

            default:
                puts("ic_backend_pancake_interpret: unknown bytecode instructions");
                return 0;
                break;
        }

        /* advance instructions */
        instruction = ic_backend_pancake_instructions_advance(instructions);
        if (!instruction) {
            puts("ic_backend_pancake_interpret: call to ic_backend_pancake_instructions_advance failed");
            return 0;
        }
    }

    puts("ic_backend_pancake_interpret: unimplemented");
    return 0;
}
