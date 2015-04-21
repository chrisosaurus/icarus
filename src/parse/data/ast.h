#ifndef ICARUS_AST_H
#define ICARUS_AST_H

#include "../../data/symbol.h"
#include "../../data/pvector.h"
#include "decl.h"

struct ic_ast {
    struct ic_pvector decls;
};

/* allocate and initialise a new ast
 *
 * returns pointer to new ast on success
 * returns 0 on failure
 */
struct ic_ast * ic_ast_new(void);

/* initialise a pre-existing ast
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_ast_init(struct ic_ast *ast);

/* calls destroy on all elements within
 *
 * this will only free the ast if `free_ast` is truthy
 *
 * the caller must determine if it is appropriate
 * or not to call free(ast)
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_ast_destroy(struct ic_ast *ast, unsigned int free_ast);

/* get item stores at index i
 *
 * returns pointer to item on success
 * returns 0 on failure
 */
struct ic_decl * ic_ast_get(struct ic_ast *ast, unsigned int i);

/* append decl to ast
 * ast will resize to make room
 *
 * returns index of item on success
 * returns -1 on failure
 */
int ic_ast_append(struct ic_ast *ast, struct ic_decl *decl);

/* returns length on success
 * returns 0 on failure
 */
unsigned int ic_ast_length(struct ic_ast *ast);

/* calls print on all elements within ast */
void ic_ast_print(struct ic_ast *ast);

#endif
