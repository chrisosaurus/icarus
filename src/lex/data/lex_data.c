#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lex_data.h"
#include "token_list.h"

/* construct a new lex data
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_lex_data *ic_lex_data_new(char *filename, char *source) {
    struct ic_lex_data *lex_data = 0;

    if (!filename) {
        puts("ic_lex_data_new: filename was null");
        return 0;
    }

    if (!source) {
        puts("ic_lex_data_new: source was null");
        return 0;
    }

    lex_data = calloc(1, sizeof(struct ic_lex_data));
    if (!lex_data) {
        puts("ic_lex_data_new: call to calloc failed");
        return 0;
    }

    if (!ic_lex_data_init(lex_data, filename, source)) {
        puts("ic_lex_data_new: call to ic_lex_data_init failed");
        free(lex_data);
        return 0;
    }

    return lex_data;
}

/* init an existing lex data
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_lex_data_init(struct ic_lex_data *lex_data, char *filename, char *source) {
    if (!lex_data) {
        puts("ic_lex_data_init: lex_data was null");
        return 0;
    }

    if (!filename) {
        puts("ic_lex_data_init: filename was null");
        return 0;
    }

    if (!source) {
        puts("ic_lex_data_init: source was null");
        return 0;
    }

    lex_data->filename = filename;
    lex_data->source = source;

    lex_data->token_list = ic_token_list_new();
    if (!lex_data->token_list) {
        puts("ic_lex_data_init: call to ic_token_list_new failed");
        return 0;
    }

    lex_data->line_num = 0;
    lex_data->offset_into_line = 0;
    lex_data->start_of_line = source;

    lex_data->s_i = 0;
    lex_data->s_len = strlen(source);

    return 1;
}

/* destroy ic_lex_data
 *
 * will only free lex_data if `free_lex_data` is truthy
 *
 * will NOT free ANY of the elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_lex_data_destroy(struct ic_lex_data *lex_data, unsigned int free_lex_data) {
    if (!lex_data) {
        puts("ic_lex_data_destroy: lex_data was null");
        return 0;
    }

    /* DO NOT free any elements within */

    if (free_lex_data) {
        free(lex_data);
    }

    return 1;
}
