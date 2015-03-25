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
* bring example/simple.ic back to it's former glory
* implement number parsing
* implement operator parsing
* need to check for operator before dispatching to string or number (say "hello" + or 1 " )

