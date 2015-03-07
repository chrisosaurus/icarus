#include <stdio.h> /* fopen, puts */
#include <stdlib.h> /* exit */
#include <string.h> /* strcmp */

#include "../../src/read.h"

static char *expected = "type Foo\n\
    a::int\n\
    b::string\n\
end\n\
\n\
function d(i::Int)\n\
    print(i)\n\
end\n\
\n\
function d(s::String)\n\
    print(s)\n\
end\n\
\n\
function d(f::Foo)\n\
    d(f.a)\n\
    d(f.b)\n\
end\n\
\n\
function main()\n\
    let f::Foo = Foo(1 \"hello\")\n\
\n\
    d(f)\n\
end\n\
\n\
";

int main(void){
    char *filename = "example/simple.ic";
    char *slurped_results;
    FILE *f;

    /* check file exists */
    f = fopen(filename, "r");
    if( ! f ){
        puts("Could not open file for reading");
        perror("test_read");
        exit(1);
    }
    fclose(f);

    slurped_results = read_slurp(filename);
    if( ! slurped_results ){
        puts("read_slurp failed");
        exit(1);
    }

    if( strcmp(slurped_results, expected) ){
        puts("strcmp failed, unexpected results");
        printf("got\n%s\n", slurped_results);
        printf("expected:\n%s\n", expected);
        exit(1);
    }

    free(slurped_results);
    return 0;
}

