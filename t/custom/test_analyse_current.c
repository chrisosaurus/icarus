#include <stdio.h>  /* puts, printf */
#include <stdlib.h> /* exit, free */
#include <string.h>

#include "helpers.h"

int main(void) {
    char *filename = 0;
    struct ic_kludge *kludge = 0;

    filename = "example/current.ic";

    kludge = test_custom_helper_gen_kludge();
    test_custom_helper_load_core(kludge);
    test_custom_helper_load_filename(kludge, filename);

    if (!ic_analyse(kludge)) {
        puts("main: call to ic_analyse failed");
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
