#include <stdio.h>

#include "../../analyse/data/type.h"
#include "2c.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"

unsigned int ic_b2c_generate(struct ic_kludge *kludge, FILE *f);
unsigned int ic_b2c_generate_builtins(struct ic_kludge *kludge, FILE *f);
unsigned int ic_b2c_generate_types_pre(struct ic_kludge *kludge, FILE *f);
unsigned int ic_b2c_generate_types(struct ic_kludge *kludge, FILE *f);
unsigned int ic_b2c_generate_types_body(struct ic_kludge *kludge, struct ic_decl_type *tdecl, FILE *f);
unsigned int ic_b2c_generate_functions_header(struct ic_kludge *kludge, struct ic_decl_func *fdecl, FILE *f);
unsigned int ic_b2c_generate_functions_pre(struct ic_kludge *kludge, FILE *f);
unsigned int ic_b2c_generate_functions(struct ic_kludge *kludge, FILE *f);
unsigned int ic_b2c_generate_functions_body(struct ic_kludge *kludge, struct ic_decl_func *fdecl, FILE *f);
unsigned int ic_b2c_generate_entry(struct ic_kludge *kludge, FILE *f);

/* taking a processed ast and a string containing the path to
 * the desired output file
 *
 * generate a c program
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_b2c_compile(struct ic_kludge *kludge, char *out_filename){
    FILE *f = 0;

    if(! kludge ){
        puts("ic_b2c_compile: kludge was null");
        return 0;
    }

    if(! out_filename ){
        puts("ic_b2c_compile: out_filename was null");
        return 0;
    }

    /* try opening file out writing, complain is exists */
    f = fopen(out_filename, "r");
    if( f ){
        printf("ic_b2c_compile: error file already existed '%s'\n", out_filename);
        return 0;
    }

    /* open file for writing */
    f = fopen(out_filename, "w");
    if( ! f ){
        printf("ic_b2c_compile: failed to open out_filename '%s'\n", out_filename);
        return 0;
    }

    /* perform heavy lifting */
    if( ! ic_b2c_generate(kludge, f) ){
        puts("ic_b2c_compile: call to ic_b2c_generate failed");
        return 0;
    }

    fclose(f);
    return 1;
}

unsigned int ic_b2c_generate(struct ic_kludge *kludge, FILE *f){

    if( ! ic_b2c_generate_builtins(kludge, f) ){
        puts("ic_b2c_generate: call to ic_b2c_generate_builtins failed");
        return 0;
    }

    /* want to generate pre-decl for all types before we define them */
    if( ! ic_b2c_generate_types_pre(kludge, f) ){
        puts("ic_b2c_generate: call to ic_b2c_generate_types_pre failed");
        return 0;
    }

    /* want to generate pre-decl for all funcs before we define them */
    if( ! ic_b2c_generate_functions_pre(kludge, f) ){
        puts("ic_b2c_generate: call to ic_b2c_generate_functions failed");
        return 0;
    }

    if( ! ic_b2c_generate_types(kludge, f) ){
        puts("ic_b2c_generate: call to ic_b2c_generate_types failed");
        return 0;
    }

    if( ! ic_b2c_generate_functions(kludge, f) ){
        puts("ic_b2c_generate: call to ic_b2c_generate_functions failed");
        return 0;
    }

    if( ! ic_b2c_generate_entry(kludge, f) ){
        puts("ic_b2c_generate: call to ic_b2c_generate_entry failed");
        return 0;
    }

    return 1;
}

/* generate code for builtins */
unsigned int ic_b2c_generate_builtins(struct ic_kludge *kludge, FILE *f){
    if( ! kludge ){
        puts("ic_b2c_generate_builtins: kludge was null");
        return 0;
    }

    if( ! f ){
        puts("ic_b2c_generate_builtins: file was null");
        return 0;
    }

    fputs("#include \"backends/2c/builtins.c\"\n", f);
    return 1;
}

/* generate type declarations */
unsigned int ic_b2c_generate_types_pre(struct ic_kludge *kludge, FILE *f){
    struct ic_decl_type *type = 0;
    unsigned int n_types = 0;
    unsigned int i = 0;
    char *type_name = 0;

    if( ! kludge ){
        puts("ic_b2c_generate_types_pre: kludge was null");
        return 0;
    }

    if( ! f ){
        puts("ic_b2c_generate_types_pre: file was null");
        return 0;
    }

    n_types = ic_pvector_length(&(kludge->tdecls));

    for( i=0; i<n_types; ++i ){
        type = ic_pvector_get(&(kludge->tdecls), i);
        if( ! type ){
            puts("ic_b2c_generate_types_pre: call to ic_pvector_get failed");
            return 0;
        }

        type_name = ic_decl_type_str(type);

        /* skip builtins */
        if( ic_decl_type_isbuiltin(type) ){
            // printf("Skipping type '%s' as builtin\n", type_name);
            continue;
        }

        /* generate */
        fprintf(f, "typedef struct %s %s;\n", type_name, type_name);
    }

    puts("ic_b2c_generate_types_pre: implementation pending");
    return 1;
}

/* generate type definitions */
unsigned int ic_b2c_generate_types(struct ic_kludge *kludge, FILE *f){
    struct ic_decl_type *type = 0;
    unsigned int n_types = 0;
    unsigned int i = 0;
    char *type_name = 0;

    if( ! kludge ){
        puts("ic_b2c_generate_types: kludge was null");
        return 0;
    }

    if( ! f ){
        puts("ic_b2c_generate_types: file was null");
        return 0;
    }

    n_types = ic_pvector_length(&(kludge->tdecls));

    for( i=0; i<n_types; ++i ){
        type = ic_pvector_get(&(kludge->tdecls), i);
        if( ! type ){
            puts("ic_b2c_generate_types: call to ic_pvector_get failed");
            return 0;
        }

        type_name = ic_decl_type_str(type);

        /* skip builtins */
        if( ic_decl_type_isbuiltin(type) ){
            //printf("Skipping type '%s' as builtin\n", type_name);
            continue;
        }

        /* generate */
        fprintf(f, "typedef struct %s {\n", type_name);
        if( ! ic_b2c_generate_types_body(kludge, type, f) ){
            puts("ic_b2c_generate_types: call to ic_b2c_generate_types_body failed");
            return 0;
        }
        fprintf(f, "} %s;\n", type_name);
    }

    return 1;
}

/* called by ic_b2c_generate_types
 * handles actual generate of type definition body
 */
unsigned int ic_b2c_generate_types_body(struct ic_kludge *kludge, struct ic_decl_type *tdecl, FILE *f){
    struct ic_field *field = 0;
    unsigned int n_fields = 0;
    unsigned int i = 0;
    char *field_type = 0;
    char *field_name = 0;

    if( ! kludge ){
        puts("ic_b2c_generate_types_body: kludge was null");
        return 0;
    }

    if( ! tdecl ){
        puts("ic_b2c_generate_types_body: tdecl was null");
        return 0;
    }

    if( ! f ){
        puts("ic_b2c_generate_types_body: file was null");
        return 0;
    }

    n_fields = ic_pvector_length(&(tdecl->fields));

    for( i=0; i<n_fields; ++i ){
        field = ic_pvector_get(&(tdecl->fields), i);
        if( ! field ){
            puts("ic_b2c_generate_types_body: call to ic_pvector_get failed");
        }

        field_name = ic_symbol_contents(&(field->name));
        field_type = ic_symbol_contents( ic_type_ref_get_symbol(&(field->type)) );

        /* generate */
        fprintf(f, "\t%s *%s;\n", field_type, field_name);
    }

    return 1;
}

/* generate function header */
unsigned int ic_b2c_generate_functions_header(struct ic_kludge *kludge, struct ic_decl_func *fdecl, FILE *f){
    char *func_name = 0;
    char *func_sig_full = 0;
    char *func_return_type_str = 0;

    if( ! kludge ){
        puts("ic_b2c_generate_functions_header: kludge was null");
        return 0;
    }

    if( ! fdecl ){
        puts("ic_b2c_generate_functions_header: fdecl was null");
        return 0;
    }

    if( ! f ){
        puts("ic_b2c_generate_functions_header: file was null");
        return 0;
    }

    func_name = ic_symbol_contents(&(fdecl->name));
    if( ! func_name ){
        puts("ic_b2c_generate_functions: call to ic_string_contents failed for name");
        return 0;
    }

    func_sig_full = ic_decl_func_sig_full(fdecl);
    if( ! func_sig_full ){
        puts("ic_b2c_generate_functions: call to ic_decl_func_sig_full failed");
        return 0;
    }

    if( fdecl->ret_type ){
        /* FIXME need to convert to c type */
        func_return_type_str = ic_symbol_contents(fdecl->ret_type);
        if( ! func_return_type_str ){
            puts("ic_b2c_generate_functions: call to ic_symbol_contents failed for return type");
            return 0;
        }
    } else {
        /* c void */
        func_return_type_str = "void";
    }

    /* print comment */
    fprintf(f, "/* %s */\n", func_sig_full);

    /* return-type func-name( args ... ); */

    /* print return-type, name, and opening bracket */
    fprintf(f, "%s %s(", func_return_type_str, func_name);

    /* FIXME args */
    printf("pre: func '%s' only partially implemented (args missing)\n", func_sig_full);

    /* closing brace*/
    fputs(")", f);

    puts("ic_b2c_generate_functions_header: implementation pending");
    return 1;
}

/* generate function declarations */
unsigned int ic_b2c_generate_functions_pre(struct ic_kludge *kludge, FILE *f){
    struct ic_decl_func *func = 0;
    unsigned int n_funcs = 0;
    unsigned int i = 0;

    if( ! kludge ){
        puts("ic_b2c_generate_functions_pre: kludge was null");
        return 0;
    }

    if( ! f ){
        puts("ic_b2c_generate_functions_pre: file was null");
        return 0;
    }

    n_funcs = ic_pvector_length(&(kludge->fdecls));

    for( i=0; i<n_funcs; ++i ){
        func = ic_pvector_get(&(kludge->fdecls), i);
        if( ! func ){
            puts("ic_b2c_generate_functions_pre: call to ic_pvector_get failed");
            return 0;
        }

        /* skip builtins */
        if( ic_decl_func_isbuiltin(func) ){
            // printf("Skipping func '%s' as builtin\n", func_name);
            continue;
        }

        if( ! ic_b2c_generate_functions_header(kludge, func, f)) {
            puts("ic_b2c_generate_functions_pre: call to ic_b2c_generate_functions_header failed");
            return 0;
        }

        /* print trailing ; and \n */
        fputs(";\n", f);
    }

    puts("ic_b2c_generate_functions_pre: implementation pending");
    return 1;
}

/* generate function definitions */
unsigned int ic_b2c_generate_functions(struct ic_kludge *kludge, FILE *f){
    struct ic_decl_func *func = 0;
    unsigned int n_funcs = 0;
    unsigned int i = 0;
    char *func_sig_call = 0;

    if( ! kludge ){
        puts("ic_b2c_generate_functions: kludge was null");
        return 0;
    }

    if( ! f ){
        puts("ic_b2c_generate_functions: file was null");
        return 0;
    }

    n_funcs = ic_pvector_length(&(kludge->fdecls));

    for( i=0; i<n_funcs; ++i ){
        func = ic_pvector_get(&(kludge->fdecls), i);
        if( ! func ){
            puts("ic_b2c_generate_functions: call to ic_pvector_get failed");
            return 0;
        }

        func_sig_call = ic_decl_func_sig_call(func);
        if( ! func_sig_call ){
            puts("ic_b2c_generate_functions: call to ic_decl_func_sig_call failed");
            return 0;
        }

        /* skip builtins */
        if( ic_decl_func_isbuiltin(func) ){
            // printf("Skipping func '%s' as builtin\n", func_name);
            continue;
        }

        if( ! ic_b2c_generate_functions_header(kludge, func, f)) {
            puts("ic_b2c_generate_functions: call to ic_b2c_generate_functions_header failed");
            return 0;
        }

        /* opening { */
        fputs("{\n", f);

        /* FIXME body */

        /* closing } */
        fputs("}\n", f);

        /* FIXME generate */
        printf("Skipping func '%s' as unimplemented\n", func_sig_call);
    }

    puts("ic_b2c_generate_functions: implementation pending");
    return 1;
}

unsigned int ic_b2c_generate_functions_body(struct ic_kludge *kludge, struct ic_decl_func *fdecl, FILE *f){
    puts("ic_b2c_generate_functions_body: unimplemented");
    return 0;
}

/* generate code needed to enter into icarus
 * this will generate a c main function
 */
unsigned int ic_b2c_generate_entry(struct ic_kludge *kludge, FILE *f){
    if( ! kludge ){
        puts("ic_b2c_generate_entry: kludge was null");
        return 0;
    }

    if( ! f ){
        puts("ic_b2c_generate_entry: file was null");
        return 0;
    }

    fputs("#include \"backends/2c/entry.c\"\n", f);
    return 1;
}

