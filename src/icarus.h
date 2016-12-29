#ifndef IC_ICARUS_H
#define IC_ICARUS_H

#include "opts.h"

/* take opts and perform needed work
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int icarus(struct ic_opts *opts);

#endif
