#include <stdio.h> /* puts, fprintf */
#include <stdlib.h> /* calloc*/

#include "ast.h"
#include "../../data/pvector.h"
#include "../../data/symbol.h"

/* allocate and initialise a new ast
 *
 * returns pointer to new ast on success
 * returns 0 on failure
 */
struct ic_ast * ic_ast_new(void){
    struct ic_ast *ast = 0;

    /* allocate new ast */
    ast = calloc(1, sizeof(struct ic_ast));
    if( ! ast ){
        puts("ic_ast_new: call to calloc failed");
        return 0;
    }

    /* initialise ast */
    if( ic_ast_init(ast) ){
        puts("ic_ast_new: call to ic_ast_init fialed");
        return 0;
    }

    return ast;
}

/* initialise a pre-existing ast
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_ast_init(struct ic_ast *ast){
    if( ! ast ){
        puts("ic_ast_init: ast was null");
        return 1;
    }

    /* initialise pvector decls to 0 cap */
    if( ic_pvector_init( &(ast->decls), 0 ) ){
        puts("ic_ast_init: call to ic_pvector_init failed");
        return 1;
    }

    return 0;
}

/* get item stores at index i
 *
 * returns pointer to item on success
 * returns 0 on failure
 */
struct ic_decl * ic_ast_get(struct ic_ast *ast, unsigned int i){
    if( ! ast ){
        puts("ic_ast_get: ast was null");
        return 0;
    }

    /* dispatch to pvector */
    return ic_pvector_get( &(ast->decls), i );
}

/* append decl to ast
 * ast will resize to make room
 *
 * returns index of item on success
 * returns -1 on failure
 */
int ic_ast_append(struct ic_ast *ast, struct ic_decl *decl){
    if( ! ast ){
        puts("ic_ast_append: ast was null");
        return -1;
    }

    if( ! decl ){
        puts("ic_ast_append: decl was null");
        return -1;
    }

    /* dispatch to pvector */
    return ic_pvector_append( &(ast->decls), decl );
}

/* returns length on success
 * returns 0 on failure
 */
unsigned int ic_ast_length(struct ic_ast *ast){
    if( ! ast ){
        puts("ic_ast_length: ast was null");
        return 0;
    }

    /* dispatch to pvector */
    return ic_pvector_length( &(ast->decls) );
}

/* calls print on all elements within ast */
void ic_ast_print(struct ic_ast *ast){
    /* current offset into pvector */
    unsigned int i = 0;
    /* length of ast pvector */
    unsigned int len = 0;
    /* current decl */
    struct ic_decl *decl = 0;

    /* current indent level
     * this is passed through the chain of print functions
     */
    unsigned int indent_level = 0;

    if( ! ast ){
        puts("ic_ast_print: ast was null");
        return ;
    }

    /* get length */
    len = ic_ast_length(ast);

    /* iterate through each element of ast */
    for( i=0; i < len; ++i ){
        /* get decl */
        decl = ic_ast_get(ast, i);
        if( ! decl ){
            puts("ic_ast_pring: call to ic_ast_get failed");
            return;
        }

        /* call print on it*/
        ic_decl_print(decl, &indent_level);

        /* pad with newlines between them
         * unless we are the last item
         */
        if( i < (len - 1) ){
            puts("");
        }
    }
}


