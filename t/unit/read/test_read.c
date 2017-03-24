#include <stdio.h>  /* fopen, puts */
#include <stdlib.h> /* exit */
#include <string.h> /* strcmp */

#include "../../../src/read/read.h"

static char *expected = "# user defined type with 2 fields, a Sint and a String\n\
type Foo\n\
    a::Sint\n\
    b::String\n\
end\n\
\n\
fn println(f::Foo)\n\
    print(\"Foo{\")\n\
    print(\"a:\")\n\
    print(f.a)\n\
    print(\", b:\")\n\
    print(f.b)\n\
    println(\"}\")\n\
end\n\
\n\
# simple function to test return values and branching\n\
fn maybe_add_one(b::Bool, i::Sint) -> Sint\n\
    if b\n\
        return i+1\n\
    else\n\
        return i\n\
    end\n\
end\n\
\n\
# entry point for program\n\
fn main()\n\
    let f::Foo = Foo(maybe_add_one(True, 1), \"hello\")\n\
    println(f)\n\
\n\
    let x = 6\n\
    if x > 14\n\
        println(\"x > 14\")\n\
    else\n\
        println(\"x <= 14\")\n\
    end\n\
end\n\
\n\
";

int main(void) {
    char *filename = "example/simple.ic";
    char *slurped_results;
    FILE *f;

    /* check file exists */
    f = fopen(filename, "r");
    if (!f) {
        puts("Could not open file for reading");
        perror("test_read");
        exit(1);
    }
    fclose(f);

    slurped_results = ic_read_slurp_filename(filename);
    if (!slurped_results) {
        puts("read_slurp failed");
        exit(1);
    }

    if (strcmp(slurped_results, expected)) {
        puts("strcmp failed, unexpected results");
        printf("got\n%s\n", slurped_results);
        printf("expected:\n%s\n", expected);
        exit(1);
    }

    puts("test_read success");
    free(slurped_results);
    return 0;
}
