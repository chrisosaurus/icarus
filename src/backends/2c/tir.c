#include "tir.h"
#include "expr.h"

#include "../../analyse/data/kludge.h"
#include "../../parse/data/stmt.h"
#include "../../transform/data/tbody.h"

unsigned int ic_b2c_compile_stmt_ret(struct ic_kludge *input_kludge, struct ic_transform_ir_ret *ret, FILE *out);
unsigned int ic_b2c_compile_stmt_let(struct ic_kludge *input_kludge, struct ic_transform_ir_let *let, FILE *out);
unsigned int ic_b2c_compile_stmt_assign(struct ic_kludge *input_kludge, struct ic_transform_ir_assign *assign, FILE *out);
unsigned int ic_b2c_compile_stmt_if(struct ic_kludge *input_kludge, struct ic_transform_ir_if *tif, FILE *out);
unsigned int ic_b2c_compile_stmt_expr(struct ic_kludge *input_kludge, struct ic_transform_ir_expr *expr, FILE *out);

/* compile a given body to specified file
 *
 * generate a c program
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_b2c_compile_body(struct ic_kludge *input_kludge, struct ic_transform_body *tbody, FILE *out) {
    /* current tir_stmt in tir_body */
    struct ic_transform_ir_stmt *tstmt = 0;
    /* index of current tir_stmt in tir_body */
    unsigned int i = 0;
    /* length of tir_body */
    unsigned int len = 0;

    if (!input_kludge) {
        puts("ic_b2c_compile_body: input_kludge was null");
        return 0;
    }

    if (!tbody) {
        puts("ic_b2c_compile_body: tbody was null");
        return 0;
    }

    if (!out) {
        puts("ic_b2c_compile_body: out was null");
        return 0;
    }

    len = ic_transform_body_length(tbody);

    for (i = 0; i < len; ++i) {
        tstmt = ic_transform_body_get(tbody, i);
        if (!tstmt) {
            puts("ic_b2c_compile_body: call to ic_transform_body_get failed");
            return 0;
        }

        if (!ic_b2c_compile_stmt(input_kludge, tstmt, out)) {
            puts("ic_b2c_compile_body: call to ic_b2c_compile_stmt failed");
            return 0;
        }
    }

    return 1;
}

/* compile a given stmt to specified file
 *
 * generate a c program
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_b2c_compile_stmt(struct ic_kludge *input_kludge, struct ic_transform_ir_stmt *tstmt, FILE *out) {
    unsigned int indent_level = 1;

    if (!input_kludge) {
        puts("ic_b2c_compile_stmt: input_kludge was null");
        return 0;
    }

    if (!tstmt) {
        puts("ic_b2c_compile_stmt: tstmt was null");
        return 0;
    }

    if (!out) {
        puts("ic_b2c_compile_stmt: out was null");
        return 0;
    }

    /* dispatch to appropriate sub handler based on tstmt type */
    switch (tstmt->tag) {
        case ic_transform_ir_stmt_type_expr:
            if (!ic_b2c_compile_stmt_expr(input_kludge, &(tstmt->u.expr), out)) {
                puts("ic_b2c_compile_stmt: call to ic_b2c_compile_stmt_expr failed");
                return 0;
            }
            return 1;
            break;

        case ic_transform_ir_stmt_type_let:
            return ic_b2c_compile_stmt_let(input_kludge, &(tstmt->u.let), out);
            break;

        case ic_transform_ir_stmt_type_ret:
            return ic_b2c_compile_stmt_ret(input_kludge, &(tstmt->u.ret), out);
            break;

        case ic_transform_ir_stmt_type_assign:
            return ic_b2c_compile_stmt_assign(input_kludge, &(tstmt->u.assign), out);
            break;

        case ic_transform_ir_stmt_type_if:
            return ic_b2c_compile_stmt_if(input_kludge, &(tstmt->u.sif), out);
            break;

        default:
            puts("ic_b2c_compile_stmt: stmt had impossible tag:");
            ic_transform_ir_stmt_print(stdout, tstmt, &indent_level);

            return 0;
            break;
    }

    puts("ic_b2c_compile_stmt: impossible...");
    return 0;
}

unsigned int ic_b2c_compile_stmt_ret(struct ic_kludge *input_kludge, struct ic_transform_ir_ret *ret, FILE *out) {
    struct ic_symbol *ret_sym = 0;
    char *ret_str = 0;

    if (!input_kludge) {
        puts("ic_b2c_compile_stmt_ret: input_kludge was null");
        return 0;
    }

    if (!ret) {
        puts("ic_b2c_compile_stmt_ret: ret was null");
        return 0;
    }

    if (!out) {
        puts("ic_b2c_compile_stmt_ret: out was null");
        return 0;
    }

    ret_sym = ret->var;
    if (!ret_sym) {
        puts("ic_b2c_compile_stmt_ret: ret_sym was null");
        return 0;
    }

    ret_str = ic_symbol_contents(ret_sym);
    if (!ret_str) {
        puts("ic_b2c_compile_stmt_ret: call to ic_symbol_contents failed");
        return 0;
    }

    fprintf(out, "  return %s;\n", ret_str);

    return 1;
}

unsigned int ic_b2c_compile_stmt_let(struct ic_kludge *input_kludge, struct ic_transform_ir_let *let, FILE *out) {
    struct ic_decl_type *let_type = 0;
    char *let_name = 0;
    struct ic_symbol *let_sym = 0;
    char *let_str = 0;

    struct ic_expr_constant *literal = 0;

    struct ic_transform_ir_let_expr *let_expr = 0;
    struct ic_transform_ir_let_faccess *let_faccess = 0;
    struct ic_transform_ir_expr *expr = 0;

    if (!input_kludge) {
        puts("ic_b2c_compile_stmt_let: input_kludge was null");
        return 0;
    }

    if (!let) {
        puts("ic_b2c_compile_stmt_let: let was null");
        return 0;
    }

    if (!out) {
        puts("ic_b2c_compile_stmt_let: out was null");
        return 0;
    }

    switch (let->tag) {
        case ic_transform_ir_let_type_literal:
            /* let name::type = literal */
            /* goes to */
            /* c_type_str name = cons(literal) */

            let_type = let->u.lit.type;

            let_sym = ic_decl_type_name(let_type);
            if (!let_sym) {
                puts("ic_b2c_compile_stmt_let: call to ic_decl_type_name failed");
                return 0;
            }

            let_str = ic_symbol_contents(let_sym);
            if (!let_str) {
                puts("ic_b2c_compile_stmt_let: call to ic_symbol_contents failed for let type");
                return 0;
            }

            let_name = ic_symbol_contents(let->u.lit.name);
            if (!let_name) {
                puts("ic_b2c_compile_stmt_let: call to ic_symbol_contents failed for let name");
                return 0;
            }

            /* print "type name = " */
            fprintf(out, "  %s %s = ", let_str, let_name);

            /* literal */
            literal = let->u.lit.literal;

            if (!ic_b2c_compile_expr_constant(input_kludge, literal, out)) {
                puts("ic_b2c_compile_stmt_let: call to ic_b2c_compile_expr_constant failed");
                return 0;
            }

            /* closing semicolon and trailing \n */
            fputs(";\n", out);

            return 1;
            break;

        case ic_transform_ir_let_type_expr:
            /* let name::type = fcall(args...) */
            /* goes to */
            /* c_type_str name = fcall(args...) */

            let_expr = &(let->u.expr);

            let_type = let_expr->type;

            let_sym = ic_decl_type_name(let_type);
            if (!let_sym) {
                puts("ic_b2c_compile_stmt_let: call to ic_decl_type_name failed");
                return 0;
            }

            let_str = ic_symbol_contents(let_sym);
            if (!let_str) {
                puts("ic_b2c_compile_stmt_let: call to ic_symbol_contents failed for let type");
                return 0;
            }

            let_name = ic_symbol_contents(let_expr->name);
            if (!let_name) {
                puts("ic_b2c_compile_stmt_let: call to ic_symbol_contents failed for let name");
                return 0;
            }

            /* print "type name = " */
            fprintf(out, "  %s %s = ", let_str, let_name);

            /* expression */
            expr = let_expr->expr;

            if (!ic_b2c_compile_expr(input_kludge, expr, out)) {
                puts("ic_b2c_compile_stmt_let: call to ic_b2c_compile_expr failed");
                return 0;
            }

            /* closing semicolon and trailing \n */
            fputs(";\n", out);

            return 1;
            break;

        case ic_transform_ir_let_type_faccess:
            /* let name::type = foo.bar */
            /* goes to */
            /* c_type_str name = foo->bar */

            let_faccess = &(let->u.faccess);

            let_type = let_faccess->type;

            let_sym = ic_decl_type_name(let_type);
            if (!let_sym) {
                puts("ic_b2c_compile_stmt_let: call to ic_decl_type_name failed");
                return 0;
            }

            let_str = ic_symbol_contents(let_sym);
            if (!let_str) {
                puts("ic_b2c_compile_stmt_let: call to ic_symbol_contents failed for let type");
                return 0;
            }

            let_name = ic_symbol_contents(let_faccess->name);
            if (!let_name) {
                puts("ic_b2c_compile_stmt_let: call to ic_symbol_contents failed for let name");
                return 0;
            }

            /* print "type name = " */
            fprintf(out, "  %s %s = ", let_str, let_name);

            let_sym = let_faccess->left;
            let_str = ic_symbol_contents(let_sym);
            if (!let_str) {
                puts("ic_b2c_compile_stmt_let: call to ic_symbol_contents failed for let type");
                return 0;
            }

            fprintf(out, "%s->", let_str);

            let_sym = let_faccess->right;
            let_str = ic_symbol_contents(let_sym);
            if (!let_str) {
                puts("ic_b2c_compile_stmt_let: call to ic_symbol_contents failed for let type");
                return 0;
            }

            fprintf(out, "%s;\n", let_str);

            return 1;
            break;

        default:
            puts("ic_b2c_compile_stmt_let: impossible let_type");
            return 0;
            break;
    }

    return 0;
}

unsigned int ic_b2c_compile_stmt_assign(struct ic_kludge *input_kludge, struct ic_transform_ir_assign *assign, FILE *out) {
    char *left_str = 0;
    struct ic_symbol *left_sym = 0;
    struct ic_transform_ir_expr *expr = 0;

    if (!input_kludge) {
        puts("ic_b2c_compile_stmt_assign: input_kludge was null");
        return 0;
    }

    if (!assign) {
        puts("ic_b2c_compile_stmt_assign: assign was null");
        return 0;
    }

    if (!out) {
        puts("ic_b2c_compile_stmt_assign: out was null");
        return 0;
    }

    left_sym = assign->left;
    if (!left_sym) {
        puts("ic_b2c_compile_stmt_assign: assign->left was falsey");
        return 0;
    }

    left_str = ic_symbol_contents(left_sym);
    if (!left_str) {
        puts("ic_b2c_compile_stmt_assign: call to ic_symbol_contents failed for let type");
        return 0;
    }

    /* print "name = " */
    fprintf(out, "  %s = ", left_str);

    /* expression */
    expr = assign->right;

    if (!ic_b2c_compile_expr(input_kludge, expr, out)) {
        puts("ic_b2c_compile_stmt_assign: call to ic_b2c_compile_expr failed");
        return 0;
    }

    /* closing semicolon and trailing \n */
    fputs(";\n", out);

    return 1;
}

unsigned int ic_b2c_compile_stmt_if(struct ic_kludge *input_kludge, struct ic_transform_ir_if *tif, FILE *out) {
    struct ic_symbol *cond = 0;
    char *cond_ch = 0;

    if (!input_kludge) {
        puts("ic_b2c_compile_stmt_if: input_kludge was null");
        return 0;
    }

    if (!tif) {
        puts("ic_b2c_compile_stmt_if: tif was null");
        return 0;
    }

    if (!out) {
        puts("ic_b2c_compile_stmt_if: out was null");
        return 0;
    }

    /* transform guarantees us that the cond will always be a name of an
     * in-scope boolean literal which we can just test
     */

    cond = tif->cond;
    if (!cond) {
        puts("ic_b2c_compile_stmt_if: cond on tif was null");
        return 0;
    }

    cond_ch = ic_symbol_contents(cond);
    if (!cond_ch) {
        puts("ic_b2c_compile_stmt_if: call to ic_symbol_contents failed");
        return 0;
    }

    fprintf(out, "  if (%s) {\n", cond_ch);

    if (!tif->then_tbody) {
        puts("ic_b2c_compile_stmt_if: then_tbody on tif was null");
        return 0;
    }
    /* if body */
    if (!ic_b2c_compile_body(input_kludge, tif->then_tbody, out)) {
        puts("ic_b2c_compile_stmt_if: call to ic_b2c_compile_body failed for then_tbody");
        return 0;
    }

    /* optional else */
    if (tif->else_tbody) {
        fputs("} else {\n", out);
        /* else body */
        if (!ic_b2c_compile_body(input_kludge, tif->else_tbody, out)) {
            puts("ic_b2c_compile_stmt_if: call to ic_b2c_compile_body failed for else_tbody");
            return 0;
        }
    }

    fputs("}\n", out);

    return 1;
}

unsigned int ic_b2c_compile_stmt_expr(struct ic_kludge *input_kludge, struct ic_transform_ir_expr *expr, FILE *out) {
    if (!input_kludge) {
        puts("ic_b2c_compile_stmt_expr: input_kludge was null");
        return 0;
    }

    if (!expr) {
        puts("ic_b2c_compile_stmt_expr: expr was null");
        return 0;
    }

    if (!out) {
        puts("ic_b2c_compile_stmt_expr: out was null");
        return 0;
    }

    if (!ic_b2c_compile_expr(input_kludge, expr, out)) {
        puts("ic_b2c_compile_stmt_expr: call to ic_b2c_compile_expr failed");
        return 0;
    }

    /* must close off expr */
    fputs(";\n", out);

    return 1;
}
