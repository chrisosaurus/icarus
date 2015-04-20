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

technical debt:

* function return value on failure, need to standardise (e.g. symbol, some 0 on failure, others 1)
* consider const-correctness on read only args
* memory leak on error cases in ic_parse loop, ast and contents are leaked on return 0 (src/parse/parse.c:121)
* memory leak on error cases in ic_parse_type_decl loop, decl and contents are leaked on return 0 (src/parse/parse.c:121)

