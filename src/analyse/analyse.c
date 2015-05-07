#include <stdlib.h> /* FIMXE exit */
#include <stdio.h>

#include "../parse/data/field.h"
#include "analyse.h"

/* ignored unused parameter */
#pragma GCC diagnostic ignored "-Wunused-parameter"
/* ignored unused variable */
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"


/* takes an ast and performs analysis on it
 * this returns a kludge
 *
 * see kludge->errors for errors
 *
 * returns kludge on success
 * returns 0 on failure
 */
struct ic_kludge * ic_analyse(struct ic_ast *ast){
    /* our offset into various lists */
    unsigned int i = 0;
    /* cached len of various lists */
    unsigned int len = 0;

    /* the current type we are analysing */
    struct ic_type_decl *tdecl = 0;
    /* the current func we are analysing */
    struct ic_func_decl *fdecl = 0;

    /* our mighty kludge */
    struct ic_kludge *kludge = 0;

    /* steps:
     *      create kludge from ast
     *      for each type call ic_analyse_type_decl
     *      for each func call ic_analyse_func_decl
     */

    if( ! ast ){
        puts("ic_analyse: ast null");
        return 0;
    }

    /* create kludge from ast */
    kludge = ic_kludge_new(ast);
    if( ! kludge ){
        puts("ic_analyse: call to ic_kludge_new failed");
        return 0;
    }
    /* for each type call ic_analyse_type_decl */
    len = ic_pvector_length(&(kludge->tdecls));
    for( i=0; i<len; ++i ){
        tdecl = ic_pvector_get(&(kludge->tdecls), i);
        if( ! tdecl ){
            puts("ic_analyse: call to ic_pvector_get failed for tdecl");
            return 0;
        }

        if( ic_analyse_type_decl(kludge, tdecl) ){
            puts("ic_analyse: call to ic_analyse_type_decl failed");
            return 0;
        }
    }

    /* for each func call analyst_func_decl */
    len = ic_pvector_length(&(kludge->fdecls));
    for( i=0; i<len; ++i ){
        fdecl = ic_pvector_get(&(kludge->fdecls), i);
        if( ! fdecl ){
            puts("ic_analyse: call to ic_pvector_get failed for fdecl");
            return 0;
        }

        if( ic_analyse_func_decl(kludge, fdecl) ){
            puts("ic_analyse: call to ic_analyse_type_decl failed");
            return 0;
        }
    }

    return kludge;
}

/* takes a type_decl and performs analysis
 *
 * FIXME need a way of signalling and passing errors
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_analyse_type_decl(struct ic_kludge *kludge, struct ic_type_decl *tdecl){
    /* index into fields */
    unsigned int i = 0;
    /* len of fields */
    unsigned int len = 0;
    /* current field */
    struct ic_field *field = 0;
    /* current name from field */
    char *name = 0;
    /* current type from field */
    char *type = 0;

    if( ! kludge ){
        puts("ic_analyse_type_decl: kludge was null");
        return 1;
    }

    if( ! tdecl ){
        puts("ic_analyse_type_decl: tdecl was null");
        return 1;
    }

    /* iterate through each field
     * for each field we check:
     *      the field name is unique within this type decl
     *      the field type exists
     */
    len = ic_pvector_length(&(tdecl->fields));
    for( i=0; i<len; ++i ){
        field = ic_pvector_get(&(tdecl->fields), i);
        if( ! field ){
            puts("ic_analyse_type_decl: call to ic_pvector_get failed");
            return 1;
        }

        name = ic_symbol_contents(&(field->name));
        if( ! type ){
            puts("ic_analyse_type_decl: call to ic_symbol_contents failed for name");
            return 1;
        }

        /* FIXME
         * check name is unique within this type decl
         */

        type = ic_symbol_contents(&(field->type));
        if( ! type ){
            puts("ic_analyse_type_decl: call to ic_symbol_contents failed for type");
            return 1;
        }

        /* check that type exists */
        if( ! ic_kludge_get_tdecl(kludge, type) ){
            printf("ic_analyse_type_decl: type '%s' \
                    mentioned in type declaration for '%s' \
                    does not exist within this kludge\n",
                    type,
                    ic_type_decl_str(tdecl));
            return 1;
        }
    }

    return 0;
}

/* takes a func_decl and performs analysis
 *
 * FIXME need a way of signalling and passing errors
 *
 * returns 0 on success
 * returns 1 on error
 */
unsigned int ic_analyse_func_decl(struct ic_kludge *kludge, struct ic_func_decl *fdecl){
    puts("ic_analyse_func_decl: unimplemented");
    exit(1);
}

/* takes an expr and returns the inferred type as a symbol
 *
 * FIXME need a way of signalling error and passing errors
 *
 * examples:
 *  infer 1 -> Int
 *  infer "helo" -> String
 *  infer addone(1) -> addone(Int)->Int -> Int
 *  infer Foo(1 "hello") -> Foo(Int String) -> Foo
 *
 * returns symbol on success
 * returns 0 on error
 */
struct ic_symbol * ic_analyse_infer(struct ic_kludge *kludge, struct ic_expr *expr){
    puts("ic_analyse_infer: unimplemented");
    exit(1);
}

/* check a statement for validity
 *
 * FIXME need a way of signalling and passing errors
 *
 * examples:
 *  check let f::Foo = Foo(addone(1) "hello")
 *  check d(f)
 *  check print(s)
 *
 * returns 0 for success
 * returns 1 for error
 */
unsigned int ic_analyse_check(struct ic_kludge *kludge, struct ic_stmt *stmt){
    puts("ic_analyse_check: unimplemented");
    exit(1);
}


