TODO:

* document parse function interfaces
* write remaining parse code
* it will be useful for callers of ic_parse_check_token to be able to distinguish between error (null tokens, dist failed) and comparison failures (the token was not as expected)
* add escaping support to lexing string (escaping " with \)
* add kludge testing
* an fcall should also store the found fdecl (post analysis)


bugs:

* return code: ic_pvector_destroy returns 1 on failure, ic_dict_destroy returns 0 on failure
* docs/internal/types.md specifies ic_type_ref -> ic_type -> ic_type_decl, however ic_type_ref currently refers directly to an ic_type_decl
* `,` vs space separated args (in decl and call) needs to be implemented correctly


testing debt:

* test_ast needs to be beefed up
* create test_parse
* test ic_expr_*
* test ic_stmt_*
* test ic_body_*


technical debt:

* duplicated code between ic_analyse_tdecl and ic_analyse_fdecl (arg list)
* parsing of '.' and ',' are not satisfactory, they are currently being caught as identifiers as the 'operator' code is only really for binary operators
* function return value on failure, need to standardise (e.g. symbol, some 0 on failure, others 1)
* all objects (esp. containers) need destructors (see `./scripts/destruct_audit.sh` for current list)
* add pvector/parray destructor helper which takes a function * and iterates through it's contents passing to function
* add shims to allow ic_pvector_destroy to be called on each type
* src/parse/data/ names, `statement`, `expression`, `decl` seems off (some shortened while others full)


considerations:

* consider const correctness
* consider how to deal with builtin types (int, string) and functions (print)
* consider const-correctness on read only args
* `ic_analyse_type_decl` currently allows co-recursive types `type Foo a::Bar end` `type bar a::Foo end`, is this allowed?
 * consider allowing free_data param to ic_pvector_destroy that is passed to (*destroy_item) rather than defaulting to true

