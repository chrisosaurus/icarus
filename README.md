Icarus
======

Icarus is a toy language with static type checking

Inspiration has been drawn from (in alphabetical order):

* C
* C++
* Go
* Haskell
* Julia
* Python
* Racket
* Rust
* Scheme (r7rs-small, chicken)

Plans
=====

* no undefined behavior
* strong statically checked type system
* no 'holes' in the type system (null and void pointer type are all banished) - see `docs/noholes.md`
* managed memory (automatic garbage collection)
* multiple dispatch (dispatch to functions is based on function name and type of arguments passed)
* operator overloading
* parametrized types (derived from C++, Julia, and Rust)
* union types (tagged enums, statically checked before being taken apart)
    * like Rust's enums
    * like haskell's ADTs (Algebraic Data Types)
* including both an interpreter and compiler (currently considering both C and LLVM bitcode as compiler backend outputs) - see `docs/components.md`
* whitespace insensitive (except using \n to terminate a comment, but comments are stripped by the lexer for now)
* no commas (hail to the lisp)

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
        d(f.a)
        d(f.b)
    end

    # entry point for program
    function main()
        let f::Foo = Foo(1 "hello")

        d(f)
    end

We can see what Icarus makes of this by running:

    make example

Hidden in the output we see the lexer output:

    ----------------
    lexer output:
    type Foo a :: Int b :: String end function d ( i :: Int ) print ( i ) end function d ( s :: String ) print ( s ) end function d ( f :: Foo ) d ( f . a ) d ( f . b ) end function main ( ) let f :: Foo = Foo ( 1 " hello " ) d ( f ) end 
    ----------------

