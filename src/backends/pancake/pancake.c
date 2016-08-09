#include <stdio.h>

#include "../../analyse/data/kludge.h"
#include "../../data/dict.h"
#include "../../transform/data/tbody.h"
#include "data/bytecode.h"
#include "data/instructions.h"
#include "data/local.h"
#include "data/runtime.h"
#include "pancake.h"
#include "tir.h"

/* compile a kludge into bytecode
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_instructions *ic_backend_pancake_compile(struct ic_kludge *kludge);

/* interpret bytecode in runtime
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_interpret(struct ic_backend_pancake_runtime *runtime);

/* compile an fdecl into bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_fdecl(struct ic_backend_pancake_instructions *instructions, struct ic_kludge *kludge, struct ic_decl_func *fdecl);

/* pancake - stack based interpreter backend
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake(struct ic_kludge *kludge) {
    struct ic_backend_pancake_instructions *instructions = 0;
    struct ic_backend_pancake_runtime *runtime = 0;

    if (!kludge) {
        puts("ic_backend_pancake: kludge was null");
        return 0;
    }

    instructions = ic_backend_pancake_compile(kludge);
    if (!instructions) {
        puts("ic_backend_pancake: call to ic_backend_pancake_compile failed");
        return 0;
    }

    runtime = ic_backend_pancake_runtime_new(instructions);
    if (!runtime) {
        puts("ic_backend_pancake: call to ic_backend_pancake_runtime_new failed");
        return 0;
    }

    if (!ic_backend_pancake_interpret(runtime)) {
        puts("ic_backend_pancake: call to ic_backend_pancake_interpret failed");
        return 0;
    }

    puts("ic_backend_pancake: unimplemented");
    return 1;
}

/* compile a kludge into bytecode
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_backend_pancake_instructions *ic_backend_pancake_compile(struct ic_kludge *kludge) {
    struct ic_backend_pancake_instructions *instructions = 0;
    /* offset into kludge fdecls */
    unsigned int offset = 0;
    /* length of kludge fdecls */
    unsigned int len = 0;
    /* current kludge fdecl */
    struct ic_decl_func *fdecl = 0;
    /* bytecode instruction used for entry label jump */
    struct ic_backend_pancake_bytecode *bc_entry_label = 0;
    /* bytecode instruction used for entry jump */
    struct ic_backend_pancake_bytecode *bc_entry_jump = 0;
    /* bytecode instruction used for entry exit */
    struct ic_backend_pancake_bytecode *bc_entry_exit = 0;
    /* offset for main function */
    unsigned int main_offset = 0;

    if (!kludge) {
        puts("ic_backend_pancake_compile: kludge was null");
        return 0;
    }

    instructions = ic_backend_pancake_instructions_new();
    if (!instructions) {
        puts("ic_backend_pancake_compile: call to ic_backend_pancake_instructions_new failed");
        return 0;
    }

    len = ic_pvector_length(&(kludge->fdecls));
    if (!len) {
        puts("ic_backend_pancake_compile: call to ic_pvector_length failed");
        return 0;
    }

    /* create dummy top-level program entry point
     * later on we will make this jump instead to 'main'
     * but first we have to make main - which we do below just as with every
     * other function
     */
    /* insert label */
    bc_entry_label = ic_backend_pancake_bytecode_new(icp_label);
    if (!bc_entry_label) {
        puts("ic_backend_pancake_compile: call to ic_backend_pancake_bytecode_new failed for entry_label");
        return 0;
    }
    if (!ic_backend_pancake_bytecode_arg1_set_char(bc_entry_label, "entry")) {
        puts("ic_backend_pancake_compile: call to ic_backend_pancake_bytecode_arg1_set_char failed for entry_label");
        return 0;
    }
    if (!ic_backend_pancake_instructions_append(instructions, bc_entry_label)) {
        puts("ic_backend_pancake_compile: call to ic_backend_pancake_instructions_append failed for entry_label");
        return 0;
    }

    /* insert jump 0 */
    bc_entry_jump = ic_backend_pancake_bytecode_new(icp_jmp);
    if (!bc_entry_jump) {
        puts("ic_backend_pancake_compile: call to ic_backend_pancake_bytecode_new failed for entry_jump");
        return 0;
    }
    if (!ic_backend_pancake_instructions_append(instructions, bc_entry_jump)) {
        puts("ic_backend_pancake_compile: call to ic_backend_pancake_instructions_append failed for entry_jump");
        return 0;
    }

    /* insert exit */
    bc_entry_exit = ic_backend_pancake_bytecode_new(icp_exit);
    if (!bc_entry_exit) {
        puts("ic_backend_pancake_compile: call to ic_backend_pancake_bytecode_new failed for entry_exit");
        return 0;
    }
    if (!ic_backend_pancake_instructions_append(instructions, bc_entry_exit)) {
        puts("ic_backend_pancake_compile: call to ic_backend_pancake_instructions_append failed for entry_exit");
        return 0;
    }

    /* sanity check our length */
    if (3 != ic_backend_pancake_instructions_length(instructions)) {
        puts("ic_backend_pancake_compile: instructions length was not 3, something went wrong");
        return 0;
    }

    /* go through each fdecl, pull it out, call compile_fdecl to do work */
    for (offset = 0; offset < len; ++offset) {
        fdecl = ic_pvector_get(&(kludge->fdecls), offset);
        if (!fdecl) {
            puts("ic_backend_pancake_compile: call to ic_pvector_get failed");
            return 0;
        }

        if (!ic_backend_pancake_compile_fdecl(instructions, kludge, fdecl)) {
            puts("ic_backend_pancake_compile: call to ic_backend_pancake_compile_fdecl failed");
            return 0;
        }
    }

    /* pull out address of offset */
    main_offset = ic_backend_pancake_instructions_get_fdecl(instructions, "main()");
    /* NB: we cannot test for failure of this function, but we do know that address
     * '0' is our entry point - so main cannot be it
     */
    if (!main_offset) {
        puts("ic_backend_pancake_compile: couldn't find main function");
        return 0;
    }
    /* modify instruction 0 to jump to main offset */
    if (!ic_backend_pancake_bytecode_arg1_set_uint(bc_entry_jump, main_offset)) {
        puts("ic_backend_pancake_compile: call to ic_backend_pancake_bytecode_arg1_set_uint failed for entry_jump");
        return 0;
    }

    puts("ic_backend_pancake_compile: implementation pending");
    return instructions;
}

/* compile an fdecl into bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_fdecl(struct ic_backend_pancake_instructions *instructions, struct ic_kludge *kludge, struct ic_decl_func *fdecl) {
    /* the stack offset this function starts at
     * NB: this is one *after* the label
     * this is the position of the first argument
     */
    unsigned int fdecl_stack_offset = 0;
    /* char * to sig_call for current fdec
     * used for bytecode
     */
    char *fdecl_sig_call = 0;
    /* tbody of current fdecl */
    struct ic_transform_body *fdecl_tbody = 0;
    /* offset we created fdecl at */
    unsigned int fdecl_instructions_offset = 0;
    /* dummy bytecode for fdecl */
    struct ic_backend_pancake_bytecode *bc_dummy_fdecl = 0;

    /* len of args OR tbody */
    unsigned int len = 0;
    /* offset into args OR tbody */
    unsigned int i = 0;
    /* current arg */
    struct ic_field *arg = 0;
    char *arg_name_ch = 0;
    /* current tstmt */
    struct ic_transform_ir_stmt *tstmt = 0;

    /* let, used only if tstmt is let */
    struct ic_transform_ir_let *tlet = 0;

    /* dict from char* to pancake/data/local */
    struct ic_dict *locals = 0;
    /* pvector of keys to dict, as we cannot recover these from dict */
    struct ic_pvector *locals_keys = 0;

    /* current local */
    struct ic_backend_pancake_local *local = 0;

    if (!instructions) {
        puts("ic_backend_pancake_compile_fdecl: instructions was null");
        return 0;
    }

    if (!kludge) {
        puts("ic_backend_pancake_compile_fdecl: kludge was null");
        return 0;
    }

    if (!fdecl) {
        puts("ic_backend_pancake_compile_fdecl: fdecl_tbody was null");
        return 0;
    }

    /* FIXME TODO compile kludge into bytecode instructions */

    /* for each fdecl
     *  get current pos, insert into map of fdecl-sig to pos
     *  deal with args ???
     *  for each instruction in fdecl
     *    compile
     *  end
     *  pop args
     * end
     */

    fdecl_sig_call = ic_decl_func_sig_call(fdecl);
    if (!fdecl_sig_call) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_decl_func_sig_call failed");
        return 0;
    }

    /* FIXME TODO how does pancake handle builtin functions? */
    /* FIXME TODO for now skip builtin functions */
    if (fdecl->builtin) {
        printf("warning: ic_backend_pancake_compile_fdecl: skipping builtin fdecl '%s'\n", fdecl_sig_call);
        return 1;
    }

    /* get length - which is offset of next instruction */
    fdecl_instructions_offset = ic_backend_pancake_instructions_length(instructions);

    /* register function at offset */
    if (!ic_backend_pancake_instructions_register_fdecl(instructions, fdecl_sig_call, fdecl_instructions_offset)) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_instructions_register_fdecl failed");
        return 0;
    }

    /* insert dummy no-op function label instruction */
    bc_dummy_fdecl = ic_backend_pancake_bytecode_new(icp_label);
    if (!bc_dummy_fdecl) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_bytecode_new failed");
        return 0;
    }

    if (!ic_backend_pancake_bytecode_arg1_set_char(bc_dummy_fdecl, fdecl_sig_call)) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_bytecode_arg1_set_char failed");
        return 0;
    }

    if (!ic_backend_pancake_instructions_append(instructions, bc_dummy_fdecl)) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_instructions_append failed");
        return 0;
    }

    /* get stack offset of first arg to this function */
    fdecl_stack_offset = ic_backend_pancake_instructions_length(instructions);
    if (!fdecl_stack_offset) {
        /* usually we cannot check failure, but we know we have just inserted a label */
        puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_instructions_length failed");
        return 0;
    }

    /* instantiate dict mapping
     *  name -> {accessed::bool, location::int, value::literal_constant}
     */
    locals = ic_dict_new();
    if (!locals) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_dict_new failed");
        return 0;
    }

    locals_keys = ic_pvector_new(0);
    if (!locals_keys) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_pvector_new failed");
        return 0;
    }

    /* register all args
     * we register each argument as the offset from the start of this function
     * calling-convention:
     *  args are passed in-order
     *  caller cleans
     */
    len = ic_pvector_length(&(fdecl->args));
    for (i = 0; i < len; ++i) {
        arg = ic_pvector_get(&(fdecl->args), i);
        if (!arg) {
            puts("ic_backend_pancake_compile_fdecl: call to ic_pvector_get failed");
            return 0;
        }

        local = ic_backend_pancake_local_new(&(arg->name), icpl_offset);
        if (!local) {
            puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_local_new failed");
            return 0;
        }

        if (!ic_backend_pancake_local_set_offset(local, i)) {
            puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_local_set_offset failed");
            return 0;
        }

        arg_name_ch = ic_symbol_contents(&(arg->name));
        if (!arg_name_ch) {
            puts("ic_backend_pancake_compile_fdecl: call to ic_symbol_contents failed");
            return 0;
        }

        if (!ic_dict_insert(locals, arg_name_ch, local)) {
            puts("ic_backend_pancake_compile_fdecl: call to ic_dict_insert failed");
            return 0;
        }

        if (-1 == ic_pvector_append(locals_keys, arg_name_ch)) {
            puts("ic_backend_pancake_compile_fdecl: call to ic_pvector_append failed");
            return 0;
        }
    }

    /* for each statement we currently have 4 possibilities:
     *
     *  let name::type = literal
     *    register name to {value:literal, accessed:0}
     *  let name::type = fcall(args...)
     *    push all args onto stack (using dict)
     *    call fcall
     *    register return position to name (along with count)
     *  fcall(args...)
     *    push all args onto stack (using dict)
     *    call fcall
     *    if non-void:
     *      pop 1 - throw away return value, give warning
     *  return name
     *    set return_register to name
     *    invoke exit process
     *
     *
     */
    /* FIXME TODO */

    fdecl_tbody = fdecl->tbody;
    if (!fdecl_tbody) {
        puts("ic_backend_pancake_compile_fdecl: no tbody found on fdecl");
        return 0;
    }

    len = ic_transform_body_length(fdecl_tbody);

    for (i = 0; i < len; ++i) {
        tstmt = ic_transform_body_get(fdecl_tbody, i);
        if (!tstmt) {
            puts("ic_backend_pancake_compile_fdecl: call to ic_transform_body_get failed");
            return 0;
        }

        switch (tstmt->tag) {
            case ic_transform_ir_stmt_type_expr:
                puts("ic_backend_pancake_compile_fdecl: expr unimplemented");
                return 0;
                break;

            case ic_transform_ir_stmt_type_let:
                tlet = ic_transform_ir_stmt_get_let(tstmt);
                if (!tlet) {
                    puts("ic_backend_pancake_compile_fdecl: call to ic_transform_ir_stmt_get_let failed");
                    return 0;
                }

                switch (tlet->tag) {
                    case ic_transform_ir_let_type_literal:
                        puts("ic_backend_pancake_compile_fdecl: let literal unimplemented");
                        return 0;
                        break;

                    case ic_transform_ir_let_type_expr:
                        puts("ic_backend_pancake_compile_fdecl: let expr unimplemented");
                        return 0;
                        break;

                    default:
                        puts("ic_backend_pancake_compile_fdecl: let impossible case");
                        return 0;
                        break;
                }

                puts("ic_backend_pancake_compile_fdecl: let impossible case");
                return 0;
                break;

            case ic_transform_ir_stmt_type_ret:
                puts("ic_backend_pancake_compile_fdecl: return unimplemented");
                return 0;
                break;

            case ic_transform_ir_stmt_type_assign:
                puts("ic_backend_pancake_compile_fdecl: assign unimplemented");
                return 0;
                break;

            default:
                puts("ic_backend_pancake_compile_fdecl: impossible case");
                return 0;
                break;
        }
    }

    /*
     *  exit process
     *    insert instruction to save return value to return_register
     *    walk dict
     *      warn for every item with an access count of 0
     *      free every literal
     *      for non-literals, add pop instruction
     *    if return_register is set, add push instruction
     *    if void and pushed, error
     *    is non-void and didn't push, error
     *    add pop-return address instruction
     *    jump to return address
     */
    /* FIXME TODO */

    /* destroy locals_keys pvector
     * free it
     * keys are all managed by arg->name symbo
     */
    if (!ic_pvector_destroy(locals_keys, 1, 0)) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_pvector_destroy failed");
        return 0;
    }

    /* destroy dict
     * free dict
     * free values (all locals created in this scope)
     */
    if (!ic_dict_destroy(locals, 1, 1)) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_dict_destroy failed");
        return 0;
    }

    return 1;
}

/* interpret bytecode in runtime
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_interpret(struct ic_backend_pancake_runtime *runtime) {
    if (!runtime) {
        puts("ic_backend_pancake_interpret: runtime was null");
        return 0;
    }

    puts("ic_backend_pancake_interpret: unimplemented");
    return 0;
}
