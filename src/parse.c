#include <stdio.h> /* puts, printf */
#include <string.h> /* strchr, strncmp */

#include "lexer.h"
#include "ast.h"
#include "parse.h"

#ifndef DEBUG_PARSE
#define DEBUG_PARSE
#endif

#define LENGTH(x) (sizeof(x) / sizeof(x[0]))

/* ignore unused parameter warnings */
#pragma GCC diagnostic ignored "-Wunused-parameter"

/* return length of token starting at source[i] */
static unsigned int ic_parse_token_len(char *source, unsigned int i);

/* advance i by one token with token length dist */
static void ic_parse_token_advance(unsigned int *i, unsigned int dist);

struct ic_field * ic_parse_field(struct ic_tokens *tokens, unsigned int *i){
    /* the field we build and return */
    struct ic_field *field = 0;
    /* name component char* and len*/
    char *name = 0;
    unsigned int name_len = 0;
    /* type component char* and len */
    char *type = 0;
    unsigned int type_len = 0;
    /* len we use for the sep */
    unsigned int sep_len = 0;

    if( ! tokens ){
        puts("ic_parse_field: tokens was null");
        return 0;
    }

    if( ! i ){
        puts("ic_parse_field: i was null");
        return 0;
    }

    /* capture field name
     * a::Int
     * `a` is the name
     */
    name = &(tokens->tokens[*i]);
    name_len = ic_parse_token_len(tokens->tokens, *i);
    if( ! name_len ){
        puts("ic_parse_field: no name token found");
        return 0;
    }

    /* advance past name */
    ic_parse_token_advance(i, name_len);


    /* check for separator `::` */
    sep_len = ic_parse_token_len(tokens->tokens, *i);
    if( ! sep_len ){
        puts("ic_parse_field: no sep (::) token len found");
        return 0;
    }
    if( sep_len != 2 || strncmp("::", &(tokens->tokens[*i]), sep_len) ){
        puts("ic_parse_field: no sep (::) token found");
        return 0;
    }

    /* advance past separator */
    ic_parse_token_advance(i, sep_len);


    /* capture field type
     * a::Int
     * `Int` is the type
     */
    type = &(tokens->tokens[*i]);
    type_len = ic_parse_token_len(tokens->tokens, *i);
    if( ! type_len ){
        puts("ic_parse_field: no type token found");
        return 0;
    }

    /* advance past type */
    ic_parse_token_advance(i, type_len);

    /* allocate and initialise new field */
    field = ic_field_new(name, name_len, type, type_len);
    if( ! field ){
        puts("ic_parse_field: call to ic_field_new failed");
        return 0;
    }

    return field;
}

struct ic_decl * ic_parse_type_decl(struct ic_tokens *tokens, unsigned int *i){
    unsigned int dist = 0;
    /* parsed field */
    struct ic_field *field = 0;
    /* our resulting ic_decl */
    struct ic_decl *decl = 0;
    /* our tdecl within the decl */
    struct ic_type_decl *tdecl = 0;

#ifdef DEBUG_PARSE
    puts("ic_parse_type_decl called");
#endif

    if( ! tokens ){
        puts("ic_parse_type_decl: tokens was null");
        return 0;
    }

    /* check we really are at a `type` token */
    dist = ic_parse_token_len(tokens->tokens, *i);
    if( dist != 4 || strncmp("type", &(tokens->tokens[*i]), 4) ){
        printf("ic_parse_type_decl: expected 'type', encountered '%.*s'\n",
                dist,
                &(tokens->tokens[*i]));
        return 0;
    }

    /* step over `type` keyword */
    ic_parse_token_advance(i, dist);

    /* allocate and init our decl */
    decl = ic_decl_new(type_decl);
    if( ! decl ){
        puts("ic_parse_type_decl: call to ic_decl_new failed");
        return 0;
    }

    /* fetch our tdecl from within decl */
    tdecl = ic_decl_get_tdecl(decl);
    if( ! tdecl ){
        puts("ic_parse_type_decl: call to ic_decl_get_tdecl failed");
        return 0;
    }

    /* get our type name dist */
    dist = ic_parse_token_len(tokens->tokens, *i);
    /* initialise our tdecl */
    if( ic_type_decl_init(tdecl, &(tokens->tokens[*i]), dist) ){
        puts("ic_parse_type_decl: call to ic_type_decl_init failed");
        return 0;
    }

#ifdef DEBUG_PARSE
    printf("ic_parse_type_decl: our name is '%.*s'\n",
            dist,
            &(tokens->tokens[*i]));
#endif

    /* step over name */
    ic_parse_token_advance(i, dist);

    /* iterate through all tokens */
    for( ; tokens->tokens[*i] != '\0' && *i < tokens->len ; ){
        dist = ic_parse_token_len(tokens->tokens, *i);

#ifdef DEBUG_PARSE
        printf("ic_parse_token_type_decl: inspecting token '%.*s'\n",
                dist,
                &(tokens->tokens[*i]) );
#endif

        /* we keep stepping through loop until we find an
         * `end` token
         * note that this means `end` is a reserved word
         */
        if( dist == 3 &&
            ! strncmp( &(tokens->tokens[*i]), "end", 3) ){
            printf("ic_parse_token_type_decl: found end of string token '%.*s'\n",
                    dist,
                    &(tokens->tokens[*i]) );

            /* step over `end` token */
            ic_parse_token_advance(i, dist);

            /* return our result */
            return decl;
        }

        /* otherwise this is a field
         * parse it
         */
        field = ic_parse_field(tokens, i);
        if( ! field ){
            puts("ic_parse_type_decl: call to ic_parse_field failed");
            return 0;
        }

        /* and store it */
        if( ic_type_decl_add_field(tdecl, field) ){
            puts("ic_parse_type_decl: call to ic_type_decl_add_field failed");
            return 0;
        }

        /* increment of i is handled by ic_parse_field */
    }

    /* this means we ran out of tokens
     * this is an error case as `end` should cause the
     * successful return
     */
    return 0;
}

struct ic_decl * ic_parse_enum_decl(struct ic_tokens *tokens, unsigned int *i){
#ifdef DEBUG_PARSE
    puts("ic_parse_enum_decl called");
#endif

    puts("ic_parse_enum_decl: UNIMPLEMENTED");

    /* FIXME */
    return 0;
}

struct ic_decl * ic_parse_union_decl(struct ic_tokens *tokens, unsigned int *i){
#ifdef DEBUG_PARSE
    puts("ic_parse_union_decl called");
#endif

    puts("ic_parse_union_decl: UNIMPLEMENTED");

    /* FIXME */
    return 0;
}

struct ic_decl * ic_parse_func_decl(struct ic_tokens *tokens, unsigned int *i){
    unsigned int dist = 0;
    /* our argument */
    struct ic_field *arg = 0;
    /* our resulting ic_decl */
    struct ic_decl *decl = 0;
    /* our tdecl within the decl */
    struct ic_func_decl *fdecl = 0;

#ifdef DEBUG_PARSE
    puts("ic_parse_func_decl called");
#endif

    if( ! tokens ){
        puts("ic_parse_func_decl: tokens was null");
        return 0;
    }

    /* check we really are at a `function` token */
    dist = ic_parse_token_len(tokens->tokens, *i);
    if( dist != 8 || strncmp("function", &(tokens->tokens[*i]), 4) ){
        printf("ic_parse_func_decl: expected 'function', encountered '%.*s'\n",
                dist,
                &(tokens->tokens[*i]));
        return 0;
    }

    /* step over `function` keyword */
    ic_parse_token_advance(i, dist);

    /* allocate and init our decl */
    decl = ic_decl_new(func_decl);
    if( ! decl ){
        puts("ic_parse_func_decl: call to ic_decl_new failed");
        return 0;
    }

    /* fetch our fdecl from within decl */
    fdecl = ic_decl_get_fdecl(decl);
    if( ! fdecl ){
        puts("ic_parse_func_decl: call to ic_decl_get_fdecl failed");
        return 0;
    }

    /* get our function name dist */
    dist = ic_parse_token_len(tokens->tokens, *i);
    /* initialise our fdecl */
    if( ic_func_decl_init(fdecl, &(tokens->tokens[*i]), dist) ){
        puts("ic_parse_func_decl: call to ic_func_decl_init failed");
        return 0;
    }

#ifdef DEBUG_PARSE
    printf("ic_parse_func_decl: our name is '%.*s'\n",
            dist,
            &(tokens->tokens[*i]));
#endif

    /* step over name */
    ic_parse_token_advance(i, dist);


    /* parse arguments */
    /* opening bracket */
    dist = ic_parse_token_len(tokens->tokens, *i);
    if( dist != 1 ||
        strncmp("(", &(tokens->tokens[*i]), 1) ){
        printf("ic_parse_func_decl: expected '(', found '%.*s'\n",
                dist,
                &(tokens->tokens[*i]) );
        return 0;
    }
    /* step over opening bracket */
    ic_parse_token_advance(i, dist);

    /* iterate until closing bracket
     * parsing arguments
     */
    for( ; tokens->tokens[*i] != '\0' && *i < tokens->len ; ){
        /* get dist */
        dist = ic_parse_token_len(tokens->tokens, *i);

        /* check for closing brace */
        if( dist == 1 && ! strncmp(")", &(tokens->tokens[*i]), 1) ){
            /* if we have found our closing bracket then
             * argument parsing is complete
             */
            break;
        }

        /* parse argument */
        arg = ic_parse_field(tokens, i);
        if( ! arg ){
            puts("ic_parse_func_decl: call to ic_parse_field failed");
            return 0;
        }

        /* save it */
        if( ic_func_decl_add_arg(fdecl, arg) ){
            puts("ic_parse_func_decl: call to if_func_decl_add_arg failed");
            return 0;
        }
    }

    /* step over closing bracket */
    ic_parse_token_advance(i, dist);


    /* FIXME parse body */

    /* iterate through all tokens
     * until `end`
     */
    for( ; tokens->tokens[*i] != '\0' && *i < tokens->len ; ){
        dist = ic_parse_token_len(tokens->tokens, *i);

#ifdef DEBUG_PARSE
        printf("ic_parse_token_func_decl: inspecting token '%.*s'\n",
                dist,
                &(tokens->tokens[*i]) );
#endif

        /* we keep stepping through loop until we find an
         * `end` token
         * note that this means `end` is a reserved word
         */
        if( dist == 3 &&
            ! strncmp( &(tokens->tokens[*i]), "end", 3) ){
            printf("ic_parse_token_func_decl: found end of string token '%.*s'\n",
                    dist,
                    &(tokens->tokens[*i]) );

            /* step over `end` token */
            ic_parse_token_advance(i, dist);

            /* return our result */
            return decl;
        }

        /* FIXME
         * for now we just skip merrily over function body
         */
        ic_parse_token_advance(i, dist);
    }

    /* this means we ran out of tokens
     * this is an error case as `end` should cause the
     * successful return
     */
    puts("ic_parse_func_decl: ran out of tokens");
    return 0;
}

/* an entry in the parse table showing the
 * length and token string that must be match
 * and if a match is found, the function to dispatch to
 */
struct ic_parse_table_entry {
    unsigned int len;
    char *token;
    struct ic_decl * (*func)(struct ic_tokens *tokens, unsigned int *i);
} ic_parse_table [] = {
    /* len    token       function    */
    {  4,     "type",     ic_parse_type_decl  },
    {  4,     "enum",     ic_parse_enum_decl  },
    {  5,     "union",    ic_parse_union_decl },
    {  8,     "function", ic_parse_func_decl  }
};

struct ic_ast * ic_parse(struct ic_tokens *tokens){
    /* offset into tokens */
    unsigned int i = 0;
    /* length of current token */
    unsigned int dist = 0;

    /* offset into ic_parse_table */
    unsigned int pt_offset = 0;

    /* function to dispatch to */
    struct ic_decl * (*func)(struct ic_tokens *tokens, unsigned int *i) = 0;

    /* return from call to func */
    struct ic_decl *ret = 0;

    /* our eventual return value */
    struct ic_ast *ast = 0;

    /* allocate and initialise our ast */
    ast = ic_ast_new();
    if( ! ast ){
        puts("ic_parse: call to ic_ast_new failed");
        return 0;
    }

    /* possible leading tokens:
     * type
     * enum
     * union
     * function
     */

    /* step through tokens until consumed */
    for( i=0; i < tokens->len; ){
        /* find length of token */
        dist = ic_parse_token_len(tokens->tokens, i);

#ifdef DEBUG_PARSE
        printf( "considering token '%.*s' with distance '%u'\n",
                dist,
                &(tokens->tokens[i]),
                dist );
#endif

        /* we clear func each time so at loop exit we can
         * check if func is set, and if so we know we made a match
         * if func is still 0 at end of loop we know we did not
         * match the token and should signal an error
         */
        func = 0;

        for( pt_offset=0; pt_offset < LENGTH(ic_parse_table); ++pt_offset ){
#ifdef DEBUG_PARSE
            printf( "comparing token token '%.*s' (%u) with parse_table entry '%.*s' (%u)\n",
                    dist,
                    &(tokens->tokens[i]),
                    dist,
                    ic_parse_table[pt_offset].len,
                    ic_parse_table[pt_offset].token,
                    ic_parse_table[pt_offset].len
                  );
#endif

            if( dist == ic_parse_table[pt_offset].len
                && ! strncmp( &(tokens->tokens[i]), ic_parse_table[pt_offset].token, dist )
              ){
#ifdef DEBUG_PARSE
                printf( "match found! token '%.*s' with parse_table entry '%.*s'\n",
                        dist,
                        &(tokens->tokens[i]),
                        ic_parse_table[pt_offset].len,
                        ic_parse_table[pt_offset].token );
#endif

                func = ic_parse_table[pt_offset].func;
                if( ! func ){
                    printf( "ic_parse: Error matched with '%.*s' but parse table function was null, bailing\n",
                            ic_parse_table[pt_offset].len,
                            ic_parse_table[pt_offset].token );
                    return 0;
                }

                /* call found function and store result to save */
                ret = func(tokens, &i);
                if( ! ret ){
                    /* presume parsing failed */
                    puts("ic_parse: error when calling parsing function");
                    return 0;
                }

                /* store ret in our ast */
                if( ic_ast_append(ast, ret) == -1 ){
                    puts("ic_parse: call to ic_ast_append failed");
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
        if( func ){
            continue;
        }

        /* no match found */
        printf("ic_parse: unsupported string '%.*s' found, bailing\n",
                dist,
                &(tokens->tokens[i]) );
        return 0;
    }

#ifdef DEBUG_PARSE
    puts("ic_parse finished, bailing");
#endif

    return ast;
}

/* return length of token starting at source[i] */
static unsigned int ic_parse_token_len(char *source, unsigned int i){
    /* position of next space in tokens */
    char *space = 0;

    /* length of current token */
    unsigned int dist = 0;

    if( ! source ){
        return 0;
    }

    /* find next space */
    space = strchr( &(source[i]), ' ' );

    if( ! space ){
        /* character not found */
        return 0;
    }

    /* find length of token
     * which is the distance from the start to the space
     */
    dist = space - &(source[i]);

    return dist;
}

/* advance i by one token with token length dist */
static void ic_parse_token_advance(unsigned int *i, unsigned int dist){
    if( ! i ){
        return;
    }

    /* dist + 1 to account for space */
    *i += (dist + 1);
}


