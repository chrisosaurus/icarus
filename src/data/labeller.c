#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "labeller.h"
#include "strdup.h"

/* construct a new labeller and initialise it
 * provided seed will be strdup-ed
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_labeller *ic_labeller_new(char *seed) {
    struct ic_labeller *labeller = 0;

    if (!seed) {
        puts("ic_labeller_new: seed was null");
        return 0;
    }

    labeller = calloc(1, sizeof(struct ic_labeller));
    if (!labeller) {
        puts("ic_labeller_new: call to calloc failed");
        return 0;
    }

    if (!ic_labeller_init(labeller, seed)) {
        puts("ic_labeller_new: call to ic_labeller_init failed");
        return 0;
    }

    return labeller;
}

/* init an existing labeller
 * provided seed will be strdup-ed
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_labeller_init(struct ic_labeller *labeller, char *seed) {
    char *new_seed = 0;

    if (!labeller) {
        puts("ic_labeller_init: labeller was null");
        return 0;
    }

    if (!seed) {
        puts("ic_labeller_init: seed was null");
        return 0;
    }

    new_seed = ic_strdup(seed);
    if (!new_seed) {
        puts("ic_labeller_init: call to ic_strdup failed");
        return 0;
    }

    labeller->seed = new_seed;
    labeller->seed_len = strlen(new_seed);
    labeller->next_count = 0;

    return 1;
}

/* destroy a labeller
 *
 * will only free labeller if `free_labeller` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_labeller_destroy(struct ic_labeller *labeller, unsigned int free_labeller) {
    if (!labeller) {
        puts("ic_labeller_destroy: labeller was null");
        return 0;
    }

    /* free internal seed */
    free(labeller->seed);

    if (free_labeller) {
        free(labeller);
    }

    return 1;
}

/* generate a new label and return it
 *
 * returns value is owned by caller
 *
 * returns * on success
 * returns 0 on failure
 */
char *ic_labeller_generate(struct ic_labeller *labeller) {
    /* our output string */
    char *out_str = 0;
    /* number of digits we need */
    int digits = 0;
    /* return value from snprintf */
    int ret;
    /* total len we will need */
    int total_len = 0;

    if (!labeller) {
        puts("ic_labeller_generate: labeller was null");
        return 0;
    }

    /* labeller only support up to 9999 labels */
    if (labeller->next_count > 9999) {
        puts("ic_labeller_generate: cannot generate more than 9999 labels");
        return 0;
    } else if (labeller->next_count > 999) {
        /* 4 digits */
        digits = 4;
    } else if (labeller->next_count > 99) {
        /* 3 digits */
        digits = 3;
    } else if (labeller->next_count > 9) {
        /* 2 digits */
        digits = 2;
    } else {
        /* single digit */
        digits = 1;
    }

    total_len = labeller->seed_len + digits;

    /* total_len + 1 for terminating \0 */
    out_str = calloc(total_len + 1, sizeof(char));
    if (!out_str) {
        puts("ic_labeller_generate: call to calloc failed");
        return 0;
    }

    /* total_len + 1 for terminating \0 */
    ret = snprintf(out_str, total_len + 1, "%s%u", labeller->seed, labeller->next_count);
    if (ret != total_len) {
        printf("ic_labeller_generate: expected to print '%d' digits but instead printed '%d'\n", total_len, ret);
        return 0;
    }

    /* increment our counter */
    labeller->next_count += 1;

    return out_str;
}

/* generate a new label and return it
*
* returns value is owned by caller
*
* returns * on success
* returns 0 on failure
*/
struct ic_symbol *ic_labeller_generate_symbol(struct ic_labeller *labeller) {
    struct ic_symbol *sym = 0;
    char *str = 0;

    if (!labeller) {
        puts("ic_labeller_generate_symbol: labeller was null");
        return 0;
    }

    str = ic_labeller_generate(labeller);
    if (!str) {
        puts("ic_labeller_generate_symbol: call to ic_labeller_generate failed");
        return 0;
    }

    sym = ic_symbol_new(str, strlen(str));
    if (!sym) {
        puts("ic_labeller_generate_symbol: call to ic_symbol_new failed");
        return 0;
    }

    return sym;
}
