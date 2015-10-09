#include <stdlib.h> /* calloc, free */
#include <stdio.h> /* puts */

#include "token.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"

/* alloc and init a new token
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_token * ic_token_new(enum ic_token_id id, char *line, unsigned int offset, char *file, unsigned int line_num){
    struct ic_token *token = 0;

    token = calloc(1, sizeof(struct ic_token));
    if( ! token ){
        puts("ic_token_new: call to calloc failed");
        return 0;
    }

    if( ! ic_token_init(token, id, line, offset, file, line_num) ){
        puts("ic_token_new: call to ic_token_init failed");
        return 0;
    }

    return token;
}

/* init an existing token
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_token_init(struct ic_token *token, enum ic_token_id id, char *line, unsigned int offset, char *file, unsigned int line_num){
    if( ! token ){
        puts("ic_token_init: token was null");
        return 0;
    }

    if( ! line ){
        puts("ic_token_init: line was null");
        return 0;
    }

    if( ! file ){
        puts("ic_token_init: file was null");
        return 0;
    }

    token->id = id;
    token->line = line;
    token->offset = offset;
    token->file = file;
    token->line_num = line_num;

    return 1;
}

/* set string data on token
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_token_set_string(struct ic_token *token, char *string, unsigned int len){
    if( ! token ){
        puts("ic_token_set_string: token was null");
        return 0;
    }

    /* check if token->id allows for a string */
    switch( token->id ){
        case IC_IDENTIFIER:
        case IC_LITERAL_STRING:
        case IC_COMMENT:
            /* string allowed */
            break;

        default:
            fputs("ic_token_set_string: called on token not allowing a payload, token->id:", stdout);
            ic_token_id_print_debug(token->id);
            puts("");
            return 1;
            break;
    }

    token->u.str.string = string;
    token->u.str.len = len;
    return 1;
}

/* get string data on token
 *
 * returns * on success
 * returns 0 on failure
 */
char * ic_token_get_string(struct ic_token *token){
    if( ! token ){
        puts("ic_token_get_string_length: token was null");
        return 0;
    }

    /* check if token->id allows for a string */
    switch( token->id ){
        case IC_IDENTIFIER:
        case IC_LITERAL_STRING:
        case IC_COMMENT:
            /* string allowed */
            break;

        default:
            fputs("ic_token_get_string: called on token not allowing a payload, token->id:", stdout);
            ic_token_id_print_debug(token->id);
            puts("");
            return 0;
            break;
    }

    return token->u.str.string;
}

/* get string length on token
 *
 * returns length on success
 * returns 0 on failure
 */
unsigned int ic_token_get_string_length(struct ic_token *token){
    if( ! token ){
        puts("ic_token_get_string_length: token was null");
        return 0;
    }

    /* check if token->id allows for a string */
    switch( token->id ){
        case IC_IDENTIFIER:
        case IC_LITERAL_STRING:
        case IC_COMMENT:
            /* string allowed */
            break;

        default:
            fputs("ic_token_get_string_length: called on token not allowing a payload, token->id:", stdout);
            ic_token_id_print_debug(token->id);
            puts("");
            return 0;
            break;
    }

    return token->u.str.len;
}

/* set integer data on token
 *
 * returns 1 on success
 * returns 0 on failure
 */
int ic_token_set_integer(struct ic_token *token, int integer){
    if( ! token ){
        puts("ic_token_set_integer: token was null");
        return 0;
    }

    /* check if token->id allows for an integer */
    switch( token->id ){
        case IC_LITERAL_INTEGER:
            /* string allowed */
            break;

        default:
            fputs("ic_token_set_integer: called on token not allowing a payload, token->id:", stdout);
            ic_token_id_print_debug(token->id);
            puts("");
            return 0;
            break;
    }

    token->u.integer = integer;

    return 1;
}

/* get integer data on token
 *
 * returns * on success
 * returns 0 on failure
 */
int ic_token_get_integer(struct ic_token *token){
    if( ! token ){
        puts("ic_token_get_integer: token was null");
        return 0;
    }

    /* check if token->id allows for an integer */
    switch( token->id ){
        case IC_LITERAL_INTEGER:
            /* string allowed */
            break;

        default:
            fputs("ic_token_get_integer: called on token not allowing a payload, token->id:", stdout);
            ic_token_id_print_debug(token->id);
            puts("");
            return 0;
            break;
    }

    return token->u.integer;
}

/* destroy
 *
 * will only free token if free_token is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_token_destroy(struct ic_token *token, unsigned int free_token){
    if( ! token ){
        puts("ic_token_destroy: token was null");
        return 0;
    }

    if( free_token ){
        free(token);
    }

    return 1;
}

void ic_token_print(struct ic_token *token){
    if( ! token ){
        puts("ic_token_print: token was null");
        return;
    }

    switch( token->id ){
        case IC_IDENTIFIER:
            printf("%.*s", token->u.str.len, token->u.str.string);
            break;
        case IC_LITERAL_INTEGER:
            printf("%ld", token->u.integer);
            break;
        case IC_LITERAL_STRING:
            printf("\"%.*s\"", token->u.str.len, token->u.str.string);
            break;
        case IC_COMMENT:
            printf("#%.*s", token->u.str.len, token->u.str.string);
            break;

        case IC_NEWLINE:
            fputs("\n", stdout);
            break;
        case IC_WHITESPACE:
            fputs(" ", stdout);
            break;

        case IC_END:
            fputs("end", stdout);
            break;

        case IC_IF:
            fputs("if", stdout);
            break;
        case IC_ELSE:
            fputs("else", stdout);
            break;
        case IC_THEN:
            fputs("then", stdout);
            break;
        case IC_RETURN:
            fputs("return", stdout);
            break;
        case IC_LET:
            fputs("let", stdout);
            break;

        case IC_TYPE:
            fputs("type", stdout);
            break;
        case IC_FUNC:
            fputs("fn", stdout);
            break;

        case IC_ARROW:
            fputs("->", stdout);
            break;
        case IC_EQUAL:
            fputs("==", stdout);
            break;
        case IC_ASSIGN:
            fputs("=", stdout);
            break;
        case IC_DOUBLECOLON:
            fputs("::", stdout);
            break;
        case IC_PERIOD:
            fputs(".", stdout);
            break;
        case IC_COMMA:
            fputs(",", stdout);
            break;

        case IC_PLUS:
            fputs("+", stdout);
            break;
        case IC_MINUS:
            fputs("-", stdout);
            break;
        case IC_DIVIDE:
            fputs("/", stdout);
            break;
        case IC_MULTIPLY:
            fputs("*", stdout);
            break;


        case IC_LRBRACKET:
            fputs("(", stdout);
            break;
        case IC_RRBRACKET:
            fputs(")", stdout);
            break;

        default:
            printf("ic_token_print: ERROR, unknown token id '%d'\n", token->id);
            break;
    }
}

void ic_token_id_print_debug(enum ic_token_id id){
    switch( id ){
        case IC_IDENTIFIER:
            /* FIXME add payload */
            fputs("IC_IDENTIFIER", stdout);
            break;
        case IC_LITERAL_INTEGER:
            /* FIXME add payload */
            fputs("IC_LITERAL_INTEGER", stdout);
            break;
        case IC_LITERAL_STRING:
            /* FIXME add payload */
            fputs("IC_LITERAL_STRING", stdout);
            break;
        case IC_COMMENT:
            /* FIXME add payload */
            fputs("IC_COMMENT", stdout);
            break;

        case IC_NEWLINE:
            fputs("IC_NEWLINE", stdout);
            break;
        case IC_WHITESPACE:
            fputs("IC_WHITSPACE", stdout);
            break;

        case IC_END:
            fputs("IC_END", stdout);
            break;

        case IC_IF:
            fputs("IC_IF", stdout);
            break;
        case IC_ELSE:
            fputs("IC_ELSE", stdout);
            break;
        case IC_THEN:
            fputs("IC_THEN", stdout);
            break;
        case IC_RETURN:
            fputs("IC_RETURN", stdout);
            break;
        case IC_LET:
            fputs("IC_LET", stdout);
            break;

        case IC_TYPE:
            fputs("IC_TYPE", stdout);
            break;
        case IC_FUNC:
            fputs("IC_FUNC", stdout);
            break;

        case IC_ARROW:
            fputs("IC_ARROW", stdout);
            break;
        case IC_EQUAL:
            fputs("IC_EQUAL", stdout);
            break;
        case IC_ASSIGN:
            fputs("IC_ASSIGN", stdout);
            break;
        case IC_DOUBLECOLON:
            fputs("IC_DOUBLECOLON", stdout);
            break;
        case IC_PERIOD:
            fputs("IC_PERIOD", stdout);
            break;
        case IC_COMMA:
            fputs("IC_COMMA", stdout);
            break;

        case IC_PLUS:
            fputs("IC_PLUS", stdout);
            break;
        case IC_MINUS:
            fputs("IC_MINUS", stdout);
            break;
        case IC_DIVIDE:
            fputs("IC_DIVIDE", stdout);
            break;
        case IC_MULTIPLY:
            fputs("IC_MULTIPLY", stdout);
            break;

        case IC_LRBRACKET:
            fputs("IC_LRBRACKET", stdout);
            break;
        case IC_RRBRACKET:
            fputs("IC_RRBRACKET", stdout);
            break;

        default:
            printf("ic_token_id_print_debug: ERROR, unknown token id '%d'\n", id);
            break;
    }
}


void ic_token_print_debug(struct ic_token *token){
    if( ! token ){
        puts("ic_token_print_debug: token was null");
        return;
    }

    ic_token_id_print_debug(token->id);
}

