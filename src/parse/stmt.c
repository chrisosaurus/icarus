#include <stdio.h> /* puts */

#include "../parse.h"

/* ignore unused parameter warnings */
#pragma GCC diagnostic ignored "-Wunused-parameter"

/* ignore unused functions */
#pragma GCC diagnostic ignored "-Wunused-function"


/* there are currently 3 types of statements
 *  let
 *  if
 *  expr (void context)
 */


/* consume token
 * returns ic_stmt* on success
 * returns 0 on failure
 */
static struct ic_stmt * ic_parse_stmt_let(struct ic_tokens *tokens, unsigned int *i){
    puts("ic_parse_stmt_let: unimplemented");
    return 0;
}

/* consume token
 * returns ic_stmt* on success
 * returns 0 on failure
 */
static struct ic_stmt * ic_parse_stmt_if(struct ic_tokens *tokens, unsigned int *i){
    puts("ic_parse_stmt_if: unimplemented");
    return 0;
}

/* consume token
 * returns ic_stmt* on success
 * returns 0 on failure
 */
static struct ic_stmt * ic_parse_stmt_expr(struct ic_tokens *tokens, unsigned int *i){
    puts("ic_parse_stmt_expr: unimplemented");
    return 0;
}

struct ic_stmt * ic_parse_stmt(struct ic_tokens *tokens, unsigned int *i){
    puts("ic_parse_stmt: unimplemented");
    return 0;
}
