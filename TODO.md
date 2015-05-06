TODO:

* test_ast needs to be beefed up
* create test_parse
* test ic_expr_*
* test ic_stmt_*
* test ic_body_*
* document parse function interfaces
* write remaining parse code
* it will be useful for callers of ic_parse_check_token to be able to distinguish between error (null tokens, dist failed) and comparison failures (the token was not as expected)
* add escaping support to lexing string (escaping " with \)
* add kludge testing
* add unique field name check to ic_analyse_type_decl
* add field type check to ic_analyse_type_decl
* add unique type / func check in kludge populate insert
* add kludge 'get_fdecl' and 'get_tdecl' functions
* consider how to deal with builtin types (int, string) and functions (print)

technical debt:

* function return value on failure, need to standardise (e.g. symbol, some 0 on failure, others 1)
* consider const-correctness on read only args
* all objects (esp. containers) need destructors (see `./scripts/destruct_audit.sh` for current list)
* add pvector/parray destructor helper which takes a function * and iterates through it's contents passing to function
* add shims to allow ic_pvector_destroy to be called on each type
* src/parse/data/ names, `statement`, `expression`, `decl` seems off (some shortened while others full)

