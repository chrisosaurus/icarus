This document quickly discusses some of the semantics on an icarus program


**Top level**
An icarus program at the top level consists of a series of declarations

One of which must be a function declaration for `main` which serves
as the entry point into the program, this function is called at
runtime and it governs the execution lifetime of the program.


**Variables**
All variables in icarus must be declared before they are referenced

Variables declarations are of the form

    let identifier::Type = expr

A let statement can occur anywhere within a function body,
all of the above elements are required:
it must specify an identifier, a type and a default value to initialise to.

Referencing an undeclared variable is a compile time error.


**Functions declarations**
In icarus you can overload functions so long as the combinations
of function name and types do not match an already declared one

This is valid:

    function foo(i::Signed) end
    function foo(i::Signed j::Signed) end
    function foo(s::String) end

However this is invalid:

    function foo(a::Signed) end
    # compile time error, existing function already declared
    function foo(b::Signed) end


**Function scope**
In icarus the top level is parsed before any function is entered,
this means that the order of function declarations is unimportant.

For example here `main` is able to call the function `foo` even though it
is after `main` in the file:

    function main()
        foo()
    end

    function foo()
        print("hello world")
    end


**Function dispatch**
In icarus functions are dispatched both on name and on argument types

If we define 3 functions `foo`:

    function foo(a::Signed) end
    function foo(a::Signed b::Signed) end
    function foo(a::String) end

And then call them:

    function main()
        foo(1)       # calls foo(a::Signed)
        foo(1 2)     # calls foo(a::Signed b::Signed)
        foo("hello") # calls foo(a::String)
    end

Function dispatch is done at compile time, and if a call cannot be
matched to a declaration then this is a compile time error:

    function bar(a::Signed) end

    function main()
        bar("hello") # compile time error, no bar(String) found
    end


