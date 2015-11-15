TODO:
-----

* `for` analysis case unimplemented
* `while` analysis case unimplemented
* removing mandated type form let
* the analysis system currently has no idea about permissions
* remove ic_type, as builtin and user defined types now both use ic_decl_type
* analyse should check for a `main` entry point
* analyse needs to check for failing to return from a non-void function
* finished population of decl_type.field_dict
* think through type_ref interface, and storing ic_type when found on fields/type_refs during ic_analyse_decl_type/ic_analyse_field_list
* remove parse/expr.c next_token usage, see IC_PERIOD handling comments in parse/expr.c
* decide on comma handling, currently skipped in parsing
* tokens should be used throughout parsing; passing around strings is lame, tokens should also be recorded against every ast node for traceability
* tokens should be used on any externally triggerable errors (users in a program's source that a user can fix)
* allow 'return' from void functions, this needs support at both parse and analyse levels
* decide on void type handling
* tidy up type_ref / symbol -> type handling, we now have kludge_get_type_from_type_ref and kludge_get_type_from_symbol
* document parse function interfaces
* write remaining parse code
* add escaping support to lexing string (escaping " with \)
* add kludge testing
* an fcall should also store the found function (fdecl or fbuiltin) (post analysis)
* symbol interning


bugs:
-----

* `,` vs space separated args (in decl and call) needs to be implemented correctly


testing debt:
-----

* test_ast needs to be beefed up
* create test_parse
* test ic_expr_*
* test ic_stmt_*
* test ic_body_*


technical debt:
-----

* not satisfied with complex expr parsing `if y == 4 && x == 5 ... end`
* having ops from core.ic AND in isoperator is a bit gross
* we changed _type to _tag for enums, but we didn't fix this values, e.g. ic_expr_tag has values of ic_expr_type_*
* figure out ownership of left/right or first/second for expr_faccess/expr_operator
* the parse module was written using the old lexer system and then retrofitted to the new, it could probably use some refactoring now that the lex output is more sane
* token_list api needs some cleanup, no one ever really uses the non _important calls
* `fdecl`, `tdecl`, `decl_type`, `decl_func` are all intermixed
* parse functions don't seem to check their arguments as well as analyse does
* see docs/coding.md 'Error handling' section
* all objects (esp. containers) need destructors (see `./scripts/destruct_audit.sh` for current list)
* add pvector/parray destructor helper which takes a function * and iterates through it's contents passing to function
* add shims to allow ic_pvector_destroy to be called on each type
* ic_dict interface set/insert is non-ideal, see linear_hash upstream TODO
* ic_scope insert interface is non-ideal
* ic_scope lacks an exists


considerations:
-----

* consider const-correctness on read only args
* `ic_analyse_decl_type` currently allows co-recursive types `type Foo a::Bar end` `type bar a::Foo end`, is this allowed?
* consider allowing free_data param to ic_pvector_destroy that is passed to (*destroy_item) rather than defaulting to true



