#include <stdio.h> /* puts, printf */
#include <string.h> /* strncmp */

#include "parse.h"
#include "permissions.h"

struct ic_field * ic_parse_field(struct ic_token_list *token_list){
    /* the field we build and return */
    struct ic_field *field = 0;
    /* name component char* and len*/
    char *name = 0;
    unsigned int name_len = 0;
    /* type component char* and len */
    char *type = 0;
    unsigned int type_len = 0;

    /* our eventual field permissions */
    unsigned int permissions = 0;

    /* current token */
    struct ic_token *token = 0;

    if( ! token_list ){
        puts("ic_parse_field: token_list was null");
        return 0;
    }

    /* check for permission */
    token = ic_token_list_peek_important(token_list);
    if( ! token ){
        puts("ic_parse_field: failed to peek at permission slot");
        return 0;
    }

    /* if we found a valid permission
     * then consume and process
     */
    if( ic_token_ispermission(token) ){
        token = ic_token_list_next_important(token_list);
        if( ! token ){
            puts("ic_parse_field: failed to get permission slot");
            return 0;
        }

        permissions = ic_parse_perm(token->id);
    } else {
        /* otherwise fallback to default permission */
        permissions = ic_parse_perm(IC_PERM_DEFAULT);
    }

    /* capture field name
     * a::Int
     * `a` is the name
     */
    token = ic_token_list_expect_important(token_list, IC_IDENTIFIER);
    if( ! token ){
        puts("ic_parse_field: no name token found");
        return 0;
    }

    name = ic_token_get_string(token);
    name_len = ic_token_get_string_length(token);
    if( !name || ! name_len ){
        puts("ic_parse_field: failed to extract name from token");
        return 0;
    }


    /* check for separator `::` */
    token = ic_token_list_expect_important(token_list, IC_DOUBLECOLON);
    if( ! token ){
        puts("ic_parse_field: no doublcolon token found");
        return 0;
    }

    /* capture field type
     * a::Int
     * `Int` is the type
     */
    token = ic_token_list_expect_important(token_list, IC_IDENTIFIER);
    if( ! token ){
        puts("ic_parse_field: no type found");
        return 0;
    }

    type = ic_token_get_string(token);
    type_len = ic_token_get_string_length(token);
    if( !name || ! name_len ){
        puts("ic_parse_field: failed to extract type from token");
        return 0;
    }

    field = ic_field_new(name, name_len, type, type_len, permissions);
    if( ! field ){
        puts("ic_parse_field: call to ic_field_new failed");
        return 0;
    }

    return field;
}

