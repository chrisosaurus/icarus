#include <stdio.h> /* puts, printf */
#include <string.h> /* strchr, strncmp */

#include "lexer.h"
#include "types.h"
#include "ast.h"
#include "parse.h"
/* ignore unused parameter warnings */
#pragma GCC diagnostic ignored "-Wunused-parameter"

struct ic_type_decl * ic_parse_type_decl(struct ic_tokens *tokens, unsigned int *i){
    /* FIXME */
    return 0;
}

void * ic_parse(struct ic_tokens *tokens){
    unsigned int i = 0;
    char *space = 0;
    unsigned int dist = 0;

    /* possible leading tokens:
     * type
     * enum
     * union
     * function
     * #
     */

    /* step through tokens until consumed */
    for( i=0; i < tokens->len; ){
        /* find next space in string */
        space = strchr( &(tokens->tokens[i]), ' ' );
        /* find length of token
         * which is the distance from the start to the space
         */
        dist = space - &(tokens->tokens[i]);
        printf("got distance %u\n", dist);

        switch( dist ){
            case 1:
                /* # */
                printf("1: unsupported string '%.*s' found, bailing\n", dist, &(tokens->tokens[i]) );
                return 0;
                break;

            case 4:
                /* type
                 * enum
                 */
                printf("4: unsupported string '%.*s' found, bailing\n", dist, &(tokens->tokens[i]) );
                return 0;
                break;

            case 5:
                /* union */
                printf("5: unsupported string '%.*s' found, bailing\n", dist, &(tokens->tokens[i]) );
                return 0;
                break;

            case 8:
                /* function */
                printf("8: unsupported string '%.*s' found, bailing\n", dist, &(tokens->tokens[i]) );
                return 0;
                break;

            default:
                printf("default: unsupported string '%.*s' found, bailing\n", dist, &(tokens->tokens[i]) );
                return 0;
                break;
        }
        return 0;
    }

    /* FIXME */
    puts("ic_parse finished, bailing");
    return 0;
}

