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
    struct ic_backend_pancake_return_stack *return_stack = 0;
    struct ic_backend_pancake_value_stack *value_stack = 0;

    /* values inside our instructions we may want to unpack */
    char *str = 0;
    int sint = 0;
    unsigned int uint = 0;
    bool boolean = false;

    /* current value we are working with */
    struct ic_backend_pancake_value *value = 0;

    if (!runtime) {
        puts("ic_backend_pancake_interpret: runtime was null");
        return 0;
    }

    instructions = runtime->instructions;
    return_stack = runtime->return_stack;
    value_stack = runtime->value_stack;

    for (;;) {
        instruction = ic_backend_pancake_instructions_get_current(instructions);
        if (!instruction) {
            puts("ic_backend_pancake_interpret: ic_backend_pancake_instructions_get failed");
            return 0;
        }

        switch (instruction->tag) {
            /* fdecl_label fdecl_sig_call */
            case icp_label:
                /* noop advance */
                /* assigning to instruction just to check, not used */
                instruction = ic_backend_pancake_instructions_advance(instructions);
                if (!instruction) {
                    puts("ic_backend_pancake_interpret: ic_backend_pancake_instructions_advance failed");
                    return 0;
                }
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

            /* push key::string */
            case icp_push:
            /* copyarg argn::uint */
            case icp_copyarg:
            /* call fname::string argn::uint */
            case icp_call:
            /* pop n::uint */
            case icp_pop:
            /* return */
            case icp_return:
            /* jmp addr::uint */
            case icp_jmp:
            /* jif addr::uint */
            case icp_jif:
            /* jnif addr::uint */
            case icp_jnif:
            /* save current top of stack to restore later
             * NB: save will overwrite any previously saved value
             */
            case icp_save:
            /* restore previously saved item to top of stack
             * NB: error if no previous item was saved
             */
            case icp_restore:
            /* store key::string
             * stores current top of stack under key */
            case icp_store:
            /* load key::string
             * loads from key onto under
             * FIXME TODO figure out load vs push */
            case icp_load:

                puts("ic_backend_pancake_interpret: unimplemeneted bytecode instruction");
                return 0;
                break;

            default:
                puts("ic_backend_pancake_interpret: unknown bytecode instructions");
                return 0;
                break;
        }
    }

    puts("ic_backend_pancake_interpret: unimplemented");
    return 0;
}