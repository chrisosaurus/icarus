#include <assert.h> /* assert */
#include <stdio.h>  /* puts */
#include <string.h> /* strncmp */
#include <stdlib.h> /* free */

#include "../../../src/data/labeller.h"

void normal(void) {
    struct ic_labeller *labeller_1 = 0;
    struct ic_labeller *labeller_2 = 0;
    char *ch = 0;

    puts("testing labeller");

    labeller_1 = ic_labeller_new("foo_");
    assert(labeller_1);

    labeller_2 = ic_labeller_new("bar_");
    assert(labeller_2);

    ch = ic_labeller_generate(labeller_1);
    assert(ch);
    assert(!strcmp("foo_0", ch));
    free(ch);

    ch = ic_labeller_generate(labeller_2);
    assert(ch);
    assert(!strcmp("bar_0", ch));
    free(ch);

    ch = ic_labeller_generate(labeller_1);
    assert(ch);
    assert(!strcmp("foo_1", ch));
    free(ch);

    ch = ic_labeller_generate(labeller_2);
    assert(ch);
    assert(!strcmp("bar_1", ch));
    free(ch);

    ch = ic_labeller_generate(labeller_1);
    assert(ch);
    assert(!strcmp("foo_2", ch));
    free(ch);

    ch = ic_labeller_generate(labeller_2);
    assert(ch);
    assert(!strcmp("bar_2", ch));
    free(ch);

    assert(ic_labeller_destroy(labeller_1, 1));
    assert(ic_labeller_destroy(labeller_2, 1));

    puts("success");
}

int main(void) {
    normal();

    return 0;
}
