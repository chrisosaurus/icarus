TODO:
-----

* 2c generated types are incorrect for generic instantiated types - should be some mangled version of Maybe[T] but is currently just Maybe
* string/symbol should have equality testing methods that take sym/str/char
* add automatic print/println implementations for user-defined types
* refactor out locals handling in pancake/compile.c - should share logic when possible
* all debug/warning messages should go to stderr to make testing easier
* 'everything' within tir (except pointers to fdecl/kludge) should be owned - copy any shared symbols
* sort out transform ownership - it is a mess
* complete tir print implementations
* remove FIXME and commented out error handling in 2c once functions_body is implemented : /* FIXME TODO silencing error for now */
* uncommented `plus` alias for `concat` in stdlib/core.ic
* output generation includes a semi-builtin from src/core.ic: fn plus(a::String, b::String) -> String return concat(a, b) end appearing as String plus();
* rejig _print infra to always have a _str method (or similar) and have _print simply call this
  to string logic should not be inside _print
* add ability to bracket arbitrary expressions
* make lack of operator precedence clear - mandate bracketing to make order explicit (same repeated op is fine)
* `for` analysis case unimplemented
* `while` analysis case unimplemented
* the analysis system currently has no idea about permissions
* remove ic_type, as builtin and user defined types now both use ic_decl_type
* analyse needs to check for failing to return from a non-void function - need branch analysis (all branches result in return)
* finish population of decl_type.field_dict
* think through type_ref interface, and storing ic_type when found on fields/type_refs during ic_analyse_decl_type/ic_analyse_field_list
* remove parse/expr.c next_token usage, see IC_PERIOD handling comments in parse/expr.c
* tokens should be used throughout parsing; passing around strings is lame, tokens should also be recorded against every ast node for traceability
* tokens should be used on any externally triggerable errors (errors in a program's source that a user can fix)
* allow 'return' from void functions, this needs support at both parse and analyse levels
* decide on void type handling
* tidy up type_ref / symbol -> type handling, we now have kludge_get_type_from_type_ref and kludge_get_type_from_symbol
* document parse function interfaces
* write remaining parse code
* add escaping support to lexing string (escaping " with \)
* add kludge testing
* symbol interning


bugs:
-----
* due to "and " and "or " tokens including whitespace, lexer output is incorrect


testing debt:
-------------

* test_ast needs to be beefed up
* create test_parse
* test ic_expr_*
* test ic_stmt_*
* test ic_body_*


technical debt:
---------------

* transform__ir_expr should handle: fcall, identifier, literal
* pancake compile if should prefer relative jmps rather than j*_labels
* operator_destroy doesn't cleanup fcall
* should only return a pointer, all other values should be return via pointer in args, so that return values can always be tested for failure
* current naming scheme is getting too long
* not satisfied with complex expr parsing `if y == 4 && x == 5 ... end`
* having ops from core.ic AND in isoperator is a bit gross
* we changed _type to _tag for enums, but we didn't fix this values, e.g. ic_expr_tag has values of ic_expr_type_*
* figure out ownership of left/right or first/second for expr_faccess/expr_operator
* the parse module was written using the old lexer system and then retrofitted to the new, it could probably use some refactoring now that the lex output is more sane
* token_list api needs some cleanup, no one ever really uses the non _important calls
* `fdecl`, `tdecl`, `decl__type`, `decl_func` are all intermixed
* see docs/coding.md 'Error handling' section
* all objects (esp. containers) need destructors (see `./scripts/destruct_audit.sh` for current list)
* add pvector/parray destructor helper which takes a function * and iterates through it's contents passing to function
* add shims to allow ic_pvector_destroy to be called on each type
* any usage of `ic_symbol_contents(&` inline (without checking return value) should be killed with fire

considerations:
---------------

* consider const-correctness on read only args
* `ic_analyse_decl_type` currently allows co-recursive types `type Foo a::Bar end` `type bar a::Foo end`, is this allowed?
* consider allowing free_data param to ic_pvector_destroy that is passed to (*destroy_item) rather than defaulting to true
* decide on comma handling, currently skipped in parsing, and inserted in printing
* should we expose fixed width types only to icarus userspace ?



