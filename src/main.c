#include <stdio.h>  /* puts, printf */
#include <stdlib.h> /* exit, free */
#include <string.h>

#include "analyse/analyse.h" /* ic_kludge */
#include "lex/lexer.h"       /* ic_lex */
#include "opts.h"
#include "parse/data/ast.h"      /* ic_ast structure */
#include "parse/parse.h"         /* ic_parse */
#include "read/read.h"           /* ic_read_slurp */
#include "transform/transform.h" /* ic_transform */

/* backends */
#include "backends/2c/2c.h"
#include "backends/pancake/pancake.h"

void help(void);
void version(void);

struct ic_opts *main_args(int argc, char **argv);

int main(int argc, char **argv) {
    char *filename = 0, *source = 0, *core_source = 0;
    struct ic_token_list *token_list = 0, *core_token_list = 0;
    struct ic_ast *ast = 0, *core_ast = 0;
    struct ic_kludge *kludge = 0;

    /* argument parsing */
    struct ic_opts *opts = 0;

    opts = main_args(argc, argv);
    if (!opts) {
        puts("option parsing failed");
        exit(1);
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

    return 0;
}

void version(void) {
    puts("Icarus version: pre-alpha");
}

void help(void) {
    puts("\
      icarus [command] [options]\n\
\n\
        command:\n\
          check          : perform static analysis on input file\n\
          transform      : transform input file\n\
          2c             : pass input file to backend 2c\n\
          pancake        : (default) pass input file to backend pancake\n\
          <default>      : defaults to `pancake`\n\
\n\
        options:\n\
          --output, -o   : output filename\n\
          --input,  -i   : input filename\n\
          --debug enable : icarus compiler debug options\n\
          --version      : print version of icarus compiler\n\
          --help, -h     : print this help message\n\
          <default>      : will try to interpret as an input file, if none is already specified\n\
\n\
        examples:\n\
          icarus foo.ic                # run file `foo.ic` through `pancake`\n\
          icarus check --input foo.ic  # perform static analysis on file `foo.ic`\n\
          icarus 2c -i foo.ic -o foo.c # compile `foo.ic` to c and output that in new file `foo.c`\n\
\n\
       ");
}

struct ic_opts *main_args(int argc, char **argv) {
    struct ic_opts *opts = 0;
    unsigned int command_count = 0;

    opts = calloc(1, sizeof(struct ic_opts));
    if (!opts) {
        puts("failed to allocate space for options");
        return 0;
    }

    struct ic_command commands[] = {
        {"check", &opts->check},
        {"transform", &opts->transform},
        {"2c", &opts->o2c},
        {"pancake", &opts->pancake},
        IC_COMMAND_END,
    };

    struct ic_arg arguments[] = {
        {"input", 'i', arg_type_string, &(opts->in_filename)},
        {"output", 'o', arg_type_string, &(opts->out_filename)},
        {"debug", 'd', arg_type_bool, &(opts->debug)},
        {"version", 0, arg_type_bool, &(opts->version)},
        {"help", 'h', arg_type_bool, &(opts->help)},
        IC_ARG_END,
    };

    if (!ic_arg_parse(commands, arguments, argv, argc, &(opts->in_filename))) {
        puts("fail to parse arguments");
        return 0;
    }

    if (opts->help) {
        help();
        exit(1);
    }

    if (opts->version) {
        version();
        exit(1);
    }

    command_count = opts->check + opts->transform + opts->o2c + opts->pancake;
    if (0 == command_count) {
        /* default to pancake */
        opts->pancake = 1;
    } else if (command_count > 1) {
        /* cannot specify more than one command */
        puts("Cannot specify more than one command");
        return 0;
    }

    /* input file must be specified */
    if (0 == opts->in_filename) {
        puts("No input file specified");
        return 0;
    }

    /* for 2c we must have an output file */
    if (opts->o2c) {
        if (0 == opts->out_filename) {
            puts("Command 2c specified but no output file provided");
            return 0;
        }
    }

    /* otherwise looking good */
    return opts;
}
