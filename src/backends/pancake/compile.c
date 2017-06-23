#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../analyse/data/kludge.h"
#include "../../data/labeller.h"
#include "../../data/scope.h"
#include "../../parse/data/expr.h"
#include "../../transform/data/tbody.h"
#include "data/bytecode.h"
#include "data/instructions.h"
#include "data/local.h"
#include "pancake.h"

/* TODO FIXME labeller isn't used in many functions
 * revisit why we pass this around and if it should be part of the
 * general interface
 */
#pragma GCC diagnostic ignored "-Wunused-parameter"

/* generate all needed functions
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_generate_functions(struct ic_backend_pancake_instructions *instructions, struct ic_kludge *kludge);

/* compile an fdecl into bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_fdecl(struct ic_backend_pancake_instructions *instructions, struct ic_kludge *kludge, struct ic_decl_func *fdecl);

/* compile a stmt into bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_stmt(struct ic_backend_pancake_instructions *instructions, struct ic_kludge *kludge, struct ic_scope *scope, struct ic_transform_ir_stmt *tstmt, struct ic_labeller *labeller);

/* compile an tbody into bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_body(struct ic_backend_pancake_instructions *instructions, struct ic_kludge *kludge, struct ic_transform_body *tbody, struct ic_scope *scope, struct ic_labeller *labeller);

/* compile an expr (function call) into pancake bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_expr(struct ic_backend_pancake_instructions *instructions, struct ic_kludge *kludge, struct ic_scope *scope, struct ic_transform_ir_expr *texpr, unsigned int *is_void, struct ic_labeller *labeller);

/* compile an function call into pancake bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_fcall(struct ic_backend_pancake_instructions *instructions, struct ic_kludge *kludge, struct ic_scope *scope, struct ic_transform_ir_fcall *tfcall, unsigned int *is_void, struct ic_labeller *labeller);

/* ensure a named local is top of the stack
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_local_push_from_string(struct ic_backend_pancake_instructions *instructions, struct ic_scope *scope, char *local_name);

/* ensure a named local is top of the stack
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_local_push_from_symbol(struct ic_backend_pancake_instructions *instructions, struct ic_scope *scope, struct ic_symbol *local_name);

/* ensure a local is top of the stack
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_local_push(struct ic_backend_pancake_instructions *instructions, struct ic_backend_pancake_local *local);

/* add a push instruction for a constant found in a local
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_push_constant_from_local(struct ic_backend_pancake_instructions *instructions, struct ic_backend_pancake_local *local);

/* add a push instruction for a constant
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_push_constant(struct ic_backend_pancake_instructions *instructions, struct ic_expr_constant *constant);

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
        puts("ic_backend_pancake_compile: call to ic_backend_pancake_bytecode_arg1_set_char failed for entry_jump");
        return 0;
    }
    if (!ic_backend_pancake_bytecode_arg2_set_uint(bc_entry_call, 0)) {
        puts("ic_backend_pancake_compile: call to ic_backend_pancake_bytecode_arg2_set_uint failed for entry_jump");
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

    /* FIXME TODO do we want to include type information within bytecode? */

    /* generate functions */
    if (!ic_backend_pancake_generate_functions(instructions, kludge)) {
        puts("ic_backend_pancake_compile: call to ic_backend_pancake_generate_functions failed");
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
    /* char * to sig_call for current fdecl
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
    struct ic_scope *scope = 0;
    /* pvector of keys to dict, as we cannot recover these from dict */
    struct ic_pvector *scope_keys = 0;

    /* current local */
    struct ic_backend_pancake_local *local = 0;

    /* labeller for this fdecl */
    struct ic_labeller *labeller = 0;

    /* local instruction */
    struct ic_backend_pancake_bytecode *instruction = 0;

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
    if (ic_decl_func_isbuiltin(fdecl)) {
        // printf("warning: ic_backend_pancake_compile_fdecl: skipping builtin fdecl '%s'\n", fdecl_sig_call);
        return 1;
    }

    labeller = ic_labeller_new(fdecl_sig_call);
    if (!labeller) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_labeller_new failed");
        return 0;
    }

    /* get length - which is offset of next instruction */
    fdecl_instructions_offset = ic_backend_pancake_instructions_length(instructions);

    /* register function at offset */
    if (!ic_backend_pancake_instructions_register_label(instructions, fdecl_sig_call, fdecl_instructions_offset)) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_instructions_register_label failed");
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
    scope = ic_scope_new(0);
    if (!scope) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_scope_new failed");
        return 0;
    }

    scope_keys = ic_pvector_new(0);
    if (!scope_keys) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_pvector_new failed");
        return 0;
    }

    /* register all args
     *
     * there are now 2 cases for args:
     *
     * 1) a read-only arg
     *   we register each read-only arg as an offset back into the value stack
     *   so for arguments (a,b,c)
     *    c = 0
     *    b = 1
     *    a = 2
     *
     * 2) an arg we write to must become a 'runtime' value
     *   similar to a local let
     *
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

        local_name_ch = ic_symbol_contents(&(arg->name));
        if (!local_name_ch) {
            puts("ic_backend_pancake_compile_fdecl: call to ic_symbol_contents failed");
            return 0;
        }

        if (-1 == ic_pvector_append(scope_keys, local_name_ch)) {
            puts("ic_backend_pancake_compile_fdecl: call to ic_pvector_append failed");
            return 0;
        }

        if (arg->assigned_to) {
            /* if an arg is asigned to, then we need to treat this as though
             * it were a local let
             * so first we copy the arg into place
             * store it
             * then set an appropriate runtime local
             */

            /* insert `copyarg argn` instruction */
            instruction = ic_backend_pancake_instructions_add(instructions, icp_copyarg);
            if (!instruction) {
                puts("ic_backend_pancake_compile_local_push: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
            if (!ic_backend_pancake_bytecode_arg1_set_uint(instruction, i)) {
                puts("ic_backend_pancake_compile_local_push: call to ic_backend_pancake_bytecode_arg1_set_uint failed for entry_jump");
                return 0;
            }

            /* insert `store name_ch` instruction */
            instruction = ic_backend_pancake_instructions_add(instructions, icp_store);
            if (!instruction) {
                puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
            if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, local_name_ch)) {
                puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_bytecode_arg1_set_uint failed for entry_jump");
                return 0;
            }

            local = ic_backend_pancake_local_new(&(arg->name), icpl_runtime);
            if (!local) {
                puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_local_new failed");
                return 0;
            }

        } else {
            local = ic_backend_pancake_local_new(&(arg->name), icpl_offset);
            if (!local) {
                puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_local_new failed");
                return 0;
            }

            if (!ic_backend_pancake_local_set_offset(local, i)) {
                puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_local_set_offset failed");
                return 0;
            }
        }
        if (!ic_scope_insert(scope, local_name_ch, local)) {
            puts("ic_backend_pancake_compile_fdecl: call to ic_scope_insert failed");
            return 0;
        }
    }

    fdecl_tbody = fdecl->tbody;
    if (!fdecl_tbody) {
        puts("ic_backend_pancake_compile_fdecl: no tbody found on fdecl");
        return 0;
    }

    if (!ic_backend_pancake_compile_body(instructions, kludge, fdecl_tbody, scope, labeller)) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_compile_body failed");
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
    len = ic_pvector_length(scope_keys);
    for (i = 0; i < len; ++i) {
        local_name_ch = ic_pvector_get(scope_keys, i);
        if (!local_name_ch) {
            puts("ic_backend_pancake_compile_fdecl: call to ic_pvector_get failed");
            return 0;
        }

        local = ic_scope_get(scope, local_name_ch);
        if (!local) {
            puts("ic_backend_pancake_compile_fdecl: call to ic_scope_get failed");
            return 0;
        }

        /* print warning if local variable was never accessed
         * unless that local's name is _
         *
         * TODO FIXME move this logic into analyse as well
         * both frontends should warn for all unused variables
         * but ignore all _ names
         * we should also prevent reads from _ names
         *
         * TODO FIXME for an _ variable we shouldn't even pass it
         * this is just wasting runtime time and space
         */
        if (!local->accessed && (0 != strcmp("_", local_name_ch))) {
            printf("Pancake: Warning: unused local variable '%s'\n", local_name_ch);
        }
    }

    /* fetch last instruction to check if we already returned */
    instruction = ic_backend_pancake_instructions_get_last(instructions);
    if (!instruction) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_instructions_get_last failed");
        return 0;
    }

    /* if last instruction was a return, then do nothing */
    if (instruction->tag != icp_return_value) {
        /* it wasn't a return, so insert return and cleanup instructions */

        /* insert a cleanup and return_void instruction */
        if (!ic_backend_pancake_instructions_add(instructions, icp_clean_stack)) {
            puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_instructions_add failed");
            return 0;
        }

        if (!ic_backend_pancake_instructions_add(instructions, icp_return_void)) {
            puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_instructions_add failed");
            return 0;
        }
    }

    /* destroy scope_keys pvector
     * free it
     * keys are all managed by arg->name symbo
     */
    if (!ic_pvector_destroy(scope_keys, 1, 0)) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_pvector_destroy failed");
        return 0;
    }

    /* destroy scope */
    if (!ic_scope_destroy(scope, 1)) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_scope_destroy failed");
        return 0;
    }

    /* destroy labeller */
    if (!ic_labeller_destroy(labeller, 1)) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_labeller_destroy failed");
        return 0;
    }

    return 1;
}
static unsigned int ic_backend_pancake_compile_stmt_match(struct ic_backend_pancake_instructions *instructions, struct ic_kludge *kludge, struct ic_scope *scope, struct ic_transform_ir_stmt *tstmt, struct ic_labeller *labeller) {
    /* vector of labels used for match cases */
    struct ic_pvector *labels = 0;
    char *label = 0;
    unsigned int label_offset = 0;

    unsigned int i_case = 0;
    unsigned int n_cases = 0;
    int pvector_ret = 0;

    struct ic_backend_pancake_bytecode *instruction = 0;

    struct ic_field *field = 0;
    unsigned int field_offset = 0;
    struct ic_decl_type *field_type = 0;

    struct ic_transform_ir_match *tmatch = 0;
    struct ic_decl_type *tdecl = 0;
    struct ic_transform_ir_match_case *tcase = 0;

    struct ic_scope *child_scope = 0;

    char *name_ch = 0;
    struct ic_backend_pancake_local *local = 0;

    if (!instructions) {
        puts("ic_backend_pancake_compile_stmt_match: instructions was null");
        return 0;
    }

    if (!kludge) {
        puts("ic_backend_pancake_compile_stmt_match: kludge was null");
        return 0;
    }

    if (!scope) {
        puts("ic_backend_pancake_compile_stmt_match: scope was null");
        return 0;
    }

    if (!tstmt) {
        puts("ic_backend_pancake_compile_stmt_match: tstmt was null");
        return 0;
    }

    if (!labeller) {
        puts("ic_backend_pancake_compile_stmt_match: labeller was null");
        return 0;
    }

    tmatch = ic_transform_ir_stmt_get_match(tstmt);
    if (!tmatch) {
        puts("ic_backend_pancake_compile_stmt_match: call to ic_transform_ir_stmt_get_match failed");
        return 0;
    }

    tdecl = tmatch->tdecl;

    /* union Foo
     *  a::Sint
     *  b::String
     * end
     *
     * fn main()
     *   let f = Foo(...)
     *   match f
     *     case a::Sint
     *       println(a)
     *     case b::String
     *       println(b)
     *     end
     *  end
     * end
     *
     * ->
     *
     * label main()
     * ...
     * <copy f onto stack>
     * load_offset_uint 0
     * pushuint 0
     * call_builtin equal(Uint,Uint) 2
     * jif_label main()0
     *
     * load_offset_uint 0
     * pushuint 1
     * call_builtin equal(Uint,Uint) 2
     * jif_label main()1
     *
     * panic "Impossible tag"
     *
     * label main()0
     * <copy f onto stack>
     * load_offset_sint 1
     * call_builtin print(Sint) 1
     * jmp_label main()2
     *
     * label main()1
     * <copy f onto stack>
     * load_offset_string 1
     * call_builtin print(String) 1
     * jmp_label main()2
     *
     * jmp_label main()2
     * label main()2
     * clean_stack
     * return_void
     *
     */

    labels = ic_pvector_new(0);
    if (!labels) {
        puts("ic_backend_pancake_compile_stmt_match: call to ic_pvector_new failed");
        return 0;
    }

    /* load struct */
    name_ch = ic_symbol_contents(tmatch->match_symbol);
    if (!name_ch) {
        puts("ic_backend_pancake_compile_stmt_match: let faccess call to ic_symbol_contents failed");
        return 0;
    }

    local = ic_scope_get(scope, name_ch);
    if (!local) {
        puts("ic_backend_pancake_compile_stmt_match: call to ic_scope_get failed");
        return 0;
    }

    /* mark as accessed */
    local->accessed = true;
    switch (local->tag) {
        case icpl_offset:
            /* insert `copyarg argn` instruction */
            instruction = ic_backend_pancake_instructions_add(instructions, icp_copyarg);
            if (!instruction) {
                puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
            if (!ic_backend_pancake_bytecode_arg1_set_uint(instruction, local->u.offset)) {
                puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_bytecode_arg1_set_uint failed for entry_jump");
                return 0;
            }

            break;

        case icpl_runtime:
            /* insert `load key` instruction */
            instruction = ic_backend_pancake_instructions_add(instructions, icp_load);
            if (!instruction) {
                puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
            if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, name_ch)) {
                puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_bytecode_arg1_set_char");
                return 0;
            }
            break;

        default:
            puts("ic_backend_pancake_compile_stmt_match: local was not runtime or offset, unsupported");
            return 0;
    }

    /* for each case
     * load_offset_uint 0
     * check if eq to i_arg
     * if so jump
     */

    n_cases = ic_pvector_length(&(tmatch->cases));

    if (n_cases == 0 && tmatch->else_body == 0) {
        puts("ic_backend_pancake_compile_stmt_match: zero-length matches not supported");
        return 0;
    }

    for (i_case = 0; i_case < n_cases; ++i_case) {
        tcase = ic_pvector_get(&(tmatch->cases), i_case);
        if (!tcase) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_pvector_get failed");
            return 0;
        }

        /* load_offset_uint 0 */
        instruction = ic_backend_pancake_instructions_add(instructions, icp_load_offset_uint);
        if (!instruction) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_instructions_add failed");
            return 0;
        }

        if (!ic_backend_pancake_bytecode_arg1_set_uint(instruction, 0)) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_bytecode_arg1_set_uint failed");
            return 0;
        }

        /* pushuint <field_offset> */
        instruction = ic_backend_pancake_instructions_add(instructions, icp_pushuint);
        if (!instruction) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_instructions_add failed");
            return 0;
        }

        field = tcase->field;
        field_offset = ic_decl_type_get_field_offset(tdecl, ic_symbol_contents(&(field->name)));

        if (!ic_backend_pancake_bytecode_arg1_set_uint(instruction, field_offset)) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_bytecode_arg1_set_uint failed");
            return 0;
        }

        /* call_builtin equal(Uint, Uint) 2 */
        instruction = ic_backend_pancake_instructions_add(instructions, icp_call_builtin);
        if (!instruction) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_instructions_add failed");
            return 0;
        }

        if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, "equal(Uint,Uint)")) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_bytecode_arg1_set_char failed");
            return 0;
        }

        if (!ic_backend_pancake_bytecode_arg2_set_uint(instruction, 2)) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_bytecode_arg2_set_uint failed");
            return 0;
        }

        /* generate our new label */
        label = ic_labeller_generate(labeller);
        if (!label) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_labeller_generate failed");
            return 0;
        }

        /* save our label for the next for loop */
        pvector_ret = ic_pvector_append(labels, label);
        if (pvector_ret != (int)i_case) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_pvector_append failed");
            printf("for case '%u' our call to ic_pvector_append returned '%d'\n", i_case, pvector_ret);
            return 0;
        }

        /* jif_label <fname><i_arg> */
        instruction = ic_backend_pancake_instructions_add(instructions, icp_jif_label);
        if (!instruction) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_instructions_add failed");
            return 0;
        }

        if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, label)) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_bytecode_arg1_set_char failed");
            return 0;
        }
    }

    /* final label */
    label = ic_labeller_generate(labeller);
    if (!label) {
        puts("ic_backend_pancake_compile_stmt_match: call to ic_labeller_generate failed");
        return 0;
    }

    pvector_ret = ic_pvector_append(labels, label);
    if (pvector_ret != (int)n_cases) {
        puts("ic_backend_pancake_compile_stmt_match: call to ic_pvector_append failed");
        printf("for n_cases '%u' our call to ic_pvector_append returned '%d'\n", n_cases, pvector_ret);
        return 0;
    }

    if (tmatch->else_body) {
        /* compile body */

        /* make new child scope for else_body */
        child_scope = ic_scope_new(scope);
        if (!child_scope) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_scope_new failed");
            return 0;
        }

        /* compile else_tbody */
        if (!ic_backend_pancake_compile_body(instructions, kludge, tmatch->else_body, child_scope, labeller)) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_compile_body failed");
            return 0;
        }

        /* tidy up child_scope */
        if (!ic_scope_destroy(child_scope, 1)) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_scope_destroy failed");
            return 0;
        }

        /* jump to final label */
        /* insert jmp to final place
         * jmp_label <fname><n_args>
         */
        instruction = ic_backend_pancake_instructions_add(instructions, icp_jmp_label);
        if (!instruction) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_instructions_add failed");
            return 0;
        }

        if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, label)) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_bytecode_arg1_set_char failed");
            return 0;
        }
    } else {
        /* insert panic in case we matched none of the above conditions */
        /* panic "impossible tag */
        instruction = ic_backend_pancake_instructions_add(instructions, icp_panic);
        if (!instruction) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_instructions_add failed");
            return 0;
        }

        if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, "impossible tag")) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_bytecode_arg1_set_char failed");
            return 0;
        }
    }

    /* for each case
     * create label
     * unpack appropriate type
     * generate call to appropriate print function
     * jump to final place
     */
    for (i_case = 0; i_case < n_cases; ++i_case) {
        tcase = ic_pvector_get(&(tmatch->cases), i_case);
        if (!tcase) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_pvector_get failed");
            return 0;
        }

        /* get our label from the pvector */
        label = ic_pvector_get(labels, i_case);
        if (!label) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_pvector_get failed");
            return 0;
        }

        /* register label */
        /* get length - which is offset of next instruction */
        label_offset = ic_backend_pancake_instructions_length(instructions);

        /* register function at offset */
        if (!ic_backend_pancake_instructions_register_label(instructions, label, label_offset)) {
            puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_instructions_register_label failed");
            return 0;
        }

        /* label print(<TYPENAME>)<i_arg> */
        instruction = ic_backend_pancake_instructions_add(instructions, icp_label);
        if (!instruction) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_instructions_add failed");
            return 0;
        }

        if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, label)) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_bytecode_arg1_set_char failed");
            return 0;
        }

        field = tcase->field;

        /* get field_type */
        field_type = ic_type_ref_get_type_decl(field->type);
        if (!field_type) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_type_ref_get_type_decl failed");
            return 0;
        }

        if (ic_decl_type_isbool(field_type)) {
            /* load_offset_bool <i_arg> */
            instruction = ic_backend_pancake_instructions_add(instructions, icp_load_offset_bool);
            if (!instruction) {
                puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
        } else if (ic_decl_type_isstring(field_type)) {
            /* load_offset_str <i_arg> */
            instruction = ic_backend_pancake_instructions_add(instructions, icp_load_offset_str);
            if (!instruction) {
                puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
        } else if (ic_decl_type_isuint(field_type)) {
            /* load_offset_uint <i_arg> */
            instruction = ic_backend_pancake_instructions_add(instructions, icp_load_offset_uint);
            if (!instruction) {
                puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
        } else if (ic_decl_type_issint(field_type)) {
            /* load_offset_sint <i_arg> */
            instruction = ic_backend_pancake_instructions_add(instructions, icp_load_offset_sint);
            if (!instruction) {
                puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
        } else {
            /* load_offset_ref <i_arg> */
            instruction = ic_backend_pancake_instructions_add(instructions, icp_load_offset_ref);
            if (!instruction) {
                puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
        }

        if (!ic_backend_pancake_bytecode_arg1_set_uint(instruction, 1)) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_bytecode_arg1_set_uint failed");
            return 0;
        }

        /* make new child scope for else_body */
        child_scope = ic_scope_new(scope);
        if (!child_scope) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_scope_new failed");
            return 0;
        }

        name_ch = ic_symbol_contents(&(field->name));
        if (!name_ch) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_symbol_contents failed");
            return 0;
        }

        /* case field is now on top of stack
         * store it
         */
        /* insert `store name_ch` instruction */
        instruction = ic_backend_pancake_instructions_add(instructions, icp_store);
        if (!instruction) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_instructions_add failed");
            return 0;
        }
        if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, name_ch)) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_bytecode_arg1_set_uint failed for entry_jump");
            return 0;
        }

        local = ic_backend_pancake_local_new(&(field->name), icpl_runtime);
        if (!local) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_local_new failed");
            return 0;
        }

        if (!ic_scope_insert(scope, name_ch, local)) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_scope_insert failed");
            return 0;
        }

        /* compile tcase->*/
        if (!ic_backend_pancake_compile_body(instructions, kludge, tcase->tbody, child_scope, labeller)) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_compile_body failed");
            return 0;
        }

        /* tidy up child_scope */
        if (!ic_scope_destroy(child_scope, 1)) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_scope_destroy failed");
            return 0;
        }

        label = ic_pvector_get(labels, n_cases);
        if (!label) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_pvector_get failed");
            return 0;
        }

        /* insert jmp to final place
         * jmp_label <fname><n_args>
         */
        instruction = ic_backend_pancake_instructions_add(instructions, icp_jmp_label);
        if (!instruction) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_instructions_add failed");
            return 0;
        }

        if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, label)) {
            puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_bytecode_arg1_set_char failed");
            return 0;
        }
    }

    label = ic_pvector_get(labels, n_cases);
    if (!label) {
        puts("ic_backend_pancake_compile_stmt_match: call to ic_pvector_get failed");
        return 0;
    }

    /* register label */
    /* get length - which is offset of next instruction */
    label_offset = ic_backend_pancake_instructions_length(instructions);

    /* register function at offset */
    if (!ic_backend_pancake_instructions_register_label(instructions, label, label_offset)) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_instructions_register_label failed");
        return 0;
    }

    /* insert label <fname><nargs> */
    instruction = ic_backend_pancake_instructions_add(instructions, icp_label);
    if (!instruction) {
        puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_instructions_add failed");
        return 0;
    }

    if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, label)) {
        puts("ic_backend_pancake_compile_stmt_match: call to ic_backend_pancake_bytecode_arg1_set_char failed");
        return 0;
    }

    /* TODO FIXME leaking labels
     * here we do NOT use ic_pvector_destroyer_free as we need the labels to
     * still be allocated when we output / interpret our instructions
     * this means we are leaking them
     * TODO FIXME we need a way of signalling that the instruction 'owns' this
     * data and should free it
     */
    if (!ic_pvector_destroy(labels, 1, 0)) {
        puts("ic_backend_pancake_compile_stmt_match: call to ic_pvector_destroy failed");
        return 0;
    }

    return 1;
}

/* compile an stmt into bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_stmt(struct ic_backend_pancake_instructions *instructions, struct ic_kludge *kludge, struct ic_scope *scope, struct ic_transform_ir_stmt *tstmt, struct ic_labeller *labeller) {
    /* let, used only if tstmt is let
     * used to decompose let
     */
    struct ic_transform_ir_let *tlet = 0;
    struct ic_transform_ir_expr *texpr = 0;
    struct ic_transform_ir_let_literal *tlet_lit = 0;
    struct ic_transform_ir_let_expr *tlet_expr = 0;
    struct ic_transform_ir_let_faccess *tlet_faccess = 0;
    struct ic_transform_ir_begin *begin = 0;
    struct ic_transform_ir_if *tif = 0;
    struct ic_transform_ir_assign *tassign = 0;

    /* name of let */
    char *name_ch = 0;

    /* current local */
    struct ic_backend_pancake_local *local = 0;

    /* instruction we generate */
    struct ic_backend_pancake_bytecode *instruction = 0;

    /* out-of-band return value from ic_backend_pancake_compile_fcall */
    unsigned int fcall_is_void = 0;

    /* labels used for if/else/end branching */
    char *else_start = 0;
    char *if_end = 0;

    /* child scope for if/else branches */
    struct ic_scope *child_scope = 0;

    /* current offset */
    unsigned int offset = 0;

    /* offset of field into type */
    struct ic_decl_type *faccess_tdecl = 0;
    unsigned int field_offset = 0;
    struct ic_backend_pancake_local *field_local = 0;

    if (!instructions) {
        puts("ic_backend_pancake_compile_stmt: instructions was null");
        return 0;
    }

    if (!kludge) {
        puts("ic_backend_pancake_compile_stmt: kludge was null");
        return 0;
    }

    if (!scope) {
        puts("ic_backend_pancake_compile_stmt: scope was null");
        return 0;
    }

    if (!tstmt) {
        puts("ic_backend_pancake_compile_stmt: tstmt was null");
        return 0;
    }

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

    switch (tstmt->tag) {
        case ic_transform_ir_stmt_type_expr:
            texpr = &(tstmt->u.expr);
            if (!ic_backend_pancake_compile_expr(instructions, kludge, scope, texpr, &fcall_is_void, labeller)) {
                puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_compile_expr failed");
                return 0;
            }

            if (!fcall_is_void) {
                puts("ic_backend_pancake_compile_stmt: function used in void context but was not void");
                printf("non-void function called in void context\n");
                return 0;
            }

            break;

        case ic_transform_ir_stmt_type_let:
            tlet = ic_transform_ir_stmt_get_let(tstmt);
            if (!tlet) {
                puts("ic_backend_pancake_compile_stmt: call to ic_transform_ir_stmt_get_let failed");
                return 0;
            }

            switch (tlet->tag) {
                case ic_transform_ir_let_type_literal:
                    /* FIXME TODO consider adding some error handling */
                    tlet_lit = &(tlet->u.lit);
                    name_ch = ic_symbol_contents(tlet_lit->name);
                    if (!name_ch) {
                        puts("ic_backend_pancake_compile_stmt: call to ic_symbol_contents failed");
                        return 0;
                    }

                    if (tlet->assigned_to) {
                        /* if this tlet is assigned_to, then cannot use literal trick below */

                        /* compile literal */
                        if (!ic_backend_pancake_compile_push_constant(instructions, tlet_lit->literal)) {
                            puts("ic_backend_pancake_compile_stmt: let expr call to ic_backend_pancake_compile_expr failed");
                            return 0;
                        }

                        /* insert `store name_ch` instruction */
                        instruction = ic_backend_pancake_instructions_add(instructions, icp_store);
                        if (!instruction) {
                            puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_instructions_add failed");
                            return 0;
                        }
                        if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, name_ch)) {
                            puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_bytecode_arg1_set_uint failed for entry_jump");
                            return 0;
                        }

                        /* create local */
                        local = ic_backend_pancake_local_new(tlet_lit->name, icpl_runtime);
                        if (!local) {
                            puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_local failed");
                            return 0;
                        }
                        if (!ic_scope_insert(scope, name_ch, local)) {
                            puts("ic_backend_pancake_compile_stmt: call to ic_scope_insert failed");
                            return 0;
                        }
                    } else {
                        local = ic_backend_pancake_local_new(tlet_lit->name, icpl_literal);
                        if (!local) {
                            puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_local failed");
                            return 0;
                        }
                        if (!ic_backend_pancake_local_set_literal(local, tlet_lit->literal)) {
                            puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_set_literal failed");
                            return 0;
                        }
                        if (!name_ch) {
                            puts("ic_backend_pancake_compile_stmt: call to ic_symbol_contents failed");
                            return 0;
                        }
                        if (!ic_scope_insert(scope, name_ch, local)) {
                            puts("ic_backend_pancake_compile_stmt: call to ic_scope_insert failed");
                            return 0;
                        }
                    }
                    break;

                case ic_transform_ir_let_type_expr:
                    /*
                     *  let name::type = fcall(args...)
                     *    push all args onto stack (using dict)
                     *    call fcall
                     *    register return value to name (along with access count)
                     *    if void:
                     *      compile-time error
                     */

                    /* FIXME TODO consider adding some error handling */
                    /* compile fcall */
                    tlet_expr = &(tlet->u.expr);
                    texpr = tlet_expr->expr;
                    if (!ic_backend_pancake_compile_expr(instructions, kludge, scope, texpr, &fcall_is_void, labeller)) {
                        puts("ic_backend_pancake_compile_stmt: let expr call to ic_backend_pancake_compile_expr failed");
                        return 0;
                    }

                    /* insert icp_store instruction under name of let */
                    name_ch = ic_symbol_contents(tlet_expr->name);
                    if (!name_ch) {
                        puts("ic_backend_pancake_compile_stmt: let expr call to ic_symbol_contents failed");
                        return 0;
                    }

                    if (fcall_is_void) {
                        puts("ic_backend_pancake_compile_stmt: function used in let was void");
                        printf("function called and assigned to let '%s', but function is void\n", name_ch);
                        return 0;
                    }

                    /* insert `store name_ch` instruction */
                    instruction = ic_backend_pancake_instructions_add(instructions, icp_store);
                    if (!instruction) {
                        puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_instructions_add failed");
                        return 0;
                    }
                    if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, name_ch)) {
                        puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_bytecode_arg1_set_uint failed for entry_jump");
                        return 0;
                    }

                    /* create local */
                    local = ic_backend_pancake_local_new(tlet_expr->name, icpl_runtime);
                    if (!local) {
                        puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_local failed");
                        return 0;
                    }
                    if (!ic_scope_insert(scope, name_ch, local)) {
                        puts("ic_backend_pancake_compile_stmt: call to ic_scope_insert failed");
                        return 0;
                    }

                    break;

                case ic_transform_ir_let_type_faccess:
                    /* let name::type = f.i */
                    tlet_faccess = &(tlet->u.faccess);

                    /* load struct */
                    name_ch = ic_symbol_contents(tlet_faccess->left);
                    if (!name_ch) {
                        puts("ic_backend_pancake_compile_stmt: let faccess call to ic_symbol_contents failed");
                        return 0;
                    }
                    field_local = ic_scope_get(scope, name_ch);
                    if (!field_local) {
                        puts("ic_backend_pancake_compile_stmt: call to ic_scope_get failed");
                        return 0;
                    }

                    /* mark as accessed */
                    field_local->accessed = true;
                    switch (field_local->tag) {
                        case icpl_offset:
                            /* insert `copyarg argn` instruction */
                            instruction = ic_backend_pancake_instructions_add(instructions, icp_copyarg);
                            if (!instruction) {
                                puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_instructions_add failed");
                                return 0;
                            }
                            if (!ic_backend_pancake_bytecode_arg1_set_uint(instruction, field_local->u.offset)) {
                                puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_bytecode_arg1_set_uint failed for entry_jump");
                                return 0;
                            }

                            break;

                        case icpl_runtime:
                            /* insert `load key` instruction */
                            instruction = ic_backend_pancake_instructions_add(instructions, icp_load);
                            if (!instruction) {
                                puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_instructions_add failed");
                                return 0;
                            }
                            if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, name_ch)) {
                                puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_bytecode_arg1_set_char");
                                return 0;
                            }
                            break;

                        default:
                            puts("ic_backend_pancake_compile_stmt: field_local was not runtime or offset, unsupported");
                            return 0;
                    }

                    /* set faccess_tdecl */
                    faccess_tdecl = tlet_faccess->left_type;
                    if (!faccess_tdecl) {
                        puts("ic_backend_pancake_compile_stmt: faccess->left_type was null");
                        return 0;
                    }

                    /* load_offset */
                    /* load struct */
                    name_ch = ic_symbol_contents(tlet_faccess->right);
                    if (!name_ch) {
                        puts("ic_backend_pancake_compile_stmt: let faccess call to ic_symbol_contents failed");
                        return 0;
                    }

                    /* need to look up field (right) on type (found via left)
                     * need to then find offset of field into type
                     */

                    field_offset = ic_decl_type_get_field_offset(faccess_tdecl, name_ch);

                    /* insert `load_offset offset` instruction */
                    if (ic_decl_type_isvoid(tlet_faccess->type)) {
                        /* error! */
                        puts("ic_backend_pancake_compile_stmt: void type as result of faccess");
                        return 0;
                    } else if (ic_decl_type_isbool(tlet_faccess->type)) {
                        instruction = ic_backend_pancake_instructions_add(instructions, icp_load_offset_bool);
                    } else if (ic_decl_type_isstring(tlet_faccess->type)) {
                        instruction = ic_backend_pancake_instructions_add(instructions, icp_load_offset_str);
                    } else if (ic_decl_type_isuint(tlet_faccess->type)) {
                        instruction = ic_backend_pancake_instructions_add(instructions, icp_load_offset_uint);
                    } else if (ic_decl_type_issint(tlet_faccess->type)) {
                        instruction = ic_backend_pancake_instructions_add(instructions, icp_load_offset_sint);
                    } else {
                        instruction = ic_backend_pancake_instructions_add(instructions, icp_load_offset_ref);
                    }

                    if (!instruction) {
                        puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_instructions_add failed");
                        return 0;
                    }
                    if (!ic_backend_pancake_bytecode_arg1_set_uint(instruction, field_offset)) {
                        puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_bytecode_arg1_set_uint failed");
                        return 0;
                    }

                    /* insert icp_store instruction under name of let */
                    name_ch = ic_symbol_contents(tlet_faccess->name);
                    if (!name_ch) {
                        puts("ic_backend_pancake_compile_stmt: let faccess call to ic_symbol_contents failed");
                        return 0;
                    }

                    /* insert `store name_ch` instruction */
                    instruction = ic_backend_pancake_instructions_add(instructions, icp_store);
                    if (!instruction) {
                        puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_instructions_add failed");
                        return 0;
                    }
                    if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, name_ch)) {
                        puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_bytecode_arg1_set_uint failed for entry_jump");
                        return 0;
                    }

                    /* create local */
                    local = ic_backend_pancake_local_new(tlet_faccess->name, icpl_runtime);
                    if (!local) {
                        puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_local failed");
                        return 0;
                    }
                    if (!ic_scope_insert(scope, name_ch, local)) {
                        puts("ic_backend_pancake_compile_stmt: call to ic_scope_insert failed");
                        return 0;
                    }

                    break;

                default:
                    puts("ic_backend_pancake_compile_stmt: let impossible case");
                    return 0;
                    break;
            }

            break;

        case ic_transform_ir_stmt_type_ret:
            /* deal with different local cases */
            if (!ic_backend_pancake_compile_local_push_from_symbol(instructions, scope, tstmt->u.ret.var)) {
                puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_compile_local_push_from_symbol failed");
                return 0;
            }

            /* FIXME TODO ideally we would only save/restore/clean if we
             * knew the stack clean had work to do
             */

            /* insert save instruction */
            instruction = ic_backend_pancake_instructions_add(instructions, icp_save);
            if (!instruction) {
                puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }

            /* insert clean_stack instruction */
            instruction = ic_backend_pancake_instructions_add(instructions, icp_clean_stack);
            if (!instruction) {
                puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }

            /* insert restore instruction */
            instruction = ic_backend_pancake_instructions_add(instructions, icp_restore);
            if (!instruction) {
                puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }

            /* insert return_value instruction */
            instruction = ic_backend_pancake_instructions_add(instructions, icp_return_value);
            if (!instruction) {
                puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }

            break;

        case ic_transform_ir_stmt_type_assign:
            /* currently tir only supports
             *  foo = fcal(...)
             *
             * which means this code is equiv to a let call of the form
             *  let foo = fcall(...)
             *
             * bar the fact we are changing an existing variable vs creating
             * a new one
             *
             * FIXME TODO refactor to share logic
             */

            tassign = ic_transform_ir_stmt_get_assign(tstmt);
            if (!tassign) {
                puts("ic_backend_pancake_compile_stmt: call to ic_transform_ir_stmt_get_assign failed");
                return 0;
            }

            name_ch = ic_symbol_contents(tassign->left);
            if (!name_ch) {
                puts("ic_backend_pancake_compile_stmt: assign expr call to ic_symbol_contents failed");
                return 0;
            }

            /* FIXME TODO consider adding some error handling */
            /* compile fcall */
            if (!ic_backend_pancake_compile_expr(instructions, kludge, scope, tassign->right, &fcall_is_void, labeller)) {
                puts("ic_backend_pancake_compile_stmt: assign expr call to ic_backend_pancake_compile_expr failed");
                return 0;
            }

            if (fcall_is_void) {
                puts("ic_backend_pancake_compile_stmt: function used in assingment was void");
                printf("function called and assigned to local '%s', but function is void\n", name_ch);
                return 0;
            }

            /* insert `store name_ch` instruction */
            instruction = ic_backend_pancake_instructions_add(instructions, icp_store);
            if (!instruction) {
                puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
            if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, name_ch)) {
                puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_bytecode_arg1_set_uint failed for entry_jump");
                return 0;
            }

            /* FIXME TODO
             * let a = 4
             * a = fcall(...)
             *
             * the let a = 4 will make a local constant
             * but the assignment needs a local runtime
             *
             * FIXME TODO
             * we need transform to mark the `let a = 4` as *needing* a runtime
             * due to the later assignment
             */

            break;

        case ic_transform_ir_stmt_type_begin:
            begin = &(tstmt->u.begin);

            /* make new child scope for body */
            child_scope = ic_scope_new(scope);
            if (!child_scope) {
                puts("ic_backend_pancake_compile_stmt: call to ic_scope_new failed");
                return 0;
            }

            /* compile tbody */
            if (!ic_backend_pancake_compile_body(instructions, kludge, begin->tbody, child_scope, labeller)) {
                puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_compile_body failed");
                return 0;
            }

            /* tidy up child_scope */
            if (!ic_scope_destroy(child_scope, 1)) {
                puts("ic_backend_pancake_compile_stmt: call to ic_scope_destroy failed");
                return 0;
            }

            break;

        case ic_transform_ir_stmt_type_if:
            /* FIXME TODO
             * if should prefer relative jmps rather than j_*label
             * this is more work at compile time, but means less at runtime
             */

            /* if cond
             *  body1
             * else
             *  body 2
             * end
             * ...
             *
             * compiles to
             *
             * cond
             * jnif ELSE_BRANCH_LABEL
             * body1
             * jmp END_LABEL
             * ELSE_BRANCH_LABEL
             * body2
             * END_LABEL
             * ...
             */

            /* NB: we must ensure labels are globally unique
             * we can use the func call signature, e.g. foo(Sint,Sint)
             * and then append _N where N is a number unique within this
             * fdecl
             *
             * so we need something similar to the tcounter system
             *
             */

            /* fn foo(a Sint) -> Sint
             *  if a < 5
             *    body1...
             *  else
             *    body2...
             *  end
             * end
             */

            /* fn foo(a Sint) -> Sint
             *  let cond::Bool = a < 5
             *  if cond
             *    body1...
             *  else
             *    body2...
             *  end
             * end
             */

            /* label foo(Sint)
             * copyarg a
             * pushint 5
             * call_builtin lessthan(Sint, Sint) 2
             * jnif foo(Sint)_1
             * body1...
             * jmp foo(Sint)_2
             * label foo(Sint)_1
             * body2...
             * label foo(Sint)_2
             */

            /* generate label for else branch */
            /* generate label for end of if/else */
            /* insert condition */
            /* insert jnif to else branch */
            /* compile if branch body */
            /* insert jmp to end */
            /* insert label for else branch */
            /* compile else branch body */
            /* insert label for end of if/else */
            tif = &(tstmt->u.sif);

            if (tif->else_tbody) {
                /* we have an else statement, generate an else label */
                else_start = ic_labeller_generate(labeller);
                if (!else_start) {
                    puts("ic_backend_pancake_compile_stmt: call to ic_labeller_generate failed");
                    return 0;
                }
            }

            /* always generate an end label */
            if_end = ic_labeller_generate(labeller);
            if (!if_end) {
                puts("ic_backend_pancake_compile_stmt: call to ic_labeller_generate failed");
                return 0;
            }

            /* we are guaranteed by transform that the if is now just a label
             * to a let
             */

            /* insert load for variable... */
            if (!ic_backend_pancake_compile_local_push_from_symbol(instructions, scope, tif->cond)) {
                puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_compile_local_push_from_symbol failed");
                return 0;
            }

            /* if else_start then insert jnif_label to else_start
             * otherwise insert jnif_label to if_end
             */
            instruction = ic_backend_pancake_instructions_add(instructions, icp_jnif_label);
            if (!instruction) {
                puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }

            if (else_start) {
                if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, else_start)) {
                    puts("ic_backend_pancake_compile_stmt: call to ic_backend_bytecode_arg1_set_char failed");
                    return 0;
                }
            } else {
                if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, if_end)) {
                    puts("ic_backend_pancake_compile_stmt: call to ic_backend_bytecode_arg1_set_char failed");
                    return 0;
                }
            }

            /* make new child scope for then_body */
            child_scope = ic_scope_new(scope);
            if (!child_scope) {
                puts("ic_backend_pancake_compile_stmt: call to ic_scope_new failed");
                return 0;
            }

            /* compile then_tbody */
            if (!ic_backend_pancake_compile_body(instructions, kludge, tif->then_tbody, child_scope, labeller)) {
                puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_compile_body failed");
                return 0;
            }

            /* tidy up child_scope */
            if (!ic_scope_destroy(child_scope, 1)) {
                puts("ic_backend_pancake_compile_stmt: call to ic_scope_destroy failed");
                return 0;
            }

            if (else_start) {
                /* if else_start then insert jmp_label to if_end
                 * otherwise do nothing
                 */
                instruction = ic_backend_pancake_instructions_add(instructions, icp_jmp_label);
                if (!instruction) {
                    puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_instructions_add failed");
                    return 0;
                }
                if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, if_end)) {
                    puts("ic_backend_pancake_compile_stmt: call to ic_backend_bytecode_arg1_set_char failed");
                    return 0;
                }

                /* if else_start then insert else_start label
                 * otherwise do nothing
                 */
                instruction = ic_backend_pancake_instructions_add(instructions, icp_label);
                if (!instruction) {
                    puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_instructions_add failed");
                    return 0;
                }
                if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, else_start)) {
                    puts("ic_backend_pancake_compile_stmt: call to ic_backend_bytecode_arg1_set_char failed");
                    return 0;
                }

                /* get length - which is offset of next instruction */
                offset = ic_backend_pancake_instructions_length(instructions);
                /* register label at offset */
                if (!ic_backend_pancake_instructions_register_label(instructions, else_start, offset)) {
                    puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_instructions_register_label failed");
                    return 0;
                }

                /* make new child scope for then_body */
                child_scope = ic_scope_new(scope);
                if (!child_scope) {
                    puts("ic_backend_pancake_compile_stmt: call to ic_scope_new failed");
                    return 0;
                }

                /* if else_start then compile else_body
                 * otherwise do nothing
                 */
                if (!ic_backend_pancake_compile_body(instructions, kludge, tif->else_tbody, child_scope, labeller)) {
                    puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_compile_body failed");
                    return 0;
                }

                /* tidy up child_scope */
                if (!ic_scope_destroy(child_scope, 1)) {
                    puts("ic_backend_pancake_compile_stmt: call to ic_scope_destroy failed");
                    return 0;
                }
            }

            /* insert end label */
            instruction = ic_backend_pancake_instructions_add(instructions, icp_label);
            if (!instruction) {
                puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
            if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, if_end)) {
                puts("ic_backend_pancake_compile_stmt: call to ic_backend_bytecode_arg1_set_char failed");
                return 0;
            }
            /* get length - which is offset of next instruction */
            offset = ic_backend_pancake_instructions_length(instructions);
            /* register label at offset */
            if (!ic_backend_pancake_instructions_register_label(instructions, if_end, offset)) {
                puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_instructions_register_label failed");
                return 0;
            }

            break;

        case ic_transform_ir_stmt_type_match:
            if (!ic_backend_pancake_compile_stmt_match(instructions, kludge, scope, tstmt, labeller)) {
                puts("ic_backend_pancake_compile_stmt: call to ic_backend_pancake_compile_stmt_match failed");
                return 0;
            }
            break;

        default:
            puts("ic_backend_pancake_compile_stmt: impossible case");
            return 0;
            break;
    }

    return 1;
}

/* compile an tbody into bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_body(struct ic_backend_pancake_instructions *instructions, struct ic_kludge *kludge, struct ic_transform_body *tbody, struct ic_scope *scope, struct ic_labeller *labeller) {
    /* current offset into body */
    unsigned int i = 0;
    /* len of body */
    unsigned int len = 0;
    /* current stmt in body */
    struct ic_transform_ir_stmt *tstmt = 0;

    if (!instructions) {
        puts("ic_backend_pancake_compile_body: instructions was null");
        return 0;
    }

    if (!kludge) {
        puts("ic_backend_pancake_compile_body: kludge was null");
        return 0;
    }

    if (!tbody) {
        puts("ic_backend_pancake_compile_body: tbody was null");
        return 0;
    }

    if (!scope) {
        puts("ic_backend_pancake_compile_body: scope was null");
        return 0;
    }

    len = ic_transform_body_length(tbody);

    for (i = 0; i < len; ++i) {
        tstmt = ic_transform_body_get(tbody, i);
        if (!tstmt) {
            puts("ic_backend_pancake_compile_body: call to ic_transform_body_get failed");
            return 0;
        }

        if (!ic_backend_pancake_compile_stmt(instructions, kludge, scope, tstmt, labeller)) {
            puts("ic_backend_pancake_compile_body: call to ic_backend_compile_stmt failed");
            return 0;
        }
    }

    return 1;
}

/* compile an expr (function call) into pancake bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_expr(struct ic_backend_pancake_instructions *instructions, struct ic_kludge *kludge, struct ic_scope *scope, struct ic_transform_ir_expr *texpr, unsigned int *is_void, struct ic_labeller *labeller) {
    struct ic_transform_ir_fcall *tfcall = 0;
    if (!instructions) {
        puts("ic_backend_pancake_compile_expr: instructions was null");
        return 0;
    }

    if (!kludge) {
        puts("ic_backend_pancake_compile_expr: kludge was null");
        return 0;
    }

    if (!scope) {
        puts("ic_backend_pancake_compile_expr: scope was null");
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

    if (!tfcall) {
        puts("ic_backend_pancake_compile_expr: tfcall was null");
        return 0;
    }

    if (!ic_backend_pancake_compile_fcall(instructions, kludge, scope, tfcall, is_void, labeller)) {
        puts("ic_backend_pancake_compile_body: let expr call to ic_backend_pancake_compile_fcall failed");
        return 0;
    }

    return 1;
}

/* ensure a named local is top of the stack
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_local_push_from_string(struct ic_backend_pancake_instructions *instructions, struct ic_scope *scope, char *local_name) {
    struct ic_backend_pancake_local *local = 0;

    if (!instructions) {
        puts("ic_backend_pancake_compile_local_push_from_string: instructions was null");
        return 0;
    }

    if (!scope) {
        puts("ic_backend_pancake_compile_local_push_from_string: scope was null");
        return 0;
    }

    if (!local_name) {
        puts("ic_backend_pancake_compile_local_push_from_string: local_name was null");
        return 0;
    }

    local = ic_scope_get(scope, local_name);
    if (!local) {
        puts("ic_backend_pancake_compile_local_push_from_string: call to ic_scope_get failed");
        return 0;
    }

    if (!ic_backend_pancake_compile_local_push(instructions, local)) {
        puts("ic_backend_pancake_compile_local_push_from_string: call to ic_backend_pancake_compile_local_push failed");
        return 0;
    }

    return 1;
}

/* ensure a named local is top of the stack
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_local_push_from_symbol(struct ic_backend_pancake_instructions *instructions, struct ic_scope *scope, struct ic_symbol *local_name) {
    char *ch = 0;

    if (!instructions) {
        puts("ic_backend_pancake_compile_local_push_from_symbol: instructions was null");
        return 0;
    }

    if (!scope) {
        puts("ic_backend_pancake_compile_local_push_from_symbol: scope was null");
        return 0;
    }

    if (!local_name) {
        puts("ic_backend_pancake_compile_local_push_from_symbol: local_name was null");
        return 0;
    }

    ch = ic_symbol_contents(local_name);
    if (!ch) {
        puts("ic_backend_pancake_compile_local_push_from_symbol: call to ic_symbol_contents failed");
        return 0;
    }

    if (!ic_backend_pancake_compile_local_push_from_string(instructions, scope, ch)) {
        puts("ic_backend_pancake_compile_local_push_from_symbol: call to ic_backend_pancake_compile_local_push_from_string failed");
        return 0;
    }

    return 1;
}

/* ensure a local is top of the stack
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_local_push(struct ic_backend_pancake_instructions *instructions, struct ic_backend_pancake_local *local) {
    /* instruction we generate */
    struct ic_backend_pancake_bytecode *instruction = 0;
    char *name = 0;

    if (!instructions) {
        puts("ic_backend_pancake_compile_local_push: instructions was null");
        return 0;
    }

    if (!local) {
        puts("ic_backend_pancake_compile_local_push: local was null");
        return 0;
    }

    switch (local->tag) {
        case icpl_literal:
            if (!ic_backend_pancake_compile_push_constant_from_local(instructions, local)) {
                puts("ic_backend_pancake_compile_local_push: call to ic_backend_pancake_compile_push_constant failed");
                return 0;
            }
            break;

        case icpl_offset:
            /* insert `copyarg argn` instruction */
            instruction = ic_backend_pancake_instructions_add(instructions, icp_copyarg);
            if (!instruction) {
                puts("ic_backend_pancake_compile_local_push: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
            if (!ic_backend_pancake_bytecode_arg1_set_uint(instruction, local->u.offset)) {
                puts("ic_backend_pancake_compile_local_push: call to ic_backend_pancake_bytecode_arg1_set_uint failed for entry_jump");
                return 0;
            }

            break;

        case icpl_runtime:
            if (!local->name) {
                puts("ic_backend_pancake_compile_local_push: local->name was null");
                return 0;
            }
            name = ic_symbol_contents(local->name);
            if (!name) {
                puts("ic_backend_pancake_compile_local_push: call to ic_symbol_contents failed");
                return 0;
            }

            /* insert `load key` instruction */
            instruction = ic_backend_pancake_instructions_add(instructions, icp_load);
            if (!instruction) {
                puts("ic_backend_pancake_compile_local_push: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
            if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, name)) {
                puts("ic_backend_pancake_compile_local_push: call to ic_backend_pancake_bytecode_arg1_set_char");
                return 0;
            }

            break;

        default:
            puts("ic_backend_pancake_compile_local_push: impossible local->arg");
            return 0;
            break;
    }

    /* mark local as accessed */
    local->accessed = true;

    return 1;
}

/* add a push instruction for a constant
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_push_constant(struct ic_backend_pancake_instructions *instructions, struct ic_expr_constant *constant) {
    char *str_value = 0;

    struct ic_backend_pancake_bytecode *instruction = 0;

    if (!instructions) {
        puts("ic_backend_pancake_compile_push_constant: instructions was null");
        return 0;
    }

    if (!constant) {
        puts("ic_backend_pancake_compile_push_constant: constant was null");
        return 0;
    }

    /* push appropriate literal
     * pushbool bool
     * pushuint uint
     * pushint  int
     * pushstr  str
     */

    switch (constant->tag) {
        case ic_expr_constant_type_unsigned_integer:
            instruction = ic_backend_pancake_instructions_add(instructions, icp_pushuint);
            if (!instruction) {
                puts("ic_backend_pancake_compile_push_constant: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
            if (!ic_backend_pancake_bytecode_arg1_set_uint(instruction, constant->u.unsigned_integer)) {
                puts("ic_backend_pancake_compile_push_constant: call to ic_backend_pancake_bytecode_arg1_set_uint failed");
                return 0;
            }

            break;

        case ic_expr_constant_type_signed_integer:
            instruction = ic_backend_pancake_instructions_add(instructions, icp_pushint);
            if (!instruction) {
                puts("ic_backend_pancake_compile_push_constant: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
            if (!ic_backend_pancake_bytecode_arg1_set_sint(instruction, constant->u.signed_integer)) {
                puts("ic_backend_pancake_compile_push_constant: call to ic_backend_pancake_bytecode_arg1_set_sint failed");
                return 0;
            }

            break;

        case ic_expr_constant_type_string:
            instruction = ic_backend_pancake_instructions_add(instructions, icp_pushstr);
            if (!instruction) {
                puts("ic_backend_pancake_compile_push_constant: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
            str_value = ic_string_contents(&(constant->u.string));
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
            if (!ic_backend_pancake_bytecode_arg1_set_bool(instruction, constant->u.boolean)) {
                puts("ic_backend_pancake_compile_push_constant: call to ic_backend_pancake_bytecode_arg1_set_uint failed");
                return 0;
            }

            break;

        default:
            puts("ic_backend_pancake_compile_push_constant: unsupported constant->tag");
            return 0;
            break;
    }

    return 1;
}

/* add a push instruction for a constant
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_push_constant_from_local(struct ic_backend_pancake_instructions *instructions, struct ic_backend_pancake_local *local) {
    struct ic_expr_constant *literal = 0;

    if (!instructions) {
        puts("ic_backend_pancake_compile_push_constant_from_local: instructions was null");
        return 0;
    }

    if (!local) {
        puts("ic_backend_pancake_compile_push_constant_from_local: local was null");
        return 0;
    }

    literal = ic_backend_pancake_local_get_literal(local);
    if (!literal) {
        puts("ic_backend_pancake_compile_push_constant_from_local: call to ic_backend_pancake_local_get_literal failed");
        return 0;
    }

    if (!ic_backend_pancake_compile_push_constant(instructions, literal)) {
        puts("ic_backend_pancake_compile_push_constant_from_local: call to ic_backend_pancake_compile_push_constant failed");
        return 0;
    }
    return 1;
}

/* compile an function call into pancake bytecode
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_compile_fcall(struct ic_backend_pancake_instructions *instructions, struct ic_kludge *kludge, struct ic_scope *scope, struct ic_transform_ir_fcall *tfcall, unsigned int *is_void, struct ic_labeller *labeller) {
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

    if (!scope) {
        puts("ic_backend_pancake_compile_fcall: scope was null");
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
            puts("ic_backend_pancake_compile_fcall: call to ic_transform_ir_fcall_get_arg failed");
            return 0;
        }
        arg_name = ic_symbol_contents(fcall_arg);
        if (!arg_name) {
            puts("ic_backend_pancake_compile_fcall: call to ic_symbol_contents failed");
            return 0;
        }
        arg_local = ic_scope_get(scope, arg_name);
        if (!arg_local) {
            puts("ic_backend_pancake_compile_fcall: call to ic_scope_get failed");
            return 0;
        }
        /* mark as accessed */
        arg_local->accessed = true;
        /* deal with different local cases */
        switch (arg_local->tag) {
            case icpl_literal:
                if (!ic_backend_pancake_compile_push_constant_from_local(instructions, arg_local)) {
                    puts("ic_backend_pancake_compile_fcall: call to ic_backend_pancake_compile_push_constant failed");
                    return 0;
                }
                break;

            case icpl_offset:
                /* insert `copyarg argn` instruction */
                instruction = ic_backend_pancake_instructions_add(instructions, icp_copyarg);
                if (!instruction) {
                    puts("ic_backend_pancake_compile_fcall: call to ic_backend_pancake_instructions_add failed");
                    return 0;
                }
                if (!ic_backend_pancake_bytecode_arg1_set_uint(instruction, arg_local->u.offset)) {
                    puts("ic_backend_pancake_compile_fcall: call to ic_backend_pancake_bytecode_arg1_set_uint failed for entry_jump");
                    return 0;
                }

                break;

            case icpl_runtime:
                /* insert `load key` instruction */
                instruction = ic_backend_pancake_instructions_add(instructions, icp_load);
                if (!instruction) {
                    puts("ic_backend_pancake_compile_fcall: call to ic_backend_pancake_instructions_add failed");
                    return 0;
                }
                if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, arg_name)) {
                    puts("ic_backend_pancake_compile_fcall: call to ic_backend_pancake_bytecode_arg1_set_char");
                    return 0;
                }

                break;

            default:
                puts("ic_backend_pancake_compile_fcall: impossible arg_local->arg");
                return 0;
                break;
        }
    }

    /* insert call instruction to fcall */
    fcall = tfcall->fcall;
    decl_func = fcall->fdecl;
    fdecl_sig_call = ic_decl_func_sig_call(decl_func);
    if (!fdecl_sig_call) {
        puts("ic_backend_pancake_compile_fcall: call to ic_decl_func_sig_call failed");
        return 0;
    }

    if (ic_decl_func_isbuiltin(fcall->fdecl)) {
        instruction = ic_backend_pancake_instructions_add(instructions, icp_call_builtin);
    } else {
        instruction = ic_backend_pancake_instructions_add(instructions, icp_call);
    }

    if (!instruction) {
        puts("ic_backend_pancake_compile_fcall: call to ic_backend_pancake_instructions_add failed");
        return 0;
    }
    if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, fdecl_sig_call)) {
        puts("ic_backend_pancake_compile_fcall: call to ic_backend_pancake_bytecode_arg1_set_uint failed for entry_jump");
        return 0;
    }
    /* set number of args we call with */
    if (!ic_backend_pancake_bytecode_arg2_set_uint(instruction, len)) {
        puts("ic_backend_pancake_compile_fcall: call to ic_backend_pancake_bytecode_arg1_set_uint failed for entry_jump");
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

static unsigned int ic_backend_pancake_generate_function_print_union(struct ic_backend_pancake_instructions *instructions, struct ic_kludge *kludge, struct ic_generate *gen) {
    /* TODO FIXME START OF COPY */
    struct ic_decl_type *tdecl = 0;
    struct ic_symbol *type_name_sym = 0;
    struct ic_decl_func *fdecl = 0;
    char *fdecl_sig_call = 0;
    unsigned int i_arg = 0;
    unsigned int n_args = 0;
    struct ic_field *field = 0;
    struct ic_decl_type *field_type = 0;
    struct ic_backend_pancake_bytecode *instruction = 0;
    unsigned int instructions_offset = 0;
    struct ic_string *fcall_label_str = 0;

    struct ic_labeller *labeller = 0;
    /* pvector of labels for each of our union fields */
    struct ic_pvector *labels = 0;
    char *label = 0;
    int pvector_ret = 0;
    unsigned int label_offset = 0;

    if (!instructions) {
        puts("ic_backend_pancake_generate_function_print_union: instructions was null");
        return 0;
    }

    if (!kludge) {
        puts("ic_backend_pancake_generate_function_print_union: kludge was null");
        return 0;
    }

    if (!gen) {
        puts("ic_backend_pancake_generate_function_print_union: gen was null");
        return 0;
    }

    tdecl = ic_generate_get_tdecl(gen);
    if (!tdecl) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_generate_get_tdecl failed");
        return 0;
    }

    /* get type name */
    type_name_sym = ic_decl_type_name(tdecl);
    if (!type_name_sym) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_decl_type_name failed");
        return 0;
    }

    fdecl = ic_generate_get_fdecl(gen);
    if (!fdecl) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_generate_get_fdecl failed");
        return 0;
    }

    fdecl_sig_call = ic_decl_func_sig_call(fdecl);
    if (!fdecl_sig_call) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_decl_func_sig_call failed");
        return 0;
    }

    labeller = ic_labeller_new(fdecl_sig_call);
    if (!labeller) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_labeller_new failed");
        return 0;
    }

    labels = ic_pvector_new(0);
    if (!labels) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_pvector_new failed");
        return 0;
    }

    /* union Foo
     *  a::Sint
     *  b::String
     *  c::Bar
     * end
     *
     * ->
     *
     * label print(Foo)
     * pushstr "Foo
     * call_builtin print(String) 1
     * pushstr "{"
     * call_builtin print(String) 1
     *
     * copyarg 0
     *
     * load_offset_uint 0
     * pushuint 0
     * call_builtin equal(Uint,Uint) 2
     * jnif_label "print(Foo)0
     *
     * load_offset_uint 0
     * pushuint 1
     * call_builtin equal(Uint,Uint) 2
     * jnif_label "print(Foo)1
     *
     * load_offset_uint 0
     * pushuint 2
     * call_builtin equal(Uint,Uint) 2
     * jnif_label "print(Foo)2
     *
     * icp_panic "impossible tag"
     *
     * label print(Foo)0
     * load_offset_sint 1
     * call_builtin print(Sint) 1
     * jmp_label print(Foo)_3
     *
     * label print(Foo)1
     * load_offset_str 1
     * call_builtin print(String) 1
     * jmp_label print(Foo)_3
     *
     * label print(Foo)2
     * load_offset_ref 1
     * call print(Bar) 1
     * jmp_label print(Foo)_3
     *
     * label print(Foo)3
     * pushstr "}"
     * call_builtin print(String) 1
     *
     * clean_stack
     * return_void
     */

    /* get length - which is offset of next instruction */
    instructions_offset = ic_backend_pancake_instructions_length(instructions);

    /* register function at offset */
    if (!ic_backend_pancake_instructions_register_label(instructions, fdecl_sig_call, instructions_offset)) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_instructions_register_label failed for print");
        return 0;
    }

    /* label print(<TYPENAME>) */
    instruction = ic_backend_pancake_instructions_add(instructions, icp_label);
    if (!instruction) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_instructions_add failed");
        return 0;
    }

    if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, fdecl_sig_call)) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_bytecode_arg1_set_char failed");
        return 0;
    }

    /* pushstr "<typename>" */
    instruction = ic_backend_pancake_instructions_add(instructions, icp_pushstr);
    if (!instruction) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_instructions_add failed");
        return 0;
    }

    if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, ic_symbol_contents(type_name_sym))) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_bytecode_arg1_set_char failed");
        return 0;
    }

    /* call_builtin print(String) 1*/
    instruction = ic_backend_pancake_instructions_add(instructions, icp_call_builtin);
    if (!instruction) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_instructions_add failed");
        return 0;
    }

    if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, "print(String)")) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_bytecode_arg1_set_char failed");
        return 0;
    }

    if (!ic_backend_pancake_bytecode_arg2_set_uint(instruction, 1)) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_bytecode_arg2_set_uint failed");
        return 0;
    }

    /* pushstr "{" */
    instruction = ic_backend_pancake_instructions_add(instructions, icp_pushstr);
    if (!instruction) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_instructions_add failed");
        return 0;
    }

    if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, "{")) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_bytecode_arg1_set_char failed");
        return 0;
    }

    /* call_builtin print(String) 1*/
    instruction = ic_backend_pancake_instructions_add(instructions, icp_call_builtin);
    if (!instruction) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_instructions_add failed");
        return 0;
    }

    if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, "print(String)")) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_bytecode_arg1_set_char failed");
        return 0;
    }

    if (!ic_backend_pancake_bytecode_arg2_set_uint(instruction, 1)) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_bytecode_arg2_set_uint failed");
        return 0;
    }

    /* copyarg 0 */
    instruction = ic_backend_pancake_instructions_add(instructions, icp_copyarg);
    if (!instruction) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_instructions_add failed");
        return 0;
    }

    if (!ic_backend_pancake_bytecode_arg1_set_uint(instruction, 0)) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_bytecode_arg1_set_uint failed");
        return 0;
    }

    /* for each arg
     * load_offset_uint 0
     * check if eq to i_arg
     * if so jump
     */
    n_args = ic_decl_type_field_length(tdecl);

    for (i_arg = 0; i_arg < n_args; ++i_arg) {
        /* load_offset_uint 0 */
        instruction = ic_backend_pancake_instructions_add(instructions, icp_load_offset_uint);
        if (!instruction) {
            puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_instructions_add failed");
            return 0;
        }

        if (!ic_backend_pancake_bytecode_arg1_set_uint(instruction, 0)) {
            puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_bytecode_arg1_set_uint failed");
            return 0;
        }

        /* pushuint <i_arg> */
        instruction = ic_backend_pancake_instructions_add(instructions, icp_pushuint);
        if (!instruction) {
            puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_instructions_add failed");
            return 0;
        }

        if (!ic_backend_pancake_bytecode_arg1_set_uint(instruction, i_arg)) {
            puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_bytecode_arg1_set_uint failed");
            return 0;
        }

        /* call_builtin equal(Uint, Uint) 2 */
        instruction = ic_backend_pancake_instructions_add(instructions, icp_call_builtin);
        if (!instruction) {
            puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_instructions_add failed");
            return 0;
        }

        if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, "equal(Uint,Uint)")) {
            puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_bytecode_arg1_set_char failed");
            return 0;
        }

        if (!ic_backend_pancake_bytecode_arg2_set_uint(instruction, 2)) {
            puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_bytecode_arg2_set_uint failed");
            return 0;
        }

        /* generate our new label */
        label = ic_labeller_generate(labeller);
        if (!label) {
            puts("ic_backend_pancake_generate_function_print_union: call to ic_labeller_generate failed");
            return 0;
        }

        /* save our label for the next for loop */
        pvector_ret = ic_pvector_append(labels, label);
        if (pvector_ret != (int)i_arg) {
            puts("ic_backend_pancake_generate_function_print_union: call to ic_pvector_append failed");
            printf("for field '%u' our call to ic_pvector_append returned '%d'\n", i_arg, pvector_ret);
            return 0;
        }

        /* jif_label <fname><i_arg> */
        instruction = ic_backend_pancake_instructions_add(instructions, icp_jif_label);
        if (!instruction) {
            puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_instructions_add failed");
            return 0;
        }

        if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, label)) {
            puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_bytecode_arg1_set_char failed");
            return 0;
        }
    }

    /* insert panic in case we matched none of the above conditions */
    /* panic "impossible tag */
    instruction = ic_backend_pancake_instructions_add(instructions, icp_panic);
    if (!instruction) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_instructions_add failed");
        return 0;
    }

    if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, "impossible tag")) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_bytecode_arg1_set_char failed");
        return 0;
    }

    /* final label */
    label = ic_labeller_generate(labeller);
    if (!label) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_labeller_generate failed");
        return 0;
    }

    pvector_ret = ic_pvector_append(labels, label);
    if (pvector_ret != (int)n_args) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_pvector_append failed");
        printf("for n_args '%u' our call to ic_pvector_append returned '%d'\n", n_args, pvector_ret);
        return 0;
    }

    /* for each arg
     * create label
     * unpack appropriate type
     * generate call to appropriate print function
     * jump to final place
     */
    for (i_arg = 0; i_arg < n_args; ++i_arg) {
        /* get our label from the pvector */
        label = ic_pvector_get(labels, i_arg);
        if (!label) {
            puts("ic_backend_pancake_generate_function_print_union: call to ic_pvector_get failed");
            return 0;
        }

        /* register label */
        /* get length - which is offset of next instruction */
        label_offset = ic_backend_pancake_instructions_length(instructions);

        /* register function at offset */
        if (!ic_backend_pancake_instructions_register_label(instructions, label, label_offset)) {
            puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_instructions_register_label failed");
            return 0;
        }

        /* label print(<TYPENAME>)<i_arg> */
        instruction = ic_backend_pancake_instructions_add(instructions, icp_label);
        if (!instruction) {
            puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_instructions_add failed");
            return 0;
        }

        if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, label)) {
            puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_bytecode_arg1_set_char failed");
            return 0;
        }

        field = ic_decl_type_field_get(tdecl, i_arg);
        if (!field) {
            puts("ic_backend_pancake_generate_function_print_union: call to ic_decl_type_field_get failed");
            return 0;
        }

        /* get field_type */
        field_type = ic_type_ref_get_type_decl(field->type);
        if (!field_type) {
            puts("ic_backend_pancake_generate_function_print_union: call to ic_type_ref_get_type_decl failed");
            return 0;
        }

        type_name_sym = ic_decl_type_name(field_type);
        if (!type_name_sym) {
            puts("ic_backend_pancake_generate_function_print_union: call to ic_decl_type_name failed");
            return 0;
        }

        if (ic_decl_type_isbool(field_type)) {
            /* load_offset_bool <i_arg> */
            instruction = ic_backend_pancake_instructions_add(instructions, icp_load_offset_bool);
            if (!instruction) {
                puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
        } else if (ic_decl_type_isstring(field_type)) {
            /* load_offset_str <i_arg> */
            instruction = ic_backend_pancake_instructions_add(instructions, icp_load_offset_str);
            if (!instruction) {
                puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
        } else if (ic_decl_type_isuint(field_type)) {
            /* load_offset_uint <i_arg> */
            instruction = ic_backend_pancake_instructions_add(instructions, icp_load_offset_uint);
            if (!instruction) {
                puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
        } else if (ic_decl_type_issint(field_type)) {
            /* load_offset_sint <i_arg> */
            instruction = ic_backend_pancake_instructions_add(instructions, icp_load_offset_sint);
            if (!instruction) {
                puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
        } else {
            /* load_offset_ref <i_arg> */
            instruction = ic_backend_pancake_instructions_add(instructions, icp_load_offset_ref);
            if (!instruction) {
                puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
        }

        if (!ic_backend_pancake_bytecode_arg1_set_uint(instruction, 1)) {
            puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_bytecode_arg1_set_uint failed");
            return 0;
        }

        /* call print(TYPENAME) 1 OR call_println print(TYPENAME) 1*/
        if (ic_decl_type_isbuiltin(field_type)) {
            /* load_offset <i_arg> */
            instruction = ic_backend_pancake_instructions_add(instructions, icp_call_builtin);
            if (!instruction) {
                puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
        } else {
            instruction = ic_backend_pancake_instructions_add(instructions, icp_call);
            if (!instruction) {
                puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
        }

        /* generate a string of the form "print(<TYPENAME>)" */
        /* TODO FIXME leaking this string! */
        fcall_label_str = ic_string_new("print(", 6);
        if (!fcall_label_str) {
            puts("ic_backend_pancake_generate_function_print_union: call to ic_string_new failed");
            return 0;
        }

        if (!ic_string_append_symbol(fcall_label_str, type_name_sym)) {
            puts("ic_backend_pancake_generate_function_print_union: call to ic_string_append_symbol failed");
            return 0;
        }

        if (!ic_string_append_char(fcall_label_str, ")", 1)) {
            puts("ic_backend_pancake_generate_function_print_union: call to ic_string_append_char failed");
            return 0;
        }

        /* TODO FIXME who will own the string 'print(TYPENAME)' ??? */

        if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, ic_string_contents(fcall_label_str))) {
            puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_bytecode_arg1_set_char failed");
            return 0;
        }

        if (!ic_backend_pancake_bytecode_arg2_set_uint(instruction, 1)) {
            puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_bytecode_arg1_set_uint failed");
            return 0;
        }

        label = ic_pvector_get(labels, n_args);
        if (!label) {
            puts("ic_backend_pancake_generate_function_print_union: call to ic_pvector_get failed");
            return 0;
        }

        /* insert jmp to final place
         * jmp_label <fname><n_args>
         */
        instruction = ic_backend_pancake_instructions_add(instructions, icp_jmp_label);
        if (!instruction) {
            puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_instructions_add failed");
            return 0;
        }

        if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, label)) {
            puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_bytecode_arg1_set_char failed");
            return 0;
        }
    }

    label = ic_pvector_get(labels, n_args);
    if (!label) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_pvector_get failed");
        return 0;
    }

    /* register label */
    /* get length - which is offset of next instruction */
    label_offset = ic_backend_pancake_instructions_length(instructions);

    /* register function at offset */
    if (!ic_backend_pancake_instructions_register_label(instructions, label, label_offset)) {
        puts("ic_backend_pancake_compile_fdecl: call to ic_backend_pancake_instructions_register_label failed");
        return 0;
    }

    /* insert label <fname><nargs> */
    instruction = ic_backend_pancake_instructions_add(instructions, icp_label);
    if (!instruction) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_instructions_add failed");
        return 0;
    }

    if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, label)) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_bytecode_arg1_set_char failed");
        return 0;
    }

    /* pushstr "}" */
    instruction = ic_backend_pancake_instructions_add(instructions, icp_pushstr);
    if (!instruction) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_instructions_add failed");
        return 0;
    }

    if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, "}")) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_bytecode_arg1_set_char failed");
        return 0;
    }

    /* call_builtin print(String) 1*/
    instruction = ic_backend_pancake_instructions_add(instructions, icp_call_builtin);
    if (!instruction) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_instructions_add failed");
        return 0;
    }

    if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, "print(String)")) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_bytecode_arg1_set_char failed");
        return 0;
    }

    if (!ic_backend_pancake_bytecode_arg2_set_uint(instruction, 1)) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_bytecode_arg2_set_uint failed");
        return 0;
    }

    /* insert a cleanup and return_void instruction */
    if (!ic_backend_pancake_instructions_add(instructions, icp_clean_stack)) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_instructions_add failed");
        return 0;
    }
    if (!ic_backend_pancake_instructions_add(instructions, icp_return_void)) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_backend_pancake_instructions_add failed");
        return 0;
    }

    if (!ic_labeller_destroy(labeller, 1)) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_labeller_destroy failed");
        return 0;
    }

    /* TODO FIXME leaking labels
     * here we do NOT use ic_pvector_destroyer_free as we need the labels to
     * still be allocated when we output / interpret our instructions
     * this means we are leaking them
     * TODO FIXME we need a way of signalling that the instruction 'owns' this
     * data and should free it
     */
    if (!ic_pvector_destroy(labels, 1, 0)) {
        puts("ic_backend_pancake_generate_function_print_union: call to ic_pvector_destroy failed");
        return 0;
    }

    /* success */
    return 1;
}

static unsigned int ic_backend_pancake_generate_function_print_struct(struct ic_backend_pancake_instructions *instructions, struct ic_kludge *kludge, struct ic_generate *gen) {
    struct ic_decl_type *tdecl = 0;
    struct ic_symbol *type_name_sym = 0;
    struct ic_decl_func *fdecl = 0;
    char *fdecl_sig_call = 0;
    unsigned int i_arg = 0;
    unsigned int n_args = 0;
    struct ic_field *field = 0;
    struct ic_decl_type *field_type = 0;
    struct ic_backend_pancake_bytecode *instruction = 0;
    unsigned int instructions_offset = 0;
    struct ic_string *fcall_label_str = 0;

    if (!instructions) {
        puts("ic_backend_pancake_generate_function_print_struct: instructions was null");
        return 0;
    }

    if (!kludge) {
        puts("ic_backend_pancake_generate_function_print_struct: kludge was null");
        return 0;
    }

    if (!gen) {
        puts("ic_backend_pancake_generate_function_print_struct: gen was null");
        return 0;
    }

    tdecl = ic_generate_get_tdecl(gen);
    if (!tdecl) {
        puts("ic_backend_pancake_generate_function_print_struct: call to ic_generate_get_tdecl failed");
        return 0;
    }

    /* get type name */
    type_name_sym = ic_decl_type_name(tdecl);
    if (!type_name_sym) {
        puts("ic_backend_pancake_generate_function_print_struct: call to ic_decl_type_name failed");
        return 0;
    }

    fdecl = ic_generate_get_fdecl(gen);
    if (!fdecl) {
        puts("ic_backend_pancake_generate_function_print_struct: call to ic_generate_get_fdecl failed");
        return 0;
    }

    fdecl_sig_call = ic_decl_func_sig_call(fdecl);
    if (!fdecl_sig_call) {
        puts("ic_backend_pancake_generate_function_print_struct: call to ic_decl_func_sig_call failed");
        return 0;
    }

    /* type Foo
     *  a::Sint
     *  b::String
     *  c::Bar
     * end
     *
     * ->
     *
     * label print(Foo)
     * pushstr "Foo"
     * call_builtin print(String) 1
     * pushstr "{"
     * call_builtin print(String) 1
     * copyarg 0
     * load_offset 0
     * call_builtin print(Sint) 1
     * load_offset 1
     * call_builtin print(String) 1
     * load_offset 2
     * call print(Bar) 1
     * pushstr "}"
     * call_builtin print(String) 1
     * clean_stack
     * return_void
     *
     * FIXME TODO does load_offset consume the object too?
     * is it okay for one copyarg to preceed many print calls?
     */

    /* get length - which is offset of next instruction */
    instructions_offset = ic_backend_pancake_instructions_length(instructions);

    /* register function at offset */
    if (!ic_backend_pancake_instructions_register_label(instructions, fdecl_sig_call, instructions_offset)) {
        puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_instructions_register_label failed for print");
        return 0;
    }

    /* label print(<TYPENAME>) */
    instruction = ic_backend_pancake_instructions_add(instructions, icp_label);
    if (!instruction) {
        puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_instructions_add failed");
        return 0;
    }

    if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, fdecl_sig_call)) {
        puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_bytecode_arg1_set_char failed");
        return 0;
    }

    /* pushstr "<typename>" */
    instruction = ic_backend_pancake_instructions_add(instructions, icp_pushstr);
    if (!instruction) {
        puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_instructions_add failed");
        return 0;
    }

    if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, ic_symbol_contents(type_name_sym))) {
        puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_bytecode_arg1_set_char failed");
        return 0;
    }

    /* call_builtin print(String) 1*/
    instruction = ic_backend_pancake_instructions_add(instructions, icp_call_builtin);
    if (!instruction) {
        puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_instructions_add failed");
        return 0;
    }

    if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, "print(String)")) {
        puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_bytecode_arg1_set_char failed");
        return 0;
    }

    if (!ic_backend_pancake_bytecode_arg2_set_uint(instruction, 1)) {
        puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_bytecode_arg2_set_uint failed");
        return 0;
    }

    /* pushstr "{" */
    instruction = ic_backend_pancake_instructions_add(instructions, icp_pushstr);
    if (!instruction) {
        puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_instructions_add failed");
        return 0;
    }

    if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, "{")) {
        puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_bytecode_arg1_set_char failed");
        return 0;
    }

    /* call_builtin print(String) 1*/
    instruction = ic_backend_pancake_instructions_add(instructions, icp_call_builtin);
    if (!instruction) {
        puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_instructions_add failed");
        return 0;
    }

    if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, "print(String)")) {
        puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_bytecode_arg1_set_char failed");
        return 0;
    }

    if (!ic_backend_pancake_bytecode_arg2_set_uint(instruction, 1)) {
        puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_bytecode_arg2_set_uint failed");
        return 0;
    }

    /* copyarg 0 */
    instruction = ic_backend_pancake_instructions_add(instructions, icp_copyarg);
    if (!instruction) {
        puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_instructions_add failed");
        return 0;
    }

    if (!ic_backend_pancake_bytecode_arg1_set_uint(instruction, 0)) {
        puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_bytecode_arg1_set_uint failed");
        return 0;
    }

    /* for each arg */
    n_args = ic_decl_type_field_length(tdecl);

    for (i_arg = 0; i_arg < n_args; ++i_arg) {
        /* insert ", " between args */
        if (i_arg > 0) {
            /* pushstr ", " */
            instruction = ic_backend_pancake_instructions_add(instructions, icp_pushstr);
            if (!instruction) {
                puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }

            if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, ", ")) {
                puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_bytecode_arg1_set_char failed");
                return 0;
            }

            /* call_builtin print(String) 1*/
            instruction = ic_backend_pancake_instructions_add(instructions, icp_call_builtin);
            if (!instruction) {
                puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }

            if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, "print(String)")) {
                puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_bytecode_arg1_set_char failed");
                return 0;
            }

            if (!ic_backend_pancake_bytecode_arg2_set_uint(instruction, 1)) {
                puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_bytecode_arg2_set_uint failed");
                return 0;
            }
        }

        field = ic_decl_type_field_get(tdecl, i_arg);
        if (!field) {
            puts("ic_backend_pancake_generate_function_print_struct: call to ic_decl_type_field_get failed");
            return 0;
        }

        /* get field_type */
        field_type = ic_type_ref_get_type_decl(field->type);
        if (!field_type) {
            puts("ic_backend_pancake_generate_function_print_struct: call to ic_type_ref_get_type_decl failed");
            return 0;
        }

        type_name_sym = ic_decl_type_name(field_type);
        if (!type_name_sym) {
            puts("ic_backend_pancake_generate_function_print_struct: call to ic_decl_type_name failed");
            return 0;
        }

        if (ic_decl_type_isbool(field_type)) {
            /* load_offset_bool <i_arg> */
            instruction = ic_backend_pancake_instructions_add(instructions, icp_load_offset_bool);
            if (!instruction) {
                puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
        } else if (ic_decl_type_isstring(field_type)) {
            /* load_offset_str <i_arg> */
            instruction = ic_backend_pancake_instructions_add(instructions, icp_load_offset_str);
            if (!instruction) {
                puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
        } else if (ic_decl_type_isuint(field_type)) {
            /* load_offset_uint <i_arg> */
            instruction = ic_backend_pancake_instructions_add(instructions, icp_load_offset_uint);
            if (!instruction) {
                puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
        } else if (ic_decl_type_issint(field_type)) {
            /* load_offset_sint <i_arg> */
            instruction = ic_backend_pancake_instructions_add(instructions, icp_load_offset_sint);
            if (!instruction) {
                puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
        } else {
            /* load_offset_ref <i_arg> */
            instruction = ic_backend_pancake_instructions_add(instructions, icp_load_offset_ref);
            if (!instruction) {
                puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
        }

        if (!ic_backend_pancake_bytecode_arg1_set_uint(instruction, i_arg)) {
            puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_bytecode_arg1_set_uint failed");
            return 0;
        }

        /* call print(TYPENAME) 1 OR call_println print(TYPENAME) 1*/
        if (ic_decl_type_isbuiltin(field_type)) {
            /* load_offset <i_arg> */
            instruction = ic_backend_pancake_instructions_add(instructions, icp_call_builtin);
            if (!instruction) {
                puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
        } else {
            instruction = ic_backend_pancake_instructions_add(instructions, icp_call);
            if (!instruction) {
                puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_instructions_add failed");
                return 0;
            }
        }

        /* generate a string of the form "print(<TYPENAME>)" */
        /* TODO FIXME leaking this string! */
        fcall_label_str = ic_string_new("print(", 6);
        if (!fcall_label_str) {
            puts("ic_backend_pancake_generate_function_print_struct: call to ic_string_new failed");
            return 0;
        }

        if (!ic_string_append_symbol(fcall_label_str, type_name_sym)) {
            puts("ic_backend_pancake_generate_function_print_struct: call to ic_string_append_symbol failed");
            return 0;
        }

        if (!ic_string_append_char(fcall_label_str, ")", 1)) {
            puts("ic_backend_pancake_generate_function_print_struct: call to ic_string_append_char failed");
            return 0;
        }

        /* TODO FIXME who will own the string 'print(TYPENAME)' ??? */

        if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, ic_string_contents(fcall_label_str))) {
            puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_bytecode_arg1_set_char failed");
            return 0;
        }

        if (!ic_backend_pancake_bytecode_arg2_set_uint(instruction, 1)) {
            puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_bytecode_arg1_set_uint failed");
            return 0;
        }
    }

    /* pushstr "}" */
    instruction = ic_backend_pancake_instructions_add(instructions, icp_pushstr);
    if (!instruction) {
        puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_instructions_add failed");
        return 0;
    }

    if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, "}")) {
        puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_bytecode_arg1_set_char failed");
        return 0;
    }

    /* call_builtin print(String) 1*/
    instruction = ic_backend_pancake_instructions_add(instructions, icp_call_builtin);
    if (!instruction) {
        puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_instructions_add failed");
        return 0;
    }

    if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, "print(String)")) {
        puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_bytecode_arg1_set_char failed");
        return 0;
    }

    if (!ic_backend_pancake_bytecode_arg2_set_uint(instruction, 1)) {
        puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_bytecode_arg2_set_uint failed");
        return 0;
    }

    /* insert a cleanup and return_void instruction */
    if (!ic_backend_pancake_instructions_add(instructions, icp_clean_stack)) {
        puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_instructions_add failed");
        return 0;
    }
    if (!ic_backend_pancake_instructions_add(instructions, icp_return_void)) {
        puts("ic_backend_pancake_generate_function_print_struct: call to ic_backend_pancake_instructions_add failed");
        return 0;
    }

    /* success */
    return 1;
}

static unsigned int ic_backend_pancake_generate_function_print(struct ic_backend_pancake_instructions *instructions, struct ic_kludge *kludge, struct ic_generate *gen) {
    struct ic_decl_type *tdecl = 0;

    if (!instructions) {
        puts("ic_backend_pancake_generate_function_print: instructions was null");
        return 0;
    }

    if (!kludge) {
        puts("ic_backend_pancake_generate_function_print: kludge was null");
        return 0;
    }

    if (!gen) {
        puts("ic_backend_pancake_generate_function_print: gen was null");
        return 0;
    }

    tdecl = ic_generate_get_tdecl(gen);
    if (!tdecl) {
    }

    switch (tdecl->tag) {
        case ic_decl_type_tag_struct:
            if (!ic_backend_pancake_generate_function_print_struct(instructions, kludge, gen)) {
                puts("ic_backend_pancake_generate_function_print: call to ic_backend_pancake_generate_function_print_struct failed");
                return 0;
            }
            break;

        case ic_decl_type_tag_union:
            if (!ic_backend_pancake_generate_function_print_union(instructions, kludge, gen)) {
                puts("ic_backend_pancake_generate_function_print: call to ic_backend_pancake_generate_function_print_union failed");
                return 0;
            }
            break;

        default:
            puts("ic_backend_pancake_generate_function_print: unknown tdecl->tag");
            return 0;
            break;
    }

    return 1;
}

/* generate all needed functions
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_generate_functions(struct ic_backend_pancake_instructions *instructions, struct ic_kludge *kludge) {
    unsigned int i_gen = 0;
    unsigned int n_gens = 0;
    struct ic_generate *gen = 0;
    struct ic_decl_func *fdecl = 0;
    struct ic_decl_type *tdecl = 0;
    /* char * to sig_call for current fdecl
     * used for bytecode
     */
    char *fdecl_sig_call = 0;
    struct ic_backend_pancake_bytecode *instruction = 0;
    struct ic_symbol *type_name_sym = 0;
    struct ic_string *fcall_label_str = 0;
    unsigned int alloc_size = 0;
    unsigned int i_arg = 0;
    unsigned int n_args = 0;
    struct ic_field *field = 0;
    unsigned int instructions_offset = 0;
    char *field_name_ch = 0;

    if (!instructions) {
        puts("ic_backend_pancake_generate_functions: instructions was null");
        return 0;
    }

    if (!kludge) {
        puts("ic_backend_pancake_generate_functions: kludge was null");
        return 0;
    }

    /* `type` is a name which maps to a pair
     * type => ( total_size, dict field_name => (field_offset, type) )
     *
     * the constructor needs to know the size to allocate and how to
     * init the fields
     *
     * every access of a field needs to know the field offsets
     * only need field type information at static time
     *
     * for things such as nested access
     * f.a.b
     * requires knowing the type of `f` and `f.a`
     *
     * although tir will have already simplified this...
     *
     * but we still need that time information as pancake values are
     * typed.
     *
     * example:
     *
     *   type Foo
     *    a::Sint
     *    b::Sint
     *    s::String
     *   end
     *
     * should generate
     *
     *   label Foo(Sint,Sint,String)
     *   alloc 3 // allocate a 3 cell object
     *   copyarg 0
     *   store_offset 0
     *   cpyarg 1
     *   store_offset 1
     *   copyarg 2
     *   store_offset 2
     *   save
     *   clean_stack
     *   restore
     *   return_value
     *
     * if we have that same type and a user access it
     *
     *   let f = Foo(1, 4, "hello")
     *   println(f.a)
     *   f.a = 4
     *
     * then this will (ignoring permissions) generate roughly
     *
     *   pushint 1
     *   pushint 4
     *   pushstr "hello"
     *   call Foo(Sint,Sint,String) 3
     *   load_offset 0
     *   call_builtin println(Sint) 1
     *   pushint 4
     *   store_offset 0
     */

    n_gens = ic_kludge_generates_length(kludge);
    for (i_gen = 0; i_gen < n_gens; ++i_gen) {

        gen = ic_kludge_generates_get(kludge, i_gen);
        if (!gen) {
            puts("ic_backend_pancake_generate_functions: call to ic_kludge_generates_get failed");
            return 0;
        }

        tdecl = ic_generate_get_tdecl(gen);
        if (!tdecl) {
            puts("ic_backend_pancake_generate_functions: call to ic_generate_get_tdecl failed");
            return 0;
        }

        /* get type name */
        type_name_sym = ic_decl_type_name(tdecl);
        if (!type_name_sym) {
            puts("ic_backend_pancake_generate_functions: call to ic_decl_type_name failed");
            return 0;
        }

        fdecl = ic_generate_get_fdecl(gen);
        if (!fdecl) {
            puts("ic_backend_pancake_generate_functions: call to ic_generate_get_fdecl failed");
            return 0;
        }

        fdecl_sig_call = ic_decl_func_sig_call(fdecl);
        if (!fdecl_sig_call) {
            puts("ic_backend_pancake_generate_functions: call to ic_decl_func_sig_call failed");
            return 0;
        }

        switch (gen->tag) {
            case ic_generate_tag_cons_struct:

                /* type Foo
                 *  a::Sint
                 *  b::String
                 * end
                 *
                 * ->
                 *
                 * label Foo(Sint, String)
                 * alloc 2
                 * copyarg 0
                 * store_offset 0
                 * copyarg 1
                 * store_offset 1
                 * save
                 * clean_stack
                 * restore
                 * return_value
                 */

                /* get length - which is offset of next instruction */
                instructions_offset = ic_backend_pancake_instructions_length(instructions);

                /* register function at offset */
                if (!ic_backend_pancake_instructions_register_label(instructions, fdecl_sig_call, instructions_offset)) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_instructions_register_label failed for cons_struct");
                    return 0;
                }

                /* insert function label instruction */
                instruction = ic_backend_pancake_instructions_add(instructions, icp_label);
                if (!instruction) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_instructions_add failed");
                    return 0;
                }
                if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, fdecl_sig_call)) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_bytecode_arg1_set_char failed");
                    return 0;
                }

                n_args = ic_decl_type_field_length(tdecl);

                /* calculate object size
                 * for now we express this as number of cells
                 * which are all runtime-sized (likely 64 bits)
                 */
                alloc_size = n_args;

                /* alloc <size> */
                /* insert `copyarg argn` instruction */
                instruction = ic_backend_pancake_instructions_add(instructions, icp_alloc);
                if (!instruction) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_instructions_add failed");
                    return 0;
                }
                if (!ic_backend_pancake_bytecode_arg1_set_uint(instruction, alloc_size)) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_bytecode_arg1_set_uint failed for cons_struct");
                    return 0;
                }

                /* for each arg */
                for (i_arg = 0; i_arg < n_args; ++i_arg) {
                    /* copyarg <i_arg> */
                    /* insert `copyarg argn` instruction */
                    instruction = ic_backend_pancake_instructions_add(instructions, icp_copyarg);
                    if (!instruction) {
                        puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_instructions_add failed");
                        return 0;
                    }
                    if (!ic_backend_pancake_bytecode_arg1_set_uint(instruction, i_arg)) {
                        puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_bytecode_arg1_set_uint failed for cons_struct");
                        return 0;
                    }

                    /* store argument */
                    instruction = ic_backend_pancake_instructions_add(instructions, icp_store_offset);
                    if (!instruction) {
                        puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_instructions_add failed");
                        return 0;
                    }
                    if (!ic_backend_pancake_bytecode_arg1_set_uint(instruction, i_arg)) {
                        puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_bytecode_arg1_set_uint failed for cons_struct");
                        return 0;
                    }
                }

                /* save */
                instruction = ic_backend_pancake_instructions_add(instructions, icp_save);
                if (!instruction) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_instructions_add failed");
                    return 0;
                }

                /* clean_stack */
                instruction = ic_backend_pancake_instructions_add(instructions, icp_clean_stack);
                if (!instruction) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_instructions_add failed");
                    return 0;
                }

                /* restore */
                instruction = ic_backend_pancake_instructions_add(instructions, icp_restore);
                if (!instruction) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_instructions_add failed");
                    return 0;
                }

                /* return_value */
                instruction = ic_backend_pancake_instructions_add(instructions, icp_return_value);
                if (!instruction) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_instructions_add failed");
                    return 0;
                }

                /* success */
                break;

            case ic_generate_tag_cons_union:
                /* union Foo
                 *  a::Sint
                 *  b::String
                 * end
                 *
                 *
                 * first member
                 * ->
                 *
                 * label Foo(Sint)
                 * alloc 2
                 * pushuint 0
                 * store_offset 0
                 * copyarg 0
                 * store_offset 1
                 * save
                 * clean_stack
                 * restore
                 * return_value
                 *
                 * second member
                 * ->
                 *
                 * label Foo(String)
                 * alloc 1
                 * pushuint 1
                 * store_offset 0
                 * copyarg 0
                 * store_offset 1
                 * save
                 * clean_stack
                 * restore
                 * return_value
                 */

                /* get length - which is offset of next instruction */
                instructions_offset = ic_backend_pancake_instructions_length(instructions);

                /* register function at offset */
                if (!ic_backend_pancake_instructions_register_label(instructions, fdecl_sig_call, instructions_offset)) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_instructions_register_label failed for cons_union");
                    return 0;
                }

                /* insert function label instruction */
                instruction = ic_backend_pancake_instructions_add(instructions, icp_label);
                if (!instruction) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_instructions_add failed");
                    return 0;
                }
                if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, fdecl_sig_call)) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_bytecode_arg1_set_char failed");
                    return 0;
                }

                n_args = ic_decl_type_field_length(tdecl);
                if (!n_args) {
                    puts("ic_backend_pancake_generate_functions: call to ic_decl_type_field_length failed, zero-length objects not supported in pancake");
                    return 0;
                }

                /* union object size is always 2
                 * slot 0 for uint tag
                 * slot 1 for data
                 */
                alloc_size = 2;

                /* alloc <size> */
                /* insert `copyarg argn` instruction */
                instruction = ic_backend_pancake_instructions_add(instructions, icp_alloc);
                if (!instruction) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_instructions_add failed");
                    return 0;
                }
                if (!ic_backend_pancake_bytecode_arg1_set_uint(instruction, alloc_size)) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_bytecode_arg1_set_uint failed for cons_union");
                    return 0;
                }

                /* we need to insert the field number into slot 0 */
                field = ic_generate_get_union_field(gen);
                if (!field) {
                    puts("ic_backend_pancake_generate_functions: call to ic_generate_get_union_field failed for cons_union");
                    return 0;
                }
                /* pushuint */
                field_name_ch = ic_symbol_contents(&(field->name));
                if (!field_name_ch) {
                    puts("ic_backend_pancake_generate_functions: call to ic_symbol_contents failed for cons_union");
                    return 0;
                }
                i_arg = ic_decl_type_get_field_offset(tdecl, field_name_ch);

                /* pushuint <field offset> */
                instruction = ic_backend_pancake_instructions_add(instructions, icp_pushuint);
                if (!instruction) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_instructions_add failed");
                    return 0;
                }
                if (!ic_backend_pancake_bytecode_arg1_set_uint(instruction, i_arg)) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_bytecode_arg1_set_uint failed for cons_union");
                    return 0;
                }

                /* store argument */
                instruction = ic_backend_pancake_instructions_add(instructions, icp_store_offset);
                if (!instruction) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_instructions_add failed");
                    return 0;
                }
                if (!ic_backend_pancake_bytecode_arg1_set_uint(instruction, 0)) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_bytecode_arg1_set_uint failed for cons_union");
                    return 0;
                }

                /* for our single arg ... */
                /* insert `copyarg argn` instruction */
                instruction = ic_backend_pancake_instructions_add(instructions, icp_copyarg);
                if (!instruction) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_instructions_add failed");
                    return 0;
                }
                if (!ic_backend_pancake_bytecode_arg1_set_uint(instruction, 0)) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_bytecode_arg1_set_uint failed for cons_union");
                    return 0;
                }

                /* we always store arg 0 in slot 1 */
                /* store argument */
                instruction = ic_backend_pancake_instructions_add(instructions, icp_store_offset);
                if (!instruction) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_instructions_add failed");
                    return 0;
                }
                if (!ic_backend_pancake_bytecode_arg1_set_uint(instruction, 1)) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_bytecode_arg1_set_uint failed for cons_union");
                    return 0;
                }

                /* save */
                instruction = ic_backend_pancake_instructions_add(instructions, icp_save);
                if (!instruction) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_instructions_add failed");
                    return 0;
                }

                /* clean_stack */
                instruction = ic_backend_pancake_instructions_add(instructions, icp_clean_stack);
                if (!instruction) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_instructions_add failed");
                    return 0;
                }

                /* restore */
                instruction = ic_backend_pancake_instructions_add(instructions, icp_restore);
                if (!instruction) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_instructions_add failed");
                    return 0;
                }

                /* return_value */
                instruction = ic_backend_pancake_instructions_add(instructions, icp_return_value);
                if (!instruction) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_instructions_add failed");
                    return 0;
                }

                break;

            case ic_generate_tag_print:
                if (!ic_backend_pancake_generate_function_print(instructions, kludge, gen)) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_generate_function_print failed");
                    return 0;
                }
                break;

            case ic_generate_tag_println:
                /* println is nice and easy:
                 *
                 * type Foo
                 *   a::Sint
                 *   b::String
                 * end
                 *
                 * ->
                 *
                 * label println(Foo)
                 * copyarg 0
                 * call print(Foo) 1
                 * call_builtin println() 0
                 * clean_stack
                 * return_void
                 */

                /* get length - which is offset of next instruction */
                instructions_offset = ic_backend_pancake_instructions_length(instructions);

                /* register function at offset */
                if (!ic_backend_pancake_instructions_register_label(instructions, fdecl_sig_call, instructions_offset)) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_instructions_register_label failed for println");
                    return 0;
                }

                /* insert function label instruction */
                instruction = ic_backend_pancake_instructions_add(instructions, icp_label);
                if (!instruction) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_instructions_add failed");
                    return 0;
                }
                if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, fdecl_sig_call)) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_bytecode_arg1_set_char failed");
                    return 0;
                }

                /* copyarg */
                /* insert `copyarg argn` instruction */
                instruction = ic_backend_pancake_instructions_add(instructions, icp_copyarg);
                if (!instruction) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_instructions_add failed");
                    return 0;
                }
                if (!ic_backend_pancake_bytecode_arg1_set_uint(instruction, 0)) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_bytecode_arg1_set_uint failed for entry_jump");
                    return 0;
                }

                /* generate a string of the form "print(<TYPENAME>)" */
                /* TODO FIXME leaking this string! */
                fcall_label_str = ic_string_new("print(", 6);
                if (!fcall_label_str) {
                    puts("ic_backend_pancake_generate_functions: call to ic_string_new failed");
                    return 0;
                }

                if (!ic_string_append_symbol(fcall_label_str, type_name_sym)) {
                    puts("ic_backend_pancake_generate_functions: call to ic_string_append_symbol failed");
                    return 0;
                }

                if (!ic_string_append_char(fcall_label_str, ")", 1)) {
                    puts("ic_backend_pancake_generate_functions: call to ic_string_append_char failed");
                    return 0;
                }

                /* TODO FIXME who will own the string 'print(TYPENAME)' ??? */

                /* call print(<TYPENAME>) 1 */
                instruction = ic_backend_pancake_instructions_add(instructions, icp_call);
                if (!instruction) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_instructions_add failed");
                    return 0;
                }

                if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, ic_string_contents(fcall_label_str))) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_bytecode_arg1_set_char failed");
                    return 0;
                }
                /* set number of args we call with */
                if (!ic_backend_pancake_bytecode_arg2_set_uint(instruction, 1)) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_bytecode_arg2_set_uint failed");
                    return 0;
                }

                /* call_builtin println() 0 */
                instruction = ic_backend_pancake_instructions_add(instructions, icp_call_builtin);
                if (!instruction) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_instructions_add failed");
                    return 0;
                }

                if (!ic_backend_pancake_bytecode_arg1_set_char(instruction, "println()")) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_bytecode_arg1_set_char failed");
                    return 0;
                }

                /* set number of args we call with */
                if (!ic_backend_pancake_bytecode_arg2_set_uint(instruction, 0)) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_bytecode_arg2_set_uint failed");
                    return 0;
                }

                /* insert a cleanup and return_void instruction */
                if (!ic_backend_pancake_instructions_add(instructions, icp_clean_stack)) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_instructions_add failed");
                    return 0;
                }
                if (!ic_backend_pancake_instructions_add(instructions, icp_return_void)) {
                    puts("ic_backend_pancake_generate_functions: call to ic_backend_pancake_instructions_add failed");
                    return 0;
                }

                /* success */
                break;

            default:
                puts("ic_backend_pancake_generate_functions: impossible gen->tag");
                return 0;
                break;
        }
    }

    /* success */
    return 1;
}
