#include <stdio.h> /* puts, printf */
#include <stdlib.h> /* calloc */

#include "expression.h"
#include "symbol.h"
#include "pvector.h"

/* allocate and initialise a new func call
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr_func_call * ic_expr_func_call_new(char *name, unsigned int name_len){
    struct ic_expr_func_call *fcall = 0;

    if( ! name ){
        puts("ic_expr_func_call_new: name was null");
        return 0;
    }

    fcall = calloc(1, sizeof(struct ic_expr_func_call));
    if( ! fcall ){
        puts("ic_expr_func_call_init: calloc failed");
        return 0;
    }

    if( ic_expr_func_call_init(fcall, name, name_len) ){
        puts("ic_expr_func_call_init: call to ic_expr_func_call_init failed");
        return 0;
    }

    return fcall;
}

/* intialise an existing func call
 *
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_expr_func_call_init(struct ic_expr_func_call *fcall, char *name, unsigned int name_len){
    if( ! fcall ){
        puts("ic_expr_func_call_init: fcall was null");
        return 1;
    }

    if( ! name ){
        puts("ic_expr_func_call_init: name was null");
        return 1;
    }

    /* call init on components */

    if( ic_symbol_init( &(fcall->fname), name, name_len ) ){
        puts("ic_expr_func_call_init: call to ic_symbol_init failed");
        return 1;
    }

    if( ic_pvector_init( &(fcall->args), 0 ) ){
        puts("ic_expr_func_call_init: call to ic_pvector_init failed");
        return 1;
    }

    return 0;

}

/* add a new argument to this function call
 *
 * returns offset of arg on success
 * returns -1 on failure
 */
int ic_expr_func_call_add_arg(struct ic_expr_func_call *fcall, struct ic_field *field){
    if( ! fcall ){
        puts("ic_expr_func_call_add_arg: fcall was null");
        return -1;
    }
    if( ! field ){
        puts("ic_expr_func_call_add_arg: field was null");
        return -1;
    }

    /* let pvector do al the work */
    return ic_pvector_append( &(fcall->args), field );
}

/* get argument
 *
 * returns field at offset on success
 * returns 0 on failure
 */
struct ic_field * ic_expr_func_call_get_arg(struct ic_expr_func_call *fcall, unsigned int i){
    if( ! fcall ){
        puts("ic_expr_func_call_get_arg: fcall was null");
        return 0;
    }

    /* let pvector do al the work */
    return ic_pvector_get( &(fcall->args), i );
}

/* returns number of arguments on success
 * returns 0 on failure
 */
unsigned int ic_expr_func_call_length(struct ic_expr_func_call *fcall){
    if( ! fcall ){
        puts("ic_expr_func_call_length: fcall was null");
        return 0;
    }

    /* let pvector do al the work */
    return ic_pvector_length( &(fcall->args) );
}

/* allocate and initialise a new identifier
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr_identifier * ic_expr_identifier_new(char *id, unsigned int id_len){
    struct ic_expr_identifier * identifier = 0;

    if( ! id ){
        puts("ic_expr_identifier_new: id was null");
        return 0;
    }

    /* allocate */
    identifier = calloc(1, sizeof(struct ic_expr_identifier));
    if( ! identifier ){
        puts("ic_expr_identifier_new: calloc failed");
        return 0;
    }

    /* initialise */
    if( ic_expr_identifier_init(identifier, id, id_len) ){
        puts("ic_expr_identifier_new: call to ic_expr_identifier_init failed");
        return 0;
    }

    /*return */
    return identifier;
}

/* initialise an existing identifier
 * returns 0 on success
 * returns 1 on failure
 */
unsigned int ic_expr_identifier_init(struct ic_expr_identifier * identifier, char *id, unsigned int id_len){
    if( ! identifier ){
        puts("ic_expr_identifier_init: identifier was null");
    }

    if( ! id ){
        puts("ic_expr_identifier_init: id was null");
        return 1;
    }

    /* init our symbol name */
    if( ic_symbol_init( &(identifier->identifier), id, id_len ) ){
        puts("ic_expr_identifier_init: call to ic_symbol_init failed");
        return 1;
    }

    return 0;
}

/* allocate and initialise a new op
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr_operator * ic_expr_operator_new(struct ic_expr *lexpr, struct ic_expr *rexpr, char *op, unsigned int op_len){
    struct ic_expr_operator *operator = 0;

    if( ! lexpr ){
        puts("ic_expr_operator_new: lexpr was null");
        return 0;
    }

    if( ! rexpr ){
        puts("ic_expr_operator_new: rexpr was null");
        return 0;
    }

    if( ! op ){
        puts("ic_expr_operator_new: op char* was null");
        return 0;
    }

    /* allocate */
    operator = calloc(1, sizeof(struct ic_expr_operator));
    if( ! operator ){
        puts("ic_expr_operator_new: calloc failed");
        return 0;
    }

    /* initialise */
    if( ic_expr_operator_init(operator, lexpr, rexpr, op, op_len) ){
        puts("ic_expr_operator_new: call to ic_expr_operator_init failed");
        return 0;
    }

    /* success */
    return operator;
}

/* initialise an existing op
 *
 * returns 0 on sucess
 * returns 1 on failure
 */
unsigned int ic_expr_operator_init(struct ic_expr_operator *operator, struct ic_expr *lexpr, struct ic_expr *rexpr, char *op, unsigned int op_len){

    if( ! operator ){
        puts("ic_expr_operator_init: operator was null");
        return 1;
    }

    if( ! lexpr ){
        puts("ic_expr_operator_init: lexpr was null");
        return 1;
    }

    if( ! rexpr ){
        puts("ic_expr_operator_init: rexpr was null");
        return 1;
    }

    if( ! op ){
        puts("ic_expr_operator_init: op char* was null");
        return 1;
    }


    /* initialise symbol op */
    if( ic_symbol_init( &(operator->op), op, op_len ) ){
        puts("ic_expr_operator_init: call to ic_symbol_init failed");
        return 1;
    }

    /* assign sub expressions */
    operator->lexpr = lexpr;
    operator->rexpr = rexpr;

    /* success */
    return 0;
}

/* allocate and initialise a new ic_expr
 * will not initialise union members
 *
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr * ic_expr_new(enum ic_expr_type type){
    struct ic_expr *expr = 0;

    /* alloc */
    expr = calloc(1, sizeof(struct ic_expr));
    if( ! expr ){
        puts("ic_expr_new: calloc failed");
        return 0;
    }

    /* init */
    if( ic_expr_init(expr, type) ){
        puts("ic_expr_new: call to ic_expr_init failed");
        return 0;
    }

    return expr;
}

/* initialise an existing ic_expr
 * will not initialise union members
 *
 * returns 0 on success
 * returns 1 on failure
 */
int ic_expr_init(struct ic_expr *expr, enum ic_expr_type type){
    if( ! expr ){
        puts("ic_expr_init: expr was null");
        return 1;
    }

    /* we only initialise the type */
    expr->type = type;

    /* we do NOT initialise the union members */

    return 0;
}

/* return pointer to fcall within,
 * will only succeed if expr is of the correct type
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_expr_func_call * ic_expr_get_fcall(struct ic_expr *expr){
    if( ! expr ){
        puts("ic_expr_get_fcall: expr was null");
        return 0;
    }

    /* check type before breaking into union */
    if( expr->type != ic_expr_type_func_call ){
        puts("ic_expr_get_fcall: type was incorrect");
        return 0;
    }

    /* all is clear, give them what they want */
    return &(expr->u.fcall);
}

/* return pointer to identifier within,
 * will only succeed if expr is of the correct type
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_expr_identifier * ic_expr_get_identifier(struct ic_expr *expr){
    if( ! expr ){
        puts("ic_expr_get_identifier: expr was null");
        return 0;
    }

    /* check type before breaking into union */
    if( expr->type != ic_expr_type_identifier ){
        puts("ic_expr_get_identifier: type was incorrect");
        return 0;
    }

    /* all is clear, give them what they want */
    return &(expr->u.id);
}

/* return pointer to constant within,
 * will only succeed if expr is of the correct type
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_expr_constant * ic_expr_get_constant(struct ic_expr *expr){
    if( ! expr ){
        puts("ic_expr_get_constant: expr was null");
        return 0;
    }

    /* check type before breaking into union */
    if( expr->type != ic_expr_type_constant ){
        puts("ic_expr_get_constant: type was incorrect");
        return 0;
    }

    /* all is clear, give them what they want */
    return &(expr->u.cons);
}

/* return pointer to operator within,
 * will only succeed if expr is of the correct type
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_expr_operator * ic_expr_get_operator(struct ic_expr *expr){
    if( ! expr ){
        puts("ic_expr_get_operator: expr was null");
        return 0;
    }

    /* check type before breaking into union */
    if( expr->type != ic_expr_type_operator ){
        puts("ic_expr_get_operator: type was incorrect");
        return 0;
    }

    /* all is clear, give them what they want */
    return &(expr->u.op);
}



