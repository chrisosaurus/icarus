#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "opts.h"
/* on success returns number of arguments consumed
 * on failure returns -1
 */
int arg_process(struct ic_arg *argument, char **argv, int *i, int argc);

/* returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_arg_parse(struct ic_command *commands, struct ic_arg *arguments, char **argv, int argc, char **default_option) {
    int i = 0;
    int j = 0;
    char *c = 0;

    struct ic_command *command = 0;
    struct ic_arg *arg = 0;

    bool restart = 0;

    int ret = 0;

    FILE *file = 0;

    if (!commands) {
        puts("commands was null");
        return 1;
    }

    if (!arguments) {
        puts("arguments was null");
        return 1;
    }

    if (!argv) {
        puts("argv was null");
        return 1;
    }

    if (!default_option) {
        puts("default_option was null");
        return 1;
    }

    for (i = 1; i < argc; ++i) {
        restart = 0;
        c = argv[i];
        if (!c) {
            puts("argv[i] was null");
            return 1;
        }

        /* check commands */
        for (j = 0;; ++j) {
            command = &(commands[j]);
            if (command->c == 0) {
                /* end of commands, stop */
                break;
            }

            if (0 == strcmp(command->c, c)) {
                /* match! */
                *(command->thing) = 1;
                /* stop now */
                restart = 1;
                break;
            }
        }

        if (restart) {
            continue;
        }

        /* check arguments */
        for (j = 0;; ++j) {
            arg = &(arguments[j]);
            if (arg->l == 0) {
                /* end of arguments, stop */
                break;
            }
            ret = arg_process(arg, argv, &i, argc);
            if (-1 == ret) {
                puts("call to arg_process failed");
                return 0;
            }
            if (ret > 0) {
                restart = 1;
                break;
            }
        }

        if (restart) {
            continue;
        }

        /* haven't matched anything so far
     *   1) either a plain filename (default to in_filename)
     *   2) invalid argument
     */

        /* check if exists as a file, if so then assume filename */
        file = fopen(c, "r");
        if (file) {
            /* was able to open, assume it was intentional */
            fclose(file);

            /* we want to set this as *default_option
       * but only if not already set
       */
            if (*default_option) {
                printf("Error: tried to set filename '%s' as default option when it was already set to '%s'\n", c, *default_option);
                return 0;
            } else {
                *default_option = c;
                /* keep going */
                continue;
            }
        }

        /* otherwise illegal option, error */
        printf("Unknown option '%s'\n", c);
        return 0;
    }

    /* success */
    return 1;
}

int arg_process(struct ic_arg *argument, char **argv, int *i, int argc) {
    bool *b = 0;
    char **c = 0;

    char *arg = 0;

    size_t len = 0;

    if (!argument) {
        puts("arg_handle: argument was null");
        return -1;
    }

    if (!i) {
        puts("arg_handle: i was null");
        return -1;
    }

    if (!argv) {
        puts("arg_handle: argv was null");
        return -1;
    }

    arg = argv[*i];
    if (arg[0] != '-') {
        return 0;
    }

    if (arg[1] == '-') {
        /* long option */
        len = strlen(argument->l);
        if (0 == strncmp(argument->l, &(arg[2]), len)) {
            /* matched */
            goto MATCHED;
        }
    } else {
        /* short option */
        if (arg[1] == argument->s) {
            /* matched */
            goto MATCHED;
        }
    }

    /* no match */
    return 0;

MATCHED:

    switch (argument->type) {
        case arg_type_string:
            /* set string to next argument in argv and consume */
            c = argument->thing;
            ++*i;
            if (*i >= argc) {
                puts("arg_handle: ran out of arguments");
                return -1;
            }
            *c = argv[*i];
            return 2;
            break;

        case arg_type_bool:
            /* set boolean to true */
            b = argument->thing;
            *b = 1;
            return 1;
            break;

        default:
            puts("arg_handle: impossible");
            return -1;
            break;
    }

    return 0;
}

/* allocate and init a new opts
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_opts *ic_arg_opts_new(void) {
    struct ic_opts *opts = 0;

    opts = calloc(1, sizeof(struct ic_opts));

    if (!opts) {
        puts("ic_arg_opts_new: call to calloc failed");
        return 0;
    }

    return opts;
}
