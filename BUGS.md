Bugs:

Current operator parsing has to be rethought with regards to precedence


Current the following 2 pieces of code are ambiguous post-lexer

    # 2 expressions in void context
    foo
    (1 + 2)

    # a call to a function
    foo(1 + 2)

post lexing these are both

    foo ( 1 + 2 )

