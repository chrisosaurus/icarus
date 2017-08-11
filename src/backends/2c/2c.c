#include <stdio.h>
#include <string.h>

#include "../../transform/data/tbody.h"
#include "2c.h"
#include "expr.h"
#include "tir.h"

unsigned int ic_b2c_compile_builtins(struct ic_kludge *kludge, FILE *f);

unsigned int ic_b2c_compile_type_struct_header(struct ic_kludge *kludge, struct ic_decl_type_struct *tdecl_struct, FILE *f);
unsigned int ic_b2c_compile_type_struct(struct ic_kludge *kludge, struct ic_decl_type_struct *tdecl_struct, FILE *f);

unsigned int ic_b2c_compile_type_union_header(struct ic_kludge *kludge, struct ic_decl_type_union *tdecl_union, FILE *f);
unsigned int ic_b2c_compile_type_union(struct ic_kludge *kludge, struct ic_decl_type_union *tdecl_union, FILE *f);

unsigned int ic_b2c_compile_type_header(struct ic_kludge *kludge, struct ic_decl_type *tdecl, FILE *f);
unsigned int ic_b2c_compile_types_pre(struct ic_kludge *kludge, FILE *f);
unsigned int ic_b2c_compile_type_body(struct ic_kludge *kludge, struct ic_decl_type *tdecl, FILE *f);
unsigned int ic_b2c_compile_type(struct ic_kludge *kludge, struct ic_decl_type *tdecl, FILE *f);
unsigned int ic_b2c_compile_types(struct ic_kludge *kludge, FILE *f);

unsigned int ic_b2c_compile_generates_pre(struct ic_kludge *kludge, FILE *f);
unsigned int ic_b2c_compile_generates(struct ic_kludge *kludge, FILE *f);

unsigned int ic_b2c_compile_function_header(struct ic_kludge *kludge, struct ic_decl_func *fdecl, FILE *f);
unsigned int ic_b2c_compile_functions_pre(struct ic_kludge *kludge, FILE *f);
unsigned int ic_b2c_compile_function_body(struct ic_kludge *kludge, struct ic_decl_func *fdecl, FILE *f);
unsigned int ic_b2c_compile_functions(struct ic_kludge *kludge, FILE *f);

unsigned int ic_b2c_compile_entry(struct ic_kludge *kludge, FILE *f);

/* taking a processed ast and a string containing the path to
 * the desired output file
 *
 * generate a c program
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_b2c_compile(struct ic_kludge *kludge, char *out_filename) {
    FILE *f = 0;

    if (!kludge) {
        puts("ic_b2c_compile: kludge was null");
        return 0;
    }

    if (!out_filename) {
        puts("ic_b2c_compile: out_filename was null");
        return 0;
    }

    /* try opening file out writing, complain is exists */
    f = fopen(out_filename, "r");
    if (f) {
        printf("ic_b2c_compile: error file already existed '%s'\n", out_filename);
        return 0;
    }

    /* open file for writing */
    f = fopen(out_filename, "w");
    if (!f) {
        printf("ic_b2c_compile: failed to open out_filename '%s'\n", out_filename);
        return 0;
    }

    if (!ic_b2c_compile_builtins(kludge, f)) {
        puts("ic_b2c_compile: call to ic_b2c_compile_builtins failed");
        return 0;
    }

    /* want to generate pre-decl for all types before we define them */
    if (!ic_b2c_compile_types_pre(kludge, f)) {
        puts("ic_b2c_compile: call to ic_b2c_compile_types_pre failed");
        return 0;
    }

    /* want to generate pre-decl for all generates before we define them */
    if (!ic_b2c_compile_generates_pre(kludge, f)) {
        puts("ic_b2c_compile: call to ic_b2c_compile_generates_pre failed");
        return 0;
    }

    /* want to generate pre-decl for all funcs before we define them */
    if (!ic_b2c_compile_functions_pre(kludge, f)) {
        puts("ic_b2c_compile: call to ic_b2c_compile_functions failed");
        return 0;
    }

    if (!ic_b2c_compile_types(kludge, f)) {
        puts("ic_b2c_compile: call to ic_b2c_compile_types failed");
        return 0;
    }

    if (!ic_b2c_compile_generates(kludge, f)) {
        puts("ic_b2c_compile: call to ic_b2c_compile_generates failed");
        return 0;
    }

    if (!ic_b2c_compile_functions(kludge, f)) {
        puts("ic_b2c_compile: call to ic_b2c_compile_functions failed");
        return 0;
    }

    if (!ic_b2c_compile_entry(kludge, f)) {
        puts("ic_b2c_compile: call to ic_b2c_compile_entry failed");
        return 0;
    }

    fclose(f);
    return 1;
}

/* generate code for builtins */
unsigned int ic_b2c_compile_builtins(struct ic_kludge *kludge, FILE *f) {
    if (!kludge) {
        puts("ic_b2c_compile_builtins: kludge was null");
        return 0;
    }

    if (!f) {
        puts("ic_b2c_compile_builtins: file was null");
        return 0;
    }

    fputs("#include \"backends/2c/builtins.c\"\n", f);
    return 1;
}

unsigned int ic_b2c_compile_generates_pre(struct ic_kludge *kludge, FILE *f) {
    unsigned int n_gens = 0;
    unsigned int i_gen = 0;
    struct ic_generate *gen = 0;
    struct ic_decl_type *tdecl = 0;
    struct ic_decl_func *fdecl = 0;

    char *sig_mangled_full_char = 0;

    struct ic_field *field = 0;
    char *field_name_char = 0;
    struct ic_symbol *field_type_sym = 0;
    char *field_type_char = 0;
    char *type_name = 0;

    if (!kludge) {
        puts("ic_b2c_compile_generates_pre: kludge was null");
        return 0;
    }

    if (!f) {
        puts("ic_b2c_compile_generates_pre: file was null");
        return 0;
    }

    n_gens = ic_kludge_generates_length(kludge);

    for (i_gen = 0; i_gen < n_gens; ++i_gen) {
        gen = ic_kludge_generates_get(kludge, i_gen);
        if (!gen) {
            puts("ic_b2c_compile_generates_pre: call to ic_kludge_generates_gen failed");
            return 0;
        }

        tdecl = ic_generate_get_tdecl(gen);
        if (!tdecl) {
            puts("ic_b2c_compile_generates_pre: call to ic_generate_get_tdecl failed");
            return 0;
        }

        fdecl = ic_generate_get_fdecl(gen);
        if (!fdecl) {
            puts("ic_b2c_compile_generates_pre: call to ic_generate_get_fdecl failed");
            return 0;
        }

        switch (gen->tag) {
            case ic_generate_tag_cons_struct:
                sig_mangled_full_char = ic_decl_type_sig_mangled_full(tdecl);
                if (!sig_mangled_full_char) {
                    puts("ic_b2c_compile_generates_pre: call to ic_decl_type_sig_mangled_full failed");
                    return 0;
                }

                fprintf(f, "%s;\n", sig_mangled_full_char);
                break;

            case ic_generate_tag_cons_union:
                /* we hit this once per field */
                sig_mangled_full_char = ic_decl_func_sig_mangled(fdecl);
                if (!sig_mangled_full_char) {
                    puts("ic_b2c_compile_generates_pre: call to ic_decl_func_sig_mangled failed");
                    return 0;
                }

                field = ic_generate_get_union_field(gen);
                if (!field) {
                    puts("ic_b2c_compile_generates_pre: call to ic_generate_get_union_field failed");
                    return 0;
                }

                field_name_char = ic_symbol_contents(&(field->name));
                if (!field_name_char) {
                    puts("ic_b2c_compile_generates_pre: call to ic_symbol_contents failed");
                    return 0;
                }

                field_type_sym = ic_type_ref_get_symbol(field->type);
                if (!field_type_sym) {
                    puts("ic_b2c_compile_generates_pre: call to ic_type_ref_get_symbol failed");
                    return 0;
                }

                field_type_char = ic_symbol_contents(field_type_sym);
                if (!field_type_char) {
                    puts("ic_b2c_compile_generates_pre: call to ic_symbol_contents failed");
                    return 0;
                }

                type_name = ic_decl_type_str(tdecl);
                if (!type_name) {
                    puts("ic_b2c_compile_generates_pre: call to ic_decl_type_str failed");
                    return 0;
                }

                /* union Foo
                 *  a::Sint
                 *  b::String <--
                 * end
                 *
                 * ->
                 *
                 * Foo i_Foo_a_String(String b);
                 */

                fprintf(f, "%s %s(%s %s);\n", type_name, sig_mangled_full_char, field_type_char, field_name_char);

                break;

            case ic_generate_tag_print:
            case ic_generate_tag_println:
                sig_mangled_full_char = ic_decl_func_sig_mangled(fdecl);
                if (!sig_mangled_full_char) {
                    puts("ic_b2c_compile_generates_pre: call to ic_decl_func_sig_mangled failed");
                    return 0;
                }

                field = ic_decl_func_args_get(fdecl, 0);
                if (!field) {
                    puts("ic_b2c_compile_generates_pre: call to ic_decl_func_args_get failed");
                    return 0;
                }

                field_name_char = ic_symbol_contents(&(field->name));
                if (!field_name_char) {
                    puts("ic_b2c_compile_generates_pre: call to ic_symbol_contents failed");
                    return 0;
                }

                field_type_sym = ic_type_ref_get_symbol(field->type);
                if (!field_type_sym) {
                    puts("ic_b2c_compile_generates_pre: call to ic_type_ref_get_symbol failed");
                    return 0;
                }

                field_type_char = ic_symbol_contents(field_type_sym);
                if (!field_type_char) {
                    puts("ic_b2c_compile_generates_pre: call to ic_symbol_contents failed");
                    return 0;
                }

                fprintf(f, "void %s(%s %s);\n", sig_mangled_full_char, field_type_char, field_name_char);

                break;

            default:
                puts("ic_b2c_compile_generates_pre: impossible gen->tag");
                return 0;
                break;
        }
    }

    return 1;
}

static unsigned int ic_b2c_compile_generate_print_struct(struct ic_kludge *kludge, FILE *f, struct ic_generate *gen, char *print_arg_name_char) {
    struct ic_decl_type *tdecl = 0;

    unsigned int i_field = 0;
    unsigned int n_fields = 0;
    struct ic_field *field = 0;

    char *field_name_char = 0;
    struct ic_symbol *field_type_sym = 0;
    char *field_type_char = 0;

    if (!kludge) {
        puts("ic_b2c_compile_generate_print_struct: kludge was null");
        return 0;
    }

    if (!f) {
        puts("ic_b2c_compile_generate_print_struct: f was null");
        return 0;
    }

    if (!gen) {
        puts("ic_b2c_compile_generate_print_struct: gen was null");
        return 0;
    }

    /* type Foo
     *  a::Sint
     *  b::String
     * end
     *
     * ->
     *
     * void print_a_Foo_i(Foo f){
     *  puts("Foo{");
     *  i_print_a_Sint(f->a);
     *  puts(", ");
     *  i_print_a_String(f->b);
     *  puts("}");
     * }
     */

    tdecl = ic_generate_get_tdecl(gen);
    if (!tdecl) {
        puts("ic_b2c_compile_generate_print_struct: call to ic_generate_get_tdecl failed");
        return 0;
    }

    /* for each field */
    n_fields = ic_decl_type_field_length(tdecl);
    for (i_field = 0; i_field < n_fields; ++i_field) {
        if (i_field > 0) {
            fputs("  fputs(\", \", stdout);\n", f);
        }

        field = ic_decl_type_field_get(tdecl, i_field);
        if (!field) {
            puts("ic_b2c_compile_generate_print_struct: call to ic_decl_field_get failed");
            return 0;
        }

        field_name_char = ic_symbol_contents(&(field->name));
        if (!field_name_char) {
            puts("ic_b2c_compile_generate_print_struct: call to ic_symbol_contents failed");
            return 0;
        }

        field_type_sym = ic_type_ref_get_symbol(field->type);
        if (!field_type_sym) {
            puts("ic_b2c_compile_generate_print_struct: call to ic_type_ref_get_symbol failed");
            return 0;
        }

        field_type_char = ic_symbol_contents(field_type_sym);
        if (!field_type_char) {
            puts("ic_b2c_compile_generate_print_struct: call to ic_symbol_contents failed");
            return 0;
        }

        fprintf(f, "  i_print_a_%s(%s->%s);\n", field_type_char, print_arg_name_char, field_name_char);
    }

    return 1;
}

static unsigned int ic_b2c_compile_generate_print_union(struct ic_kludge *kludge, FILE *f, struct ic_generate *gen, char *print_arg_name_char) {
    struct ic_decl_type *tdecl = 0;
    unsigned int n_fields = 0;
    unsigned int i_field = 0;
    struct ic_field *field = 0;

    char *field_name_char = 0;
    struct ic_symbol *field_type_sym = 0;
    char *field_type_char = 0;
    char *type_name_ch = 0;

    if (!kludge) {
        puts("ic_b2c_compile_generate_print_union: kludge was null");
        return 0;
    }

    if (!f) {
        puts("ic_b2c_compile_generate_print_union: f was null");
        return 0;
    }

    if (!gen) {
        puts("ic_b2c_compile_generate_print_union: gen was null");
        return 0;
    }

    if (!print_arg_name_char) {
        puts("ic_b2c_compile_generate_print_union: print_arg_name_char was null");
        return 0;
    }

    tdecl = ic_generate_get_tdecl(gen);
    if (!tdecl) {
        puts("ic_b2c_compile_generate_print_struct: call to ic_generate_get_tdecl failed");
        return 0;
    }

    type_name_ch = ic_decl_type_str(tdecl);
    if (!type_name_ch) {
        puts("ic_b2c_compile_generate: call to ic_decl_type_str failed");
        return 0;
    }

    /* union Foo
     *  a::Sint
     *  b::String
     * end
     *
     * ->
     *
     * void i_print_a_Foo(Foo f){
     *   i_print_a_String("Foo{");
     *   switch (f->_tag) {
     *     case Foo_tag_Sint_a:
     *       i_print_a_Sint(a->u.a);
     *       break;
     *     case Foo_tag_String_b:
     *       i_print_a_Sint(a->u.b);
     *       break;
     *     default:
     *       panic("impossibel tag on Foo);
     *   }
     *   i_print_a_String("}");
     * }
     */

    fprintf(f, "  switch (%s->_tag) {\n", print_arg_name_char);

    /* for each field */
    n_fields = ic_decl_type_field_length(tdecl);
    for (i_field = 0; i_field < n_fields; ++i_field) {
        field = ic_decl_type_field_get(tdecl, i_field);
        if (!field) {
            puts("ic_b2c_compile_generate_print_struct: call to ic_decl_field_get failed");
            return 0;
        }

        field_name_char = ic_symbol_contents(&(field->name));
        if (!field_name_char) {
            puts("ic_b2c_compile_generate_print_struct: call to ic_symbol_contents failed");
            return 0;
        }

        field_type_sym = ic_type_ref_get_symbol(field->type);
        if (!field_type_sym) {
            puts("ic_b2c_compile_generate_print_struct: call to ic_type_ref_get_symbol failed");
            return 0;
        }

        field_type_char = ic_symbol_contents(field_type_sym);
        if (!field_type_char) {
            puts("ic_b2c_compile_generate_print_struct: call to ic_symbol_contents failed");
            return 0;
        }

        fprintf(f, "    case %s_tag_%s_%s:\n", type_name_ch, field_type_char, field_name_char);
        fprintf(f, "      i_print_a_%s(%s->u.%s);\n", field_type_char, print_arg_name_char, field_name_char);
        fprintf(f, "      break;\n");
    }

    /* default */
    fputs("    default:\n", f);
    fprintf(f, "      panic(\"impossible tag on %s\");\n", type_name_ch);

    /* close switch */
    fputs("  }\n", f);

    /* success */
    return 1;
}

static unsigned int ic_b2c_compile_generate_print(struct ic_kludge *kludge, FILE *f, struct ic_generate *gen) {
    struct ic_decl_type *tdecl = 0;
    struct ic_decl_func *fdecl = 0;
    char *type_name_ch = 0;
    char *sig_mangled_full_char = 0;

    char *field_name_char = 0;
    char *print_arg_name_char = 0;

    unsigned int n_fields = 0;
    struct ic_field *field = 0;

    if (!kludge) {
        puts("ic_b2c_compile_generate_print: kludge was null");
        return 0;
    }

    if (!f) {
        puts("ic_b2c_compile_generate_print: f was null");
        return 0;
    }

    if (!gen) {
        puts("ic_b2c_compile_generate_print: gen was null");
        return 0;
    }

    tdecl = ic_generate_get_tdecl(gen);
    if (!tdecl) {
        puts("ic_b2c_compile_generate_print: call to ic_generate_get_tdecl failed");
        return 0;
    }

    /* common to both */
    tdecl = ic_generate_get_tdecl(gen);
    if (!tdecl) {
        puts("ic_b2c_compile_generate_print_struct: call to ic_generate_get_tdecl failed");
        return 0;
    }

    type_name_ch = ic_decl_type_str(tdecl);
    if (!type_name_ch) {
        puts("ic_b2c_compile_generate: call to ic_decl_type_str failed");
        return 0;
    }

    fdecl = ic_generate_get_fdecl(gen);
    if (!fdecl) {
        puts("ic_b2c_compile_generate_print_struct: call to ic_generate_get_fdecl failed");
        return 0;
    }

    sig_mangled_full_char = ic_decl_func_sig_mangled(fdecl);
    if (!sig_mangled_full_char) {
        puts("ic_b2c_compile_generate_print_struct: call to ic_decl_func_sig_mangled failed");
        return 0;
    }

    n_fields = ic_decl_func_args_length(fdecl);
    if (1 != n_fields) {
        puts("ic_b2c_compile_generate_print_struct: fdecl had wrong number of args for print");
        return 0;
    }

    field = ic_decl_func_args_get(fdecl, 0);
    if (!field) {
        puts("ic_b2c_compile_generate_print_struct: call to ic_decl_func_args_add failed");
        return 0;
    }

    field_name_char = ic_symbol_contents(&(field->name));
    if (!field_name_char) {
        puts("ic_b2c_compile_generate_print_struct: call to ic_symbol_contents failed");
        return 0;
    }

    print_arg_name_char = field_name_char;

    fprintf(f, "void %s(%s %s){\n", sig_mangled_full_char, type_name_ch, field_name_char);
    fprintf(f, "  fputs(\"%s{\", stdout);\n", type_name_ch);

    /* dispatch for internals */
    switch (tdecl->tag) {
        case ic_decl_type_tag_struct:
            if (!ic_b2c_compile_generate_print_struct(kludge, f, gen, print_arg_name_char)) {
                puts("ic_b2c_compile_generate_print: call to ic_b2c_compile_generate_print_struct failed");
                return 0;
            }
            break;

        case ic_decl_type_tag_union:
            if (!ic_b2c_compile_generate_print_union(kludge, f, gen, print_arg_name_char)) {
                puts("ic_b2c_compile_generate_print: call to ic_b2c_compile_generate_print_union failed");
                return 0;
            }
            break;

        default:
            puts("ic_b2c_compile_generate_print: impossible tdecl->tag");
            return 0;
            break;
    }

    /* close off */
    fputs("  fputs(\"}\", stdout);\n}\n", f);

    return 1;
}

unsigned int ic_b2c_compile_generates(struct ic_kludge *kludge, FILE *f) {
    unsigned int n_gens = 0;
    unsigned int i_gen = 0;
    struct ic_generate *gen = 0;
    struct ic_decl_type *tdecl = 0;
    struct ic_decl_func *fdecl = 0;
    char *type_name_ch = 0;

    char *sig_mangled_full_char = 0;

    unsigned int n_fields = 0;
    unsigned int i_field = 0;
    struct ic_field *field = 0;
    char *field_name_char = 0;

    struct ic_symbol *field_type_sym = 0;
    char *field_type_char = 0;

    if (!kludge) {
        puts("ic_b2c_compile_generate: kludge was null");
        return 0;
    }

    if (!f) {
        puts("ic_b2c_compile_generate: file was null");
        return 0;
    }

    n_gens = ic_kludge_generates_length(kludge);

    for (i_gen = 0; i_gen < n_gens; ++i_gen) {
        gen = ic_kludge_generates_get(kludge, i_gen);
        if (!gen) {
            puts("ic_b2c_compile_generate: call to ic_kludge_generates_get failed");
            return 0;
        }

        tdecl = ic_generate_get_tdecl(gen);
        if (!tdecl) {
            puts("ic_b2c_compile_generate: call to ic_generate_get_tdecl failed");
            return 0;
        }

        fdecl = ic_generate_get_fdecl(gen);
        if (!tdecl) {
            puts("ic_b2c_compile_generate: call to ic_generate_get_fdecl failed");
            return 0;
        }

        type_name_ch = ic_decl_type_str(tdecl);
        if (!type_name_ch) {
            puts("ic_b2c_compile_generate: call to ic_decl_type_str failed");
            return 0;
        }

        switch (gen->tag) {
            case ic_generate_tag_cons_struct:
                /* FIXME TODO this should instead use the fdecl inside gen
                 * rather than using tdecl's sig mangled full
                 */

                sig_mangled_full_char = ic_decl_type_sig_mangled_full(tdecl);
                if (!sig_mangled_full_char) {
                    puts("ic_b2c_compile_generate: call to ic_decl_type_sig_mangled_full failed");
                    return 0;
                }

                fprintf(f, "%s {\n", sig_mangled_full_char);

                /* checked alloc*/
                fprintf(f, "  %s tmp = ic_alloc(sizeof(struct %s));\n", type_name_ch, type_name_ch);

                /* assign each argument to field of same name */
                n_fields = ic_decl_type_field_length(tdecl);
                for (i_field = 0; i_field < n_fields; ++i_field) {
                    field = ic_decl_type_field_get(tdecl, i_field);
                    if (!field) {
                        puts("ic_b2c_compile_generate: call to ic_decl_field_get failed");
                        return 0;
                    }

                    field_name_char = ic_symbol_contents(&(field->name));
                    if (!field_name_char) {
                        puts("ic_b2c_compile_generate: call to ic_symbol_contents failed");
                        return 0;
                    }

                    fprintf(f, "  tmp->%s = %s;\n", field_name_char, field_name_char);
                }

                /* return */
                fputs("  return tmp;\n", f);

                fputs("}\n", f);
                break;

            case ic_generate_tag_cons_union:

                /* we hit this once per field */
                sig_mangled_full_char = ic_decl_func_sig_mangled(fdecl);
                if (!sig_mangled_full_char) {
                    puts("ic_b2c_compile_generates: call to ic_decl_func_sig_mangled failed");
                    return 0;
                }

                field = ic_generate_get_union_field(gen);
                if (!field) {
                    puts("ic_b2c_compile_generates: call to ic_generate_get_union_field failed");
                    return 0;
                }

                field_name_char = ic_symbol_contents(&(field->name));
                if (!field_name_char) {
                    puts("ic_b2c_compile_generates: call to ic_symbol_contents failed");
                    return 0;
                }

                field_type_sym = ic_type_ref_get_symbol(field->type);
                if (!field_type_sym) {
                    puts("ic_b2c_compile_generates: call to ic_type_ref_get_symbol failed");
                    return 0;
                }

                field_type_char = ic_symbol_contents(field_type_sym);
                if (!field_type_char) {
                    puts("ic_b2c_compile_generates: call to ic_symbol_contents failed");
                    return 0;
                }

                type_name_ch = ic_decl_type_str(tdecl);
                if (!type_name_ch) {
                    puts("ic_b2c_compile_generates: call to ic_decl_type_str failed");
                    return 0;
                }

                /* union Foo
                 *  a::Sint
                 *  b::String <--
                 * end
                 *
                 * ->
                 *
                 * Foo i_Foo_a_String(String b);
                 */

                fprintf(f, "%s %s(%s %s){\n", type_name_ch, sig_mangled_full_char, field_type_char, field_name_char);

                /* checked alloc*/
                fprintf(f, "  %s tmp = ic_alloc(sizeof(struct %s));\n", type_name_ch, type_name_ch);

                /* assign correct tag */
                fprintf(f, "  tmp->_tag = %s_tag_%s_%s;\n", type_name_ch, field_type_char, field_name_char);

                /* assign correct field */
                fprintf(f, "  tmp->u.%s = %s;\n", field_name_char, field_name_char);

                /* return */
                fputs("  return tmp;\n", f);

                fputs("}\n", f);
                break;

            case ic_generate_tag_print:
                if (!ic_b2c_compile_generate_print(kludge, f, gen)) {
                    puts("ic_b2c_compile_generate: call to ic_b2c_compile_generate_print failed");
                    return 0;
                }
                break;

            case ic_generate_tag_println:
                fdecl = ic_generate_get_fdecl(gen);
                if (!fdecl) {
                    puts("ic_b2c_compile_generate: call to ic_generate_get_fdecl failed");
                    return 0;
                }

                sig_mangled_full_char = ic_decl_func_sig_mangled(fdecl);
                if (!sig_mangled_full_char) {
                    puts("ic_b2c_compile_generate: call to ic_decl_func_sig_mangled failed");
                    return 0;
                }

                n_fields = ic_decl_func_args_length(fdecl);
                if (1 != n_fields) {
                    puts("ic_b2c_compile_generate: fdecl had wrong number of args for print");
                    return 0;
                }

                field = ic_decl_func_args_get(fdecl, 0);
                if (!field) {
                    puts("ic_b2c_compile_generate: call to ic_decl_func_args_add failed");
                    return 0;
                }

                field_name_char = ic_symbol_contents(&(field->name));
                if (!field_name_char) {
                    puts("ic_b2c_compile_generate: call to ic_symbol_contents failed");
                    return 0;
                }

                /* type Foo
                 *  ...
                 * end
                 *
                 * ->
                 *
                 * void println_a_Foo_i(Foo f){
                 *   i_print_a_Foo(f);
                 *   i_println_a();
                 * }
                 */

                fprintf(f, "void %s(%s %s){\n", sig_mangled_full_char, type_name_ch, field_name_char);
                fprintf(f, "  i_print_a_%s(%s);\n", type_name_ch, field_name_char);
                fputs("  i_println_a();\n}\n", f);
                break;

            default:
                puts("ic_b2c_compile_generate: impossible gen->tag");
                return 0;
                break;
        }
    }

    return 1;
}

unsigned int ic_b2c_compile_type_struct_header(struct ic_kludge *kludge, struct ic_decl_type_struct *tdecl_struct, FILE *f) {
    char *type_name = 0;
    if (!kludge) {
        puts("ic_b2c_compile_type_struct_header: kludge was null");
        return 0;
    }

    if (!tdecl_struct) {
        puts("ic_b2c_compile_type_struct_header: tdecl_struct was null");
        return 0;
    }

    if (!f) {
        puts("ic_b2c_compile_type_struct_header: f was null");
        return 0;
    }

    type_name = ic_decl_type_struct_str(tdecl_struct);
    if (!type_name) {
        puts("ic_b2c_compile_type_struct_header: call to ic_decl_type_struct_str failed");
        return 0;
    }

    /* generate */
    fprintf(f, "typedef struct %s * %s;\n", type_name, type_name);

    return 1;
}

unsigned int ic_b2c_compile_type_struct(struct ic_kludge *kludge, struct ic_decl_type_struct *tdecl_struct, FILE *f) {
    struct ic_field *field = 0;
    unsigned int n_fields = 0;
    unsigned int i = 0;
    char *field_type = 0;
    char *field_name = 0;
    char *type_name = 0;

    if (!kludge) {
        puts("ic_b2c_compile_type_struct: kludge was null");
        return 0;
    }

    if (!tdecl_struct) {
        puts("ic_b2c_compile_type_struct: tdecl_struct was null");
        return 0;
    }

    if (!f) {
        puts("ic_b2c_compile_type_struct: f was null");
        return 0;
    }

    type_name = ic_decl_type_struct_str(tdecl_struct);
    if (!type_name) {
        puts("ic_b2c_compile_type_struct: call to ic_decl_type_struct_str failed");
        return 0;
    }

    /* generate */
    fprintf(f, "struct %s {\n", type_name);

    n_fields = ic_decl_type_struct_field_length(tdecl_struct);

    for (i = 0; i < n_fields; ++i) {
        field = ic_decl_type_struct_field_get(tdecl_struct, i);
        if (!field) {
            puts("ic_b2c_compile_types_body: call to ic_decl_type_struct_field_get failed");
            return 0;
        }

        field_name = ic_symbol_contents(&(field->name));
        field_type = ic_symbol_contents(ic_type_ref_get_symbol(field->type));

        /* generate */
        fprintf(f, "  %s %s;\n", field_type, field_name);
    }

    fputs("};\n", f);

    return 1;
}

unsigned int ic_b2c_compile_type_union_header(struct ic_kludge *kludge, struct ic_decl_type_union *tdecl_union, FILE *f) {
    char *type_name = 0;

    if (!kludge) {
        puts("ic_b2c_compile_type_union_header: kludge was null");
        return 0;
    }

    if (!tdecl_union) {
        puts("ic_b2c_compile_type_union_header: tdecl_union was null");
        return 0;
    }

    if (!f) {
        puts("ic_b2c_compile_type_union_header: f was null");
        return 0;
    }

    type_name = ic_decl_type_union_str(tdecl_union);
    if (!type_name) {
        puts("ic_b2c_compile_type_union_header: call to ic_decl_type_union_str failed");
        return 0;
    }

    /* generate enum header */
    fprintf(f, "enum %s_tag;\n", type_name);

    /* generate type header */
    fprintf(f, "typedef struct %s * %s;\n", type_name, type_name);

    return 1;
}

unsigned int ic_b2c_compile_type_union(struct ic_kludge *kludge, struct ic_decl_type_union *tdecl_union, FILE *f) {
    char *type_name = 0;
    unsigned int i = 0;
    unsigned int n_fields = 0;
    struct ic_field *field = 0;
    char *field_name = 0;
    char *field_type = 0;

    if (!kludge) {
        puts("ic_b2c_compile_type_union: kludge was null");
        return 0;
    }

    if (!tdecl_union) {
        puts("ic_b2c_compile_type_union: tdecl_union was null");
        return 0;
    }

    if (!f) {
        puts("ic_b2c_compile_type_union: f was null");
        return 0;
    }

    type_name = ic_decl_type_union_str(tdecl_union);
    if (!type_name) {
        puts("ic_b2c_compile_type_union: call to ic_decl_type_union_str failed");
        return 0;
    }

    /* union Foo
     *  a::Sint
     *  b::String
     * end
     *
     * ->
     *
     * enum Foo_tag {
     *  Foo_tag_Sint_a,
     *  Foo_tag_String_b,
     * };
     *
     * struct Foo {
     *  enum Foo_tag _tag;
     *  union {
     *      Sint a;
     *      String b;
     *  };
     * };
     *
     */

    /* generate enum */
    fprintf(f, "enum %s_tag {\n", type_name);

    n_fields = ic_decl_type_union_field_length(tdecl_union);

    for (i = 0; i < n_fields; ++i) {
        field = ic_decl_type_union_field_get(tdecl_union, i);
        if (!field) {
            puts("ic_b2c_compile_types_body: call to ic_decl_type_union_field_get failed");
            return 0;
        }

        field_name = ic_symbol_contents(&(field->name));
        field_type = ic_symbol_contents(ic_type_ref_get_symbol(field->type));

        /* generate tag */
        fprintf(f, "  %s_tag_%s_%s,\n", type_name, field_type, field_name);
    }

    fputs("};\n", f);

    /* generate type */
    fprintf(f, "struct %s {\n", type_name);

    /* insert tag */
    fprintf(f, "  enum %s_tag _tag;\n", type_name);

    /* start union */
    fputs("  union {\n", f);

    /* insert fields */
    n_fields = ic_decl_type_union_field_length(tdecl_union);

    for (i = 0; i < n_fields; ++i) {
        field = ic_decl_type_union_field_get(tdecl_union, i);
        if (!field) {
            puts("ic_b2c_compile_types_body: call to ic_decl_type_union_field_get failed");
            return 0;
        }

        field_name = ic_symbol_contents(&(field->name));
        field_type = ic_symbol_contents(ic_type_ref_get_symbol(field->type));

        /* generate */
        fprintf(f, "    %s %s;\n", field_type, field_name);
    }

    /* end union */
    fputs("  } u;\n", f);

    fputs("};\n", f);

    return 1;
}

/* generate type header */
unsigned int ic_b2c_compile_type_header(struct ic_kludge *kludge, struct ic_decl_type *tdecl, FILE *f) {
    struct ic_decl_type_struct *tdecl_struct = 0;
    struct ic_decl_type_union *tdecl_union = 0;

    if (!kludge) {
        puts("ic_b2c_compile_type_header: kludge was null");
        return 0;
    }

    if (!tdecl) {
        puts("ic_b2c_compile_type_header: tdecl was null");
        return 0;
    }

    if (!f) {
        puts("ic_b2c_compile_type_header: file was null");
        return 0;
    }

    switch (tdecl->tag) {
        case ic_decl_type_tag_struct:
            tdecl_struct = ic_decl_type_get_struct(tdecl);
            if (!tdecl_struct) {
                puts("ic_b2c_compile_type_header: call to ic_decl_type_get_struct failed");
                return 0;
            }
            if (!ic_b2c_compile_type_struct_header(kludge, tdecl_struct, f)) {
                puts("ic_b2c_compile_type_header: call to ic_b2c_compile_type_struct_header failed");
                return 0;
            }
            return 1;
            break;

        case ic_decl_type_tag_union:
            tdecl_union = ic_decl_type_get_union(tdecl);
            if (!tdecl_union) {
                puts("ic_b2c_compile_type_header: call to ic_decl_type_get_union failed");
                return 0;
            }
            if (!ic_b2c_compile_type_union_header(kludge, tdecl_union, f)) {
                puts("ic_b2c_compile_type_header: call to ic_b2c_compile_type_union_header failed");
                return 0;
            }
            return 1;
            break;

        default:
            puts("ic_b2c_compile_type_header: unknown tag");
            return 0;
            break;
    }
}

/* generate type declarations */
unsigned int ic_b2c_compile_types_pre(struct ic_kludge *kludge, FILE *f) {
    struct ic_decl_type *tdecl = 0;
    unsigned int n_types = 0;
    unsigned int i = 0;

    if (!kludge) {
        puts("ic_b2c_compile_types_pre: kludge was null");
        return 0;
    }

    if (!f) {
        puts("ic_b2c_compile_types_pre: file was null");
        return 0;
    }

    n_types = ic_pvector_length(&(kludge->tdecls));

    for (i = 0; i < n_types; ++i) {
        tdecl = ic_pvector_get(&(kludge->tdecls), i);
        if (!tdecl) {
            puts("ic_b2c_compile_types_pre: call to ic_pvector_get failed");
            return 0;
        }

        /* skip builtins */
        if (ic_decl_type_isbuiltin(tdecl)) {
            /* printf("Skipping type '%s' as builtin\n", type_name); */
            continue;
        }

        /* do not compile is non-instantiated generic */
        if (!ic_decl_type_is_instantiated(tdecl)) {
            continue;
        }

        if (!ic_b2c_compile_type_header(kludge, tdecl, f)) {
            puts("ic_b2c_compile_types_pre: call to ic_b2c_compile_type_header failed");
            return 0;
        }
    }

    return 1;
}
unsigned int ic_b2c_compile_type(struct ic_kludge *kludge, struct ic_decl_type *tdecl, FILE *f) {
    if (!kludge) {
        puts("ic_b2c_compile_type: kludge was null");
        return 0;
    }

    if (!tdecl) {
        puts("ic_b2c_compile_type: tdecl was null");
        return 0;
    }

    if (!f) {
        puts("ic_b2c_compile_type: f was null");
        return 0;
    }

    /* do not compile is non-instantiated generic */
    if (!ic_decl_type_is_instantiated(tdecl)) {
        return 1;
    }

    switch (tdecl->tag) {
        case ic_decl_type_tag_struct:
            if (!ic_b2c_compile_type_struct(kludge, &(tdecl->u.tstruct), f)) {
                puts("ic_b2c_compile_type: call to ic_b2c_compile_type_struct failed");
                return 0;
            }
            break;

        case ic_decl_type_tag_union:
            if (!ic_b2c_compile_type_union(kludge, &(tdecl->u.tunion), f)) {
                puts("ic_b2c_compile_type: call to ic_b2c_compile_type_union failed");
                return 0;
            }
            break;

        default:
            puts("ic_b2c_compile_type: unknown tag");
            return 0;
            break;
    }

    return 1;
}

/* generate type definitions */
unsigned int ic_b2c_compile_types(struct ic_kludge *kludge, FILE *f) {
    struct ic_decl_type *tdecl = 0;
    unsigned int n_types = 0;
    unsigned int i = 0;

    if (!kludge) {
        puts("ic_b2c_compile_types: kludge was null");
        return 0;
    }

    if (!f) {
        puts("ic_b2c_compile_types: file was null");
        return 0;
    }

    n_types = ic_pvector_length(&(kludge->tdecls));

    for (i = 0; i < n_types; ++i) {
        tdecl = ic_pvector_get(&(kludge->tdecls), i);
        if (!tdecl) {
            puts("ic_b2c_compile_types: call to ic_pvector_get failed");
            return 0;
        }

        /* skip builtins */
        if (ic_decl_type_isbuiltin(tdecl)) {
            /* printf("Skipping type '%s' as builtin\n", type_name); */
            continue;
        }

        /* do not compile is non-instantiated generic */
        if (!ic_decl_type_is_instantiated(tdecl)) {
            continue;
        }

        if (!ic_b2c_compile_type(kludge, tdecl, f)) {
            puts("ic_b2c_compile_types: call to ic_b2c_compile_type failed");
            return 0;
        }
    }

    return 1;
}

/* called by ic_b2c_compile_types
 * handles actual generate of type definition body
 */
unsigned int ic_b2c_compile_type_body(struct ic_kludge *kludge, struct ic_decl_type *tdecl, FILE *f) {
    if (!kludge) {
        puts("ic_b2c_compile_types_body: kludge was null");
        return 0;
    }

    if (!tdecl) {
        puts("ic_b2c_compile_types_body: tdecl was null");
        return 0;
    }

    if (!f) {
        puts("ic_b2c_compile_types_body: file was null");
        return 0;
    }

    return 1;
}

/* generate function header */
unsigned int ic_b2c_compile_function_header(struct ic_kludge *kludge, struct ic_decl_func *fdecl, FILE *f) {
    char *func_sig_mangled = 0;
    char *func_sig_full = 0;
    char *func_return_type_str = 0;

    unsigned int i = 0;
    unsigned int len = 0;
    struct ic_field *arg = 0;
    char *arg_name = 0;
    char *arg_type = 0;

    if (!kludge) {
        puts("ic_b2c_compile_function_header: kludge was null");
        return 0;
    }

    if (!fdecl) {
        puts("ic_b2c_compile_function_header: fdecl was null");
        return 0;
    }

    if (!f) {
        puts("ic_b2c_compile_function_header: file was null");
        return 0;
    }

    func_sig_mangled = ic_decl_func_sig_mangled(fdecl);
    if (!func_sig_mangled) {
        puts("ic_b2c_compile_function_headers: call to ic_string_contents failed for sig_mangled");
        return 0;
    }

    func_sig_full = ic_decl_func_sig_full(fdecl);
    if (!func_sig_full) {
        puts("ic_b2c_compile_function_headers: call to ic_decl_func_sig_full failed");
        return 0;
    }

    /* print comment */
    fprintf(f, "/* %s */\n", func_sig_full);

    if (!fdecl->ret_type) {
        puts("ic_b2c_compile_function_headers: fdecl lacked return type");
        return 0;
    }

    /* FIXME need to convert to c type */
    func_return_type_str = ic_symbol_contents(fdecl->ret_type);
    if (!func_return_type_str) {
        puts("ic_b2c_compile_function_headers: call to ic_symbol_contents failed for return type");
        return 0;
    }

    fprintf(f, "%s %s(", func_return_type_str, func_sig_mangled);

    /* output arguments */
    len = ic_pvector_length(&(fdecl->args));
    for (i = 0; i < len; ++i) {
        if (i > 0) {
            fputs(", ", f);
        }

        arg = ic_pvector_get(&(fdecl->args), i);
        if (!arg) {
            puts("ic_b2c_compile_function_headers: call to ic_pvector_get failed");
            return 0;
        }

        arg_name = ic_symbol_contents(&(arg->name));
        arg_type = ic_symbol_contents(ic_type_ref_get_symbol(arg->type));

        /* generate */
        fprintf(f, "%s %s", arg_type, arg_name);
    }

    /* closing brace*/
    fputs(")", f);

    return 1;
}

/* generate function declarations */
unsigned int ic_b2c_compile_functions_pre(struct ic_kludge *kludge, FILE *f) {
    struct ic_decl_func *func = 0;
    unsigned int n_funcs = 0;
    unsigned int i = 0;

    if (!kludge) {
        puts("ic_b2c_compile_functions_pre: kludge was null");
        return 0;
    }

    if (!f) {
        puts("ic_b2c_compile_functions_pre: file was null");
        return 0;
    }

    n_funcs = ic_pvector_length(&(kludge->fdecls));

    for (i = 0; i < n_funcs; ++i) {
        func = ic_pvector_get(&(kludge->fdecls), i);
        if (!func) {
            puts("ic_b2c_compile_functions_pre: call to ic_pvector_get failed");
            return 0;
        }

        /* skip builtins */
        if (ic_decl_func_isbuiltin(func)) {
            /* printf("Skipping func '%s' as builtin\n", func_name); */
            continue;
        }

        /* do not compile is non-instantiated generic */
        if (!ic_decl_func_is_instantiated(func)) {
            continue;
        }

        if (!ic_b2c_compile_function_header(kludge, func, f)) {
            puts("ic_b2c_compile_functions_pre: call to ic_b2c_compile_function_header failed");
            return 0;
        }

        /* print trailing ; and \n */
        fputs(";\n", f);
    }

    return 1;
}

unsigned int ic_b2c_compile_function_body(struct ic_kludge *kludge, struct ic_decl_func *fdecl, FILE *f) {
    if (!kludge) {
        puts("ic_b2c_compile_function_body: kludge was null");
        return 0;
    }

    if (!fdecl) {
        puts("ic_b2c_compile_function_body: fdecl was null");
        return 0;
    }

    if (!f) {
        puts("ic_b2c_compile_function_body: f was null");
        return 0;
    }

    if (!ic_b2c_compile_body(kludge, fdecl->tbody, f)) {
        puts("ic_b2c_compile_function_body: call to ic_b2c_compile_body failed");
        return 0;
    }

    return 1;
}

/* generate function definitions */
unsigned int ic_b2c_compile_functions(struct ic_kludge *kludge, FILE *f) {
    struct ic_decl_func *func = 0;
    unsigned int n_funcs = 0;
    unsigned int i = 0;
    char *func_sig_call = 0;

    if (!kludge) {
        puts("ic_b2c_compile_functions: kludge was null");
        return 0;
    }

    if (!f) {
        puts("ic_b2c_compile_functions: file was null");
        return 0;
    }

    n_funcs = ic_pvector_length(&(kludge->fdecls));

    for (i = 0; i < n_funcs; ++i) {
        func = ic_pvector_get(&(kludge->fdecls), i);
        if (!func) {
            puts("ic_b2c_compile_functions: call to ic_pvector_get failed");
            return 0;
        }

        /* skip builtins */
        if (ic_decl_func_isbuiltin(func)) {
            /* printf("Skipping func '%s' as builtin\n", func_name); */
            continue;
        }

        /* do not compile is non-instantiated generic */
        if (!ic_decl_func_is_instantiated(func)) {
            continue;
        }

        func_sig_call = ic_decl_func_sig_call(func);
        if (!func_sig_call) {
            puts("ic_b2c_compile_functions: call to ic_decl_func_sig_call failed");
            return 0;
        }

        if (!ic_b2c_compile_function_header(kludge, func, f)) {
            puts("ic_b2c_compile_functions: call to ic_b2c_compile_function_header failed");
            return 0;
        }

        /* opening { */
        fputs("{\n", f);

        /* generate body */
        if (!ic_b2c_compile_function_body(kludge, func, f)) {
            puts("ic_b2c_compile_functions: call to ic_b2c_compile_function_body failed");
            return 0;
        }

        /* closing } */
        fputs("}\n", f);
    }

    return 1;
}

/* generate code needed to enter into icarus
 * this will generate a c main function
 */
unsigned int ic_b2c_compile_entry(struct ic_kludge *kludge, FILE *f) {
    if (!kludge) {
        puts("ic_b2c_compile_entry: kludge was null");
        return 0;
    }

    if (!f) {
        puts("ic_b2c_compile_entry: file was null");
        return 0;
    }

    fputs("#include \"backends/2c/entry.c\"\n", f);
    return 1;
}
