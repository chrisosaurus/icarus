TODO:
=====

 * break decl_union and decl_type into decl_type, decl_type_struct, and decl_type_union
 * there should be no 'symbols' which are types, these should all be type_ref

Should be:
==========

  decl
    decl_func
    decl_type
      name::sym
      enum {struct, union, enum, void, boolean, builtin?} (1)
      union {
        decl_type_struct
        decl_type_union
        decl_type_enum
      }

  (1) a builtin means 'provided by implementation' and 'opaque'
      so a builtin cannot have fields - Sint, Uint, Bool, String, Vector?
      but void and boolean are also builtin
      why do we cache void/bool here? optimisation?

  field
    name::sym
    type::type_ref

  type_ref (usage of a type, may be explicit symbol or inferred)
    enum state {symbol, found}
    union{
      sym::Sym
      type::decl_type
    }

Previous:
=========

  decl
    decl_func
      ...
    decl_type
      name::Sym
      fields...
      isvoid::Bool
      isbool::Bool
      isbuiltin::Bool
    decl_union.
      isvoid::
      name::Sym

  ic_field
    name::sym
    type::type_ref

  ic_type_ref
    enum {unknown, symbol, error}

  ic_type
    enum {user, builtin}
    union {decl_type}


