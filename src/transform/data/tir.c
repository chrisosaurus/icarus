#include <stdio.h>
#include <stdlib.h>

#include "../../parse/parse.h"
#include "tbody.h"

/* allocate and initialise a new expr
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_expr *ic_transform_ir_expr_new(enum ic_transform_ir_expr_tag tag) {
    struct ic_transform_ir_expr *expr = 0;

    expr = calloc(1, sizeof(struct ic_transform_ir_expr));
    if (!expr) {
        puts("ir_transform_ir_expr_new: call to calloc failed");
        return 0;
    }

    if (!ic_transform_ir_expr_init(expr, tag)) {
        puts("ir_transform_ir_expr_new: call to ic_transform_expr_init failed");
        return 0;
    }

    return expr;
}

/* initialise an existing expr
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_expr_init(struct ic_transform_ir_expr *expr, enum ic_transform_ir_expr_tag tag) {
    if (!expr) {
        puts("ic_transform_ir_expr_init: expr was null");
        return 0;
    }

    expr->tag = tag;

    return 1;
}

/* destroy expr
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free expr if `free_expr` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_expr_destroy(struct ic_transform_ir_expr *expr, unsigned int free_expr) {
    if (!expr) {
        puts("ic_transform_ir_expr_destroy: expr was null");
        return 0;
    }

    if (free_expr) {
        free(expr);
    }

    return 1;
}

/* print expr
 *
 * returns 1 on success
 * return 0 on failure
 */
unsigned int ic_transform_ir_expr_print(FILE *fd, struct ic_transform_ir_expr *expr, unsigned int *indent) {
    if (!expr) {
        puts("ic_transform_ir_expr_print: expr was null");
        return 0;
    }

    if (!indent) {
        puts("ic_transform_ir_expr_print: indent was null");
        return 0;
    }

    switch (expr->tag) {
        case ic_transform_ir_expr_type_literal:
            ic_transform_ir_expr_literal_print(fd, expr->u.literal, indent);
            break;

        case ic_transform_ir_expr_type_var:
            ic_transform_ir_expr_var_print(fd, expr->u.var, indent);
            break;

        case ic_transform_ir_expr_type_faccess:
            ic_transform_ir_expr_faccess_print(fd, expr->u.faccess, indent);
            break;

        case ic_transform_ir_expr_type_fcall:
            ic_transform_ir_expr_fcall_print(fd, expr->u.fcall, indent);
            break;

        default:
            puts("ic_transform_ir_expr_print: impossible tag");
            break;
    }

    return 1;
}

/* allocate and initialise a new expr_literal
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_expr_literal *ic_transform_ir_expr_literal_new(void) {
    struct ic_transform_ir_expr_literal *let = 0;

    let = calloc(1, sizeof(struct ic_transform_ir_expr_literal));
    if (!let) {
        puts("ir_transform_ir_expr_literal_new: call to calloc failed");
        return 0;
    }

    if (!ic_transform_ir_expr_literal_init(let)) {
        puts("ir_transform_ir_expr_literal_new: call to ic_transform_expr_literal_init failed");
        return 0;
    }

    return let;
}

/* initialise an existing expr_literal
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_expr_literal_init(struct ic_transform_ir_expr_literal *let) {
    if (!let) {
        puts("ic_transform_ir_expr_literal_init: let was null");
        return 0;
    }

    let->literal = 0;

    return 1;
}

/* destroy expr_literal
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free let if `free_let` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_expr_literal_destroy(struct ic_transform_ir_expr_literal *let, unsigned int free_let) {
    if (!let) {
        puts("ic_transform_ir_let_expr_literal_destroy: let was null");
        return 0;
    }

    if (free_let) {
        free(let);
    }

    return 1;
}

/* print expr_literal
 *
 * returns 1 on success
 * return 0 on failure
 */
unsigned int ic_transform_ir_expr_literal_print(FILE *fd, struct ic_transform_ir_expr_literal *literal, unsigned int *indent) {
    unsigned int fake_indent = 0;

    if (!literal) {
        puts("ic_transform_ir_expr_literal_print: literal was null");
        return 0;
    }

    if (!indent) {
        puts("ic_transform_ir_expr_literal_print: indent was null");
        return 0;
    }

    if (!literal->literal) {
        puts("ic_transform_ir_expr_literal_print: this let is not ready for printing - has null fields");
        return 0;
    }

    /* literal */
    ic_expr_constant_print(fd, literal->literal, &fake_indent);

    return 1;
}

/* allocate and initialise a new expr_var
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_expr_var *ic_transform_ir_expr_var_new(void) {
    struct ic_transform_ir_expr_var *var = 0;

    var = calloc(1, sizeof(struct ic_transform_ir_expr_var));
    if (!var) {
        puts("ic_transform_ir_expr_var_new: call to calloc failed");
        return 0;
    }

    if (!ic_transform_ir_expr_var_init(var)) {
        puts("ic_transform_ir_expr_var_new: call to ic)transform_ir_expr_var_init failed");
        return 0;
    }

    return var;
}

/* initialise an existing expr_var
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_expr_var_init(struct ic_transform_ir_expr_var *var) {
    if (!var) {
        puts("ic_transform_ir_expr_var_init: var was null");
        return 0;
    }

    var->sym = 0;

    return 1;
}

/* destroy expr_var
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free let if `free_let` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_expr_var_destroy(struct ic_transform_ir_expr_var *var, unsigned int free_var) {
    if (!var) {
        puts("ic_transform_ir_expr_var_destroy: var was null");
        return 0;
    }

    if (free_var) {
        free(var);
    }

    return 1;
}

/* print expr_var
 *
 * returns 1 on success
 * return 0 on failure
 */
unsigned int ic_transform_ir_expr_var_print(FILE *fd, struct ic_transform_ir_expr_var *var, unsigned int *indent) {

    if (!fd) {
        puts("ic_transform_ir_expr_var_print: fd was null");
        return 0;
    }

    if (!var) {
        puts("ic_transform_ir_expr_var_print: var was null");
        return 0;
    }

    if (!indent) {
        puts("ic_transform_ir_expr_var_print: indent was null");
        return 0;
    }

    if (!var->sym) {
        puts("ic_transform_ir_expr_var_print: var->sym was null");
        return 0;
    }

    ic_parse_print_indent(fd, *indent);

    ic_symbol_print(fd, var->sym);

    return 1;
}

/* allocate and initialise a new expr_faccess
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_expr_faccess *ic_transform_ir_expr_faccess_new(void) {
    struct ic_transform_ir_expr_faccess *let = 0;

    let = calloc(1, sizeof(struct ic_transform_ir_expr_faccess));
    if (!let) {
        puts("ir_transform_ir_expr_faccess_new: call to calloc failed");
        return 0;
    }

    if (!ic_transform_ir_expr_faccess_init(let)) {
        puts("ir_transform_ir_expr_faccess_new: call to ic_transform_let_expr_init failed");
        return 0;
    }

    return let;
}

/* initialise an existing expr_faccess
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_expr_faccess_init(struct ic_transform_ir_expr_faccess *faccess) {
    if (!faccess) {
        puts("ic_transform_ir_expr_faccess_init: faccess was null");
        return 0;
    }

    faccess->left = 0;
    faccess->right = 0;

    return 1;
}

/* destroy expr_faccess
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free let if `free_let` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_expr_faccess_destroy(struct ic_transform_ir_expr_faccess *faccess, unsigned int free_faccess) {
    if (!faccess) {
        puts("ic_transform_ir_expr_faccess_destroy: faccess was null");
        return 0;
    }

    if (free_faccess) {
        free(faccess);
    }

    return 1;
}

/* print expr_faccess
 *
 * returns 1 on success
 * return 0 on failure
 */
unsigned int ic_transform_ir_expr_faccess_print(FILE *fd, struct ic_transform_ir_expr_faccess *faccess, unsigned int *indent) {
    if (!faccess) {
        puts("ic_transform_ir_expr_faccess_print: faccess was null");
        return 0;
    }

    if (!indent) {
        puts("ic_transform_ir_expr_faccess_print: indent was null");
        return 0;
    }

    if (!faccess->left || !faccess->right) {
        puts("ic_transform_ir_expr_faccess_print: this let is not ready for printing - has null fields");
        return 0;
    }

    ic_parse_print_indent(fd, *indent);

    fputs("let ", fd);

    ic_symbol_print(fd, faccess->left);

    fputs(".", fd);

    ic_symbol_print(fd, faccess->right);

    /* trailing \n */
    fputs("\n", fd);

    return 1;
}

/* allocate and initialise a new let
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_let *ic_transform_ir_let_new(void) {
    struct ic_transform_ir_let *let = 0;

    let = calloc(1, sizeof(struct ic_transform_ir_let));
    if (!let) {
        puts("ir_transform_ir_let_new: call to calloc failed");
        return 0;
    }

    if (!ic_transform_ir_let_init(let)) {
        puts("ir_transform_ir_let_new: call to ic_transform_let_init failed");
        return 0;
    }

    return let;
}

/* initialise an existing let
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_let_init(struct ic_transform_ir_let *let) {
    if (!let) {
        puts("ic_transform_ir_let_init: let was null");
        return 0;
    }

    return 1;
}

/* destroy let
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free let if `free_let` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_let_destroy(struct ic_transform_ir_let *let, unsigned int free_let) {
    if (!let) {
        puts("ic_transform_ir_let_destroy: let was null");
        return 0;
    }

    if (free_let) {
        free(let);
    }

    return 1;
}

/* print let
 *
 * returns 1 on success
 * return 0 on failure
 */
unsigned int ic_transform_ir_let_print(FILE *fd, struct ic_transform_ir_let *let, unsigned int *indent) {
    unsigned int fake_indent = 0;
    struct ic_symbol *type_full_name = 0;
    char *type_full_name_ch = 0;

    if (!let) {
        puts("ic_transform_ir_let_print: let was null");
        return 0;
    }

    if (!indent) {
        puts("ic_transform_ir_let_print: indent was null");
        return 0;
    }

    ic_parse_print_indent(fd, *indent);

    fputs("let ", fd);

    /* identifier name */
    ic_symbol_print(fd, let->name);

    /* type name */
    type_full_name = ic_decl_type_full_name(let->type);
    if (!type_full_name) {
        puts("ic_transform_ir_let_expr_print: call to ic_decl_type_full_name failed");
        return 0;
    }

    type_full_name_ch = ic_symbol_contents(type_full_name);
    if (!type_full_name_ch) {
        puts("ic_transform_ir_let_expr_print: call to ic_symbol_contents failed");
        return 0;
    }

    fprintf(fd, "::%s = ", type_full_name_ch);

    /* expr */
    if (!ic_transform_ir_expr_print(fd, let->expr, &fake_indent)) {
        puts("ic_transform_ir_let_expr_print: call to ic_transform_ir_expr_print failed");
        return 0;
    }

    /* trailing \n */
    fputs("\n", fd);

    return 1;
}

/* get pointer to internal expr
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_expr *ic_transform_ir_let_get_expr(struct ic_transform_ir_let *let) {
    if (!let) {
        puts("ic_transform_ir_let_get_expr: let was null");
        return 0;
    }

    return let->expr;
}

/* allocate and initialise a new assign
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_assign *ic_transform_ir_assign_new(void) {
    struct ic_transform_ir_assign *assign = 0;

    assign = calloc(1, sizeof(struct ic_transform_ir_assign));
    if (!assign) {
        puts("ir_transform_ir_assign_new: call to calloc failed");
        return 0;
    }

    if (!ic_transform_ir_assign_init(assign)) {
        puts("ir_transform_ir_assign_new: call to ic_transform_assign_init failed");
        return 0;
    }

    return assign;
}

/* initialise an existing assign
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_assign_init(struct ic_transform_ir_assign *assign) {
    if (!assign) {
        puts("ic_transform_ir_let_assign_init: assign was null");
        return 0;
    }

    assign->left = 0;
    assign->right = 0;

    return 1;
}

/* destroy assign
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free assign if `free_assign` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_assign_destroy(struct ic_transform_ir_assign *assign, unsigned int free_assign) {
    if (!assign) {
        puts("ic_transform_ir_let_assign_destroy: assign was null");
        return 0;
    }

    if (free_assign) {
        free(assign);
    }

    return 1;
}

/* print assign
 *
 * returns 1 on success
 * return 0 on failure
 */
unsigned int ic_transform_ir_assign_print(FILE *fd, struct ic_transform_ir_assign *assign, unsigned int *indent) {
    unsigned int fake_indent = 0;

    if (!assign) {
        puts("ic_transform_ir_assign_print: assign was null");
        return 0;
    }

    if (!indent) {
        puts("ic_transform_ir_assign_print: indent was null");
        return 0;
    }

    if (!assign->left || !assign->right) {
        puts("ic_transform_ir_assign_print: this assign is not ready for printing - has null fields");
        return 0;
    }

    ic_parse_print_indent(fd, *indent);

    /* left symbol */
    ic_symbol_print(fd, assign->left);

    fputs(" = ", fd);

    /* right symbol */
    if (!ic_transform_ir_expr_print(fd, assign->right, &fake_indent)) {
        puts("ic_transform_ir_assign_print: call to ic_transform_ir_expr_print failed");
        return 0;
    }

    /* trailing \n */
    fputs("\n", fd);

    return 1;
}

/* allocate and initialise a new begin
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_begin *ic_transform_ir_begin_new(void) {
    struct ic_transform_ir_begin *begin = 0;

    begin = calloc(1, sizeof(struct ic_transform_ir_begin));
    if (!begin) {
        puts("ir_transform_ir_begin_new: call to calloc failed");
        return 0;
    }

    if (!ic_transform_ir_begin_init(begin)) {
        puts("ir_transform_ir_begin_new: call to ic_transform_begin_init failed");
        free(begin);
        return 0;
    }

    return begin;
}

/* initialise an existing begin
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_begin_init(struct ic_transform_ir_begin *begin) {
    if (!begin) {
        puts("ic_transform_ir_begin_init: begin was null");
        return 0;
    }

    begin->tbody = 0;

    return 1;
}

/* destroy begin
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free assign begin `free_begin` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_begin_destroy(struct ic_transform_ir_begin *begin, unsigned int free_begin) {
    if (!begin) {
        puts("ic_transform_ir_let_begin_destroy: begin was null");
        return 0;
    }

    if (free_begin) {
        free(begin);
    }

    return 1;
}

/* print begin
 *
 * returns 1 on success
 * return 0 on failure
 */
unsigned int ic_transform_ir_begin_print(FILE *fd, struct ic_transform_ir_begin *begin, unsigned int *indent) {
    if (!begin) {
        puts("ic_transform_ir_begin_print: begin was null");
        return 0;
    }

    if (!indent) {
        puts("ic_transform_ir_begin_print: indent was null");
        return 0;
    }

    ic_parse_print_indent(fd, *indent);

    fputs("begin\n", fd);

    *indent += 1;

    /* body */
    if (!ic_transform_body_print(fd, begin->tbody, indent)) {
        puts("ic_transform_ir_begin_print: call to ic_transform_body_print failed");
        return 0;
    }

    *indent -= 1;

    /* trailing end and \n */
    ic_parse_print_indent(fd, *indent);
    fputs("end\n", fd);

    return 1;
}

/* allocate and initialise a new if
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_if *ic_transform_ir_if_new(struct ic_symbol *cond_sym) {
    struct ic_transform_ir_if *tif = 0;

    if (!cond_sym) {
        puts("ir_transform_ir_if_new: cond_sym was null");
        return 0;
    }

    tif = calloc(1, sizeof(struct ic_transform_ir_if));
    if (!tif) {
        puts("ir_transform_ir_if_new: call to calloc failed");
        return 0;
    }

    if (!ic_transform_ir_if_init(tif, cond_sym)) {
        puts("ir_transform_ir_if_new: call to ic_transform_if_init failed");
        free(tif);
        return 0;
    }

    return tif;
}

/* initialise an existing if
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_if_init(struct ic_transform_ir_if *tif, struct ic_symbol *cond_sym) {
    if (!tif) {
        puts("ic_transform_ir_let_if_init: if was null");
        return 0;
    }

    if (!cond_sym) {
        puts("ic_transform_ir_let_if_init: cond_sym was null");
        return 0;
    }

    tif->cond = cond_sym;
    tif->then_tbody = 0;
    tif->else_tbody = 0;

    return 1;
}

/* destroy if
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free if if `free_if` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_if_destroy(struct ic_transform_ir_if *tif, unsigned int free_if) {
    if (!tif) {
        puts("ic_transform_ir_let_if_destroy: if was null");
        return 0;
    }

    if (free_if) {
        free(tif);
    }

    return 1;
}

/* print if
 *
 * returns 1 on success
 * return 0 on failure
 */
unsigned int ic_transform_ir_if_print(FILE *fd, struct ic_transform_ir_if *tif, unsigned int *indent) {
    if (!tif) {
        puts("ic_transform_ir_if_print: if was null");
        return 0;
    }

    if (!indent) {
        puts("ic_transform_ir_if_print: indent was null");
        return 0;
    }

    ic_parse_print_indent(fd, *indent);

    /* if statement */
    fputs("if ", fd);

    /* cond */
    ic_symbol_print(fd, tif->cond);
    fputs("\n", fd);

    *indent += 1;

    /* body */
    if (!ic_transform_body_print(fd, tif->then_tbody, indent)) {
        puts("ic_transform_ir_if_print: call to ic_transform_body_print failed");
        return 0;
    }

    *indent -= 1;

    /* if we have an else clause */
    if (tif->else_tbody) {
        ic_parse_print_indent(fd, *indent);
        *indent += 1;
        puts("else");
        if (!ic_transform_body_print(fd, tif->else_tbody, indent)) {
            puts("ic_transform_ir_if_print: call to ic_transform_body_print failed");
            return 0;
        }
        *indent -= 1;
    }

    /* trailing end and \n */
    ic_parse_print_indent(fd, *indent);
    fputs("end\n", fd);

    return 1;
}

/* allocate and initialise a new ret
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_ret *ic_transform_ir_ret_new(void) {
    struct ic_transform_ir_ret *ret = 0;

    ret = calloc(1, sizeof(struct ic_transform_ir_ret));
    if (!ret) {
        puts("ir_transform_ir_ret_new: call to calloc failed");
        return 0;
    }

    if (!ic_transform_ir_ret_init(ret)) {
        puts("ir_transform_ir_ret_new: call to ic_transform_ret_init failed");
        return 0;
    }

    return ret;
}

/* initialise an existing ret
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_ret_init(struct ic_transform_ir_ret *ret) {
    if (!ret) {
        puts("ic_transform_ir_ret_init: ret was null");
        return 0;
    }

    ret->var = 0;

    return 1;
}

/* destroy ret
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free ret if `free_ret` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_ret_destroy(struct ic_transform_ir_ret *ret, unsigned int free_ret) {
    if (!ret) {
        puts("ic_transform_ir_ret_destroy: ret was null");
        return 0;
    }

    if (free_ret) {
        free(ret);
    }

    return 1;
}

/* print ret
 *
 * returns 1 on success
 * return 0 on failure
 */
unsigned int ic_transform_ir_ret_print(FILE *fd, struct ic_transform_ir_ret *ret, unsigned int *indent) {
    if (!ret) {
        puts("ic_transform_ir_ret_print: ret was null");
        return 0;
    }

    if (!indent) {
        puts("ic_transform_ir_ret_print: indent was null");
        return 0;
    }

    ic_parse_print_indent(fd, *indent);

    fputs("return", fd);

    /* print symbol if it's set */
    if (ret->var) {
        fputs(" ", fd);
        ic_symbol_print(fd, ret->var);
    }

    /* trailing \n */
    fputs("\n", fd);

    return 1;
}

/* allocate and initialise a new fcall
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_expr_fcall *ic_transform_ir_expr_fcall_new(struct ic_expr_func_call *fcall, struct ic_pvector *args) {
    struct ic_transform_ir_expr_fcall *tir_expr_fcall = 0;

    if (!fcall) {
        puts("ir_transform_ir_expr_fcall_new: fcall was null");
        return 0;
    }

    if (!args) {
        puts("ir_transform_ir_expr_fcall_new: args was null");
        return 0;
    }

    tir_expr_fcall = calloc(1, sizeof(struct ic_transform_ir_expr_fcall));
    if (!tir_expr_fcall) {
        puts("ir_transform_ir_expr_fcall_new: call to calloc failed");
        return 0;
    }

    if (!ic_transform_ir_expr_fcall_init(tir_expr_fcall, fcall, args)) {
        puts("ir_transform_ir_expr_fcall_new: call to ic_transform_fcall_init failed");
        return 0;
    }

    return tir_expr_fcall;
}

/* initialise an existing fcall
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_expr_fcall_init(struct ic_transform_ir_expr_fcall *tir_expr_fcall, struct ic_expr_func_call *fcall, struct ic_pvector *args) {
    if (!tir_expr_fcall) {
        puts("ic_transform_ir_expr_fcall_init: tir_expr_fcall was null");
        return 0;
    }

    if (!fcall) {
        puts("ic_transform_ir_expr_fcall_init: fcall was null");
        return 0;
    }

    if (!args) {
        puts("ic_transform_ir_expr_fcall_init: args was null");
        return 0;
    }

    tir_expr_fcall->fcall = fcall;
    tir_expr_fcall->args = args;

    return 1;
}

/* destroy fcall
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free fcall if `free_fcall` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_expr_fcall_destroy(struct ic_transform_ir_expr_fcall *fcall, unsigned int free_fcall) {
    if (!fcall) {
        puts("ic_transform_ir_expr_fcall_destroy: fcall was null");
        return 0;
    }

    if (free_fcall) {
        free(fcall);
    }

    return 1;
}

/* print fcall
 *
 * fcallurns 1 on success
 * fcallurn 0 on failure
 */
unsigned int ic_transform_ir_expr_fcall_print(FILE *fd, struct ic_transform_ir_expr_fcall *fcall, unsigned int *indent) {
    /* offset into fcall args */
    unsigned int i = 0;
    /* length of fcall args */
    unsigned int len = 0;
    /* current arg */
    struct ic_symbol *arg = 0;

    if (!fcall) {
        puts("ic_transform_ir_expr_fcall_print: fcall was null");
        return 0;
    }

    if (!indent) {
        puts("ic_transform_ir_expr_fcall_print: indent was null");
        return 0;
    }

    /* function name and indent */
    ic_expr_print(fd, fcall->fcall->fname, indent);

    /* opening ( */
    fputs("(", fd);

    /* args */
    len = ic_transform_ir_expr_fcall_length(fcall);

    for (i = 0; i < len; ++i) {
        arg = ic_transform_ir_expr_fcall_get_arg(fcall, i);
        if (!arg) {
            puts("ic_transform_ir_expr_fcall_print: call to ic_transform_ir_expr_fcall_get_arg failed");
            return 0;
        }

        if (i > 0) {
            /* comma and space to sep. args */
            fputs(", ", fd);
        }

        /* print arg */
        ic_symbol_print(fd, arg);
    }

    /* closing ) */
    fputs(")", fd);

    return 1;
}

/* get number of args
 *
 * returns number on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_expr_fcall_length(struct ic_transform_ir_expr_fcall *fcall) {
    unsigned len = 0;

    if (!fcall) {
        puts("ic_transform_ir_expr_fcall_length: fcall was null");
        return 0;
    }

    len = ic_pvector_length(fcall->args);

    return len;
}

/* get arg at offset i
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_symbol *ic_transform_ir_expr_fcall_get_arg(struct ic_transform_ir_expr_fcall *fcall, unsigned int i) {
    struct ic_symbol *sym = 0;

    if (!fcall) {
        puts("ic_transform_ir_expr_fcall_get_arg: fcall was null");
        return 0;
    }

    sym = ic_pvector_get(fcall->args, i);
    if (!sym) {
        puts("ic_transform_ir_expr_fcall_get_arg: call to ic_pvector_get failed");
        return 0;
    }

    return sym;
}

/* allocate and initialise a new case
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_match_case *ic_transform_ir_match_case_new(struct ic_field *field) {
    struct ic_transform_ir_match_case *scase = 0;

    if (!field) {
        puts("ic_transform_ir_match_case_new: field was null");
        return 0;
    }

    scase = calloc(1, sizeof(struct ic_transform_ir_match_case));
    if (!scase) {
        puts("ic_transform_ir_match_case_new: call to calloc failed");
        return 0;
    }

    if (!ic_transform_ir_match_case_init(scase, field)) {
        puts("ic_transform_ir_match_case_new: call to ic_transform_ir_match_case_init failed");
        return 0;
    }

    return scase;
}

/* initialise an existing case
 *
 * returns 1 on success
 * return 0 on failure
 */
unsigned int ic_transform_ir_match_case_init(struct ic_transform_ir_match_case *scase, struct ic_field *field) {
    if (!scase) {
        puts("ic_transform_ir_match_case_init: scase was null");
        return 0;
    }

    if (!field) {
        puts("ic_transform_ir_match_case_init: field was null");
        return 0;
    }

    scase->field = field;
    scase->tbody = 0;

    return 1;
}

/* destroy case
 *
 * TODO doesn't touch any fields
 *
 * will only free case if `free_case` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_match_case_destroy(struct ic_transform_ir_match_case *scase, unsigned int free_case) {
    if (!scase) {
        puts("ic_transform_ir_match_case_destroy: scase was null");
        return 0;
    }

    if (free_case) {
        free(scase);
    }

    return 1;
}

/* print case
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_match_case_print(FILE *fd, struct ic_transform_ir_match_case *scase, unsigned int *indent) {
    if (!fd) {
        puts("ic_transform_ir_match_case_print: fd was null");
        return 0;
    }

    if (!scase) {
        puts("ic_transform_ir_match_case_print: scase was null");
        return 0;
    }

    if (!indent) {
        puts("ic_transform_ir_match_case_print: indent was null");
        return 0;
    }

    ic_parse_print_indent(fd, *indent);
    fputs("case ", fd);

    /* field */
    ic_field_print(fd, scase->field);

    fputs("\n", fd);

    *indent += 1;

    if (!ic_transform_body_print(fd, scase->tbody, indent)) {
        puts("ic_transform_ir_match_case_print: call to ic_transform_body_print failed");
        return 0;
    }

    *indent -= 1;

    return 1;
}

/* allocate and initialise a new match
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_match *ic_transform_ir_match_new(struct ic_decl_type *tdecl, struct ic_symbol *match_symbol) {
    struct ic_transform_ir_match *match = 0;

    if (!tdecl) {
        puts("ic_transform_ir_match_new: tdecl was null");
        return 0;
    }

    if (!match_symbol) {
        puts("ic_transform_ir_match_new: match_symbol was null");
        return 0;
    }

    match = calloc(1, sizeof(struct ic_transform_ir_match));
    if (!match) {
        puts("ic_transform_ir_match_new: call to calloc failed");
        return 0;
    }

    if (!ic_transform_ir_match_init(match, tdecl, match_symbol)) {
        puts("ic_transform_ir_match_new: call to ic_transform_ir_match_init failed");
        return 0;
    }

    return match;
}

/* initialise an existing match
 *
 * returns 1 on success
 * return 0 on failure
 */
unsigned int ic_transform_ir_match_init(struct ic_transform_ir_match *match, struct ic_decl_type *tdecl, struct ic_symbol *match_symbol) {

    if (!match) {
        puts("ic_transform_ir_match_init: scase was null");
        return 0;
    }

    if (!tdecl) {
        puts("ic_transform_ir_match_init: match_symbol was null");
        return 0;
    }

    if (!match_symbol) {
        puts("ic_transform_ir_match_init: match_symbol was null");
        return 0;
    }

    match->tdecl = tdecl;
    match->match_symbol = match_symbol;
    match->else_body = 0;

    if (!ic_pvector_init(&(match->cases), 0)) {
        puts("ic_transform_ir_match_init: call to ic_pvector_init failed");
        return 0;
    }

    return 1;
}

/* destroy match
 *
 * TODO doesn't touch any fields
 *
 * will only free match if `free_match` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_match_destroy(struct ic_transform_ir_match *match, unsigned int free_match) {
    if (!match) {
        puts("ic_transform_ir_match_destroy: scase was null");
        return 0;
    }

    if (free_match) {
        free(match);
    }

    return 1;
}

/* print match
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_match_print(FILE *fd, struct ic_transform_ir_match *match, unsigned int *indent) {
    unsigned int i = 0;
    unsigned int len = 0;
    struct ic_transform_ir_match_case *scase = 0;

    if (!fd) {
        puts("ic_transform_ir_match_print: fd was null");
        return 0;
    }

    if (!match) {
        puts("ic_transform_ir_match_print: match was null");
        return 0;
    }

    if (!indent) {
        puts("ic_transform_ir_match_print: indent was null");
        return 0;
    }

    ic_parse_print_indent(fd, *indent);
    fputs("match ", fd);

    /* identifier name */
    ic_symbol_print(fd, match->match_symbol);
    fputs("\n", fd);

    *indent += 1;

    len = ic_pvector_length(&(match->cases));
    for (i = 0; i < len; ++i) {
        scase = ic_pvector_get(&(match->cases), i);
        if (!scase) {
            puts("ic_transform_ir_match_print: call to ic_pvector_get failed");
            return 0;
        }

        if (!ic_transform_ir_match_case_print(fd, scase, indent)) {
            puts("ic_transform_ir_match_print: call to ic_transform_ir_match_case_print failed");
            return 0;
        }
    }

    if (match->else_body) {
        ic_parse_print_indent(fd, *indent);
        fputs("else\n", fd);
        *indent += 1;
        if (!ic_transform_body_print(fd, match->else_body, indent)) {
            puts("ic_transform_ir_match_print: call to ic_transform_body_print failed");
            return 0;
        }
        *indent -= 1;
    }

    ic_parse_print_indent(fd, *indent);
    fputs("end\n", fd);
    *indent -= 1;

    ic_parse_print_indent(fd, *indent);
    fputs("end\n", fd);

    return 1;
}

/* allocate and initialise a new stmt
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_transform_ir_stmt *ic_transform_ir_stmt_new(enum ic_transform_ir_stmt_tag tag) {
    struct ic_transform_ir_stmt *stmt = 0;

    stmt = calloc(1, sizeof(struct ic_transform_ir_stmt));
    if (!stmt) {
        puts("ir_transform_ir_stmt_new: call to calloc failed");
        return 0;
    }

    if (!ic_transform_ir_stmt_init(stmt, tag)) {
        puts("ir_transform_ir_stmt_new: call to ic_transform_stmt_init failed");
        return 0;
    }

    return stmt;
}

/* initialise an existing stmt
 *
 * TODO doesn't touch any of the contained elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_stmt_init(struct ic_transform_ir_stmt *stmt, enum ic_transform_ir_stmt_tag tag) {
    if (!stmt) {
        puts("ic_transform_ir_stmt_init: stmt was null");
        return 0;
    }

    stmt->tag = tag;

    return 1;
}

/* destroy stmt
 *
 * TODO doesn't touch any of the contained elements
 *
 * will only free stmt if `free_stmt` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_ir_stmt_destroy(struct ic_transform_ir_stmt *stmt, unsigned int free_stmt) {
    if (!stmt) {
        puts("ic_transform_ir_stmt_destroy: stmt was null");
        return 0;
    }

    if (free_stmt) {
        free(stmt);
    }

    return 1;
}

/* print stmt
 *
 * returns 1 on success
 * return 0 on failure
 */
unsigned int ic_transform_ir_stmt_print(FILE *fd, struct ic_transform_ir_stmt *stmt, unsigned int *indent) {
    if (!stmt) {
        puts("ic_transform_ir_stmt_print: stmt was null");
        return 0;
    }

    if (!indent) {
        puts("ic_transform_ir_stmt_print: indent was null");
        return 0;
    }

    switch (stmt->tag) {
        case ic_transform_ir_stmt_type_fcall:
            if (!ic_transform_ir_expr_fcall_print(fd, stmt->u.fcall, indent)) {
                puts("ic_transform_ir_stmt_print: call to ic_transform_ir_expr_fcall_print failed");
                return 0;
            }
            /* trailing \n after expr */
            puts("");
            break;

        case ic_transform_ir_stmt_type_let:
            if (!ic_transform_ir_let_print(fd, &(stmt->u.let), indent)) {
                puts("ic_transform_ir_stmt_print: call to ic_transform_ir_let_print failed");
                return 0;
            }
            break;

        case ic_transform_ir_stmt_type_ret:
            if (!ic_transform_ir_ret_print(fd, &(stmt->u.ret), indent)) {
                puts("ic_transform_ir_stmt_print: call to ic_transform_ir_ret_print failed");
                return 0;
            }
            break;

        case ic_transform_ir_stmt_type_begin:
            if (!ic_transform_ir_begin_print(fd, &(stmt->u.begin), indent)) {
                puts("ic_transform_ir_stmt_print: call to ic_transform_ir_begin_print failed");
                return 0;
            }
            break;

        case ic_transform_ir_stmt_type_if:
            if (!ic_transform_ir_if_print(fd, &(stmt->u.sif), indent)) {
                puts("ic_transform_ir_stmt_print: call to ic_transform_ir_if_print failed");
                return 0;
            }
            break;

        case ic_transform_ir_stmt_type_assign:
            if (!ic_transform_ir_assign_print(fd, &(stmt->u.assign), indent)) {
                puts("ic_transform_ir_stmt_print: call to ic_transform_ir_assign_print failed");
                return 0;
            }
            break;

        case ic_transform_ir_stmt_type_match:
            if (!ic_transform_ir_match_print(fd, &(stmt->u.match), indent)) {
                puts("ic_transform_ir_stmt_print: call to ic_transform_ir_assign_print failed");
                return 0;
            }
            break;

        default:
            puts("ic_transform_ir_stmt_print: impossible stmt->tag");
            return 0;
            break;
    }

    return 1;
}

/* get pointer to internal expr
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_expr_fcall *ic_transform_ir_stmt_get_fcall(struct ic_transform_ir_stmt *stmt) {
    if (!stmt) {
        puts("ic_transform_ir_stmt_get_expr_fcall: stmt was null");
        return 0;
    }

    if (stmt->tag != ic_transform_ir_stmt_type_fcall) {
        puts("ic_transform_ir_stmt_get_expr_fcall: stmt was not of type expr");
        return 0;
    }

    return stmt->u.fcall;
}

/* get pointer to internal let
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_let *ic_transform_ir_stmt_get_let(struct ic_transform_ir_stmt *stmt) {
    if (!stmt) {
        puts("ic_transform_ir_stmt_get_let: stmt was null");
        return 0;
    }

    if (stmt->tag != ic_transform_ir_stmt_type_let) {
        puts("ic_transform_ir_stmt_get_let: stmt was not of type let");
        return 0;
    }

    return &(stmt->u.let);
}

/* get pointer to internal ret
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_ret *ic_transform_ir_stmt_get_ret(struct ic_transform_ir_stmt *stmt) {
    if (!stmt) {
        puts("ic_transform_ir_stmt_get_ret: stmt was null");
        return 0;
    }

    if (stmt->tag != ic_transform_ir_stmt_type_ret) {
        puts("ic_transform_ir_stmt_get_ret: stmt was not of type ret");
        return 0;
    }

    return &(stmt->u.ret);
}

/* get pointer to internal begin
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_begin *ic_transform_ir_stmt_get_begin(struct ic_transform_ir_stmt *stmt) {
    if (!stmt) {
        puts("ic_transform_ir_stmt_get_begin: stmt was null");
        return 0;
    }

    if (stmt->tag != ic_transform_ir_stmt_type_begin) {
        puts("ic_transform_ir_stmt_get_begin: stmt was not of type begin");
        return 0;
    }

    return &(stmt->u.begin);
}

/* get pointer to internal if
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_if *ic_transform_ir_stmt_get_if(struct ic_transform_ir_stmt *stmt) {
    if (!stmt) {
        puts("ic_transform_ir_stmt_get_if: stmt was null");
        return 0;
    }

    if (stmt->tag != ic_transform_ir_stmt_type_if) {
        puts("ic_transform_ir_stmt_get_if: stmt was not of type if");
        return 0;
    }

    return &(stmt->u.sif);
}

/* get pointer to internal match
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_match *ic_transform_ir_stmt_get_match(struct ic_transform_ir_stmt *stmt) {
    if (!stmt) {
        puts("ic_transform_ir_stmt_get_match: stmt was null");
        return 0;
    }

    if (stmt->tag != ic_transform_ir_stmt_type_match) {
        puts("ic_transform_ir_stmt_get_match: stmt was not of type match");
        return 0;
    }

    return &(stmt->u.match);
}

/* get pointer to internal assign
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_assign *ic_transform_ir_stmt_get_assign(struct ic_transform_ir_stmt *stmt) {
    if (!stmt) {
        puts("ic_transform_ir_stmt_get_assign: stmt was null");
        return 0;
    }

    if (stmt->tag != ic_transform_ir_stmt_type_assign) {
        puts("ic_transform_ir_stmt_get_assign: stmt was not of type assign");
        return 0;
    }

    return &(stmt->u.assign);
}

/* allocate and initialise a new stmt->let->literal
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_stmt *ic_transform_ir_stmt_let_literal_new(struct ic_symbol *name, struct ic_decl_type *type, struct ic_expr_constant *literal) {
    struct ic_transform_ir_stmt *stmt = 0;

    if (!name) {
        puts("ic_transform_ir_stmt_let_literal_new: name was null");
        return 0;
    }

    if (!type) {
        puts("ic_transform_ir_stmt_let_literal_new: type was null");
        return 0;
    }

    if (!literal) {
        puts("ic_transform_ir_stmt_let_literal_new: literal was null");
        return 0;
    }

    stmt = ic_transform_ir_stmt_new(ic_transform_ir_stmt_type_let);
    if (!stmt) {
        puts("ic_transform_ir_stmt_let_literal_new: call to ic_transform_ir_stmt_new failed");
        return 0;
    }

    if (!ic_transform_ir_let_init(&(stmt->u.let))) {
        puts("ic_transform_ir_stmt_let_literal_new: call to ic_transform_ir_let_init failed");
        return 0;
    }

    stmt->u.let.name = name;
    stmt->u.let.type = type;

    stmt->u.let.expr = ic_transform_ir_expr_new(ic_transform_ir_expr_type_literal);
    if (!stmt->u.let.expr) {
        puts("ic_transform_ir_stmt_let_literal_new: call to ic_transform_ir_expr_new failed");
        return 0;
    }

    stmt->u.let.expr->u.literal = ic_transform_ir_expr_literal_new();
    if (!stmt->u.let.expr->u.literal) {
        puts("ic_transform_ir_stmt_let_literal_new: call to ic_transform_ir_expr_literal_new failed");
        return 0;
    }

    stmt->u.let.expr->u.literal->literal = literal;

    return stmt;
}

/* allocate and initialise a new stmt->let->expr
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_stmt *ic_transform_ir_stmt_let_expr_new(struct ic_symbol *name, struct ic_decl_type *type, struct ic_transform_ir_expr *expr) {
    struct ic_transform_ir_stmt *stmt = 0;

    if (!name) {
        puts("ic_transform_ir_stmt_let_expr_new: name was null");
        return 0;
    }

    if (!type) {
        puts("ic_transform_ir_stmt_let_expr_new: type was null");
        return 0;
    }

    if (!expr) {
        puts("ic_transform_ir_stmt_let_expr_new: expr was null");
        return 0;
    }

    stmt = ic_transform_ir_stmt_new(ic_transform_ir_stmt_type_let);
    if (!stmt) {
        puts("ic_transform_ir_stmt_let_expr_new: call to ic_transform_ir_stmt_new failed");
        return 0;
    }

    if (!ic_transform_ir_let_init(&(stmt->u.let))) {
        puts("ic_transform_ir_stmt_let_expr_new: call to ic_transform_ir_let_init failed");
        return 0;
    }

    stmt->u.let.name = name;
    stmt->u.let.type = type;
    stmt->u.let.expr = expr;

    return stmt;
}

/* allocate and initialise a new stmt->let->faccess
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_stmt *ic_transform_ir_stmt_let_faccess_new(struct ic_symbol *name, struct ic_decl_type *type, struct ic_symbol *left, struct ic_decl_type *left_type, struct ic_symbol *right) {
    struct ic_transform_ir_stmt *stmt = 0;

    if (!name) {
        puts("ic_transform_ir_stmt_let_faccess_new: name was null");
        return 0;
    }

    if (!type) {
        puts("ic_transform_ir_stmt_let_faccess_new: type was null");
        return 0;
    }

    if (!left) {
        puts("ic_transform_ir_stmt_let_faccess_new: left was null");
        return 0;
    }

    if (!left_type) {
        puts("ic_transform_ir_stmt_let_faccess_new: left_type was null");
        return 0;
    }

    if (!right) {
        puts("ic_transform_ir_stmt_let_faccess_new: right was null");
        return 0;
    }

    stmt = ic_transform_ir_stmt_new(ic_transform_ir_stmt_type_let);
    if (!stmt) {
        puts("ic_transform_ir_stmt_let_faccess_new: call to ic_transform_ir_stmt_new failed");
        return 0;
    }

    if (!ic_transform_ir_let_init(&(stmt->u.let))) {
        puts("ic_transform_ir_stmt_let_faccess_new: call to ic_transform_ir_let_init failed");
        return 0;
    }

    stmt->u.let.name = name;
    stmt->u.let.type = type;

    stmt->u.let.expr = ic_transform_ir_expr_new(ic_transform_ir_expr_type_faccess);
    if (!stmt->u.let.expr) {
        puts("ic_transform_ir_stmt_let_faccess_new: call to ic_transform_ir_expr_new failed");
        return 0;
    }

    stmt->u.let.expr->u.faccess = ic_transform_ir_expr_faccess_new();
    if (!stmt->u.let.expr->u.faccess) {
        puts("ic_transform_ir_stmt_let_faccess_new: call to ic_transform_ir_expr_literal_new failed");
        return 0;
    }

    stmt->u.let.expr->u.faccess->left = left;
    stmt->u.let.expr->u.faccess->left_type = left_type;
    stmt->u.let.expr->u.faccess->right = right;

    return stmt;
}

/* allocate and initialise a new stmt->ret
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_stmt *ic_transform_ir_stmt_ret_new(struct ic_symbol *var) {
    struct ic_transform_ir_stmt *stmt = 0;

    if (!var) {
        puts("ic_transform_ir_stmt_ret_new: var was null");
        return 0;
    }

    stmt = ic_transform_ir_stmt_new(ic_transform_ir_stmt_type_ret);
    if (!stmt) {
        puts("ic_transform_ir_stmt_ret_new: call to ic_transform_ir_stmt_new failed");
        return 0;
    }

    if (!ic_transform_ir_ret_init(&(stmt->u.ret))) {
        puts("ic_transform_ir_stmt_let_expr_new: call to ic_transform_ir_ret_init failed");
        return 0;
    }

    stmt->u.ret.var = var;

    return stmt;
}

/* allocate and initialise a new stmt->begin
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_stmt *ic_transform_ir_stmt_begin_new(void) {
    struct ic_transform_ir_stmt *stmt = 0;

    stmt = ic_transform_ir_stmt_new(ic_transform_ir_stmt_type_begin);
    if (!stmt) {
        puts("ic_transform_ir_stmt_begin_new: call to ic_transform_ir_stmt_new failed");
        return 0;
    }

    if (!ic_transform_ir_begin_init(&(stmt->u.begin))) {
        puts("ic_transform_ir_stmt_let_begin_new: call to ic_transform_ir_begin_init failed");
        free(stmt);
        return 0;
    }

    return stmt;
}

/* allocate and initialise a new stmt->if
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_stmt *ic_transform_ir_stmt_if_new(struct ic_symbol *cond_sym) {
    struct ic_transform_ir_stmt *stmt = 0;

    if (!cond_sym) {
        puts("ic_transform_ir_stmt_if_new: var was null");
        return 0;
    }

    stmt = ic_transform_ir_stmt_new(ic_transform_ir_stmt_type_if);
    if (!stmt) {
        puts("ic_transform_ir_stmt_if_new: call to ic_transform_ir_stmt_new failed");
        return 0;
    }

    if (!ic_transform_ir_if_init(&(stmt->u.sif), cond_sym)) {
        puts("ic_transform_ir_stmt_let_if_new: call to ic_transform_ir_if_init failed");
        free(stmt);
        return 0;
    }

    return stmt;
}

/* allocate and initialise a new stmt->match
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_stmt *ic_transform_ir_stmt_match_new(struct ic_decl_type *tdecl, struct ic_symbol *match_symbol) {
    struct ic_transform_ir_stmt *stmt = 0;

    if (!match_symbol) {
        puts("ic_transform_ir)stmt_match_new: match_symbol was null");
        return 0;
    }

    stmt = ic_transform_ir_stmt_new(ic_transform_ir_stmt_type_match);
    if (!stmt) {
        puts("ic_transform_ir_stmt_match_new: call to ic_transform_ir_stmt_new failed");
        return 0;
    }

    if (!ic_transform_ir_match_init(&(stmt->u.match), tdecl, match_symbol)) {
        puts("ic_transform_ir_stmt_match_expr_new: call to ic_transform_ir_match_init failed");
        free(stmt);
        return 0;
    }

    return stmt;
}
