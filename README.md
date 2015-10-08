# Icarus [![Build Status](https://travis-ci.org/mkfifo/icarus.svg?branch=master)](https://travis-ci.org/mkfifo/icarus) [![Coverage Status](https://coveralls.io/repos/mkfifo/icarus/badge.svg?branch=master)](https://coveralls.io/r/mkfifo/icarus?branch=master) <a href="https://scan.coverity.com/projects/4854"> <img alt="Coverity Scan Build Status" src="https://scan.coverity.com/projects/4854/badge.svg"/> </a>

Icarus is a toy language exploring static verifiability

Goals
=====

* Explicit, obvious and statically verifiable mutability
* Strong statically verified type system
* Runtime memory safety (no pointer arithmetic, arrays are bounds checked, all memory is intialised)

Work so far
===========

So far Icarus only has a lexer and basic parser (syntactic analysis),
work is progressing on the analyse (semantic analyse) stage,
there are no backends yet.

Icarus is currently in the process of a major refactor of the lexing system,
for a transition period icarus will have an additional lexer 'hanging off the side'.

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
        let f::Foo = Foo(add_one(1), "hello")

        d(f)
    end

We can see what Icarus makes of this by running:

    make example

Hidden in the output we see the old lexer output:

    old lexer output:
    ----------------
    type Foo a :: Int b :: String end fn d ( i :: Int ) print ( i ) end fn d ( s :: String ) print ( s ) end fn d ( f :: Foo ) d ( f . a ) d ( f . b ) end fn add_one ( i :: Int ) -> Int return i + 1 end fn main ( ) let f :: Foo = Foo ( add_one ( 1 ) , "hello" ) d ( f ) end fn print ( s :: String ) end fn print ( i :: Int ) end 
    ----------------

and the new lexer output:

    lexer output:
    ----------------
    # user defined type with 2 fields, an Int and a String
    fn Foo
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
        let f::Foo = Foo(add_one(1), "hello")

        d(f)
    end

    # temporary hack to allow type and function analysis to pass
    fn print(s::String) end
    fn print(i::Int) end

    ----------------

Hidden elsewhere in the output we can see the parser reconstructing the program from it's current understanding (based on old lexer):

    parser output:
    ----------------
    type Foo
        a::Int
        b::String
    end

    # d(Int)
    fn d(i::Int) -> Void
        print(i)
    end

    # d(String)
    fn d(s::String) -> Void
        print(s)
    end

    # d(Foo)
    fn d(f::Foo) -> Void
        d(f . a)
        d(f . b)
    end

    # add_one(Int)
    fn add_one(i::Int) -> Int
        return i + 1
    end

    # main()
    fn main() -> Void
        let f::Foo = Foo(add_one(1) , "hello")
        d(f)
    end
    ----------------

