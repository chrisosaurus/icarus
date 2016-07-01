#ifndef IC_LEX_DATA
#define IC_LEX_DATA

struct ic_lex_data {
    char *filename;
    char *source;

    /* our output */
    struct ic_token_list *token_list;
    /* current line number into source, attached to tokens */
    unsigned int line_num;
    /* offset into line, attached to tokens */
    unsigned int offset_into_line;
    /* first char of this line */
    char *start_of_line;

    /* offset into source */
    unsigned int s_i;
    /* length of source */
    unsigned int s_len;
};

/* construct a new lex data
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_lex_data *ic_lex_data_new(char *filename, char *source);

/* init an existing lex data
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_lex_data_init(struct ic_lex_data *lex_data, char *filename, char *source);

/* destroy ic_lex_data
 *
 * will only free lex_data if `free_lex_data` is truthy
 *
 * will NOT free ANY of the elements
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_lex_data_destroy(struct ic_lex_data *lex_data, unsigned int free_lex_data);

#endif
