types
======


future state
===============

This document is a work in progress and reflects the future state of Icarus


high level
----------

In Icarus the concept of a type has a few moving parts

at parse time we have:
* ic_type_ref - a reference to a type, built during parse
* ic_decl_type - the information captured for a user declared type

eventually we want the following dep. diagram:

            ic_decl_type - a type definition
             ^   ^
             |    \
             |     \   (reference via the symbol -> ic_type dict, not explicit)
             |      \
             |   ic_type_ref - a use of a type, the type of a variable or field at parse time, a symbol
             |       ^
             |       |
             |       |
             |    ic_field - a name and a type_ref, built at parse time for fields and variables
             |
             |
             |
          ic_slot - a name and an ic_type, built during analysis time from an ic_field


we also will have a mapping of symbols to ic_type for type lookup, this is how we get from an ic_type_ref to an ic_type.

ic_type_ref
-----------

ic_type_ref(s) are constructed during parse time and are a use of a type
at parse time.

In icarus a type_ref has 3 possible states:

* unknown - we have no information about this type, rely on type inference
* string - set by parser from source text of program
* error - a user fixable error occurred during inference

during the analyse phase an ic_type_ref will be converted into an ic_type *


