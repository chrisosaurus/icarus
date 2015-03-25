#include <stdio.h> /* puts, printf */
#include <stdlib.h> /* calloc */

#include "expression.h"
#include "symbol.h"
#include "pvector.h"
#include "parse.h"

/* ignore unused parameter */
#pragma GCC diagnostic ignored "-Wunused-parameter"

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
 * returns 0 on success
 * returns 1 on error
 */
int ic_expr_func_call_add_arg(struct ic_expr_func_call *fcall, struct ic_expr *expr){

    if( ! fcall ){
        puts("ic_expr_func_call_add_arg: fcall was null");
        return 0;
    }
    if( ! expr ){
        puts("ic_expr_func_call_add_arg: field was null");
        return 0;
    }

    /* let pvector do al the work */
    if( ic_pvector_append( &(fcall->args), expr ) == -1 ){
        puts("ic_expr_func_call_add_arg: call to ic_pvector_append failed");
        return 1;
    }

    return 0;
}

/* get argument
 *
 * returns field at offset on success
 * returns 0 on failure
 */
struct ic_expr * ic_expr_func_call_get_arg(struct ic_expr_func_call *fcall, unsigned int i){
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

/* print this func call */
void ic_expr_func_call_print(struct ic_expr_func_call *fcall, unsigned int *indent_level){
    /* our eventual return value */
    struct ic_expr *arg = 0;
    /* out iterator through args */
    unsigned int i = 0;
    /* cached length of vector */
    unsigned int len = 0;
    /* our fake indent level we use
     * as args do not need to be indented
     */
    unsigned int fake_indent = 0;

    if( ! fcall ){
        puts("ic_expr_func_call_print: fcall was null");
        return;
    }
    if( ! indent_level ){
        puts("ic_expr_func_call_print: fcall was indent_level");
        return;
    }

    len = ic_pvector_length(&(fcall->args));

    /* print indent */
    ic_parse_print_indent(*indent_level);

    /* print function name */
    ic_symbol_print(&(fcall->fname));

    /* print bracket */
    fputs("(", stdout);

    /* print args */
    for( i=0; i<len; ++i ){
        arg = ic_pvector_get( &(fcall->args), i );
        if( ! arg ){
            puts("ic_expr_func_call_print: call to ic_pvector_get failed");
            continue;
        }

        ic_expr_print(arg, &fake_indent);

        /* if we are not the last argument then print a space */
        if( i < (len - 1) ){
            fputs(" ", stdout);
        }
    }

    /* closing bracket */
    fputs(")", stdout);

    /* up to caller to work out \n placement */
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

/* allocate and init a new constant
 * returns pointer on success
 * returns 0 on failure
 */
struct ic_expr_constant * ic_expr_constant_new(enum ic_expr_constant_type type){
    struct ic_expr_constant * constant = 0;

    /* alloc */
    constant = calloc(1, sizeof(struct ic_expr_constant));
    if( ! constant ){
        puts("ic_expr_constant_new: call to calloc failed");
        return 0;
    }

    /* init */
    if( ic_expr_constant_init(constant, type) ){
        puts("ic_expr_constant_new: call to ic_expr_constant_init failed");
        return 0;
    }

    /* return */
    return constant;
}

/* initialise an existing constant
 *
 * returns 0 on success
 * returns 1 on error
 */
int ic_expr_constant_init(struct ic_expr_constant *constant, enum ic_expr_constant_type type){
    if( ! constant ){
        puts("ic_expr_constant_init: constant was null");
        return 1;
    }

    /* for now our only job is to set the type */
    constant->type = type;

    /* victory */
    return 0;
}

/* return pointer to integer within,
 * will only succeed if constant is of the correct type
 *
 * returns pointers on success
 * returns 0 on failure
 */
long int * ic_expr_constant_get_integer(struct ic_expr_constant *constant){
    if( ! constant ){
        puts("ic_expr_constant_get_integer: constant was null");
        return 0;
    }

    /* check type before handing out pointer */
    if( constant->type != ic_expr_constant_type_integer ){
        puts("ic_expr_constant_get_integer: not an integer");
        return 0;
    }

    /* give them what they want */
    return &(constant->u.integer);
}

/* return pointer to ic_string within,
 * will only succeed if constant is of the correct type
 *
 * returns pointers on success
 * returns 0 on failure
 */
struct ic_string * ic_expr_constant_get_string(struct ic_expr_constant *constant){
    if( ! constant ){
        puts("ic_expr_constant_get_string: constant was null");
        return 0;
    }

    /* check type before handing out pointer */
    if( constant->type != ic_expr_constant_type_string ){
        puts("ic_expr_constant_get_string: not an string");
        return 0;
    }

    /* give them what they want */
    return &(constant->u.string);
}


/* print this constant */
void ic_expr_constant_print(struct ic_expr_constant *constant, unsigned int *indent_level){
    /* pointer to string if we need it */
    struct ic_string *string = 0;
    /* pointer to long int if we need it */
    long int *integer = 0;

    if( ! constant ){
        puts("ic_expr_constant_print: constant was null");
        return;
    }
    if( ! indent_level ){
        puts("ic_expr_constant_print: indent_level was null");
        return;
    }

    switch( constant->type ){
        case ic_expr_constant_type_string:
            /* pull out our string */
            string = ic_expr_constant_get_string(constant);
            if( ! string ){
                puts("ic_expr_constant_print: call to ic_expr_constant_get_string failed");
                return;
            }

            /* print indent */
            ic_parse_print_indent(*indent_level);

            /* print out the backing string surrounded by double quotes */
            printf("\"%s\"", ic_string_contents(string));
            break;

        case ic_expr_constant_type_integer:
            /* print indent */
            ic_parse_print_indent(*indent_level);

            /* pull out our integer */
            integer = ic_expr_constant_get_integer(constant);
            if( ! integer ){
                puts("ic_expr_constant_print: call to ic_expr_constant_get_integer failed");
                return;
            }

            /* print out long integer */
            printf("%ld", *integer);
            break;

        default:
            puts("ic_expr_constant_print: unsupported type");
            break;
    }
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

/* print this operator */
void ic_expr_operator_print(struct ic_expr_operator *op, unsigned int *indent_level){
    /* fake indent we pass into sub expr */
    unsigned int fake_indent = 0;

    if( ! op ){
        puts("ic_expr_operator_print: op was null");
        return;
    }
    if( ! indent_level ){
        puts("ic_expr_operator_print: indent_level was null");
        return;
    }

    /* print indent before expr */
    ic_parse_print_indent(*indent_level);

    /* assuming infix
     * print:
     *  left op right
     */
    ic_expr_print(op->lexpr, &fake_indent);
    ic_symbol_print(&(op->op));
    ic_expr_print(op->rexpr, &fake_indent);
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

/* print this identifier */
void ic_expr_identifier_print(struct ic_expr_identifier * identifier, unsigned int *indent_level){
    if( ! identifier ){
        puts("ic_expr_identifier_print: identifier was null");
        return;
    }
    if( ! indent_level ){
        puts("ic_expr_identifier_print: indent_level was null");
        return;
    }

    /* print out indent */
    ic_parse_print_indent(*indent_level);

    /* print our identifier */
    ic_symbol_print(&(identifier->identifier));
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

/* print this expr */
void ic_expr_print(struct ic_expr *expr, unsigned int *indent_level){
    if( ! expr ){
        puts("ic_expr_print: called with null expr");
        return;
    }
    if( ! indent_level ){
        puts("ic_expr_print: called with indent_level expr");
        return;
    }

    switch( expr->type ){

        case ic_expr_type_func_call:
            ic_expr_func_call_print(&(expr->u.fcall), indent_level);
            break;

        case ic_expr_type_identifier:
            ic_expr_identifier_print(&(expr->u.id), indent_level);
            break;

        case ic_expr_type_constant:
            ic_expr_constant_print(&(expr->u.cons), indent_level);
            break;

        case ic_expr_type_operator:
            ic_expr_operator_print(&(expr->u.op), indent_level);
            break;

        default:
            puts("ic_expr_print: unsupported type");
            break;
    }
}


