#ifndef ICARUS_READ_H
#define ICARUS_READ_H

/* slurp entire file into single alloc-ed buffer
 *
 * returns this buffer on success
 * returns 0 on failure
 */
char *ic_read_slurp_filename(char *filename);

/* slurp entire file into single alloc-ed buffer
 *
 * returns this buffer on success
 * returns 0 on failure
 */
char *ic_read_slurp_file(FILE *file);

#endif
