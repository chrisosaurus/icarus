#include <stdio.h>
#include <string.h>

#include "data/token.h"
#include "data/token_list.h"
#include "table.h"
#include "data/lex_data.h"
#include "lexer.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-function"

static unsigned int ic_lex_comment(struct ic_lex_data *lex_data);
static unsigned int ic_lex_identifier(struct ic_lex_data *lex_data);

/* takes a character array of the source program
 *
 * takes the filename for current source unit, this
 * is attached to each token
 *
 * returns a token_list on success
 * returns 0 on failure
 */
struct ic_token_list * ic_lex(char *filename, char *source){
    struct ic_lex_data * lex_data = 0;

    /* str from table */
    char * table_str = 0;
    /* len from table */
    unsigned int table_len = 0;
    /* id from table */
    enum ic_token_id table_id = 0;

    /* temporary token we construct */
    struct ic_token *token = 0;

    /* offset into table */
    unsigned int t_i = 0;
    /* table length */
    unsigned int t_len = 0;

    if( ! source ){
        puts("ic_lex: source was null");
        return 0;
    }

    lex_data = ic_lex_data_new(filename, source);
    if( ! lex_data ){
        puts("ic_lex: call to ic_lex_data_new failed");
        return 0;
    }

    t_len = IC_LEX_TABLE_LEN();

    /* go through source */
    for( lex_data->s_i = 0; lex_data->s_i < lex_data->s_len; ){
        token = 0;

        /* check if this exists within the table */
        for( t_i = 0; t_i < t_len; ++t_i ){
            table_str = table[t_i].str;
            table_len = table[t_i].len;
            table_id  = table[t_i].id;

            if( lex_data->s_i + table_len > lex_data->s_len ){
                /* no way it could fit */
                continue;
            }

            if( strncmp(&(source[lex_data->s_i]), table_str, table_len) ){
                /* strings did not match */
                continue;
            }

            /* we have a match !  */

            /* build a new token */
            token = ic_token_new(table_id, lex_data->start_of_line, lex_data->offset_into_line, lex_data->filename, lex_data->line_num);
            if( ! token ){
                puts("ic_lex: call to ic_token_new failed");
                return 0;
            }

            /* append token */
            if( ! ic_token_list_append(lex_data->token_list, token) ){
                puts("ic_lex: call to ic_token_list_append failed");
                return 0;
            }

            /* book keeping */
            /* update i
             * update offset into line
             */
            lex_data->s_i += table_len;
            lex_data->offset_into_line += table_len;

            /* if our match was a \n then we need to:
             *  increment the line counter
             *  reset offset into line
             *  capture current start of line
             */
            if( table_id == IC_NEWLINE ){
                lex_data->line_num += 1;
                lex_data->offset_into_line = 0;
                lex_data->start_of_line = &(lex_data->source[lex_data->s_i]);
            } else if( table_id == IC_COMMENT ){
                /* FIXME consume rest of line into token */
                /* FIXME maintain lex_data while we do so */
                /* FIXME add payload */
            }
        }

        if( token ){
            /* inner loop matched, stop processing this time round */
            continue;
        }

        /* otherwise this is one of:
         *  identifier
         *  literal
         *  FIXME
         */
    }

    puts("ic_lex: unimplemented");
    return 0;
}

static unsigned int ic_lex_comment(struct ic_lex_data *lex_data){
    puts("ic_lex_comment: unimplemented");
    return 0;
}

static unsigned int ic_lex_identifier(struct ic_lex_data *lex_data){
    puts("ic_lex_comment: unimplemented");
    return 0;
}
