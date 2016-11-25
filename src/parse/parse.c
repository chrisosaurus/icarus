#include <stdio.h>  /* puts, printf */
#include <stdlib.h> /* free */
#include <string.h> /* strchr, strncmp */

#include "../lex/lexer.h"
#include "data/ast.h"
#include "parse.h"
#include "permissions.h"

/* indent level defined as 4 spaces */
#define INDENT_PER_LEVEL 4

/* please note that only a small subset of ic_parse_* is implemented
 * here, many of the ic_parse_* functions are not implemented in parse.c
 * and have instead been moved into src/parse/ source files
 * based on their return types
 */

/* an entry in the parse table showing the
 * length and token string that must be match
 * and if a match is found, the function to dispatch to
 */
static struct ic_parse_table_entry {
    enum ic_token_id id;
    struct ic_decl *(*func)(struct ic_token_list *token_list);
} ic_parse_table[] = {
    /* token          function    */
    {IC_BUILTIN, ic_parse_decl_builtin},
    {IC_FUNC, ic_parse_decl_func},
    {IC_TYPE, ic_parse_decl_type},
    {IC_ENUM, ic_parse_decl_enum},
    {IC_UNION, ic_parse_decl_union},
};

struct ic_ast *ic_parse(struct ic_token_list *token_list) {

    /* offset into ic_parse_table */
    unsigned int pt_offset = 0;

    /* function to dispatch to */
    struct ic_decl *(*func)(struct ic_token_list * token_list) = 0;

    /* return from call to func */
    struct ic_decl *ret = 0;

    /* our eventual return value */
    struct ic_ast *ast = 0;

    /* current token */
    struct ic_token *token = 0;

    if (!token_list) {
        puts("ic_parse: token_list was null");
        return 0;
    }

    /* allocate and initialise our ast */
    ast = ic_ast_new();
    if (!ast) {
        puts("ic_parse: call to ic_ast_new failed");
        return 0;
    }

    /* possible leading tokens:
     * type
     * enum
     * union
     * function
     */

    /* step through tokens until we run out */
    while ((token = ic_token_list_peek_important(token_list))) {
        /* we clear func each time so at loop exit we can
         * check if func is set, and if so we know we made a match
         * if func is still 0 at end of loop we know we did not
         * match the token and should signal an error
         */
        func = 0;

        for (pt_offset = 0; pt_offset < LENGTH(ic_parse_table); ++pt_offset) {
            if (token->id == ic_parse_table[pt_offset].id) {

                func = ic_parse_table[pt_offset].func;
                if (!func) {
                    fputs("ic_parse: Error matched with:", stdout);
                    ic_token_id_print_debug(stdout, token->id);
                    puts("  but parse table function was null, bailing");

                    /* free ast and all contents */
                    if (!ic_ast_destroy(ast, 1)) {
                        puts("ic_parse: call to ic_ast_destroy failed in error case tidy up");
                    }
                    return 0;
                }

                /* call found function and store result to save */
                ret = func(token_list);
                if (!ret) {
                    /* presume parsing failed */
                    puts("ic_parse: error when calling parsing function");

                    /* free ast and all contents */
                    if (!ic_ast_destroy(ast, 1)) {
                        puts("ic_parse: call to ic_ast_destroy failed in error case tidy up");
                    }
                    return 0;
                }

                /* store ret in our ast */
                if (-1 == ic_ast_append(ast, ret)) {
                    puts("ic_parse: call to ic_ast_append failed");

                    /* free ast and all contents */
                    if (!ic_ast_destroy(ast, 1)) {
                        puts("ic_parse: call to ic_ast_destroy failed in error case tidy up");
                    }
                    return 0;
                }

                /* exit inner for loop as this token is done */
                break;
            }

            /* FIXME
             * eventually we can rely on the parse entires being in order and bail out early
             * for now we will just keep going
             */
        }

        /* if func is set then it means we had a match */
        if (func) {
            continue;
        }

        /* no match found */
        fputs("ic_parse: unsupported token_id:", stdout);
        ic_token_id_print_debug(stdout, token->id);
        puts(" found, bailing");

        /* free ast and all contents */
        if (!ic_ast_destroy(ast, 1)) {
            puts("ic_parse: call to ic_ast_destroy failed in error case tidy up");
        }
        return 0;
    }

#ifdef DEBUG_PARSE
    puts("ic_parse finished");
#endif

    return ast;
}

/* print levels worth of indent levels
 * an indent level is defined in parse.c
 */
void ic_parse_print_indent(FILE *fd, unsigned int levels) {
    /* print an empty string with width
     * levels * INDENT_PER_LEVELS
     */
    fprintf(fd, "%*s", levels * INDENT_PER_LEVEL, "");
}

unsigned int ic_parse_permissions(struct ic_token_list *token_list) {
    unsigned int permissions = 0;
    struct ic_token *token = 0;

    if (!token_list) {
        puts("ic_parse_permissions: token_list was null");
        return 0;
    }

    /* check for permissions */
    token = ic_token_list_peek_important(token_list);
    if (!token) {
        puts("ic_parse_stmt_let: failed to peek at permissions token");
        return 0;
    }

    /* if we find it */
    if (ic_token_ispermission(token)) {
        /* consume it */
        token = ic_token_list_next_important(token_list);
        if (!token) {
            puts("ic_parse_stmt_let: failed to get permissions token");
            return 0;
        }

        /* set permissions from it */
        permissions = ic_parse_perm(token->id);
    } else {

        /* otherwise, use defaults */
        permissions = ic_parse_perm_default();
    }

    return permissions;
}
