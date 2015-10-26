#include <stdio.h> /* puts, printf */
#include <stdlib.h> /* exit, free */
#include <string.h>

#include "../../src/read/read.h" /* ic_read_slurp */
#include "../../src/lex/lexer.h" /* ic_lex */
#include "../../src/parse/parse.h" /* ic_parse */
#include "../../src/parse/data/ast.h" /* ic_ast structure */
#include "../../src/analyse/analyse.h" /* ic_kludge */

int main(int argc, char **argv){
    char *filename = 0, *source = 0, *core_source = 0;
    struct ic_token_list *token_list = 0, *core_token_list = 0;
    struct ic_ast *ast = 0, *core_ast = 0;
    struct ic_kludge *kludge = 0;

    if( argc < 2 ){
        puts("No source file specified");
        exit(1);
    } else if( argc > 2 ){
        puts("Too many arguments supplied, only source file was expected");
        exit(1);
    }

    filename = argv[1];

    kludge = ic_kludge_new();
    if( ! kludge ){
        puts("call to ic_kludge_new failed");
        exit(1);
    }

    /* populate from core.ic */
    core_source = ic_read_slurp("src/stdlib/core.ic");
    if( ! core_source ){
        puts("slurping failed for src/stdlib/core.ic");
        exit(1);
    }

    core_token_list = ic_lex("src/stdlib/core.ic", core_source);
    if( ! core_token_list ){
        puts("lexing failed for src/stdlib/core.ic");
        exit(1);
    }

    core_ast = ic_parse(core_token_list);
    if( ! core_ast ){
        puts("parsing failed for src/stdlib/core.ic");
        exit(1);
    }

    if( ! ic_kludge_populate(kludge, core_ast) ){
        puts("call to ic_kludge_populate failed for src/stdlib/core.ic");
        exit(1);
    }


    source = ic_read_slurp(filename);
    if( ! source ){
        puts("test_analyse_invalid: slurping failed");
        exit(1);
    }

    token_list = ic_lex(filename, source);
    if( ! token_list ){
        puts("test_analyse_invalid: lexing failed");
        exit(1);
    }

    ast = ic_parse(token_list);
    if( ! ast ){
        puts("test_analyse_invalid: parsing failed");
        exit(1);
    }

    if( ! ic_kludge_populate(kludge, ast) ){
        puts("call to ic_kludge_populate failed");
        exit(1);
    }

    if( ! ic_analyse(kludge) ){
        puts("analysis failed");
        exit(1);
    }

    puts("(partial) analyse was a success");

    /* clean up time
     * this will destroy both the kludge and the ast (via aast)
     */
    if( ! ic_kludge_destroy(kludge, 1) ){
        puts("main: ic_kludge_destroy call failed");
    }

    /* FIXME is token_list cleaned up correctly ? */
#if 0
    if( ! ic_token_list_destroy(token_list, 1) ){
        puts("test_analyse_invalid: ic_token_list_destroy failed");
    }
#endif

    free(source);

    return 0;
}

