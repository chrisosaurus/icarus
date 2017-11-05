#ifndef ICARUS_PARSE_NAME_HELPERS_H
#define ICARUS_PARSE_NAME_HELPERS_H

#include "../../data/pvector.h"
#include "../../data/symbol.h"

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
struct ic_symbol * ic_parse_helper_mangled_name(struct ic_symbol *name, struct ic_pvector *type_params, struct ic_pvector *type_args, struct ic_pvector *args);

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
struct ic_symbol * ic_parse_helper_full_name(struct ic_symbol *name, struct ic_pvector *type_params, struct ic_pvector *type_args, struct ic_pvector *args);

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
struct ic_symbol * ic_parse_helper_generic_name(struct ic_symbol *name, struct ic_pvector *type_slots, struct ic_pvector *args);

#endif
