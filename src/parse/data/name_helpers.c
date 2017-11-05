
#include <stdio.h>

#include "../../data/string.h"
#include "name_helpers.h"
#include "type_ref.h"
#include "type_param.h"

/*
 * generate a fully mangled name for an instantiated thing
 *
 * name is mandatory
 * type_params, type_args, and args are optional
 * only one of type_params OR type_args may be provided
 * returns symbol is owned by caller
 *
 * fn foo[Sint,Maybe[Uint]](m::Maybe[Vector[Sint]], b::Uint)
 *
 * =>
 *
 * foo_t_Sint_Maybe_t_Uint_u_u_a_Maybe_t_Vector_t_Sint_u_u_Uint_b
 * foo [ Sint Maybe [ Uint ] ] ( Maybe [ Vector [ Sint ] ],Uint )
 *
 *
 * so this function returns "foo_t_Sint_Maybe_t_Uint_u_u_a_Maybe_t_Vector_t_Sint_u_u_Uint_b" as a symbol
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_symbol * ic_parse_helper_mangled_name(struct ic_symbol *name, struct ic_pvector *type_params, struct ic_pvector *type_args, struct ic_pvector *args) {
    struct ic_symbol *sym = 0;
    struct ic_string *str = 0;

    struct ic_symbol *inner = 0;
    struct ic_field *arg = 0;
    struct ic_type_ref *type_ref = 0;
    struct ic_type_param *type_param = 0;

    unsigned int i = 0;
    unsigned int len = 0;

    if (!name) {
        puts("ic_parse_helper_mangled_name: name was null");
        return 0;
    }

    if (type_params && type_args) {
        puts("ic_parse_helper_mangled_name: both type_params and type_args were provided");
        return 0;
    }

    sym = ic_symbol_deep_copy(name);
    if (!sym) {
        puts("ic_parse_helper_mangled_name: call to ic_symbol_deep_copy failed");
        return 0;
    }

    str = &(sym->internal);

    if (type_params) {
        len = ic_pvector_length(type_params);
        if (len > 0) {
            if (!ic_string_append_char(str, "_t", 2)) {
                puts("ic_parse_helper_mangled_name: call to ic_string_append_char failed");
                return 0;
            }

            for (i=0; i<len; ++i) {
                if (!ic_string_append_char(str, "_", 1)) {
                    puts("ic_parse_helper_mangled_name: call to ic_string_append_char failed");
                    return 0;
                }

                type_param = ic_pvector_get(type_params, i);
                if (!type_param) {
                    puts("ic_parse_helper_mangled_name: call to ic_pvector_get failed");
                    return 0;
                }

                inner = ic_type_param_mangled_name(type_param);
                if (!inner) {
                    puts("ic_parse_helper_mangled_name: call to ic_type_param_mangled_name failed");
                    return 0;
                }

                if (!ic_string_append_symbol(str, inner)) {
                    puts("ic_parse_helper_mangled_name: call to ic_string_append_symbol failed");
                    return 0;
                }
            }

            if (!ic_string_append_char(str, "_u", 2)) {
                puts("ic_parse_helper_mangled_name: call to ic_string_append_char failed");
                return 0;
            }
        }
    } else if (type_args) {
        len = ic_pvector_length(type_args);
        if (len > 0) {
            if (!ic_string_append_char(str, "_t", 2)) {
                puts("ic_parse_helper_mangled_name: call to ic_string_append_char failed");
                return 0;
            }

            for (i=0; i<len; ++i) {
                if (!ic_string_append_char(str, "_", 1)) {
                    puts("ic_parse_helper_mangled_name: call to ic_string_append_char failed");
                    return 0;
                }

                type_ref = ic_pvector_get(type_args, i);
                if (!type_ref) {
                    puts("ic_parse_helper_mangled_name: call to ic_pvector_get failed");
                    return 0;
                }

                inner = ic_type_ref_mangled_name(type_ref);
                if (!inner) {
                    puts("ic_parse_helper_mangled_name: call to ic_type_param_mangled_name failed");
                    return 0;
                }

                if (!ic_string_append_symbol(str, inner)) {
                    puts("ic_parse_helper_mangled_name: call to ic_string_append_symbol failed");
                    return 0;
                }
            }

            if (!ic_string_append_char(str, "_u", 2)) {
                puts("ic_parse_helper_mangled_name: call to ic_string_append_char failed");
                return 0;
            }
        }
    }

    if (args) {
        if (!ic_string_append_char(str, "_a", 2)) {
            puts("ic_parse_helper_mangled_name: call to ic_string_append_char failed");
            return 0;
        }

        len = ic_pvector_length(args);
        for (i=0; i<len; ++i) {
            if (!ic_string_append_char(str, "_", 1)) {
                puts("ic_parse_helper_mangled_name: call to ic_string_append_char failed");
                return 0;
            }

            arg = ic_pvector_get(args, i);
            if (!arg) {
                puts("ic_parse_helper_mangled_name: call to ic_pvector_get failed");
                return 0;
            }

            type_ref = arg->type;
            if (!type_ref) {
                puts("ic_parse_helper_mangled_name: arg->type was null");
                return 0;
            }

            inner = ic_type_ref_mangled_name(type_ref);
            if (!inner) {
                puts("ic_parse_helper_mangled_name: call to ic_type_ref_mangled_name failed");
                return 0;
            }

            if (!ic_string_append_symbol(str, inner)) {
                puts("ic_parse_helper_mangled_name: call to ic_string_append_symbol failed");
                return 0;
            }
        }

        if (!ic_string_append_char(str, "_b", 2)) {
            puts("ic_parse_helper_mangled_name: call to ic_string_append_char failed");
            return 0;
        }
    }

    return sym;
}


/* for a fully instantiated generic thing, build a fully instantiated name
 *
 * name is mandatory
 * type_params,type_args, and args are optional
 * only one of type_params OR type_args may be provided
 * returns symbol is owned by caller
 *
 * fn println(Sint) ... end
 * =>
 * println(Sint)
 *
 * fn id[T](t::T) -> T ... end
 * instantiated for Sint
 * =>
 * id[Sint](Sint)
 *
 * union Maybe[T] ... end
 * instantiated for Sint
 * =>
 * Maybe[Sint]
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_symbol * ic_parse_helper_full_name(struct ic_symbol *name, struct ic_pvector *type_params, struct ic_pvector *type_args, struct ic_pvector *args) {
    struct ic_symbol *sym = 0;
    struct ic_string *str = 0;

    struct ic_symbol *inner = 0;
    struct ic_field *arg = 0;
    struct ic_type_ref *type_ref = 0;
    struct ic_type_param *type_param = 0;

    unsigned int i = 0;
    unsigned int len = 0;

    if (!name) {
        puts("ic_parse_helper_full_name: name was null");
        return 0;
    }

    if (type_params && type_args) {
        puts("ic_parse_helper_full_name: both type_params and type_args were provided");
        return 0;
    }

    sym = ic_symbol_deep_copy(name);
    if (!sym) {
        puts("ic_parse_helper_full_name: call to ic_symbol_deep_copy failed");
        return 0;
    }

    str = &(sym->internal);

    if (type_params) {
        len = ic_pvector_length(type_params);
        if (len > 0) {
            if (!ic_string_append_char(str, "[", 1)) {
                puts("ic_parse_helper_full_name: call to ic_string_append_char failed");
                return 0;
            }

            for (i=0; i<len; ++i) {
                if (i > 0) {
                    if (!ic_string_append_char(str, ",", 1)) {
                        puts("ic_parse_helper_mangled_name: call to ic_string_append_char failed");
                        return 0;
                    }
                }

                type_param = ic_pvector_get(type_params, i);
                if (!type_param) {
                    puts("ic_parse_helper_full_name: call to ic_pvector_get failed");
                    return 0;
                }

                inner = ic_type_param_full_name(type_param);
                if (!inner) {
                    puts("ic_parse_helper_full_name: call to ic_type_param_full_name failed");
                    return 0;
                }

                if (!ic_string_append_symbol(str, inner)) {
                    puts("ic_parse_helper_full_name: call to ic_string_append_symbol failed");
                    return 0;
                }
            }

            if (!ic_string_append_char(str, "]", 1)) {
                puts("ic_parse_helper_full_name: call to ic_string_append_char failed");
                return 0;
            }
        }
    } else if (type_args) {
        len = ic_pvector_length(type_args);
        if (len > 0 ){
            if (!ic_string_append_char(str, "[", 1)) {
                puts("ic_parse_helper_full_name: call to ic_string_append_char failed");
                return 0;
            }

            for (i=0; i<len; ++i) {
                if (i > 0) {
                    if (!ic_string_append_char(str, ",", 1)) {
                        puts("ic_parse_helper_mangled_name: call to ic_string_append_char failed");
                        return 0;
                    }
                }

                type_ref = ic_pvector_get(type_args, i);
                if (!type_ref) {
                    puts("ic_parse_helper_full_name: call to ic_pvector_get failed");
                    return 0;
                }

                inner = ic_type_ref_full_name(type_ref);
                if (!inner) {
                    puts("ic_parse_helper_full_name: call to ic_type_param_full_name failed");
                    return 0;
                }

                if (!ic_string_append_symbol(str, inner)) {
                    puts("ic_parse_helper_full_name: call to ic_string_append_symbol failed");
                    return 0;
                }
            }

            if (!ic_string_append_char(str, "]", 1)) {
                puts("ic_parse_helper_full_name: call to ic_string_append_char failed");
                return 0;
            }
        }
    }

    if (args) {
        if (!ic_string_append_char(str, "(", 1)) {
            puts("ic_parse_helper_full_name: call to ic_string_append_char failed");
            return 0;
        }

        len = ic_pvector_length(args);
        for (i=0; i<len; ++i) {
            if (i > 0) {
                if (!ic_string_append_char(str, ",", 1)) {
                    puts("ic_parse_helper_mangled_name: call to ic_string_append_char failed");
                    return 0;
                }
            }

            arg = ic_pvector_get(args, i);
            if (!arg) {
                puts("ic_parse_helper_full_name: call to ic_pvector_get failed");
                return 0;
            }

            type_ref = arg->type;
            if (!type_ref) {
                puts("ic_parse_helper_full_name: arg->type was null");
                return 0;
            }

            inner = ic_type_ref_full_name(type_ref);
            if (!inner) {
                puts("ic_parse_helper_full_name: call to ic_type_ref_full_name failed");
                return 0;
            }

            if (!ic_string_append_symbol(str, inner)) {
                puts("ic_parse_helper_full_name: call to ic_string_append_symbol failed");
                return 0;
            }
        }

        if (!ic_string_append_char(str, ")", 1)) {
            puts("ic_parse_helper_full_name: call to ic_string_append_char failed");
            return 0;
        }
    }

    return sym;
}

/* for a thing, build a full name with holes
 *
 * name is mandatory
 * type_slots and args are optional
 * returns symbol is owned by caller
 *
 * note that type_slots is just a pvector, the contents are not inspected
 * so it may contain either type_params or type_refs (for type_args)
 *
 * fn println(_) ... end
 * =>
 * println(_)
 *
 * fn id[T](t::T) -> T ... end
 * =>
 * id[_](_]
 *
 * union Maybe[T] ... end
 * =>
 * Maybe[Sint]
 *
 * fn foo[Sint,Maybe[Uint]](m::Maybe[Vector[Sint]], b::Uint)
 * =>
 * foo[_,_](_,_)
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_symbol * ic_parse_helper_generic_name(struct ic_symbol *name, struct ic_pvector *type_slots, struct ic_pvector *args) {
    struct ic_symbol *sym = 0;
    struct ic_string *str = 0;

    unsigned int i = 0;
    unsigned int len = 0;

    if (!name) {
        puts("ic_parse_helper_generic_name: name was null");
        return 0;
    }

    sym = ic_symbol_deep_copy(name);
    if (!sym) {
        puts("ic_parse_helper_generic_name: call to ic_symbol_deep_copy failed");
        return 0;
    }

    str = &(sym->internal);

    if (type_slots) {
        len = ic_pvector_length(type_slots);
        if (len > 0 ) {
            if (!ic_string_append_char(str, "[", 1)) {
                puts("ic_parse_helper_generic_name: call to ic_string_append_char failed");
                return 0;
            }

            for (i=0; i<len; ++i) {
                if (i > 0) {
                    if (!ic_string_append_char(str, ",", 1)) {
                        puts("ic_parse_helper_generic_name: call to ic_string_append_char failed");
                        return 0;
                    }
                }


                if (!ic_string_append_char(str, "_", 1)) {
                    puts("ic_parse_helper_generic_name: call to ic_string_append_char failed");
                    return 0;
                }
            }

            if (!ic_string_append_char(str, "]", 1)) {
                puts("ic_parse_helper_generic_name: call to ic_string_append_char failed");
                return 0;
            }
        }
    }

    if (args) {
        if (!ic_string_append_char(str, "(", 1)) {
            puts("ic_parse_helper_generic_name: call to ic_string_append_char failed");
            return 0;
        }

        len = ic_pvector_length(args);
        for (i=0; i<len; ++i) {
            if (i > 0) {
                if (!ic_string_append_char(str, ",", 1)) {
                    puts("ic_parse_helper_generic_name: call to ic_string_append_char failed");
                    return 0;
                }
            }

            if (!ic_string_append_char(str, "_", 1)) {
                puts("ic_parse_helper_generic_name: call to ic_string_append_char failed");
                return 0;
            }

        }

        if (!ic_string_append_char(str, ")", 1)) {
            puts("ic_parse_helper_generic_name: call to ic_string_append_char failed");
            return 0;
        }
    }

    return sym;
}

