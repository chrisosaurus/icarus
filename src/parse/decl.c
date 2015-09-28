#include <stdio.h> /* puts, printf */
#include <string.h> /* strncmp */
#include <stdlib.h> /* free */

#include "parse.h"

/* ignore unused parameter warnings */
#pragma GCC diagnostic ignored "-Wunused-parameter"

struct ic_decl * ic_parse_type_decl(struct ic_old_tokens *tokens, unsigned int *i){
    unsigned int dist = 0;
    /* parsed field */
    struct ic_field *field = 0;
    /* our resulting ic_decl */
    struct ic_decl *decl = 0;
    /* our tdecl within the decl */
    struct ic_type_decl *tdecl = 0;
    /* integer for ic_parse_this_is_not_the_end */
    int ret = 0;

#ifdef DEBUG_PARSE
    puts("ic_parse_type_decl called");
#endif

    if( ! tokens ){
        puts("ic_parse_type_decl: tokens was null");
        return 0;
    }

    /* check we really are at a `type` token */
    dist = ic_parse_token_length(tokens->tokens, *i);
    if( dist != 4 || strncmp("type", &(tokens->tokens[*i]), 4) ){
        printf("ic_parse_type_decl: expected 'type', encountered '%.*s'\n",
                dist,
                &(tokens->tokens[*i]));
        return 0;
    }

    /* step over `type` keyword */
    ic_parse_token_advance(i, dist);

    /* allocate and init our decl */
    decl = ic_decl_new(ic_decl_type_decl);
    if( ! decl ){
        puts("ic_parse_type_decl: call to ic_decl_new failed");
        return 0;
    }

    /* fetch our tdecl from within decl */
    tdecl = ic_decl_get_tdecl(decl);
    if( ! tdecl ){
        puts("ic_parse_type_decl: call to ic_decl_get_tdecl failed");
        free(decl);
        return 0;
    }

    /* get our type name dist */
    dist = ic_parse_token_length(tokens->tokens, *i);
    /* initialise our tdecl */
    if( ! ic_type_decl_init(tdecl, &(tokens->tokens[*i]), dist) ){
        puts("ic_parse_type_decl: call to ic_type_decl_init failed");
        free(decl);
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
    /* keep iterating through all the tokens until we find an unexpected 'end' */
    while( (ret = ic_parse_this_is_not_the_end(tokens, i)) > 0 ){
        dist = ic_parse_token_length(tokens->tokens, *i);

#ifdef DEBUG_PARSE
        printf("ic_parse_token_type_decl: inspecting token '%.*s'\n",
                dist,
                &(tokens->tokens[*i]) );
#endif

        /* otherwise this is a field
         * parse it
         */
        field = ic_parse_field(tokens, i);
        if( ! field ){
            puts("ic_parse_type_decl: call to ic_parse_field failed");

            /* free decl and all contents */
            if( ! ic_decl_destroy(decl, 1) ){
                puts("ic_parse_type_decl: in error tidyup call to ic_decl_destroy failed");
            }
            return 0;
        }

        /* and store it */
        if( ! ic_type_decl_add_field(tdecl, field) ){
            puts("ic_parse_type_decl: call to ic_type_decl_add_field failed");

            /* free field and all contents */
            if( ! ic_field_destroy(field, 1) ){
                puts("ic_parse_type_decl: in error tidyup call to ic_field_destroy failed");
            }

            /* free decl and all contents */
            if( ! ic_decl_destroy(decl, 1) ){
                puts("ic_parse_type_decl: in error tidyup call to ic_decl_destroy failed");
            }
            return 0;
        }

        /* increment of i is handled by ic_parse_field */
    }

    /* if ret is 0 then this means we hit `end` as
     * we expected and our type decl is finished
     */
    if( ! ret ){
        /* victory ! */
        return decl;
    }

    /* if ret is not 0 then
     * this means we ran out of tokens
     * this is an error case as `end` should cause the
     * successful return
     */
    puts("ic_parse_type_decl: call to ic_parse_this_is_not_the_end encountered an error");

    /* free decl and all contents */
    if( ! ic_decl_destroy(decl, 1) ){
        puts("ic_parse_type_decl: in error tidyup call to ic_decl_destroy failed");
    }
    return 0;
}

struct ic_decl * ic_parse_enum_decl(struct ic_old_tokens *tokens, unsigned int *i){
#ifdef DEBUG_PARSE
    puts("ic_parse_enum_decl called");
#endif

    puts("ic_parse_enum_decl: UNIMPLEMENTED");

    /* FIXME */
    return 0;
}

struct ic_decl * ic_parse_union_decl(struct ic_old_tokens *tokens, unsigned int *i){
#ifdef DEBUG_PARSE
    puts("ic_parse_union_decl called");
#endif

    puts("ic_parse_union_decl: UNIMPLEMENTED");

    /* FIXME */
    return 0;
}

struct ic_decl * ic_parse_func_decl(struct ic_old_tokens *tokens, unsigned int *i){
    unsigned int dist = 0;
    /* our argument */
    struct ic_field *arg = 0;
    /* our resulting ic_decl */
    struct ic_decl *decl = 0;
    /* our tdecl within the decl */
    struct ic_func_decl *fdecl = 0;
    /* return of this_is_not_the_end */
    int ret  = 0;
    /* pointer used for each stmt within body */
    struct ic_stmt *stmt = 0;

    /* fn name(args...) -> return_type
     *      body...
     * end
     *
     * # default to void return type
     * fn name(args...)
     *      body...
     * end
     */

#ifdef DEBUG_PARSE
    puts("ic_parse_func_decl called");
#endif

    if( ! tokens ){
        puts("ic_parse_func_decl: tokens was null");
        return 0;
    }

    /* check we really are at a `function` token */
    dist = ic_parse_token_length(tokens->tokens, *i);
    if( (dist != 8 || strncmp("function", &(tokens->tokens[*i]), 8)) &&
        (dist != 2 || strncmp("fn",       &(tokens->tokens[*i]), 2)) ){
        printf("ic_parse_func_decl: expected 'function' or 'fn', encountered '%.*s'\n",
                dist,
                &(tokens->tokens[*i]));
        return 0;
    }

    /* step over `function` keyword */
    ic_parse_token_advance(i, dist);

    /* allocate and init our decl */
    decl = ic_decl_new(ic_decl_func_decl);
    if( ! decl ){
        puts("ic_parse_func_decl: call to ic_decl_new failed");
        return 0;
    }

    /* fetch our fdecl from within decl */
    fdecl = ic_decl_get_fdecl(decl);
    if( ! fdecl ){
        puts("ic_parse_func_decl: call to ic_decl_get_fdecl failed");
        free(decl);
        return 0;
    }

    /* get our function name dist */
    dist = ic_parse_token_length(tokens->tokens, *i);

    /* initialise our fdecl */
    if( ! ic_func_decl_init(fdecl, &(tokens->tokens[*i]), dist) ){
        puts("ic_parse_func_decl: call to ic_func_decl_init failed");
        free(decl);
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
    dist = ic_parse_token_length(tokens->tokens, *i);
    if( dist != 1 ||
        strncmp("(", &(tokens->tokens[*i]), 1) ){
        printf("ic_parse_func_decl: expected '(', found '%.*s'\n",
                dist,
                &(tokens->tokens[*i]) );
        free(decl);
        return 0;
    }
    /* step over opening bracket */
    ic_parse_token_advance(i, dist);

    /* iterate until closing bracket
     * parsing arguments
     */
    for( ; tokens->tokens[*i] != '\0' && *i < tokens->len ; ){
        /* get dist */
        dist = ic_parse_token_length(tokens->tokens, *i);

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
            free(decl);
            return 0;
        }

        /* save it */
        if( ! ic_func_decl_add_arg(fdecl, arg) ){
            puts("ic_parse_func_decl: call to if_func_decl_add_arg failed");
            free(decl);
            return 0;
        }
    }

    /* step over closing bracket */
    ic_parse_token_advance(i, dist);

    /* parse return type
     * check if we have a return type arrow ->
     *
     * if we do, consume it, and then parse return type
     *
     * if we do not, then there is no return type and we set this function as Void
     * then begin parsing the body
     */
    if( ! ic_parse_check_token("->", 2, tokens->tokens, i) ){
        /* we found a type arrow and have skipped over it
         * parse return type
         */
        dist = ic_parse_token_length(tokens->tokens, *i);
        if( ! dist ) {
            puts("ic_parse_func_decl: call to if_parse_token_length failed when looking for arg type");
            free(decl);
            return 0;
        }

        /* add to our fdecl */
        if( ! ic_func_decl_set_return( fdecl, &(tokens->tokens[*i]), dist ) ){
            puts("ic_parse_func_decl: call to ic_func_decl_set_return failed");
            free(decl);
            return 0;
        }

        /* advance over our type */
        ic_parse_token_advance(i, dist);
    } else {
        /* add Void type to our fdecl */
        if( ! ic_func_decl_set_return(fdecl, "Void", 4) ){
            puts("ic_parse_func_decl: call to ic_func_decl_set_return failed for 'Void'");
            free(decl);
            return 0;
        }
    }

    /* parse body */

    /* iterate through all tokens
     * until `end`
     */
    while( (ret = ic_parse_this_is_not_the_end(tokens, i)) > 0 ){
        dist = ic_parse_token_length(tokens->tokens, *i);

#ifdef DEBUG_PARSE
        printf("ic_parse_token_func_decl: inspecting token '%.*s'\n",
                dist,
                &(tokens->tokens[*i]) );
#endif

        /* leave stmt parsing up to the experts */
        stmt = ic_parse_stmt(tokens, i);
        if( ! stmt ){
            puts("ic_parse_func_decl: call to ic_parse_stmt failed");
            free(decl);
            return 0;
        }

        /* save to our body */
        if( ! ic_func_decl_add_stmt(fdecl, stmt) ){
            puts("ic_parse_func_decl: call to ic_func_decl_add_stmt failed");
            free(decl);
            return 0;
        }

    }

    /* if ret is 0 then we found an `end` token */
    if( ! ret ){
        /* victory ! */
        return decl;
    }

    /* this means we ran out of tokens
     * or that some other error occured
     *
     * this is an error case as `end` should cause the
     * successful return
     */
    puts("ic_parse_func_decl: error occurred in ic_parse_this_is_not_the_end");
    free(decl);
    return 0;
}


