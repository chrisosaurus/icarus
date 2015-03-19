#include <stdio.h> /* puts, printf */
#include <string.h> /* strncmp */

#include "../parse.h"

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
    name_len = ic_parse_token_length(tokens->tokens, *i);
    if( ! name_len ){
        puts("ic_parse_field: no name token found");
        return 0;
    }

    /* advance past name */
    ic_parse_token_advance(i, name_len);


    /* check for separator `::` */
    sep_len = ic_parse_token_length(tokens->tokens, *i);
    if( ! sep_len ){
        puts("ic_parse_field: call ot ic_parse_token_length failed");
        return 0;
    }
    if( sep_len != 2 || strncmp("::", &(tokens->tokens[*i]), sep_len) ){
        printf("ic_parse_field: expected sep '::', found '%.*s'\n", sep_len, &(tokens->tokens[*i]));
        return 0;
    }

    /* advance past separator */
    ic_parse_token_advance(i, sep_len);


    /* capture field type
     * a::Int
     * `Int` is the type
     */
    type = &(tokens->tokens[*i]);
    type_len = ic_parse_token_length(tokens->tokens, *i);
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

