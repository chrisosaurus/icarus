#ifndef ICARUS_READ_H
#define ICARUS_READ_H

/* slurp entire file into single alloc-ed buffer
 * return this buffer on success
 * return 0 on error
 */
char * ic_read_slurp(char *filename);

#endif
