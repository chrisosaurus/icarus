#ifndef IC_BACKENDS_2C_H
#define IC_BACKENDS_2C_H

#include "../../analyse/data/kludge.h"

/* taking a processed ast and a string containing the path to
 * the desired output file
 *
 * generate a c program
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_b2c_compile(struct ic_kludge *input_kludge, char *out_filename);

#endif
