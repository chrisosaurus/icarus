#include <stdio.h>  /* puts, printf */
#include <stdlib.h> /* exit, free */
#include <string.h>

#include "helpers.h"

int main(int argc, char **argv) {
    char *filename = 0;
    struct ic_kludge *kludge = 0;

    if (argc < 2) {
        puts("No source file specified");
        exit(1);
    } else if (argc > 2) {
        puts("Too many arguments supplied, only source file was expected");
        exit(1);
    }

    filename = argv[1];

    kludge = test_custom_helper_gen_kludge();
    test_custom_helper_load_core(kludge);
    test_custom_helper_load_filename(kludge, filename);

    if (!ic_analyse(kludge)) {
        puts("analysis failed");
        exit(1);
    }

    puts("(partial) analyse was a success");

    /* clean up time
     * this will destroy both the kludge and the ast (via aast)
     */
    if (!ic_kludge_destroy(kludge, 1)) {
        puts("main: ic_kludge_destroy call failed");
    }

    return 0;
}
