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

Goals
=====

* Obvious and statically verified mutability
* Strong statically verified type system
* Runtime memory safety (no pointer arithmetic, arrays are bounds checked)

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

So far Icarus only has a lexer and parser (syntactic analysis),
work is starting on the analyse (semantic analyse) stage is beginning,
there are no backends yet.

Here is an example of the lex and parse steps in action:

We have the contents of `example/simple.ic`:


    # user defined type with 2 fields, an Int and a String
    type Foo
        a::Int
        b::String
    end

    fn d(i::Int)
        print(i)
    end

    fn d(s::String)
        print(s)
    end

    # break apart a Foo and call d on each field
    fn d(f::Foo)
        d(f.a)
        d(f.b)
    end

    # simple function to test return values
    fn add_one(i::Int) -> Int
        return i + 1
    end

    # entry point for program
    fn main()
        let f::Foo = Foo(add_one(1) "hello")

        d(f)
    end

We can see what Icarus makes of this by running:

    make example

Hidden in the output we see the lexer output:

    lexer output:
    ----------------
    type Foo a :: Int b :: String end fn d ( i :: Int ) print ( i ) end fn d ( s :: String ) print ( s ) end fn d ( f :: Foo ) d ( f . a ) d ( f . b ) end fn add_one ( i :: Int ) -> Int return i + 1 end fn main ( ) let f :: Foo = Foo ( add_one ( 1 ) "hello" ) d ( f ) end 
    ----------------

Hidden elsewhere in the output we can see the parser reconstructing the program from it's current understanding (the current parser implementation is only partial):

    parser output:
    ----------------
    type Foo
        a::Int
        b::String
    end

    fn d(i::Int) -> Void
        print(i)
    end

    fn d(s::String) -> Void
        print(s)
    end

    fn d(f::Foo) -> Void
        d(f . a)
        d(f . b)
    end

    fn add_one(i::Int) -> Int
        return i + 1
    end

    fn main() -> Void
        let f::Foo = Foo(add_one(1) "hello")
        d(f)
    end
    ----------------

