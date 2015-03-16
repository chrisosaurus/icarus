#include <stdio.h> /* puts */
#include <stdlib.h> /* calloc */

#include "statement.h"

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
    return &(stmt->u.expr);
}


