#include <stdio.h>
#include <string.h>

#include "../../analyse/data/kludge.h"
#include "../../data/dict.h"
#include "data/instructions.h"
#include "data/runtime_data.h"
#include "pancake.h"

/* run instructions in fresh pancake runtime
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_backend_pancake_run(struct ic_backend_pancake_instructions *instructions, struct ic_opts *opts);

/* run instructions in fresh pancake runtime
 *
 * returns 1 on success
 * returns 0 on failure
 */
static unsigned int ic_backend_pancake_run(struct ic_backend_pancake_instructions *instructions, struct ic_opts *opts) {
    struct ic_backend_pancake_runtime_data *runtime_data = 0;

    if (!instructions) {
        puts("ic_backend_pancake_run: instructions was null");
        return 0;
    }

    if (!opts) {
        puts("ic_backend_pancake_run: opts was null");
        return 0;
    }

    runtime_data = ic_backend_pancake_runtime_data_new(instructions);
    if (!runtime_data) {
        puts("ic_backend_pancake_run: call to ic_backend_pancake_runtime_data_new failed");
        return 0;
    }

    if (!ic_backend_pancake_interpret(runtime_data)) {
        puts("ic_backend_pancake_run: call to ic_backend_pancake_interpret failed");
        return 0;
    }

    if (opts->debug) {
        puts("\npancake value_stack after execution finished:");
        puts("----------------");
        if (!ic_backend_pancake_value_stack_print(stdout, runtime_data->value_stack)) {
            puts("ic_backend_pancake_from_bytecode: call to ic_backend_pancake_value_stack_print failed");
            return 0;
        }
        puts("----------------\n");
    }

    return 1;
}

/* pancake - stack based interpreter backend
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake(struct ic_kludge *kludge, struct ic_opts *opts) {

    struct ic_backend_pancake_instructions *instructions = 0;
    FILE *out_fh = 0;

    /* do we need to close the fh or not */
    unsigned int close_fh = 0;

    if (!kludge) {
        puts("ic_backend_pancake: kludge was null");
        return 0;
    }

    if (!opts) {
        puts("ic_backend_pancake: opts was null");
        return 0;
    }

    instructions = ic_backend_pancake_compile(kludge);
    if (!instructions) {
        puts("ic_backend_pancake: call to ic_backend_pancake_compile failed");
        return 0;
    }

    if (opts->debug) {
        /* print instructions */
        puts("Pancake bytecode:");
        puts("==========================");
        if (!ic_backend_pancake_instructions_print(stdout, instructions)) {
            puts("ic_backend_pancake: call to ic_backend_pancake_instructions_print failed");
            return 0;
        }
        puts("==========================");
        puts("");

        puts("Pancake interpreter output");
        puts("==========================");
    }

    /* if out_filename is specified then output pancake bytecode */
    if (opts->out_filename) {
        if (0 == strcmp("-", opts->out_filename)) {
            out_fh = stdout;
        } else {
            out_fh = fopen(opts->out_filename, "w");
            close_fh = 1;
            if (NULL == out_fh) {
                puts("ic_backend_pancake: call to fopen failed");
                return 0;
            }
        }

        if (!out_fh) {
            printf("ic_backend_pancake: call to fopen failed for file '%s'\n", opts->out_filename);
            return 0;
        }
        if (!ic_backend_pancake_instructions_print(out_fh, instructions)) {
            puts("ic_backend_pancake: call to ic_backend_pancake_instructions_print failed");
            return 0;
        }

        if (close_fh) {
            fclose(out_fh);
        }
    }

    if (!ic_backend_pancake_run(instructions, opts)) {
        puts("ic_backend_pancake: call to ic_backend_pancake_run failed");
        return 0;
    }

    return 1;
}

/* load bytecode from file (opts->in_bytecode_filename) and then interpret it
 * in a fresh pancake runtime
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake_run_bytecode_from_file(struct ic_opts *opts) {
    struct ic_backend_pancake_instructions *instructions = 0;

    FILE *in_fh = 0;
    /* boolean as to whether or not we should close in_fh when we are finished */
    unsigned int close_fh = 0;

    if (!opts->in_bytecode_filename) {
        puts("ic_backend_pancake_run_bytecode: in_bytecode_filename not specified");
        return 0;
    }

    if (0 == strcmp("-", opts->in_bytecode_filename)) {
        in_fh = stdin;
    } else {
        in_fh = fopen(opts->in_bytecode_filename, "r");
        close_fh = 1;
        if (NULL == in_fh) {
            puts("ic_backend_pancake_run_bytecode: call to fopen failed");
            return 0;
        }
    }

    instructions = ic_backend_pancake_instructions_load(in_fh);
    if (!instructions) {
        puts("main: call to ic_backend_pancake_instructions_load failed");
        return 0;
    }

    if (close_fh) {
        fclose(in_fh);
    }

    if (!ic_backend_pancake_run(instructions, opts)) {
        puts("ic_backend_pancake: call to ic_backend_pancake_run failed");
        return 0;
    }

    return 1;
}
