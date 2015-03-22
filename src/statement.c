#include <stdio.h> /* puts */
#include <stdlib.h> /* calloc */

#include "statement.h"
#include "symbol.h"
#include "body.h"

/* allocate and initialise a new let
 * does not touch init ic_expr
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_stmt_let * ic_stmt_let_new(char *id_src, unsigned int id_len, char *type_src, unsigned int type_len){
    struct ic_stmt_let *let = 0;

    /* alloc */
    let = calloc(1, sizeof(struct ic_stmt_let));
    if( ! let ){
        puts("ic_stmt_let_new: calloc failed");
        return 0;
    }

    /* hand over for init
     * NB: we leave arg checking up to init
     */
    if( ic_stmt_let_init(let, id_src, id_len, type_src, type_len) ){
        puts("ic_stmt_let_new: call to ic_stmt_let_init failed");
        return 0;
    }

    return let;
}

/* initialise an existing let
 * does not touch the init expression
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_stmt_let_init(struct ic_stmt_let *let, char *id_src, unsigned int id_len, char *type_src, unsigned int type_len){
    if( ! let ){
        puts("ic_stmt_let_init: let was null");
        return 1;
    }

    if( ! id_src ){
        puts("ic_stmt_let_init: id_src was null");
        return 1;
    }

    if( ! type_src ){
        puts("ic_stmt_let_init: type_src was null");
        return 1;
    }

    /* dispatch to symbol init for id */
    if( ic_symbol_init( &(let->identifier), id_src, id_len ) ){
        puts("ic_smtm_let_init: call to ic_symbol_init for id failed");
        return 1;
    }

    /* dispatch to symbol init for type */
    if( ic_symbol_init( &(let->type), type_src, type_len ) ){
        puts("ic_smtm_let_init: call to ic_symbol_init for type failed");
        return 1;
    }

    /* zero out init */
    let->init = 0;

    return 0;
}

/* get the ic_expr * contained within
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr * ic_stmt_let_get_expr(struct ic_stmt_let *let){
    if( ! let ){
        puts("ic_stmt_let_get_expr: let was null");
        return 0;
    }

    /* return what they want */
    return let->init;
}

/* print this let */
void ic_stmt_let_print(struct ic_stmt_let *let){
    if( ! let ){
        puts("ic_stmt_let_print: let was null");
        return;
    }

    /* want to output
     * let identifier::type = init
     */

    fputs("let ", stdout);

    ic_symbol_print( &(let->identifier) );
    fputs("::", stdout);
    ic_symbol_print( &(let->type) );

    fputs(" = ", stdout);

    ic_expr_print(let->init);

    /* statements are displayed on their own line */
    puts("");
}


/* allocate and initialise a new ic_stmtm_if
 * this will initialise the body
 * but will NOT initialise the expression
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_stmt_if * ic_stmt_if_new(void){
    struct ic_stmt_if *sif = 0;

    /* alloc */
    sif = calloc(1, sizeof(struct ic_stmt_if));
    if( ! sif ){
        puts("ic_stmt_if_new: calloc failed");
        return 0;
    }

    /* init */
    if( ic_stmt_if_init(sif) ){
        puts("ic_stmt_if_new: call to ic_stmt_if_init failed");
        return 0;
    }

    return sif;
}

/* initialise an existing new ic_stmtm_if
 * this will initialise the body
 * but will NOT initialise the expression
 *
 * returns 0 on sucess
 * returns 1 on failure
 */
unsigned int ic_stmt_if_init(struct ic_stmt_if *sif){
    if( ! sif ){
        puts("ic_stmt_if_init: sif was null");
        return 1;
    }

    /* let body_init handle the work */
    if( ic_body_init( &(sif->body) ) ){
        puts("ic_stmt_if_init: call ot ic_body_init failed");
        return 1;
    }

    /* just zero out expr */
    sif->expr = 0;

    /* return success */
    return 0;
}

/* returns pointer on success
 * returns 0 on failure
 */
struct ic_expr * ic_stmt_if_get_expr(struct ic_stmt_if *sif){
    if( ! sif ){
        puts("ic_stmt_if_get_expr: sif was null");
        return 0;
    }

    /* return our expr innards */
    return sif->expr;
}

/* get statement of offset i within the body
 *
 * returns pointer to element on success
 * returns 0 on error
 */
struct ic_stmt * ic_stmt_if_get_stmt(struct ic_stmt_if *sif, unsigned int i){
    if( ! sif ){
        puts("ic_stmt_if_get_stmt: sif was null");
        return 0;
    }

    /* let body do the lifting */
    return ic_body_get( &(sif->body), i );
}

/* get length of body
 *
 * returns length on sucess
 * returns 0 on failure
 */
unsigned int ic_stmt_if_length(struct ic_stmt_if *sif){
    if( ! sif ){
        puts("ic_stmt_if_length: sif was null");
        return 0;
    }

    /* let body do the lifting */
    return ic_body_length( &(sif->body) );
}

/* print this if */
void ic_stmt_if_print(struct ic_stmt_if *sif){
    if( ! sif ){
        puts("ic_stmt_if_print: sif was null");
        return;
    }

    /* we want to print
     *  if expr
     *      body
     *  end
     */
    fputs("if ", stdout);
    ic_expr_print( sif->expr );
    puts("");

    /* print body */
    ic_body_print( &(sif->body) );

    /* statements are displayed on their own line */
    puts("end");
}


/* allocate and initialise anew ic_stmt
 * will not initialise union members
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_stmt * ic_stmt_new(enum ic_stmt_type type){
    struct ic_stmt * stmt = 0;

    /* alloc */
    stmt = calloc(1, sizeof(struct ic_stmt));
    if( ! stmt ){
        puts("ic_stmt_new: calloc failed");
        return 0;
    }

    /* init */
    if( ic_stmt_init(stmt, type) ){
        puts("ic_stmt_new: call to ic_stmt_init failed");
        return 0;
    }

    return stmt;
}

/* initialise an existing ic_stmt
 *
 * returns 0 on success
 * returns 1 on failure
 */
int ic_stmt_init(struct ic_stmt *stmt, enum ic_stmt_type type){
    if( ! stmt ){
        puts("ic_stmt_init: stmt was null");
        return 1;
    }

    /* only initialise type */
    stmt->type = type;

    /* do not touch union */

    return 0;
}

/* get a pointer to the let within
 * will only succeed if ic_stmt is of the correct type
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_stmt_let * ic_stmt_get_let(struct ic_stmt *stmt){
    if( ! stmt ){
        puts("ic_stmt_get_let: stmt was null");
        return 0;
    }

    /* check type before giving out */
    if( stmt->type != ic_stmt_type_let ){
        puts("ic_stmt_get_let: not of the correct type");
        return 0;
    }

    /* otherwise give them what they asked for */
    return &(stmt->u.let);
}

/* get a pointer to the sif within
 * will only succeed if ic_stmt is of the correct type
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_stmt_if * ic_stmt_get_sif(struct ic_stmt *stmt){
    if( ! stmt ){
        puts("ic_stmt_get_sif: stmt was null");
        return 0;
    }

    /* check type before giving out */
    if( stmt->type != ic_stmt_type_if ){
        puts("ic_stmt_get_sif: not of the correct type");
        return 0;
    }

    /* otherwise give them what they asked for */
    return &(stmt->u.sif);
}

/* get a pointer to the expr within
 * will only succeed if ic_stmt is of the correct type
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr * ic_stmt_get_expr(struct ic_stmt *stmt){
    if( ! stmt ){
        puts("ic_stmt_get_expr: stmt was null");
        return 0;
    }

    /* check type before giving out */
    if( stmt->type != ic_stmt_type_expr ){
        puts("ic_stmt_get_expr: not of the correct type");
        return 0;
    }

    /* otherwise give them what they asked for */
    return stmt->u.expr;
}

/* print this stmt */
void ic_stmt_print(struct ic_stmt *stmt){
    if( ! stmt ){
        puts("ic_stmt_print: stmt was null");
        return;
    }

    switch( stmt->type ){
        case ic_stmt_type_let:
            ic_stmt_let_print( &(stmt->u.let) );
            break;

        case ic_stmt_type_if:
            ic_stmt_if_print( &(stmt->u.sif) );
            break;

        case ic_stmt_type_expr:
            ic_expr_print( stmt->u.expr );
            /* statements are displayed on their own line */
            puts("");
            break;

        default:
            puts("ic_stmt_print: unknown type");
            return;
    }
}


