In icarus a type has 3 possible states:

* unknown - we have no information yet about this type
* string - set by parser - we have a string type name from the parsre
* tdecl - set by inference - we have a type_decl found from the string or context

This is also a progression from least to most understood, and after analysis
all types will either be an error or a tdecl

we now have a type for this concept `ic_type`

most of the code using types exists within `src/parse/data/*` and
currently all of this code uses an `ic_symbol`

we have to refactor all uses of a symbol to store a type to now use `ic_type`

This should be used in:

* fields - on tdecls and fdecls
* variables
* return values - fdecls

