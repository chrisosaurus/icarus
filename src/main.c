#include <stdio.h>  /* puts, printf */
#include <stdlib.h> /* exit */

#include "icarus.h"

void help(void);
void version(void);

struct ic_opts *main_args(int argc, char **argv);

int main(int argc, char **argv) {
    /* argument parsing */
    struct ic_opts *opts = 0;

    opts = main_args(argc, argv);
    if (!opts) {
        puts("main: option parsing failed");
        exit(1);
    }

    if (!icarus(opts)) {
        puts("main: call to icarus failed");
        exit(1);
    }

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

    opts = ic_arg_opts_new();
    if (!opts) {
        puts("main_args: call to ic_arg_opts_new failed");
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
        puts("main: Failed to parse arguments");
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
        puts("main: Cannot specify more than one command");
        return 0;
    }

    /* input file must be specified */
    if (0 == opts->in_filename) {
        puts("main: No input file specified");
        return 0;
    }

    /* for 2c we must have an output file */
    if (opts->o2c) {
        if (0 == opts->out_filename) {
            puts("main: Command 2c specified but no output file provided");
            return 0;
        }
    }

    /* otherwise looking good */
    return opts;
}
