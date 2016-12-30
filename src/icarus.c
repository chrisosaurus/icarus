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

/* check if file exists
 *
 * will returns 1 if file exists
 * will returns 0 if file doesn't exist
 */
static unsigned int file_exists(char *filename);

/* check provided options
 *
 * will exit(1) for incorrect options
 *
 * returns 1 on success
 * returns 0 on error
 */
static unsigned int check_options(struct ic_opts *opts);

unsigned int icarus(struct ic_opts *opts) {
    char *filename = 0, *source = 0, *core_source = 0;
    struct ic_token_list *token_list = 0, *core_token_list = 0;
    struct ic_ast *ast = 0, *core_ast = 0;
    struct ic_kludge *kludge = 0;

    if (!check_options(opts)) {
        puts("icarus: call to check_options failed");
        return 0;
    }

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
        if (!ic_transform_print(stdout, kludge)) {
            puts("call to ic_transform_print failed");
            exit(1);
        }

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

/* check if file exists
 *
 * will returns 1 if file exists
 * will returns 0 if file doesn't exist
 */
static unsigned int file_exists(char *filename) {
    FILE *fh = 0;

    if (!filename) {
        puts("file_exists: filename was null");
        return 0;
    }

    fh = fopen(filename, "r");

    if (!fh) {
        /* file failed to open
     * assume doesn't exist
     */

        /* FIXME TODO check for different kinds of errors */
        return 0;
    }

    fclose(fh);

    /* file exists and we can read it */
    return 1;
}

/* check provided options
 *
 * will exit(1) for incorrect options
 *
 * returns 1 on success
 * returns 0 on error
 */
static unsigned int check_options(struct ic_opts *opts) {
    unsigned int command_count = 0;

    if (!opts) {
        puts("check_options: opts was null");
        return 0;
    }

    /* input file must be specified */
    if (0 == opts->in_filename) {
        puts("Icarus error: No input file specified");
        exit(1);
    }

    /* for 2c we must have an output file */
    if (opts->o2c) {
        if (0 == opts->out_filename) {
            puts("Icarus error: Command 2c specified but no output file provided");
            exit(1);
        }
    }

    /* input file has to exist */
    if (!file_exists(opts->in_filename)) {
        printf("Icarus error: input file '%s' could not be read\n", opts->out_filename);
        exit(1);
    }

    /* if an output file was provided, it has to NOT exist */
    if (opts->out_filename) {
        if (file_exists(opts->out_filename)) {
            printf("Icarus error: output file '%s' already exists\n", opts->out_filename);
            exit(1);
        }
    }

    /* check command(s) set */
    command_count = opts->check + opts->transform + opts->o2c + opts->pancake;
    if (0 == command_count) {
        /* default to pancake */
        opts->pancake = 1;
    } else if (command_count > 1) {
        /* cannot specify more than one command */
        puts("Icarus error: Cannot specify more than one command");
        exit(1);
    }

    /* at this point help and version no longer make sense
     * help and version are both handled within `src/main.c`
     * so consider it an error if they are set here
     */
    if (opts->help) {
        puts("Icarus internal error: option `help` set");
        exit(1);
    }
    if (opts->version) {
        puts("Icarus internal error: option `version` set");
        exit(1);
    }

    return 1;
}
