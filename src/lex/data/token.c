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

/* get string representation of token
 *
 * returns * on success
 * returns 0 on failure
 */
char * ic_token_get_representation(struct ic_token *token){
    if( ! token ){
        puts("ic_token_get_representation: token was null");
        return 0;
    }

    switch( token->id ){
        case IC_IDENTIFIER:
        case IC_LITERAL_INTEGER:
        case IC_LITERAL_STRING:
        case IC_COMMENT:
            puts("ic_token_get_representation: payload types are not supported");
            return 0;
            break;

        case IC_NEWLINE:
            return "\n";
            break;
        case IC_WHITESPACE:
            return " ";
            break;

        case IC_END:
            return "end";
            break;

        case IC_IF:
            return "if";
            break;
        case IC_ELSIF:
            return "elsif";
            break;
        case IC_ELSE:
            return "else";
            break;
        case IC_RETURN:
            return "return";
            break;
        case IC_LET:
            return "let";
            break;
        case IC_FOR:
            return "for";
            break;
        case IC_IN:
            return "in";
            break;
        case IC_WHILE:
            return "while";
            break;

        case IC_IMPORT:
            return "import";
            break;
        case IC_BUILTIN:
            return "builtin";
            break;
        case IC_OP:
            return "op";
            break;
        case IC_FUNC:
            return "fn";
            break;
        case IC_TYPE:
            return "type";
            break;
        case IC_ENUM:
            return "enum";
            break;
        case IC_UNION:
            return "union";
            break;

        case IC_ARROW:
            return "->";
            break;
        case IC_EQUAL:
            return "==";
            break;
        case IC_ASSIGN:
            return "=";
            break;
        case IC_DOUBLECOLON:
            return "::";
            break;
        case IC_PERIOD:
            return ".";
            break;
        case IC_COMMA:
            return ",";
            break;
        case IC_AND:
            return "&&";
            break;
        case IC_OR:
            return "||";
            break;

        case IC_DIVIDE:
            return "/";
            break;

        case IC_DOLLAR:
            return "$";
            break;
        case IC_PERCENT:
            return "%";
            break;
        case IC_AMPERSAND:
            return "&";
            break;
        case IC_AT:
            return "@";
            break;
        case IC_ASTERISK:
            return "*";
            break;
        case IC_CARET:
            return "^";
            break;
        case IC_PLUS:
            return "+";
            break;
        case IC_MINUS:
            return "-";
            break;

        case IC_LRBRACKET:
            return "(";
            break;
        case IC_RRBRACKET:
            return ")";
            break;

        default:
            printf("ic_token_get_representation: ERROR, unknown token id '%d'\n", token->id);
            return 0;
            break;
    }
}

/* get string data on token
 *
 * returns * on success
 * returns 0 on failure
 */
char * ic_token_get_string(struct ic_token *token){
    if( ! token ){
        puts("ic_token_get_string: token was null");
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

/* check if token is an operator
 *
 * returns 1 if it is an operator
 * returns 0 if not
 */
unsigned int ic_token_isoperator(struct ic_token *token){
    if( ! token ){
        puts("ic_token_isoperator: token was null");
        return 0;
    }

    switch( token->id ){
        case IC_PLUS:
        case IC_MINUS:
        case IC_DIVIDE:
        case IC_ASTERISK:
        case IC_PERCENT:
            return 1;

        case IC_EQUAL:
        case IC_AND:
        case IC_OR:
            return 1;

        case IC_PERIOD:
            /* a period is not an operator
             * it is a field access */
            return 0;

        default:
            break;
    }

    return 0;
}

/* check if token is a permission
 *
 * returns 1 if it is a permission
 * returns 0 if not
 */
unsigned int ic_token_ispermission(struct ic_token *token){
    if( ! token ){
        puts("ic_token_ispermission: token was null");
        return 0;
    }

    switch( token->id ){
        case IC_DOLLAR:
        case IC_PERCENT:
        case IC_AMPERSAND:
        case IC_AT:
        case IC_ASTERISK:
        case IC_CARET:
        case IC_PLUS:
        case IC_MINUS:
            return 1;

        default:
            break;
    }

    return 0;
}

void ic_token_print(struct ic_token *token){
    char * str = 0;
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

        default:
            str = ic_token_get_representation(token);
            if( ! str ){
                puts("ic_token_print: call to ic_token_get_representation failed");
                return;
            }

            fputs(str, stdout);
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
        case IC_ELSIF:
            fputs("IC_ELSIF", stdout);
            break;
        case IC_ELSE:
            fputs("IC_ELSE", stdout);
            break;
        case IC_RETURN:
            fputs("IC_RETURN", stdout);
            break;
        case IC_LET:
            fputs("IC_LET", stdout);
            break;
        case IC_FOR:
            fputs("IC_FOR", stdout);
            break;
        case IC_IN:
            fputs("IC_IN", stdout);
            break;
        case IC_WHILE:
            fputs("IC_WHILE", stdout);
            break;

        case IC_IMPORT:
            fputs("IC_IMPORT", stdout);
            break;
        case IC_BUILTIN:
            fputs("IC_BUILTIN", stdout);
            break;
        case IC_OP:
            fputs("IC_OP", stdout);
            break;
        case IC_FUNC:
            fputs("IC_FUNC", stdout);
            break;
        case IC_TYPE:
            fputs("IC_TYPE", stdout);
            break;
        case IC_ENUM:
            fputs("IC_ENUM", stdout);
            break;
        case IC_UNION:
            fputs("IC_UNION", stdout);
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
        case IC_AND:
            fputs("IC_AND", stdout);
            break;
        case IC_OR:
            fputs("IC_OR", stdout);
            break;

        case IC_DIVIDE:
            fputs("IC_DIVIDE", stdout);
            break;

        case IC_DOLLAR:
            fputs("IC_DOLLAR", stdout);
            break;
        case IC_PERCENT:
            fputs("IC_PERCENT", stdout);
            break;
        case IC_AMPERSAND:
            fputs("IC_AMPERSAND", stdout);
            break;
        case IC_AT:
            fputs("IC_AT", stdout);
            break;
        case IC_ASTERISK:
            fputs("IC_ASTERISK", stdout);
            break;
        case IC_CARET:
            fputs("IC_CARET", stdout);
            break;
        case IC_PLUS:
            fputs("IC_PLUS", stdout);
            break;
        case IC_MINUS:
            fputs("IC_MINUS", stdout);
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

void ic_token_print_line(struct ic_token *token){
    /* number of chars until line ends */
    unsigned int len = 0;

    if( ! token ){
        puts("ic_token_print_line: token was null");
        return;
    }

    for( len=0; ; ++len ){
        switch( token->line[len] ){
            case '\0':
            case '\n':
            case '\r':
                goto END;
                break;

            default:
                break;
        }
    }

END:

    printf("%.*s\n", len, token->line);
}

void ic_token_debug(struct ic_token *token){
    if( ! token ){
        puts("ic_token_debug: token was null");
        return;
    }

    fputs("ic_token_debug:", stdout);
    ic_token_print_debug(token);
    printf(" found in context: '%.*s'\nline:\n", 10, &(token->line[token->offset]));
    ic_token_print_line(token);
}


