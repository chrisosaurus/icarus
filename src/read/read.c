#include <stdio.h>  /* fopen, fclose, fread */
#include <stdlib.h> /* malloc */

#include "read.h"

/* slurp entire file into single alloc-ed buffer
 *
 * returns this buffer on success
 * returns 0 on failure
 */
char *ic_read_slurp_filename(char *filename) {
    /* file object */
    FILE *file;
    /* malloc-ed buffer we store file contents in and return */
    char *buffer;

    /* open file */
    file = fopen(filename, "r");
    if (!file) {
        puts("ERROR: fopen failed in read_slurp_filename");
        perror("read_slurp_filename::fopen");
        return 0;
    }

    buffer = ic_read_slurp_file(file);

    if (!buffer) {
        puts("Error: read_slurp_filename::read_slurp_file failed");
        fclose(file);
        return 0;
    }

    /* close file */
    if (fclose(file)) {
        puts("Error: read_slurp_filename::fclose failed");
        free(buffer);
        return 0;
    }

    return buffer;
}

/* slurp entire file into single alloc-ed buffer
 *
 * returns this buffer on success
 * returns 0 on failure
 */
char *ic_read_slurp_file(FILE *file) {
    /* size of file (from ftell) */
    long fsize;
    /* unsigned version of comparison with size_t read */
    size_t ufsize;
    /* number of bytes read (from fread) */
    size_t read;
    /* malloc-ed buffer we store file contents in and return */
    char *buffer;

    /* seek to end of file */
    if (fseek(file, 0, SEEK_END)) {
        puts("ERROR: fseek SEEK_END failed in read_slurp_file");
        perror("read_slurp_file::fseek SEEK_END");
        return 0;
    }

    /* get current offset */
    fsize = ftell(file);
    ufsize = fsize;
    if (fsize < 0) {
        puts("ERROR: fstell failed in read_slurp_file");
        perror("read_slurp_file::ftell");
        return 0;
    }

    /* back to the start */
    if (fseek(file, 0, SEEK_SET)) {
        puts("ERROR: fseek SEEK_SET failed in read_slurp_file");
        perror("read_slurp_file::fseek SEEK_SET");
        return 0;
    }

    buffer = malloc(ufsize + 1);
    if (!buffer) {
        puts("ERROR: malloc failed in read_slurp_file");
        return 0;
    }

    /* make sure our buffer looks like a string */
    buffer[fsize] = '\0';

    /* slurp in the contents */
    read = fread(buffer, 1, fsize, file);

    /* check read and file size both line up */
    if (read != ufsize) {
        printf("ERROR: read_slurp_file::fread read '%zd' when we expected '%lu'\n", read, ufsize);
        free(buffer);
        return 0;
    }

    return buffer;
}
