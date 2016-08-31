#include <stdio.h>
#include <stdlib.h>

#include "data/runtime.h"
#include "pancake.h"

#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

/* interpret bytecode in runtime
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_interpret(struct ic_backend_pancake_runtime *runtime) {
    struct ic_backend_pancake_bytecode *instruction = 0;
    struct ic_backend_pancake_instructions *instructions = 0;
    struct ic_backend_pancake_call_info_stack *call_info_stack = 0;
    struct ic_backend_pancake_value_stack *value_stack = 0;

    /* values inside our instructions we may want to unpack */
    char *str = 0;
    int sint = 0;
    unsigned int uint = 0;
    bool boolean = false;

    /* new offset */
    unsigned int new_offset = 0;
    /* current offset */
    unsigned int cur_offset = 0;

    /* offset into value_stack */
    unsigned int value_stack_offset = 0;

    /* current value we are working with */
    struct ic_backend_pancake_value *value = 0;
    /* value for copyarg to copy into */
    struct ic_backend_pancake_value *to_value = 0;

    /* current call_info we are working with */
    struct ic_backend_pancake_call_info *call_info = 0;

    if (!runtime) {
        puts("ic_backend_pancake_interpret: runtime was null");
        return 0;
    }

    instructions = runtime->instructions;
    call_info_stack = runtime->call_info_stack;
    value_stack = runtime->value_stack;

    for (;;) {
        instruction = ic_backend_pancake_instructions_get_current(instructions);
        if (!instruction) {
            puts("ic_backend_pancake_interpret: ic_backend_pancake_instructions_get failed");
            return 0;
        }

        fputs("ic_backend_pancake_interpret: DEBUG looking at: ", stdout);
        ic_backend_pancake_bytecode_print(instruction, stdout);
        puts("");

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
                printf("Runtime panic: %s\n", str);
                exit(1);
                break;

            /* exit_success */
            case icp_exit:
                exit(0);
                break;

            /* push_bool bool */
            case icp_pushbool:
                boolean = ic_backend_pancake_bytecode_arg1_get_bool(instruction);

                value = ic_backend_pancake_value_stack_push(value_stack);
                if (!value) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_value_stack_push failed");
                    return 0;
                }

                value->tag = ic_backend_pancake_value_type_bool;
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

                if (value->tag != ic_backend_pancake_value_type_bool) {
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

                if (value->tag != ic_backend_pancake_value_type_bool) {
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

                /* make new element on top of call_info_stack */
                call_info = ic_backend_pancake_call_info_stack_push(call_info_stack);
                if (!call_info) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_call_info_stack_push failed");
                    return 0;
                }

                /* height of value stack */
                value_stack_offset = ic_backend_pancake_value_stack_height(value_stack);

                /* set appropriate call_info values */
                call_info->return_offset = cur_offset;
                call_info->arg_count = uint;
                call_info->call_start_offset = new_offset;

                /* FIXME TODO ignoring uint */

                /* FIXME TODO ignoring arg cleanup details */

                /* offset to jump to
                 * NB: this cannot be 0 as 0 is always our entry point and not a valid function
                 * address
                 */
                new_offset = ic_backend_pancake_instructions_get_fdecl(instructions, str);

                if (!new_offset) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_instructions_get_fdecl failed");
                    printf("Couldn't find address of function '%s' called at offset '%u'\n", str, cur_offset);
                    return 0;
                }

                /* jump to offset */
                if (!ic_backend_pancake_instructions_set_offset(instructions, new_offset)) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_instructions_set_offset failed");
                    return 0;
                }

                /* DEBUG */
                printf("ic_backend_pancake_interpret: DEBUG icp_call: going to %s at offset %u\n", str, new_offset);

                /* do not advance this round */
                continue;

                break;

            /* return_void */
            case icp_return_void:
                call_info = ic_backend_pancake_call_info_stack_peek(call_info_stack);
                if (!call_info) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_call_info_stack_peek failed");
                    return 0;
                }

                new_offset = call_info->return_offset;

                /* remove value at top of return stack */
                if (!ic_backend_pancake_call_info_stack_pop(call_info_stack)) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_call_info_stack_pop failed");
                    return 0;
                }

                /* FIXME TODO ignoring arg cleanup details */

                /* jump to offset */
                if (!ic_backend_pancake_instructions_set_offset(instructions, new_offset)) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_instructions_set_offset failed");
                    return 0;
                }

                break;

            /* copyarg argn::uint */
            case icp_copyarg:
                call_info = ic_backend_pancake_call_info_stack_peek(call_info_stack);
                if (!call_info) {
                    puts("ic_backend_pancake_interpret: call to ic_backend_pancake_call_info_stack_peek failed");
                    return 0;
                }

                /* offset for the start of this function */
                value_stack_offset = call_info->call_start_offset;

                /* our arg numbers */
                uint = ic_backend_pancake_bytecode_arg1_get_uint(instruction);

                /* the offset of our arg into the value_stack */
                value_stack_offset += uint;

                /* value to copy onto head of stack */
                value = ic_backend_pancake_value_stack_get_offset(value_stack, new_offset);

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

            /* return_value */
            case icp_return_value:
            /* save current top of stack to restore later
             * NB: save will overwrite any previously saved value
             */
            case icp_save:
            /* restore previously saved item to top of stack
             * NB: error if no previous item was saved
             */
            case icp_restore:
            /* store key::string
             * stores current top of stack under key
             */
            case icp_store:
            /* load key::string
             * loads from key onto under
             */
            case icp_load:
            /* tailcall fname::string argn::uint */
            case icp_tailcall:
            /* call_builtin fname::string argn::uint */
            case icp_call_builtin:

                puts("ic_backend_pancake_interpret: unimplemeneted bytecode instruction");
                return 0;
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
