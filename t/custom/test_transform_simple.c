#include <stdio.h>  /* puts, printf */
#include <stdlib.h> /* exit, free */
#include <string.h>

#include "test_custom_helpers.h"
#include "../../src/transform/transform.h"

int main(int argc, char **argv) {
    char *filename = 0;
    struct ic_kludge *kludge = 0;

    if (argc != 2) {
        puts("incorrect number of args");
        exit(1);
    }

    filename = argv[1];

    kludge = test_custom_helper_gen_kludge();
    test_custom_helper_load_core(kludge);
    test_custom_helper_load_filename(kludge, filename);

    if (!ic_analyse(kludge)) {
        puts("main: call to ic_analyse failed");
        exit(1);
    }

    /* transform */
    if (!ic_transform(kludge)) {
        puts("transform failed");
        exit(1);
    }

    /* print tir */
    if (!ic_transform_print(kludge)) {
        puts("transform print failed");
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
