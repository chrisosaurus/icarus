Icarus
======

Icarus is a toy language with static type checking

Inspiration has been drawn from (in alphabetical order):

* C
* C++
* C#
* Go
* Haskell
* Julia
* Python
* Racket
* Rust
* Scheme (r7rs-small, chicken)

Plans
=====

* arguments are passed as immutable reference by default, have to explicitly specify mutable reference - see `docs/reference.md`
* explicit function interface contracts around mutability of arguments - `example/mutability.ic`
* mutable values
* strong statically checked type system
* no 'holes' in the type system (null and void pointer type are all banished) - see `docs/noholes.md`
* no undefined behavior
* multiple dispatch (dispatch to functions is based on function name and type of arguments passed)
* operator overloading
* per-task managed memory (automatic garbage collection)
* concurrent message passing
* parametrized types (derived from C++, Julia, and Rust)
* union types (tagged enums, statically checked before being taken apart)
    * like Rust's enums
    * like haskell's ADTs (Algebraic Data Types)
* including both an interpreter and compiler(s) (currently considering both C and LLVM bitcode as compiler backend outputs) - see `docs/components.md`
* whitespace insensitive (except using \n to terminate a comment, but comments are stripped by the lexer for now)
* no commas (hail to the lisp)
* no semicolons

Work so far
===========

We have the contents of `example/simple.ic`:

    # user defined type with 2 fields, an Int and a String
    type Foo
        a::Int
        b::String
    end

    function d(i::Int)
        print(i)
    end

    function d(s::String)
        print(s)
    end

    # break apart a Foo and call d on each field
    function d(f::Foo)
        # commenting out as parser does not yet support operators
        #d(f.a)
        #d(f.b)
    end

    # entry point for program
    function main()
        # parser does not yet support numbers
        #let f::Foo = Foo(1 "hello")
        let f::Foo = Foo("hello")

        d(f)
    end

We can see what Icarus makes of this by running:

    make example

Hidden in the output we see the lexer output:

    lexer output:
    ----------------
    type Foo a :: Int b :: String end function d ( i :: Int ) print ( i ) end function d ( s :: String ) print ( s ) end function d ( f :: Foo ) end function main ( ) let f :: Foo = Foo ( "hello" ) d ( f ) end 
    ----------------

Hidden elsewhere in the output we can see the parser reconstructing the program from it's current understanding (the current parser implementation is only partial):

    parser output:
    ----------------
    type Foo
        a::Int
        b::String
    end

    function d(i::Int)
        print(i)
    end

    function d(s::String)
        print(s)
    end

    function d(f::Foo)
    end

    function main()
        let f::Foo = Foo("hello")
        d(f)
    end
    ----------------

