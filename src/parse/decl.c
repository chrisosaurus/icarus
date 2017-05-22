#include <stdio.h>  /* puts, printf */
#include <stdlib.h> /* free */
#include <string.h> /* strncmp */

#include "parse.h"

struct ic_decl *ic_parse_decl_builtin(struct ic_token_list *token_list) {
    /* our resulting ic_decl */
    struct ic_decl *decl = 0;
    /* current token we are looking at */
    struct ic_token *token = 0;

#ifdef DEBUG_PARSE
    puts("ic_parse_decl_builtin called");
#endif

    if (!token_list) {
        puts("ic_parse_decl_builtin: token_list was null");
        return 0;
    }

    /* check we really are at a `builtin` token */
    token = ic_token_list_expect_important(token_list, IC_BUILTIN);
    if (!token) {
        puts("ic_parse_decl_builtin: call to ic_token_list_expect_important failed for builtin");
        return 0;
    }

    token = ic_token_list_peek_important(token_list);
    if (!token) {
        puts("ic_parse_decl_builtin: call to ic_token_list_peek_important failed for builtin");
        return 0;
    }

    switch (token->id) {
        case IC_FUNC:
            decl = ic_parse_decl_func_header(token_list);
            if (!decl) {
                puts("ic_parse_decl_builtin: call to ic_parse_decl_func_header failed");
                return 0;
            }
            if (!ic_decl_mark_builtin(decl)) {
                puts("ic_parse_decl_builtin: call to ic_decl_mark_builtin failed");
                return 0;
            }
            break;

        case IC_TYPE:
            decl = ic_parse_decl_type_struct_header(token_list);
            if (!decl) {
                puts("ic_parse_decl_builtin: call to ic_parse_decl_type_struct_header failed");
                return 0;
            }
            if (!ic_decl_mark_builtin(decl)) {
                puts("ic_parse_decl_builtin: call to ic_decl_mark_builtin failed");
                return 0;
            }
            break;

        case IC_OP:
            decl = ic_parse_decl_op(token_list);
            if (!decl) {
                puts("ic_parse_decl_builtin: call to ic_parse_decl_op failed");
                return 0;
            }
            decl->tag = ic_decl_tag_builtin_op;
            break;

        default:
            puts("ic_parse_decl_builtin: unexpected id found, none of: func, type or op");
            return 0;
            break;
    }

    return decl;
}

struct ic_decl *ic_parse_decl_type_struct(struct ic_token_list *token_list) {
    /* our resulting ic_decl */
    struct ic_decl *decl = 0;

    if (!token_list) {
        puts("ic_parse_decl_type_struct: token_list was null");
        return 0;
    }

    decl = ic_parse_decl_type_struct_header(token_list);
    if (!decl) {
        puts("ic_parse_decl_type_struct: call to ic_parse_decl_type_struct_header failed");
        return 0;
    }

    decl = ic_parse_decl_type_struct_body(token_list, decl);
    if (!decl) {
        puts("ic_parse_decl_type_struct: call to ic_parse_decl_type_struct_body failed");
        return 0;
    }

    return decl;
}

struct ic_decl *ic_parse_decl_type_struct_header(struct ic_token_list *token_list) {
    /* our resulting ic_decl */
    struct ic_decl *decl = 0;
    /* our tdecl within the decl */
    struct ic_decl_type *tdecl = 0;
    /* our tdecl_struct within the tdecl */
    struct ic_decl_type_struct *tdecl_struct = 0;
    /* current token we are looking at */
    struct ic_token *token = 0;
    /* token string */
    char *token_str = 0;
    /* token string length */
    unsigned int token_str_len = 0;

    if (!token_list) {
        puts("ic_parse_decl_type_struct_header: token_list was null");
        return 0;
    }

    /* check we really are at a `type` token */
    token = ic_token_list_expect_important(token_list, IC_TYPE);
    if (!token) {
        puts("ic_parse_decl_type_struct_header: call to ic_token_list_expect_important failed for TYPE");
        return 0;
    }

    /* allocate and init our decl */
    decl = ic_decl_new(ic_decl_tag_type);
    if (!decl) {
        puts("ic_parse_decl_type_struct_header: call to ic_decl_new failed");
        return 0;
    }

    /* fetch our tdecl from within decl */
    tdecl = ic_decl_get_tdecl(decl);
    if (!tdecl) {
        puts("ic_parse_decl_type_struct_header: call to ic_decl_get_tdecl failed");
        goto ERROR;
    }

    /* get our type name */
    token = ic_token_list_next_important(token_list);
    if (!token) {
        puts("ic_parse_decl_type_struct_header: call to ic_token_list_next failed for func name");
        goto ERROR;
    }

    token_str = ic_token_get_string(token);
    if (!token_str) {
        puts("ic_parse_decl_type_struct_header: call to ic_token_get_string failed for func name");
        goto ERROR;
    }
    token_str_len = ic_token_get_string_length(token);

    /* initialise our tdecl_struct */
    if (!ic_decl_type_init_struct(tdecl, token_str, token_str_len)) {
        puts("ic_parse_decl_type_struct_header: call to ic_decl_type_init_struct failed");
        goto ERROR;
    }

    tdecl_struct = ic_decl_type_get_struct(tdecl);
    if (!tdecl_struct) {
        puts("ic_parse_decl_type_struct_header: call to ic_decl_type_get_struct failed");
        goto ERROR;
    }

    /* if this is a 'special' type, then mark it so */
    if (!strncmp(token_str, "Void", 4)) {
        /* if this is the void type then mark it as so */
        if (!ic_decl_type_struct_mark_void(tdecl_struct)) {
            puts("ic_parse_decl_type_struct_header: call to ic_decl_type_struct_mark_void failed");
            goto ERROR;
        }
    } else if (!strncmp(token_str, "Bool", 4)) {
        /* if this is the bool type then mark it as so */
        if (!ic_decl_type_struct_mark_bool(tdecl_struct)) {
            puts("ic_parse_decl_type_struct_header: call to ic_decl_type_struct_mark_bool failed");
            goto ERROR;
        }
    } else if (!strncmp(token_str, "String", 4)) {
        /* if this is the bool type then mark it as so */
        if (!ic_decl_type_struct_mark_string(tdecl_struct)) {
            puts("ic_parse_decl_type_struct_header: call to ic_decl_type_struct_mark_string failed");
            goto ERROR;
        }
    } else if (!strncmp(token_str, "Uint", 4)) {
        /* if this is the bool type then mark it as so */
        if (!ic_decl_type_struct_mark_uint(tdecl_struct)) {
            puts("ic_parse_decl_type_struct_header: call to ic_decl_type_struct_mark_uint failed");
            goto ERROR;
        }
    } else if (!strncmp(token_str, "Sint", 4)) {
        /* if this is the bool type then mark it as so */
        if (!ic_decl_type_struct_mark_sint(tdecl_struct)) {
            puts("ic_parse_decl_type_struct_header: call to ic_decl_type_struct_mark_sint failed");
            goto ERROR;
        }
    }

    /* optionally parse type parameters */
    if (!ic_parse_decl_optional_type_params(token_list, &(tdecl->u.tstruct.type_params))) {
        puts("ic_parse_decl_struct_header: call to ic_parse_decl_optional_type_params failed");
        free(decl);
        return 0;
    }

    return decl;

ERROR:
    puts("ic_parse_decl_type_struct_header: error");
    if (decl) {
        free(decl);
    }
    return 0;
}

struct ic_decl *ic_parse_decl_type_struct_body(struct ic_token_list *token_list, struct ic_decl *decl) {
    /* parsed field */
    struct ic_field *field = 0;
    /* our tdecl within the decl */
    struct ic_decl_type *tdecl = 0;
    /* used to mark success of while loop */
    int success = 0;
    /* current token we are looking at */
    struct ic_token *token = 0;

#ifdef DEBUG_PARSE
    puts("ic_parse_decl_type called");
#endif

    if (!token_list) {
        puts("ic_parse_decl_type_struct_body: token_list was null");
        return 0;
    }

    if (!decl) {
        puts("ic_parse_decl_type_struct_body: decl was null");
        return 0;
    }

    /* fetch our tdecl from within decl */
    tdecl = ic_decl_get_tdecl(decl);
    if (!tdecl) {
        puts("ic_parse_decl_type_struct_body: call to ic_decl_get_tdecl failed");
        free(decl);
        return 0;
    }

    /* iterate through all tokens */
    /* keep iterating through all the tokens until we find an unexpected 'end' */
    while ((token = ic_token_list_peek_important(token_list))) {
        /* check if this is the end */
        if (token->id == IC_END) {
            success = 1;
            break;
        }

        /* otherwise this is a field
         * parse it
         */
        field = ic_parse_field(token_list);
        if (!field) {
            puts("ic_parse_decl_type_struct_body: call to ic_parse_field failed");

            /* free decl and all contents */
            if (!ic_decl_destroy(decl, 1)) {
                puts("ic_parse_decl_type_struct_body: in error tidyup call to ic_decl_destroy failed");
            }
            return 0;
        }

        /* and store it */
        if (!ic_decl_type_add_field(tdecl, field)) {
            puts("ic_parse_decl_type_struct_body: call to ic_decl_type_add_field failed");

            /* free field and all contents */
            if (!ic_field_destroy(field, 1)) {
                puts("ic_parse_decl_type_struct_body: in error tidyup call to ic_field_destroy failed");
            }

            /* free decl and all contents */
            if (!ic_decl_destroy(decl, 1)) {
                puts("ic_parse_decl_type_struct_body: in error tidyup call to ic_decl_destroy failed");
            }
            return 0;
        }

        /* increment of i is handled by ic_parse_field */
    }

    /* if it is then consume */
    token = ic_token_list_expect_important(token_list, IC_END);
    if (!token) {
        puts("ic_parse_decl_type_struct_body: call to ic_token_list_expect for end failed");
        return 0;
    }

    /* if success is 1 then this means we hit `end` as
     * we expected and our type decl is finished
     */
    if (success) {
        /* victory ! */
        return decl;
    }

    /* if ret is not 1 then
     * this means we ran out of tokens
     * this is an error case as `end` should cause the
     * successful return
     */
    puts("ic_parse_decl_type_struct_body: something went wrong in finding end");

    /* free decl and all contents */
    if (!ic_decl_destroy(decl, 1)) {
        puts("ic_parse_decl_type_struct_body: in error tidyup call to ic_decl_destroy failed");
    }
    return 0;
}

struct ic_decl *ic_parse_decl_enum(struct ic_token_list *token_list) {
#ifdef DEBUG_PARSE
    puts("ic_parse_decl_enum called");
#endif

    if (!token_list) {
        puts("ic_parse_decl_enum: token_list was null");
        return 0;
    }

    puts("ic_parse_decl_enum: UNIMPLEMENTED");

    /* FIXME */
    return 0;
}

struct ic_decl *ic_parse_decl_type_union(struct ic_token_list *token_list) {
    /* our resulting ic_decl */
    struct ic_decl *decl = 0;

    if (!token_list) {
        puts("ic_parse_decl_type_struct: token_list was null");
        return 0;
    }

    decl = ic_parse_decl_type_union_header(token_list);
    if (!decl) {
        puts("ic_parse_decl_type_struct: call to ic_parse_decl_type_union_header failed");
        return 0;
    }

    decl = ic_parse_decl_type_union_body(token_list, decl);
    if (!decl) {
        puts("ic_parse_decl_type_struct: call to ic_parse_decl_type_union_body failed");
        return 0;
    }

    return decl;
}

struct ic_decl *ic_parse_decl_type_union_header(struct ic_token_list *token_list) {
    /* our resulting ic_decl */
    struct ic_decl *decl = 0;
    /* our tdecl within the decl */
    struct ic_decl_type *tdecl = 0;
    /* current token we are looking at */
    struct ic_token *token = 0;

    if (!token_list) {
        puts("ic_parse_decl_type_union_header: token_list was null");
        return 0;
    }

    /* check we really are at a `union` token */
    token = ic_token_list_expect_important(token_list, IC_UNION);
    if (!token) {
        puts("ic_parse_decl_type_union_header: call to ic_token_list_expect_important failed for UNION");
        return 0;
    }

    /* allocate and init our decl */
    decl = ic_decl_new(ic_decl_tag_type);
    if (!decl) {
        puts("ic_parse_decl_type_union_header: call to ic_decl_new failed");
        return 0;
    }

    /* fetch our tdecl from within decl */
    tdecl = ic_decl_get_tdecl(decl);
    if (!tdecl) {
        puts("ic_parse_decl_type_union_header: call to ic_decl_get_tdecl failed");
        free(decl);
        return 0;
    }

    /* get our type name */
    token = ic_token_list_next_important(token_list);
    if (!token) {
        puts("ic_parse_decl_type_union_header: call to ic_token_list_next failed for func name");
        return 0;
    }

    /* initialise our tdecl */
    if (!ic_decl_type_init_union(tdecl, ic_token_get_string(token), ic_token_get_string_length(token))) {
        puts("ic_parse_decl_type_union_header: call to ic_decl_type_init_union failed");
        free(decl);
        return 0;
    }

    /* optionally parse type parameters */
    if (!ic_parse_decl_optional_type_params(token_list, &(tdecl->u.tunion.type_params))) {
        puts("ic_parse_decl_type_union_header: call to ic_parse_decl_optional_type_params failed");
        free(decl);
        return 0;
    }

    return decl;
}

struct ic_decl *ic_parse_decl_type_union_body(struct ic_token_list *token_list, struct ic_decl *decl) {
    /* parsed field */
    struct ic_field *field = 0;
    /* our tdecl within the decl */
    struct ic_decl_type *tdecl = 0;
    /* used to mark success of while loop */
    int success = 0;
    /* current token we are looking at */
    struct ic_token *token = 0;

#ifdef DEBUG_PARSE
    puts("ic_parse_decl_type called");
#endif

    if (!token_list) {
        puts("ic_parse_decl_type_union_body: token_list was null");
        return 0;
    }

    if (!decl) {
        puts("ic_parse_decl_type_union_body: decl was null");
        return 0;
    }

    /* fetch our tdecl from within decl */
    tdecl = ic_decl_get_tdecl(decl);
    if (!tdecl) {
        puts("ic_parse_decl_type_union_body: call to ic_decl_get_tdecl failed");
        free(decl);
        return 0;
    }

    /* iterate through all tokens */
    /* keep iterating through all the tokens until we find an unexpected 'end' */
    while ((token = ic_token_list_peek_important(token_list))) {
        /* check if this is the end */
        if (token->id == IC_END) {
            success = 1;
            break;
        }

        /* otherwise this is a field
         * parse it
         */
        field = ic_parse_field(token_list);
        if (!field) {
            puts("ic_parse_decl_type_union_body: call to ic_parse_field failed");

            /* free decl and all contents */
            if (!ic_decl_destroy(decl, 1)) {
                puts("ic_parse_decl_type_union_body: in error tidyup call to ic_decl_destroy failed");
            }
            return 0;
        }

        /* and store it */
        if (!ic_decl_type_add_field(tdecl, field)) {
            puts("ic_parse_decl_type_union_body: call to ic_decl_type_add_field failed");

            /* free field and all contents */
            if (!ic_field_destroy(field, 1)) {
                puts("ic_parse_decl_type_union_body: in error tidyup call to ic_field_destroy failed");
            }

            /* free decl and all contents */
            if (!ic_decl_destroy(decl, 1)) {
                puts("ic_parse_decl_type_union_body: in error tidyup call to ic_decl_destroy failed");
            }
            return 0;
        }

        /* increment of i is handled by ic_parse_field */
    }

    /* if it is then consume */
    token = ic_token_list_expect_important(token_list, IC_END);
    if (!token) {
        puts("ic_parse_decl_type_union_body: call to ic_token_list_expect for end failed");
        return 0;
    }

    /* if success is 1 then this means we hit `end` as
     * we expected and our type decl is finished
     */
    if (success) {
        /* victory ! */
        return decl;
    }

    /* if ret is not 1 then
     * this means we ran out of tokens
     * this is an error case as `end` should cause the
     * successful return
     */
    puts("ic_parse_decl_type_union_body: something went wrong in finding end");

    /* free decl and all contents */
    if (!ic_decl_destroy(decl, 1)) {
        puts("ic_parse_decl_type_union_body: in error tidyup call to ic_decl_destroy failed");
    }
    return 0;
}

unsigned int ic_parse_decl_optional_type_params(struct ic_token_list *token_list, struct ic_pvector *type_params) {
    struct ic_token *token = 0;
    struct ic_type_param *tparam = 0;
    char *name = 0;
    unsigned int name_len = 0;

    if (!token_list) {
        puts("ic_parse_decl_optional_type_params: token_list was null");
        return 0;
    }

    if (!type_params) {
        puts("ic_parse_decl_optional_type_params: type_params was null");
        return 0;
    }

    token = ic_token_list_peek_important(token_list);
    if (!token) {
        puts("ic_parse_decl_optional_type_params: call to ic_token_list_peek_important failed");
        return 0;
    }

    /* if we find a left square bracket then we are ready */
    if (token->id != IC_LSBRACKET) {
        /* no such bracket found, still a success */
        return 1;
    }

    /* consume [ */
    token = ic_token_list_expect_important(token_list, IC_LSBRACKET);
    if (!token) {
        puts("ic_parse_decl_optional_type_params: call to ic_token_list_expect_important failed for LSBRACKET");
        return 0;
    }

    /* process each type param */
    while ((token = ic_token_list_peek_important(token_list))) {
        /* keep going until ] */
        if (token->id == IC_RSBRACKET) {
            /* a right bracket `)` is the end of our arg list */
            break;
        }

        /* if there is a COMMA, consume it out of the way
         * this makes commas optional
         * FIXME TODO consider if this is right
         */
        if (token->id == IC_COMMA) {
            /* consume */
            token = ic_token_list_expect_important(token_list, IC_COMMA);
            if (!token) {
                puts("ic_parse_decl_optional_type_params: call to ic_token_list_expect_important failed for ','");
                return 0;
            }
        }

        /* move token along one */
        token = ic_token_list_expect_important(token_list, IC_IDENTIFIER);
        if (!token) {
            puts("ic_parse_decl_optional_type_params: call to ic_token_list_expect_important failed");
            return 0;
        }

        /* pull out our name */
        name = ic_token_get_string(token);
        name_len = ic_token_get_string_length(token);
        if (!name || !name_len) {
            puts("ic_parse_decl_optional_type_params: failed to extract name from token");
            return 0;
        }

        /* make a tparam */
        tparam = ic_type_param_new(name, name_len);
        if (!tparam) {
            puts("ic_parse_decl_optional_type_params: call to ic_type_param_new failed");
            return 0;
        }

        /* append it */
        if (-1 == ic_pvector_append(type_params, tparam)) {
            puts("ic_parse_decl_optional_type_params: call to ic_pvector failed");
            return 0;
        }
    }

    if (!token) {
        puts("ic_parse_decl_optional_type_params: call to ic_token_list_peek_important failed");
        return 0;
    }

    /* consume ] */
    token = ic_token_list_expect_important(token_list, IC_RSBRACKET);
    if (!token) {
        puts("ic_parse_decl_optional_type_params: call to ic_token_list_expect_important failed for RSBRACKET");
        return 0;
    }

    /* success */
    return 1;
}

struct ic_decl *ic_parse_decl_func(struct ic_token_list *token_list) {
    struct ic_decl *decl = 0;

    if (!token_list) {
        puts("ic_parse_decl_func: token_list was null");
        return 0;
    }

    decl = ic_parse_decl_func_header(token_list);
    if (!decl) {
        puts("ic_parse_decl_func: call to ic_parse_decl_func_header failed");
        return 0;
    }

    decl = ic_parse_decl_func_body(token_list, decl);
    if (!decl) {
        puts("ic_parse_decl_func: call to ic_parse_decl_func_body failed");
        return 0;
    }

    return decl;
}

struct ic_decl *ic_parse_decl_func_header(struct ic_token_list *token_list) {
    /* our argument */
    struct ic_field *arg = 0;
    /* our resulting ic_decl */
    struct ic_decl *decl = 0;
    /* our tdecl within the decl */
    struct ic_decl_func *fdecl = 0;
    /* current token */
    struct ic_token *token = 0;

    if (!token_list) {
        puts("ic_parse_decl_func_header: token_list was null");
        return 0;
    }

    /* check we really are at a `function` token */
    token = ic_token_list_expect_important(token_list, IC_FUNC);
    if (!token) {
        puts("ic_parse_decl_func_header: call to ic_token_list_expect_important failed for FUNC");
        return 0;
    }

    /* allocate and init our decl */
    decl = ic_decl_new(ic_decl_tag_func);
    if (!decl) {
        puts("ic_parse_decl_func_header: call to ic_decl_new failed");
        return 0;
    }

    /* fetch our fdecl from within decl */
    fdecl = ic_decl_get_fdecl(decl);
    if (!fdecl) {
        puts("ic_parse_decl_func_header: call to ic_decl_get_fdecl failed");
        free(decl);
        return 0;
    }

    /* get our function name token */
    token = ic_token_list_next_important(token_list);
    if (!token) {
        puts("ic_parse_decl_func_header: call to ic_token_list_next failed for func name");
        return 0;
    }

    if (token->id != IC_IDENTIFIER) {
        printf("ic_parse_decl_func_header: expected IC_IDENTIFIER token, got: ");
        ic_token_id_print_debug(stdout, token->id);
        puts("");
        return 0;
    }

    /* initialise our fdecl */
    if (!ic_decl_func_init(fdecl, ic_token_get_string(token), ic_token_get_string_length(token))) {
        puts("ic_parse_decl_func_header: call to ic_decl_func_init failed");
        free(decl);
        return 0;
    }

    /* optionally parse type parameters */
    if (!ic_parse_decl_optional_type_params(token_list, &(fdecl->type_params))) {
        puts("ic_parse_decl_func_header: call to ic_parse_decl_optional_type_params failed");
        free(decl);
        return 0;
    }

    /* parse arguments */
    /* opening bracket */
    token = ic_token_list_expect_important(token_list, IC_LRBRACKET);
    if (!token) {
        puts("ic_parse_decl_func_header: call to ic_token_list_expect_important failed for '('");
        return 0;
    }

    /* iterate until closing bracket
     * parsing arguments
     */
    while ((token = ic_token_list_peek_important(token_list))) {
        if (token->id == IC_RRBRACKET) {
            /* a right bracket `)` is the end of our arg list */
            break;
        }

        /* if we see a comma then just skip over it
         * FIXME this makes commas optional, decide if this is correct
         */
        if (token->id == IC_COMMA) {
            /* consume */
            token = ic_token_list_expect_important(token_list, IC_COMMA);
            if (!token) {
                puts("ic_parse_decl_func_header: call to ic_token_list_expect_important failed for ','");
                return 0;
            }

            /* move token along one */
            token = ic_token_list_peek_important(token_list);
            if (!token) {
                puts("ic_parse_decl_func_header: call to ic_token_list_peek_important failed after ','");
                return 0;
            }
        }

        /* parse argument */
        arg = ic_parse_field(token_list);
        if (!arg) {
            puts("ic_parse_decl_func_header: call to ic_parse_field failed");
            free(decl);
            return 0;
        }

        /* save it */
        if (!ic_decl_func_args_add(fdecl, arg)) {
            puts("ic_parse_decl_func_header: call to if_decl_func_add_arg failed");
            free(decl);
            return 0;
        }
    }

    token = ic_token_list_expect_important(token_list, IC_RRBRACKET);
    if (!token) {
        puts("ic_parse_decl_func_header: call to ic_token_list_expect_important failed for ')'");
        return 0;
    }

    /* parse return type
     * check if we have a return type arrow ->
     *
     * if we do, consume it, and then parse return type
     *
     * if we do not, then there is no return type and we set this function as Void
     * then begin parsing the body
     */
    token = ic_token_list_peek_important(token_list);

    /* note token being null *is* allowed
     * as this could mean we have the following as the last decl. in our file
     *  builtin foo(a::Sint)
     */
    if ((!token) || (token->id != IC_ARROW)) {
        /* add Void type to our fdecl */
        if (!ic_decl_func_set_return(fdecl, "Void", 4)) {
            puts("ic_parse_decl_func_header: call to ic_decl_func_set_return failed for 'Void'");
            free(decl);
            return 0;
        }
    } else {
        /* consume arrow */
        token = ic_token_list_next_important(token_list);
        if (!token) {
            puts("ic_parse_decl_func_header: call to ic_token_list_next_important failed when trying to consume arrow");
            return 0;
        }

        /* we found a type arrow and have skipped over it
         * parse return type
         */
        token = ic_token_list_expect_important(token_list, IC_IDENTIFIER);
        if (!token) {
            puts("ic_parse_decl_func_header: call to ic_token_list_next failed when looking for return type after arrow");
            return 0;
        }

        /* add to our fdecl */
        if (!ic_decl_func_set_return(fdecl, ic_token_get_string(token), ic_token_get_string_length(token))) {
            puts("ic_parse_decl_func_header: call to ic_decl_func_set_return failed");
            free(decl);
            return 0;
        }
    }

    return decl;
}

struct ic_decl *ic_parse_decl_func_body(struct ic_token_list *token_list, struct ic_decl *decl) {
    /* our tdecl within the decl */
    struct ic_decl_func *fdecl = 0;
    /* used to mark success of while loop */
    int success = 0;
    /* pointer used for each stmt within body */
    struct ic_stmt *stmt = 0;
    /* current token */
    struct ic_token *token = 0;

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
    puts("ic_parse_decl_func called");
#endif

    if (!token_list) {
        puts("ic_parse_decl_func_body: token_list was null");
        return 0;
    }

    if (!decl) {
        puts("ic_parse_decl_func_body: decl was null");
        return 0;
    }

    /* fetch our fdecl from within decl */
    fdecl = ic_decl_get_fdecl(decl);
    if (!fdecl) {
        puts("ic_parse_decl_func_body: call to ic_decl_get_fdecl failed");
        free(decl);
        return 0;
    }

    /* parse body */

    /* iterate through all tokens
     * until `end`
     */
    while ((token = ic_token_list_peek_important(token_list))) {
        if (token->id == IC_END) {
            success = 1;
            break;
        }

        /* leave stmt parsing up to the experts */
        stmt = ic_parse_stmt(token_list);
        if (!stmt) {
            puts("ic_parse_decl_func_body: call to ic_parse_stmt failed");
            free(decl);
            return 0;
        }

        /* save to our body */
        if (!ic_decl_func_add_stmt(fdecl, stmt)) {
            puts("ic_parse_decl_func_body: call to ic_decl_func_add_stmt failed");
            free(decl);
            return 0;
        }
    }

    /* consume end token */
    token = ic_token_list_expect_important(token_list, IC_END);
    if (!token) {
        puts("ic_parse_decl_func_body: call to ic_token_list_expect for end failed");
        return 0;
    }

    /* if success is 1 then we found an `end` token */
    if (success) {
        /* victory ! */
        return decl;
    }

    /* this means we ran out of tokens
     * or that some other error occured
     *
     * this is an error case as `end` should cause the
     * successful return
     */
    puts("ic_parse_decl_func_body: error occurred in ic_parse_this_is_not_the_end");
    free(decl);
    return 0;
}

struct ic_decl *ic_parse_decl_op(struct ic_token_list *token_list) {
    struct ic_decl *decl = 0;
    /* our op within the decl */
    struct ic_decl_op *op = 0;
    /* current token */
    struct ic_token *token = 0;

    char *from_str = 0;
    unsigned int from_len = 0;
    char *to_str = 0;
    unsigned int to_len = 0;

/* builtin op from to
     * builtin op +    plus
     */

#ifdef DEBUG_PARSE
    puts("ic_parse_decl_op called");
#endif

    if (!token_list) {
        puts("ic_parse_decl_op: token_list was null");
        return 0;
    }

    /* allocate and init our decl */
    decl = ic_decl_new(ic_decl_tag_builtin_op);
    if (!decl) {
        puts("ic_parse_decl_op: call to ic_decl_new failed");
        return 0;
    }

    /* fetch our op from within decl */
    op = ic_decl_get_op(decl);
    if (!op) {
        puts("ic_parse_decl_op: call to ic_decl_get_op failed");
        free(decl);
        return 0;
    }

    /* check we really are at an `op` token */
    token = ic_token_list_expect_important(token_list, IC_OP);
    if (!token) {
        puts("ic_parse_decl_op: call to ic_token_list_expect_important failed for OP");
        return 0;
    }

    /* get from symbol */
    token = ic_token_list_next_important(token_list);
    if (!token) {
        puts("ic_parse_decl_op: call to ic_token_list_next_important failed for op");
        return 0;
    }

    if (!ic_token_isoperator(token)) {
        puts("ic_parse_decl_op: token was not an operator");
        ic_token_print_debug(stdout, token);
        return 0;
    }

    /* get the parts we need */
    from_str = ic_token_get_representation(token);
    if (!from_str) {
        puts("ic_parse_decl_op: call to ic_token_get_representation failed for from");
        return 0;
    }

    from_len = strlen(from_str);
    if (!from_len) {
        puts("ic_parse_decl_op: call to strlen failed for from");
        return 0;
    }

    /* get to symbol */
    token = ic_token_list_expect_important(token_list, IC_IDENTIFIER);
    if (!token) {
        puts("ic_parse_decl_op: call to ic_token_list_expect_important failed for IDENTIFIER");
        return 0;
    }

    /* get the parts we need */
    to_str = ic_token_get_string(token);
    if (!to_str) {
        puts("ic_parse_decl_op: call to ic_token_get_string failed for 'to'");
        return 0;
    }

    to_len = ic_token_get_string_length(token);
    if (!to_len) {
        puts("ic_parse_decl_op: call to ic_token_get_string_length failed for 'to'");
        return 0;
    }

    /* init op */
    if (!ic_decl_op_init(op, from_str, from_len, to_str, to_len)) {
        puts("ic_parse_decl_op: call to ic_decl_op_init failed");
        return 0;
    }

    /* done */
    return decl;
}
