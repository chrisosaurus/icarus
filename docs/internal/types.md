types
======

This document is a work in progress and reflects the future state of Icarus

In Icarus the concept of a type has a few moving parts

* ic_type - the concept of a type, either builtin or user defined, and points to ic_type_decl or ic_type_builtin
* ic_type_builtin - the information needed for a builtin
* ic_type_decl - the information captured for a user declared type
* ic_type_ref - a reference to a type, built during parse and modified during analysis


type_ref
--------

In icarus a type_ref has 5 possible states:

* unknown - we have no information yet about this type
* string - set by parser - we have a string type name from the parsre
* tdecl - set by inference - we have a type_decl found from the string or context
* builtin - set by inference - we have a type_builtin froun from the string or context
* error - a user fixable error occurred during inference

This models a progression from least to most known in the case of

    unknown -> string -> tdecl
    unknown -> string -> builtin
    string -> tdecl
    string -> builtin

After parsing all types will either be `unknown` or `string`

After analysis all types will either be `builtin`, `tdecl` or `error`

we now have a type for this concept `ic_type_ref`

most of the code using types exists within `src/parse/data/*` and
currently all of this code uses an `ic_symbol`

This should be used in:

* fields - on tdecls and fdecls
* variables
* return values - fdecls

