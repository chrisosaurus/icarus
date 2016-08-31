#include <stdio.h>  /* puts, printf */
#include <stdlib.h> /* exit, free */
#include <string.h>

#include "../../src/analyse/analyse.h" /* ic_kludge */
#include "../../src/lex/lexer.h"       /* ic_lex */
#include "../../src/parse/data/ast.h"  /* ic_ast structure */
#include "../../src/parse/parse.h"     /* ic_parse */
#include "../../src/read/read.h"       /* ic_read_slurp */
#include "../../src/transform/transform.h"

/* make a new kludge
 *
 * returns * on success
 * exit(1)'s on failure
 */
struct ic_kludge *test_custom_helper_gen_kludge(void);

/* load core.ic into kludge
 *
 * FIXME TODO will leak memory
 *
 * returns 1 on success
 * exit(1)'s on failure
 */
unsigned int test_custom_helper_load_core(struct ic_kludge *kludge);

/* load filename into kludge
 *
 * FIXME TODO will leak memory
 *
 * returns 1 on success
 * exit(1)s on failure
 */
unsigned int test_custom_helper_load_filename(struct ic_kludge *kludge, char *filename);

/* make a new kludge
 *
 * returns * on success
 * exit(1)'s on failure
 */
struct ic_kludge *test_custom_helper_gen_kludge(void) {
    struct ic_kludge *kludge = 0;

    kludge = ic_kludge_new();

    if (!kludge) {
        puts("call to ic_kludge_new failed");
        exit(1);
    }

    return kludge;
}

/* load core.ic into kludge
 *
 * FIXME TODO will leak memory
 *
 * returns 1 on success
 * exit(1)'s on failure
 */
unsigned int test_custom_helper_load_core(struct ic_kludge *kludge) {
    char *filename = "src/stdlib/core.ic";
    return test_custom_helper_load_filename(kludge, filename);
}

/* load filename into kludge
 *
 * FIXME TODO will leak memory
 *
 * returns 1 on success
 * exit(1)s on failure
 */
unsigned int test_custom_helper_load_filename(struct ic_kludge *kludge, char *filename) {
    char *source = 0;
    struct ic_token_list *token_list = 0;
    struct ic_ast *ast = 0;

    if (!kludge) {
        puts("test_custom_helpers: kludge was null");
        exit(1);
    }

    if (!filename) {
        puts("test_custom_helpers: filename was null");
        exit(1);
    }

    source = ic_read_slurp_filename(filename);

    if (!source) {
        printf("test_custom_helpers: slurping failed for filename '%s'\n", filename);
        exit(1);
    }

    token_list = ic_lex(filename, source);
    if (!token_list) {
        printf("test_custom_helpers: lexing failed for filename '%s'\n", filename);
        exit(1);
    }

    ast = ic_parse(token_list);
    if (!ast) {
        printf("test_custom_helpers: parsing failed for filename '%s'\n", filename);
        exit(1);
    }

    if (!ic_kludge_populate(kludge, ast)) {
        printf("test_custom_helpers: call to ic_kludge_populate failed for filename '%s'\n", filename);
        exit(1);
    }

    return 1;
}
