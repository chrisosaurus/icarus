#ifndef ICARUS_PARSE_NAME_HELPERS_H
#define ICARUS_PARSE_NAME_HELPERS_H

#include "../data/pvector.h"
#include "../data/symbol.h"

/*
 * generate a fully mangled name for an instantiated thing
 *
 * name is mandatory
 * type_params, type_args, and args are optional
 * only one of type_params OR type_args may be provided
 * returned symbol is owned by caller
 *
 * fn foo[Signed,Maybe[Unsigned]](m::Maybe[Vector[Signed]], b::Unsigned)
 *
 * =>
 *
 * foo_t_Signed_Maybe_t_Unsigned_u_u_a_Maybe_t_Vector_t_Signed_u_u_Unsigned_b
 * foo [ Signed Maybe [ Unsigned ] ] ( Maybe [ Vector [ Signed ] ],Unsigned )
 *
 *
 * so this function returns "foo_t_Signed_Maybe_t_Unsigned_u_u_a_Maybe_t_Vector_t_Signed_u_u_Unsigned_b" as a symbol
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
 * returned symbol is owned by caller
 *
 * fn println(Signed) ... end
 * =>
 * println(Signed)
 *
 * fn id[T](t::T) -> T ... end
 * instantiated for Signed
 * =>
 * id[Signed](Signed)
 *
 * union Maybe[T] ... end
 * instantiated for Signed
 * =>
 * Maybe[Signed]
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_symbol * ic_parse_helper_full_name(struct ic_symbol *name, struct ic_pvector *type_params, struct ic_pvector *type_args, struct ic_pvector *args);

/* for a thing, build a full name with holes
 *
 * name is mandatory
 * type_slots and args are optional
 * returned symbol is owned by caller
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
 * Maybe[Signed]
 *
 * fn foo[Signed,Maybe[Unsigned]](m::Maybe[Vector[Signed]], b::Unsigned)
 * =>
 * foo[_,_](_,_)
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_symbol * ic_parse_helper_generic_name(struct ic_symbol *name, struct ic_pvector *type_slots, struct ic_pvector *args);

/* for a fully non-generic thing, build a guess at a fully instantiated name
 *
 * this does not require analysis and will only look for symbols
 * this is only used by kludge to generate a name for an fdecl *before* analysis is performed
 *
 * name is mandatory
 * type_args, and args are optional
 * returned symbol is owned by caller
 *
 * fn foo(a::Signed,b::Maybe[Signed]) -> Bar ... end
 * =>
 * println(Signed,Maybe[Signed])
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_symbol * ic_parse_helper_initial_name(struct ic_symbol *name, struct ic_pvector *type_args, struct ic_pvector *args);

#endif
