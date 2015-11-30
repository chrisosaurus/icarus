#ifndef IC_BACKENDS_2C_HEADER
#define IC_BACKENDS_2C_HEADER

#include "../../analyse/data/kludge.h"

/* taking a processed ast and a string containing the path to
 * the desired output file
 *
 * generate a c program
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_b2c_compile(struct ic_kludge *input_kludge, char *out_filename);

#endif
