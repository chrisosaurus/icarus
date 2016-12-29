#include "expr.h"

#include "../../analyse/data/kludge.h"
#include "../../parse/data/expr.h"
#include "../../transform/data/tir.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"

unsigned int ic_b2c_compile_expr_fcall(struct ic_kludge *input_kludge, struct ic_transform_ir_fcall *fcall, FILE *out);

unsigned int ic_b2c_compile_expr_identifier(struct ic_kludge *input_kludge, struct ic_expr_identifier *identifier, FILE *out);
unsigned int ic_b2c_compile_expr_constant(struct ic_kludge *input_kludge, struct ic_expr_constant *constant, FILE *out);
unsigned int ic_b2c_compile_expr_operator(struct ic_kludge *input_kludge, struct ic_expr_operator *operator, FILE *out);
unsigned int ic_b2c_compile_expr_faccess(struct ic_kludge *input_kludge, struct ic_expr_faccess *faccess, FILE *out);

/* compile a given expr to specified file
 *
 * generate a c program
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_b2c_compile_expr(struct ic_kludge *input_kludge, struct ic_transform_ir_expr *texpr, FILE *out) {
    if (!input_kludge) {
        puts("ic_b2c_compile_expr: input_kludge was null");
        return 0;
    }

    if (!texpr) {
        puts("ic_b2c_compile_expr: texpr was null");
        return 0;
    }

    if (!out) {
        puts("ic_b2c_compile_expr: out was null");
        return 0;
    }

    if (!ic_b2c_compile_expr_fcall(input_kludge, texpr->fcall, out)) {
        puts("ic_b2c_compile_expr: call to ic_b2d_compile_expr_fcall failed");
        return 0;
    }

    return 1;
}

unsigned int ic_b2c_compile_expr_fcall(struct ic_kludge *input_kludge, struct ic_transform_ir_fcall *fcall, FILE *out) {
    /* function decl */
    struct ic_decl_func *fdecl = 0;
    /* mangled function signature, suitable for output */
    char *fdecl_sig_mangled = 0;
    /* offset into fcall args pvector */
    unsigned int i = 0;
    /* length of fcall args pvector */
    unsigned int length = 0;
    /* current arg */
    struct ic_symbol *sym = 0;

    if (!input_kludge) {
        puts("ic_b2c_compile_expr_fcall: input_kludge was null");
        return 0;
    }

    if (!fcall) {
        puts("ic_b2c_compile_expr_fcall: fcall was null");
        return 0;
    }

    if (!out) {
        puts("ic_b2c_compile_expr_fcall: out was null");
        return 0;
    }

    fdecl = ic_expr_func_call_get_fdecl(fcall->fcall);
    if (!fdecl) {
        puts("ic_b2c_compile_expr_fcall: call to ic_expr_func_call_get_fdecl failed");
        return 0;
    }

    /* find function name to call */
    fdecl_sig_mangled = ic_decl_func_sig_mangled(fdecl);
    if (!fdecl_sig_mangled) {
        puts("ic_b2c_compile_expr_fcall: call to ic_decl_func_sig_mangled failed");
        return 0;
    }

    /* omit function name */
    fputs(fdecl_sig_mangled, out);

    /* omit opening ( */
    fputs("(", out);

    /* omit arguments */
    length = ic_transform_ir_fcall_length(fcall);

    for (i = 0; i < length; ++i) {
        /* add commas between args */
        if (i > 0) {
            fputs(", ", out);
        }

        sym = ic_transform_ir_fcall_get_arg(fcall, i);
        if (!sym) {
            puts("ic_b2c_compile_expr_fcall: call to ic_transform_ir_fcall_get_arg");
            return 0;
        }

        /* output symbol */
        ic_symbol_print(out, sym);
    }

    /* omit closing ) */
    fputs(")", out);

    /* FIXME communicate to caller the need to end (e.g. optionally append `;`) */

    return 1;
}

unsigned int ic_b2c_compile_expr_identifier(struct ic_kludge *input_kludge, struct ic_expr_identifier *identifier, FILE *out) {
    unsigned int indent_level = 1;

    if (!input_kludge) {
        puts("ic_b2c_compile_expr_identifier: input_kludge was null");
        return 0;
    }

    if (!identifier) {
        puts("ic_b2c_compile_expr_identifier: identifier was null");
        return 0;
    }

    if (!out) {
        puts("ic_b2c_compile_expr_identifier: out was null");
        return 0;
    }

    puts("ic_b2c_compile_expr_identifier: call for");
    ic_expr_identifier_print(stdout, identifier, &indent_level);
    puts("");

    puts("ic_b2c_compile_expr_identifier: unimplemented");
    return 0;
}

unsigned int ic_b2c_compile_expr_constant(struct ic_kludge *input_kludge, struct ic_expr_constant *constant, FILE *out) {
    unsigned int indent_level = 1;

    if (!input_kludge) {
        puts("ic_b2c_compile_expr_constant: input_kludge was null");
        return 0;
    }

    if (!constant) {
        puts("ic_b2c_compile_expr_constant: constant was null");
        return 0;
    }

    if (!out) {
        puts("ic_b2c_compile_expr_constant: out was null");
        return 0;
    }

    switch (constant->tag) {
        case ic_expr_constant_type_integer:
            /* FIXME leaks memory from integer temporary */

            fprintf(out, "ic_sint_new(%ld)", constant->u.integer);

            return 1;

            break;

        case ic_expr_constant_type_string:
            /* FIXME leaks memory from string temporary */

            /* create temporary string ... */
            fputs("ic_string_new(", out);

            /* output opening " */
            fputs("\"", out);

            /* output string data */
            fputs(ic_string_contents(&(constant->u.string)), out);

            /* output closing " */
            fputs("\"", out);

            /* , */
            fputs(", ", out);

            /* string length */
            fprintf(out, "%d", ic_string_length(&(constant->u.string)));

            /* output closing ) */
            fputs(")", out);

            return 1;

            break;

        case ic_expr_constant_type_boolean:
            fprintf(out, "ic_bool_new(%u)\n", constant->u.boolean);

            return 1;

            break;

        default:
            puts("ic_b2c_compile_expr_constant: call for");
            ic_expr_constant_print(stdout, constant, &indent_level);
            puts("");

            puts("ic_b2c_compile_expr_constant: unknown tag");
            return 0;

            break;
    }

    puts("ic_b2c_compile_expr_constant: impossible");
    return 0;
}

unsigned int ic_b2c_compile_expr_operator(struct ic_kludge *input_kludge, struct ic_expr_operator *operator, FILE *out) {
    unsigned int indent_level = 1;

    if (!input_kludge) {
        puts("ic_b2c_compile_expr_operator: input_kludge was null");
        return 0;
    }

    if (!operator) {
        puts("ic_b2c_compile_expr_operator: operator was null");
        return 0;
    }

    if (!out) {
        puts("ic_b2c_compile_expr_operator: out was null");
        return 0;
    }

    puts("ic_b2c_compile_expr_operator: call for");
    ic_expr_operator_print(stdout, operator, &indent_level);
    puts("");

    puts("ic_b2c_compile_expr_operator: unimplemented");
    return 0;
}

unsigned int ic_b2c_compile_expr_faccess(struct ic_kludge *input_kludge, struct ic_expr_faccess *faccess, FILE *out) {
    unsigned int indent_level = 1;

    if (!input_kludge) {
        puts("ic_b2c_compile_expr_faccess: input_kludge was null");
        return 0;
    }

    if (!faccess) {
        puts("ic_b2c_compile_expr_faccess: faccess was null");
        return 0;
    }

    if (!out) {
        puts("ic_b2c_compile_expr_faccess: out was null");
        return 0;
    }

    puts("ic_b2c_compile_expr_faccess: call for");
    ic_expr_faccess_print(stdout, faccess, &indent_level);
    puts("");

    puts("ic_b2c_compile_expr_faccess: unimplemented");
    return 0;
}
