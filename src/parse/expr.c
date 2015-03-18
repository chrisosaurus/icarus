#include <stdio.h> /* puts */

#include "../parse.h"

/* ignore unused parameter warnings */
#pragma GCC diagnostic ignored "-Wunused-parameter"

/* ignore unused functions */
#pragma GCC diagnostic ignored "-Wunused-function"

/* current supported expression types:
 *  func call
 *  identifier
 *  constant
 *  operator application
 */


/* consume token
 * returns ic_expr* on success
 * returns 0 on failure
 */
static struct ic_expr * ic_parse_expr_fcall(struct ic_tokens *tokens, unsigned int *i){
    puts("ic_parse_expr_fcall: unimplemented");
    return 0;
}

/* consume token
 * returns ic_expr* on success
 * returns 0 on failure
 */
static struct ic_expr * ic_parse_expr_identifier(struct ic_tokens *tokens, unsigned int *i){
    puts("ic_parse_expr_identifier: unimplemented");
    return 0;
}

/* consume token
 * returns ic_expr* on success
 * returns 0 on failure
 */
static struct ic_expr * ic_parse_expr_constant(struct ic_tokens *tokens, unsigned int *i){
    puts("ic_parse_expr_constant: unimplemented");
    return 0;
}

/* consume token
 * returns ic_expr* on success
 * returns 0 on failure
 */
static struct ic_expr * ic_parse_expr_operator(struct ic_tokens *tokens, unsigned int *i){
    puts("ic_parse_expr_operator: unimplemented");
    return 0;
}


struct ic_expr * ic_parse_expr(struct ic_tokens *tokens, unsigned int *i){
    puts("ic_parse_expr: unimplemented");
    return 0;
}

