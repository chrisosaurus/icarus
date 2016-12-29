#include <stdio.h>  /* puts, printf */
#include <stdlib.h> /* exit, free */

#include "analyse/analyse.h"     /* ic_kludge */
#include "lex/lexer.h"           /* ic_lex */
#include "parse/data/ast.h"      /* ic_ast structure */
#include "parse/parse.h"         /* ic_parse */
#include "read/read.h"           /* ic_read_slurp */
#include "transform/transform.h" /* ic_transform */

#include "icarus.h"

/* backends */
#include "backends/2c/2c.h"
#include "backends/pancake/pancake.h"

unsigned int icarus(struct ic_opts *opts) {
    char *filename = 0, *source = 0, *core_source = 0;
    struct ic_token_list *token_list = 0, *core_token_list = 0;
    struct ic_ast *ast = 0, *core_ast = 0;
    struct ic_kludge *kludge = 0;

    kludge = ic_kludge_new();
    if (!kludge) {
        puts("call to ic_kludge_new failed");
        exit(1);
    }

    /* populate from core.ic */
    core_source = ic_read_slurp_filename("src/stdlib/core.ic");
    if (!core_source) {
        puts("slurping failed for src/stdlib/core.ic");
        exit(1);
    }

    core_token_list = ic_lex("src/stdlib/core.ic", core_source);
    if (!core_token_list) {
        puts("lexing failed for src/stdlib/core.ic");
        exit(1);
    }

    core_ast = ic_parse(core_token_list);
    if (!core_ast) {
        puts("parsing failed for src/stdlib/core.ic");
        exit(1);
    }

    if (!ic_kludge_populate(kludge, core_ast)) {
        puts("call to ic_kludge_populate failed for src/stdlib/core.ic");
        exit(1);
    }

    filename = opts->in_filename;

    source = ic_read_slurp_filename(filename);
    if (!source) {
        printf("slurping failed for input file '%s'\n", filename);
        exit(1);
    }

    token_list = ic_lex(filename, source);
    if (!token_list) {
        puts("lexing failed");
        exit(1);
    }

    if (opts->debug) {
        puts("\nlexer output:");
        puts("----------------");
        ic_token_list_print(stdout, token_list);
        puts("----------------\n");
    }

    ast = ic_parse(token_list);
    if (!ast) {
        puts("parsing failed");
        exit(1);
    }

    if (opts->debug) {
        puts("\nparser output:");
        puts("----------------");
        ic_ast_print(stdout, ast);
        puts("----------------\n");
    }

    if (!ic_kludge_populate(kludge, ast)) {
        puts("call to ic_kludge_populate failed");
        exit(1);
    }

    if (opts->debug) {
        puts("\nanalyse output:");
        puts("----------------");
    }
    if (!ic_analyse(kludge)) {
        puts("analysis failed");
        exit(1);
    }

    if (opts->debug) {
        puts("warning: main implementation pending, icarus is currently only partially functional");
        puts("analysis complete");
        puts("----------------\n");
    }

    /* if our command was 'check' then stop here */
    if (opts->check) {
        /* success */
        goto CLEANUP;
    }

    if (opts->debug) {

        puts("\ntransform output (PENDING):");
        puts("----------------");
    }

    if (!ic_transform(kludge)) {
        puts("transform failed");
        exit(1);
    }

    if (opts->debug) {
        puts("----------------");
        if (!ic_transform_print(stdout, kludge)) {
            puts("call to ic_transform_print failed");
            exit(1);
        }
    }

    /* if our command was 'transform' then stop here */
    if (opts->transform) {
        /* success */
        goto CLEANUP;
    }

    /* backend time */
    if (opts->pancake) {
        if (opts->debug) {
            puts("\nbackend pancake selected (PENDING):");
        }
        if (!ic_backend_pancake(kludge, opts)) {
            puts("Pancake backend failed");
            exit(1);
        }

    } else if (opts->o2c) {
        if (opts->debug) {
            puts("\nbackend 2c selected (PENDING):");
        }
        if (!ic_b2c_compile(kludge, opts->out_filename)) {
            puts("compilation failed");
            exit(1);
        }

    } else {
        puts("Impossible situation, compiler error when selecting backend");
        exit(1);
    }

CLEANUP:
    /* clean up time */

    if (!ic_kludge_destroy(kludge, 1)) {
        puts("main: ic_kludge_destroy call failed");
    }

    if (!ic_ast_destroy(ast, 1)) {
        puts("main: ic_ast_destroy call failed");
    }

    if (!ic_ast_destroy(core_ast, 1)) {
        puts("main: ic_ast_destroy call failed");
    }

    if (!ic_token_list_destroy(token_list, 1)) {
        puts("ic_token_list_destroy failed");
    }

    if (!ic_token_list_destroy(core_token_list, 1)) {
        puts("ic_token_list_destroy failed");
    }

    free(source);
    free(core_source);
    free(opts);

    return 1;
}
