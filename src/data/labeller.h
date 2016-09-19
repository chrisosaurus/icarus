#ifndef IC_LABELLER_H
#define IC_LABELLER_H

#include "string.h"
#include "symbol.h"

/* a labeller is used for generating labels from a string and a count
 * this is used within various parts of the compiler for making temporary variables
 *
 * e.g. if our seed is "foo_" and we ask for the first label, our output
 * should be "foo_0", the next label would be "foo_1", next then "foo_2"
 * and so on
 */
struct ic_labeller {
    char *seed;
    size_t seed_len;
    unsigned int next_count;
};

/* construct a new labeller and initialise it
 * provided seed will be strdup-ed
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_labeller *ic_labeller_new(char *seed);

/* init an existing labeller
 * provided seed will be strdup-ed
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_labeller_init(struct ic_labeller *labeller, char *seed);

/* destroy a labeller
 *
 * will only free labeller if `free_labeller` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_labeller_destroy(struct ic_labeller *labeller, unsigned int free_labeller);

/* generate a new label and return it
 *
 * returns value is owned by caller
 *
 * returns * on success
 * returns 0 on failure
 */
char *ic_labeller_generate(struct ic_labeller *labeller);

/* generate a new label and return it
 *
 * returns value is owned by caller
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_symbol *ic_labeller_generate_symbol(struct ic_labeller *labeller);

#endif /* IC_LABELLER_H */
