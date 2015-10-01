#include <stdio.h>
#include <string.h>

#include "data/token.h"
#include "data/token_list.h"
#include "table.h"
#include "lexer.h"

/* takes a character array of the source program
 *
 * takes the filename for current source unit, this
 * is attached to each token
 *
 * returns a token_list on success
 * returns 0 on failure
 */
struct ic_token_list * ic_lex(char *filename, char *source){
    /* our output */
    struct ic_token_list * token_list = 0;
    /* current line number into source, attached to tokens */
    unsigned int line_num = 0;
    /* offset into line, attached to tokens */
    unsigned int offset_into_line = 0;
    /* first char of this line */
    char * start_of_line = 0;

    /* temporary token we construct */
    struct ic_token *token = 0;

    /* offset into source */
    unsigned int s_i = 0;
    /* length of source */
    unsigned int s_len = 0;

    /* offset into table */
    unsigned int t_i = 0;
    /* length of table */
    unsigned int t_len = 0;
    /* str from table */
    char * table_str = 0;
    /* len from table */
    unsigned int table_len = 0;
    /* id from table */
    enum ic_token_id table_id = 0;

    if( ! source ){
        puts("ic_lex: source was null");
        return 0;
    }

    token_list = ic_token_list_new();
    if( ! token_list ){
        puts("ic_lex: call to ic_token_list_new failed");
        return 0;
    }

    s_len = strlen(source);
    t_len = IC_LEX_TABLE_LEN();

    /* go through source */
    for( s_i = 0; s_i < s_len; ){
        token = 0;

        /* check if this exists within the table */
        for( t_i = 0; t_i < t_len; ++t_i ){
            table_str = table[t_i].str;
            table_len = table[t_i].len;
            table_id  = table[t_i].id;

            if( source[s_i] != table_str[0] ){
                /* first char doesn't match */
                continue;
            }

            if( s_i + table_len > s_len ){
                /* no way it could fit */
                continue;
            }

            if( strncmp(&(source[s_i]), table_str, table_len) ){
                /* strings did not match */
                continue;
            }

            /* we have a match !
             * update i
             */
            s_i += table_len;

            /* build a new token
             * FIXME most of these fields are not currently populated / upkept
             */
            token = ic_token_new(table_id, start_of_line, offset_into_line, filename, line_num);
            if( ! token ){
                puts("ic_lex: call to ic_token_new failed");
                return 0;
            }

            /* append token */
            if( ! ic_token_list_append(token_list, token) ){
                puts("ic_lex: call to ic_token_list_append failed");
                return 0;
            }
        }

        if( token ){
            /* inner loop matched, stop processing this time round */
            continue;
        }

        /* otherwise this is one of:
         *  comment
         *  identifier
         *  literal
         *  FIXME
         */
    }

    puts("ic_lex: unimplemented");
    return 0;
}

