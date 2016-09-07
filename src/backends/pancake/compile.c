#include <stdio.h>

#include "../../analyse/data/kludge.h"
#include "../../parse/data/expr.h"
#include "../../transform/data/tbody.h"
#include "data/bytecode.h"
#include "data/instructions.h"
#include "data/local.h"
#include "pancake.h"

/* compile an fdecl into bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_fdecl(struct ic_backend_pancake_instructions *instructions, struct ic_kludge *kludge, struct ic_decl_func *fdecl);

/* compile an fdecl_body into bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_fdecl_body(struct ic_backend_pancake_instructions *instructions, struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *fdecl_tbody, struct ic_dict *locals);

/* compile an expr (function call) into pancake bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_expr(struct ic_backend_pancake_instructions *instructions, struct ic_kludge *kludge, struct ic_dict *locals, struct ic_transform_ir_expr *texpr, unsigned int *is_void);

/* compile an function call into pancake bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_fcall(struct ic_backend_pancake_instructions *instructions, struct ic_kludge *kludge, struct ic_dict *locals, struct ic_transform_ir_fcall *tfcall, unsigned int *is_void);

/* add a push instruction for a constant
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_push_constant(struct ic_backend_pancake_instructions *instructions, struct ic_backend_pancake_local *local);

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
    /* bytecode instruction used for entry call to main */
    struct ic_backend_pancake_bytecode *bc_entry_call = 0;
    /* bytecode instruction used for entry exit */
    struct ic_backend_pancake_bytecode *bc_entry_exit = 0;

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
    bc_entry_label = ic_backend_pancake_instructions_add(instructions, icp_label);
    if (!bc_entry_label) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_instructions_add failed");
        return 0;
    }
    if (!ic_backend_pancake_bytecode_arg1_set_char(bc_entry_label, "entry")) {
        puts("ic_backend_pancake_compile: call to ic_backend_pancake_bytecode_arg1_set_char failed for entry_label");
        return 0;
    }

    /* insert jump 0
     * later on the '0' will be replaced with the address of main
     */
    bc_entry_call = ic_backend_pancake_instructions_add(instructions, icp_call);
    if (!bc_entry_call) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_instructions_add failed");
        return 0;
    }
    if (!ic_backend_pancake_bytecode_arg1_set_char(bc_entry_call, "main()")) {
        puts("ic_backend_pancake_compile: call to ic_backend_pancake_bytecode_arg1_set_uint failed for entry_jump");
        return 0;
    }
    if (!ic_backend_pancake_bytecode_arg2_set_uint(bc_entry_call, 0)) {
        puts("ic_backend_pancake_compile: call to ic_backend_pancake_bytecode_arg1_set_uint failed for entry_jump");
        return 0;
    }

    /* insert exit */
    bc_entry_exit = ic_backend_pancake_instructions_add(instructions, icp_exit);
    if (!bc_entry_exit) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_instructions_add failed");
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
    char *local_name_ch = 0;

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

    /* skip builtin function */
    if (fdecl->builtin) {
        // printf("warning: ic_backend_pancake_compile_fdecl: skipping builtin fdecl '%s'\n", fdecl_sig_call);
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
    bc_dummy_fdecl = ic_backend_pancake_instructions_add(instructions, icp_label);
    if (!bc_dummy_fdecl) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_instructions_add failed");
        return 0;
    }
    if (!ic_backend_pancake_bytecode_arg1_set_char(bc_dummy_fdecl, fdecl_sig_call)) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_bytecode_arg1_set_char failed");
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

        local_name_ch = ic_symbol_contents(&(arg->name));
        if (!local_name_ch) {
            puts("ic_backend_pancake_compile_fdecl: call to ic_symbol_contents failed");
            return 0;
        }

        if (!ic_dict_insert(locals, local_name_ch, local)) {
            puts("ic_backend_pancake_compile_fdecl: call to ic_dict_insert failed");
            return 0;
        }

        if (-1 == ic_pvector_append(locals_keys, local_name_ch)) {
            puts("ic_backend_pancake_compile_fdecl: call to ic_pvector_append failed");
            return 0;
        }
    }

    fdecl_tbody = fdecl->tbody;
    if (!fdecl_tbody) {
        puts("ic_backend_pancake_compile_fdecl: no tbody found on fdecl");
        return 0;
    }

    if (!ic_backend_pancake_compile_fdecl_body(instructions, kludge, fdecl, fdecl_tbody, locals)) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_compile_fdecl_body failed");
        return 0;
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
    len = ic_pvector_length(locals_keys);
    for (i = 0; i < len; ++i) {
        local_name_ch = ic_pvector_get(locals_keys, i);
        if (!local_name_ch) {
            puts("ic_backend_pancake_compile_fdecl: call to ic_pvector_get failed");
            return 0;
        }

        local = ic_dict_get(locals, local_name_ch);
        if (!local) {
            puts("ic_backend_pancake_compile_fdecl: call to ic_dict_get failed");
            return 0;
        }

        /* print warning if local variable was never accessed */
        if (!local->accessed) {
            printf("Pancake: Warning: unused local variable '%s'\n", local_name_ch);
        }
    }

    /* insert a cleanup and return_void instruction
     * if a function used a return statement then it is likely we will never
     * actually hit these instructions
     *
     * FIXME TODO only insert if not preceeded by return statement
     */
    if (!ic_backend_pancake_instructions_add(instructions, icp_clean_frame)) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_instructions_add failed");
        return 0;
    }
    if (!ic_backend_pancake_instructions_add(instructions, icp_return_void)) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_instructions_add failed");
        return 0;
    }

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

/* compile an fdecl_body into bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_fdecl_body(struct ic_backend_pancake_instructions *instructions, struct ic_kludge *kludge, struct ic_decl_func *fdecl, struct ic_transform_body *fdecl_tbody, struct ic_dict *locals) {
    /* current offset into body */
    unsigned int i = 0;
    /* len of body */
    unsigned int len = 0;
    /* current stmt in body */
    struct ic_transform_ir_stmt *tstmt = 0;

    /* let, used only if tstmt is let
     * used to decompose let
     */
    struct ic_transform_ir_let *tlet = 0;
    struct ic_transform_ir_expr *texpr = 0;
    struct ic_transform_ir_let_literal *tlet_lit = 0;
    struct ic_transform_ir_let_expr *tlet_expr = 0;

    /* name of literal */
    char *let_literal_name_ch = 0;

    /* name of variable in return stmt */
    char *ret_name = 0;

    /* current local */
    struct ic_backend_pancake_local *local = 0;

    /* instruction we generate */
    struct ic_backend_pancake_bytecode *instruction = 0;

    /* out-of-band return value from ic_backend_pancake_compile_fcall */
    unsigned int fcall_is_void = 0;

    if (!instructions) {
        puts("ic_backend_pancake_compile_fdecl_body: instructions was null");
        return 0;
    }

    if (!kludge) {
        puts("ic_backend_pancake_compile_fdecl_body: kludge was null");
        return 0;
    }

    if (!fdecl) {
        puts("ic_backend_pancake_compile_fdecl_body: fdecl was null");
        return 0;
    }

    if (!fdecl_tbody) {
        puts("ic_backend_pancake_compile_fdecl_body: fdecl_tbody was null");
        return 0;
    }

    if (!locals) {
        puts("ic_backend_pancake_compile_fdecl_body: locals was null");
        return 0;
    }

    len = ic_transform_body_length(fdecl_tbody);

    /* for each statement we currently have 4 possibilities:
     *
     *  let name::type = literal
     *    register name to {value:literal, accessed:0}
     *  let name::type = fcall(args...)
     *    push all args onto stack (using dict)
     *    call fcall
     *    register return position to name (along with count)
     *    if void:
     *      compile-time error
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

    for (i = 0; i < len; ++i) {
        tstmt = ic_transform_body_get(fdecl_tbody, i);
        if (!tstmt) {
            puts("ic_backend_pancake_compile_fdecl_body: call to ic_transform_body_get failed");
            return 0;
        }

        switch (tstmt->tag) {
            case ic_transform_ir_stmt_type_expr:
                texpr = &(tstmt->u.expr);
                if (!ic_backend_pancake_compile_expr(instructions, kludge, locals, texpr, &fcall_is_void)) {
                    puts("ic_backend_pancake_compile_fdecl_body: call to ic_backend_pancake_compile_expr failed");
                    return 0;
                }

                if (!fcall_is_void) {
                    puts("ic_backend_pancake_compile_fdecl_body: function used in void context but was not void");
                    printf("non-void function called in void context\n");
                    return 0;
                }

                break;

            case ic_transform_ir_stmt_type_let:
                tlet = ic_transform_ir_stmt_get_let(tstmt);
                if (!tlet) {
                    puts("ic_backend_pancake_compile_fdecl_body: call to ic_transform_ir_stmt_get_let failed");
                    return 0;
                }

                switch (tlet->tag) {
                    case ic_transform_ir_let_type_literal:
                        /* FIXME TODO consider adding some error handling */
                        tlet_lit = &(tlet->u.lit);
                        local = ic_backend_pancake_local_new(tlet_lit->name, icpl_literal);
                        if (!local) {
                            puts("ic_backend_pancake_compile_fdecl_body: call to ic_backend_pancake_local failed");
                            return 0;
                        }
                        if (!ic_backend_pancake_local_set_literal(local, tlet_lit->literal)) {
                            puts("ic_backend_pancake_compile_fdecl_body: call to ic_backend_pancake_set_literal failed");
                            return 0;
                        }
                        let_literal_name_ch = ic_symbol_contents(tlet_lit->name);
                        if (!let_literal_name_ch) {
                            puts("ic_backend_pancake_compile_fdecl_body: call to ic_symbol_contents failed");
                            return 0;
                        }
                        if (!ic_dict_insert(locals, let_literal_name_ch, local)) {
                            puts("ic_backend_pancake_compile_fdecl_body: call to ic_dict_insert failed");
                            return 0;
                        }
                        break;

                    case ic_transform_ir_let_type_expr:
                        /*
                         *  let name::type = fcall(args...)
                         *    push all args onto stack (using dict)
                         *    call fcall
                         *    register return position to name (along with access count)
                         *    if void:
                         *      compile-time error
                         */

                        /* FIXME TODO consider adding some error handling */
                        tlet_expr = &(tlet->u.expr);
                        texpr = tlet_expr->expr;
                        if (!ic_backend_pancake_compile_expr(instructions, kludge, locals, texpr, &fcall_is_void)) {
                            puts("ic_backend_pancake_compile_fdecl_body: let expr call to ic_backend_pancake_compile_expr failed");
                            return 0;
                        }

                        if (fcall_is_void) {
                            puts("ic_backend_pancake_compile_fdecl_body: function used in let was void");
                            printf("function called and assigned to let '%s', but function is void\n", let_literal_name_ch);
                            return 0;
                        }

                        puts("ic_backend_pancake_compile_fdecl_body: let expr unimplemented");
                        return 0;
                        break;

                    default:
                        puts("ic_backend_pancake_compile_fdecl_body: let impossible case");
                        return 0;
                        break;
                }

                break;

            case ic_transform_ir_stmt_type_ret:
                ret_name = ic_symbol_contents(tstmt->u.ret.var);
                if (!ret_name) {
                    puts("ic_backend_pancake_compile_fdecl_body: call to ic_symbol_contents failed");
                    return 0;
                }
                local = ic_dict_get(locals, ret_name);
                if (!local) {
                    puts("ic_backend_pancake_compile_fdecl_body: call to ic_dict_get failed");
                    return 0;
                }
                /* mark as accessed */
                local->accessed = true;
                /* deal with different local cases */
                switch (local->tag) {
                    case icpl_literal:
                        if (!ic_backend_pancake_compile_push_constant(instructions, local)) {
                            puts("ic_backend_pancake_compile_fdecl_body: call to ic_backend_pancake_compile_push_constant failed");
                            return 0;
                        }
                        break;

                    case icpl_offset:
                        /* insert `copyarg argn` instruction */
                        instruction = ic_backend_pancake_instructions_add(instructions, icp_copyarg);
                        if (!instruction) {
                            puts("ic_backend_pancake_compile_fdecl_body: call to ic_backend_pancake_instructions_add failed");
                            return 0;
                        }
                        if (!ic_backend_pancake_bytecode_arg1_set_uint(instruction, local->u.offset)) {
                            puts("ic_backend_pancake_compile_fdecl_body: call to ic_backend_pancake_bytecode_arg1_set_uint failed for entry_jump");
                            return 0;
                        }

                        break;

                    default:
                        puts("ic_backend_pancake_compile_fdecl_body: impossible local->arg");
                        return 0;
                        break;
                }

                /* FIXME TODO ideally we would only save/restore/clean if we
                 * knew the stack clean had work to do
                 */

                /* insert save instruction */
                instruction = ic_backend_pancake_instructions_add(instructions, icp_save);
                if (!instruction) {
                    puts("ic_backend_pancake_compile_fdecl_body: call to ic_backend_pancake_instructions_add failed");
                    return 0;
                }

                /* insert clean_frame instruction */
                instruction = ic_backend_pancake_instructions_add(instructions, icp_clean_frame);
                if (!instruction) {
                    puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_instructions_add failed");
                    return 0;
                }

                /* insert restore instruction */
                instruction = ic_backend_pancake_instructions_add(instructions, icp_restore);
                if (!instruction) {
                    puts("ic_backend_pancake_compile_fdecl_body: call to ic_backend_pancake_instructions_add failed");
                    return 0;
                }

                /* insert return_value instruction */
                instruction = ic_backend_pancake_instructions_add(instructions, icp_return_value);
                if (!instruction) {
                    puts("ic_backend_pancake_compile_fdecl_body: call to ic_backend_pancake_instructions_add failed");
                    return 0;
                }

                break;

            case ic_transform_ir_stmt_type_assign:
                puts("ic_backend_pancake_compile_fdecl_body: assign unimplemented");
                return 0;
                break;

            case ic_transform_ir_stmt_type_if:
                puts("ic_backend_pancake_compile_fdecl_body: if unimplemented");
                return 0;
                break;

            default:
                puts("ic_backend_pancake_compile_fdecl_body: impossible case");
                return 0;
                break;
        }
    }

    return 1;
}

/* compile an expr (function call) into pancake bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_expr(struct ic_backend_pancake_instructions *instructions, struct ic_kludge *kludge, struct ic_dict *locals, struct ic_transform_ir_expr *texpr, unsigned int *is_void) {
    struct ic_transform_ir_fcall *tfcall = 0;
    if (!instructions) {
        puts("ic_backend_pancake_compile_expr: instructions was null");
        return 0;
    }

    if (!kludge) {
        puts("ic_backend_pancake_compile_expr: kludge was null");
        return 0;
    }

    if (!locals) {
        puts("ic_backend_pancake_compile_expr: locals was null");
        return 0;
    }

    if (!texpr) {
        puts("ic_backend_pancake_compile_expr: texpr was null");
        return 0;
    }

    if (!is_void) {
        puts("ic_backend_pancake_compile_expr: is_void was null");
        return 0;
    }

    tfcall = texpr->fcall;
    if (!ic_backend_pancake_compile_fcall(instructions, kludge, locals, tfcall, is_void)) {
        puts("ic_backend_pancake_compile_fdecl_body: let expr call to ic_backend_pancake_compile_fcall failed");
        return 0;
    }

    return 1;
}

/* add a push instruction for a constant
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_push_constant(struct ic_backend_pancake_instructions *instructions, struct ic_backend_pancake_local *local) {
    struct ic_expr_constant *literal = 0;
    struct ic_backend_pancake_bytecode *instruction = 0;
    char *str_value = 0;

    /* push appropriate literal
     * pushbool bool
     * pushuint uint FIXME TODO
     * pushint  int
     * pushstr  str
     * push     key::string FIXME TODO
     */
    if (!instructions) {
        puts("ic_backend_pancake_compile_push_constant: instructions was null");
        return 0;
    }

    if (!local) {
        puts("ic_backend_pancake_compile_push_constant: local was null");
        return 0;
    }

    literal = ic_backend_pancake_local_get_literal(local);
    if (!literal) {
        puts("ic_backend_pancake_compile_push_constant: call to ic_backend_pancake_local_get_literal failed");
        return 0;
    }

    switch (literal->tag) {
        case ic_expr_constant_type_integer:
            instruction = ic_backend_pancake_instructions_add(instructions, icp_pushint);
            if (!instruction) {
                puts("ic_backend_pancake_compile_push_constant: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
            if (!ic_backend_pancake_bytecode_arg1_set_sint(instruction, literal->u.integer)) {
                puts("ic_backend_pancake_compile_push_constant: call to ic_backend_pancake_bytecode_arg1_set_uint failed");
                return 0;
            }

            break;

        case ic_expr_constant_type_string:
            instruction = ic_backend_pancake_instructions_add(instructions, icp_pushstr);
            if (!instruction) {
                puts("ic_backend_pancake_compile_push_constant: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
            str_value = ic_string_contents(&(literal->u.string));
            if (!str_value) {
                puts("ic_backend_pancake_compile_push_constant: call to ic_string_contents failed");
                return 0;
            }
            if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, str_value)) {
                puts("ic_backend_pancake_compile_push_constant: call to ic_backend_pancake_bytecode_arg1_set_uint failed");
                return 0;
            }

            break;

        case ic_expr_constant_type_boolean:
            instruction = ic_backend_pancake_instructions_add(instructions, icp_pushbool);
            if (!instruction) {
                puts("ic_backend_pancake_compile_push_constant: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
            if (!ic_backend_pancake_bytecode_arg1_set_bool(instruction, literal->u.boolean)) {
                puts("ic_backend_pancake_compile_push_constant: call to ic_backend_pancake_bytecode_arg1_set_uint failed");
                return 0;
            }

            break;

        default:
            puts("ic_backend_pancake_compile_push_constant: unsupported literal->tag");
            return 0;
            break;
    }

    return 1;
}

/* compile an function call into pancake bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_fcall(struct ic_backend_pancake_instructions *instructions, struct ic_kludge *kludge, struct ic_dict *locals, struct ic_transform_ir_fcall *tfcall, unsigned int *is_void) {
    /* length of fcall args */
    unsigned int len = 0;
    /* current offset into fcall args */
    unsigned int i = 0;
    /* current fcall arg */
    struct ic_symbol *fcall_arg = 0;
    /* function to call */
    char *fdecl_sig_call = 0;
    /* instruction */
    struct ic_backend_pancake_bytecode *instruction = 0;

    struct ic_expr_func_call *fcall = 0;
    struct ic_decl_func *decl_func = 0;

    /* name of each arg */
    char *arg_name = 0;
    /* local for each arg */
    struct ic_backend_pancake_local *arg_local = 0;

    if (!instructions) {
        puts("ic_backend_pancake_compile_fcall: instructions was null");
        return 0;
    }

    if (!kludge) {
        puts("ic_backend_pancake_compile_fcall: kludge was null");
        return 0;
    }

    if (!locals) {
        puts("ic_backend_pancake_compile_fcall: locals was null");
        return 0;
    }

    if (!tfcall) {
        puts("ic_backend_pancake_compile_fcall: tfcall was null");
        return 0;
    }

    if (!is_void) {
        puts("ic_backend_pancake_compile_fcall: is_void was null");
        return 0;
    }

    /* for each arg, push onto stack */
    len = ic_transform_ir_fcall_length(tfcall);
    for (i = 0; i < len; ++i) {
        fcall_arg = ic_transform_ir_fcall_get_arg(tfcall, i);
        if (!fcall_arg) {
            puts("ic_backend_pancake_compile_fdecl_body: call to ic_transform_ir_fcall_get_arg failed");
            return 0;
        }
        arg_name = ic_symbol_contents(fcall_arg);
        if (!arg_name) {
            puts("ic_backend_pancake_compile_fdecl_body: call to ic_symbol_contents failed");
            return 0;
        }
        arg_local = ic_dict_get(locals, arg_name);
        if (!arg_local) {
            puts("ic_backend_pancake_compile_fdecl_body: call to ic_dict_get failed");
            return 0;
        }
        /* mark as accessed */
        arg_local->accessed = true;
        /* deal with different local cases */
        switch (arg_local->tag) {
            case icpl_literal:
                if (!ic_backend_pancake_compile_push_constant(instructions, arg_local)) {
                    puts("ic_backend_pancake_compile_fdecl_body: call to ic_backend_pancake_compile_push_constant failed");
                    return 0;
                }
                break;

            case icpl_offset:
                /* insert `copyarg argn` instruction */
                instruction = ic_backend_pancake_instructions_add(instructions, icp_copyarg);
                if (!instruction) {
                    puts("ic_backend_pancake_compile_fdecl_body: call to ic_backend_pancake_instructions_add failed");
                    return 0;
                }
                if (!ic_backend_pancake_bytecode_arg1_set_uint(instruction, arg_local->u.offset)) {
                    puts("ic_backend_pancake_compile_fdecl_body: call to ic_backend_pancake_bytecode_arg1_set_uint failed for entry_jump");
                    return 0;
                }

                break;

            default:
                puts("ic_backend_pancake_compile_fdecl_body: impossible arg_local->arg");
                return 0;
                break;
        }
    }

    /* insert call instruction to fcall */
    fcall = tfcall->fcall;
    decl_func = fcall->fdecl;
    fdecl_sig_call = ic_decl_func_sig_call(decl_func);
    if (!fdecl_sig_call) {
        puts("ic_backend_pancake_compile_fdecl_body: call to ic_decl_func_sig_call failed");
        return 0;
    }

    if (ic_decl_func_isbuiltin(fcall->fdecl)) {
        instruction = ic_backend_pancake_instructions_add(instructions, icp_call_builtin);
    } else {
        instruction = ic_backend_pancake_instructions_add(instructions, icp_call);
    }

    if (!instruction) {
        puts("ic_backend_pancake_compile_fdecl_body: call to ic_backend_pancake_instructions_add failed");
        return 0;
    }
    if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, fdecl_sig_call)) {
        puts("ic_backend_pancake_compile_fdecl_body: call to ic_backend_pancake_bytecode_arg1_set_uint failed for entry_jump");
        return 0;
    }
    /* set number of args we call with */
    if (!ic_backend_pancake_bytecode_arg2_set_uint(instruction, len)) {
        puts("ic_backend_pancake_compile_fdecl_body: call to ic_backend_pancake_bytecode_arg1_set_uint failed for entry_jump");
        return 0;
    }

    /* if void: compile-time error */
    if (ic_decl_func_is_void(decl_func)) {
        *is_void = 1;
    } else {
        *is_void = 0;
    }

    return 1;
}
