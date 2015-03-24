TODO:

* test_ast needs to be beefed up
* create test_parse
* test ic_expr_*
* test ic_stmt_*
* test ic_body_*
* need to work on ic constant print
* document parse function interfaces
* write remaining parse code
* parse_expr *
* call parse_body from fcall, if, etc.
* need to be able to print with indentation levels
* it will be useful for callers of ic_parse_check_token to be able to distinguish between error (null tokens, dist failed) and comparison failures (the token was not as expected)
* add escaping support to lexing string (escaping " with \)

