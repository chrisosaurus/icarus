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
    check                          : perform static analysis on input file\n\
    transform                      : transform input file\n\
    2c                             : run 2c - compiles icarus code to C\n\
    pancake                        : run pancake - the icarus interpreter\n\
    <default>                      : defaults to `pancake` if a command is omitted\n\
\n\
  options:\n\
    --debug                        : enable icarus internal debug output\n\
    --help,     -h                 : print this help message\n\
    --input,    -i     file        : input filename\n\
    --output,   -o     file        : output filename\n\
    --version                      : print version of icarus\n\
    <default>                      : an argument will default to being an `input` file if the preceding option is omitted\n\
\n\
  examples:\n\
    icarus foo.ic                  # run file `foo.ic` through `pancake`\n\
    icarus check --input foo.ic    # perform static analysis on file `foo.ic`\n\
    icarus 2c -i foo.ic -o foo.c   # compile `foo.ic` to c and output that in new file `foo.c`\n\
    icarus pancake foo.ic --debug  # run `foo.ic` through pancake with debug output\n\
    icarus -h                      # print this help page\n\
\n\
");
}

struct ic_opts *main_args(int argc, char **argv) {
    struct ic_opts *opts = 0;

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

    return opts;
}
