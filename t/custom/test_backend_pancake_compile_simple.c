#include <stdio.h>  /* puts, printf */
#include <stdlib.h> /* exit, free */
#include <string.h>

#include "../../src/backends/pancake/data/instructions.h"
#include "../../src/backends/pancake/pancake.h"
#include "helpers.h"

int main(int argc, char **argv) {
    char *in_filename = 0;
    char *out_filename = 0;
    FILE *out_file = 0;
    struct ic_kludge *kludge = 0;
    struct ic_backend_pancake_instructions *instructions = 0;

    if (argc < 3) {
        puts("not enough args, need both input source and output file");
        exit(1);
    } else if (argc > 3) {
        puts("Too many arguments supplied, only source and out files were expected");
        exit(1);
    }

    in_filename = argv[1];
    out_filename = argv[2];

    out_file = fopen(out_filename, "w");
    if (!out_file) {
        puts("main: call to fopen failed");
        exit(1);
    }

    kludge = test_custom_helper_gen_kludge();
    test_custom_helper_load_core(kludge);
    test_custom_helper_load_filename(kludge, in_filename);

    if (!ic_analyse(kludge)) {
        puts("main: call to ic_analyse failed");
        exit(1);
    }

    if (!ic_transform(kludge)) {
        puts("main: call to ic_transform failed");
        exit(1);
    }

    instructions = ic_backend_pancake_compile(kludge);
    if (!instructions) {
        puts("main: call to ic_backend_pancake_compile failed");
        exit(1);
    }

    if (!ic_backend_pancake_instructions_print(instructions, out_file)) {
        puts("main: call to ic_backend_pancake_instructions_print failed");
        exit(1);
    }

    /* clean up time
     * this will destroy both the kludge and the ast (via aast)
     */
    if (!ic_kludge_destroy(kludge, 1)) {
        puts("main: ic_kludge_destroy call failed");
    }

    return 0;
}
