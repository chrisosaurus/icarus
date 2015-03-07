#include <stdlib.h> /* malloc */
#include <stdio.h> /* fopen, fclose, fread */

#include "read.h"

/* slurp entire file into single alloc-ed buffer
 * return this buffer on success
 * return 0 on error
 */
char * read_slurp(char *filename){
    /* file object */
    FILE *f;
    /* size of file (from ftell) */
    long fsize;
    /* unsigned version of comparison with size_t read */
    size_t ufsize;
    /* number of bytes read (from fread) */
    size_t read;
    /* malloc-ed buffer we store file contents in and return */
    char *buffer;

    /* open file */
    f = fopen(filename, "r");
    if( ! f ){
        puts("ERROR: fopen failed in read_slurp");
        perror("read_slurp::fopen");
        return 0;
    }

    /* seek to end of filej */
    if( fseek(f, 0, SEEK_END) ){
        puts("ERROR: fseek SEEK_END failed in read_slurp");
        perror("read_slurp::fseek SEEK_END");
        fclose(f);
        return 0;
    }

    /* get current offset */
    fsize = ftell(f);
    ufsize = fsize;
    if( fsize < 0 ){
        puts("ERROR: fstell failed in read_slurp");
        perror("read_slurp::ftell");
        fclose(f);
        return 0;
    }

    /* back to the start */
    if( fseek(f, 0, SEEK_SET) ){
        puts("ERROR: fseek SEEK_SET failed in read_slurp");
        perror("read_slurp::fseek SEEK_SET");
        fclose(f);
        return 0;
    }

    buffer = malloc( ufsize + 1 );
    if( ! buffer ){
        puts("ERROR: malloc failed in read_slurp");
        fclose(f);
        return 0;
    }

    /* make sure our buffer looks like a string */
    buffer[fsize] = '\0';

    /* slurp in the contents */
    read = fread(buffer, 1, fsize, f);

    /* check read and file size both line up */
    if( read != ufsize ){
        printf("ERROR: read_slurp::fread read '%zd' when we expected '%lu'\n", read, ufsize);
        fclose(f);
        free(buffer);
        return 0;
    }

    /* close file */
    if( fclose(f) ){
        puts("Error: read_flup::fcose failed");
        free(buffer);
        return 0;
    }

    return buffer;
}

