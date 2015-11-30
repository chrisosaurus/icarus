#include <stdio.h>

#include "2c.h"

unsigned int ic_b2c_generate(struct ic_kludge *kludge, FILE *f);
unsigned int ic_b2c_generate_builtins(struct ic_kludge *kludge, FILE *f);
unsigned int ic_b2c_generate_types(struct ic_kludge *kludge, FILE *f);
unsigned int ic_b2c_generate_functions(struct ic_kludge *kludge, FILE *f);
unsigned int ic_b2c_generate_entry(struct ic_kludge *kludge, FILE *f);

/* taking a processed ast and a string containing the path to
 * the desired output file
 *
 * generate a c program
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_b2c_compile(struct ic_kludge *kludge, char *out_filename){
    FILE *f = 0;

    if(! kludge ){
        puts("ic_b2c_compile: kludge was null");
        return 1;
    }

    if(! out_filename ){
        puts("ic_b2c_compile: out_filename was null");
        return 1;
    }

    /* try opening file out writing, complain is exists */
    f = fopen(out_filename, "r");
    if( f ){
        printf("ic_b2c_compile: error file already existed '%s'\n", out_filename);
        return 0;
    }

    /* open file for writing */
    f = fopen(out_filename, "w");
    if( ! f ){
        printf("ic_b2c_compile: failed to open out_filename '%s'\n", out_filename);
        return 1;
    }

    /* perform heavy lifting */
    if( ! ic_b2c_generate(kludge, f) ){
        puts("ic_b2c_compile: call to ic_b2c_generate failed");
        return 1;
    }

    fclose(f);
    return 0;
}

unsigned int ic_b2c_generate(struct ic_kludge *kludge, FILE *f){

    if( ! ic_b2c_generate_builtins(kludge, f) ){
        puts("ic_b2c_generate: call to ic_b2c_generate_builtins failed");
        return 1;
    }

    if( ! ic_b2c_generate_types(kludge, f) ){
        puts("ic_b2c_generate: call to ic_b2c_generate_types failed");
        return 1;
    }

    if( ! ic_b2c_generate_functions(kludge, f) ){
        puts("ic_b2c_generate: call to ic_b2c_generate_functions failed");
        return 1;
    }

    if( ! ic_b2c_generate_entry(kludge, f) ){
        puts("ic_b2c_generate: call to ic_b2c_generate_entry failed");
        return 1;
    }

    return 0;
}


unsigned int ic_b2c_generate_builtins(struct ic_kludge *kludge, FILE *f){
    if( ! kludge ){
        puts("ic_b2d_generate_builtins: kludge was null");
        return 1;
    }

    if( ! f ){
        puts("ic_b2d_generate_builtins: file was null");
        return 1;
    }

    puts("ic_b2d_generate_builtins: unimplemented");
    return 1;
}

unsigned int ic_b2c_generate_types(struct ic_kludge *kludge, FILE *f){
    if( ! kludge ){
        puts("ic_b2d_generate_types: kludge was null");
        return 1;
    }

    if( ! f ){
        puts("ic_b2d_generate_types: file was null");
        return 1;
    }

    puts("ic_b2d_generate_types: unimplemented");
    return 1;
}

unsigned int ic_b2c_generate_functions(struct ic_kludge *kludge, FILE *f){
    if( ! kludge ){
        puts("ic_b2d_generate_functions: kludge was null");
        return 1;
    }

    if( ! f ){
        puts("ic_b2d_generate_functions: file was null");
        return 1;
    }

    puts("ic_b2d_generate_functions: unimplemented");
    return 1;
}

unsigned int ic_b2c_generate_entry(struct ic_kludge *kludge, FILE *f){
    if( ! kludge ){
        puts("ic_b2d_generate_entry: kludge was null");
        return 1;
    }

    if( ! f ){
        puts("ic_b2d_generate_entry: file was null");
        return 1;
    }

    puts("ic_b2d_generate_entry: unimplemented");
    return 1;
}

