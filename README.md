# Icarus [![Build Status](https://travis-ci.org/mkfifo/icarus.svg?branch=master)](https://travis-ci.org/mkfifo/icarus) [![Coverage Status](https://coveralls.io/repos/mkfifo/icarus/badge.svg?branch=master)](https://coveralls.io/r/mkfifo/icarus?branch=master) <a href="https://scan.coverity.com/projects/4854"> <img alt="Coverity Scan Build Status" src="https://scan.coverity.com/projects/4854/badge.svg"/> </a>

Icarus is a toy language exploring static verifiability

Goals
=====

* Explicit, obvious and statically verifiable mutability
* Strong statically verified type system
* Runtime memory safety (no pointer arithmetic, arrays are bounds checked, all memory is intialised)

Work so far
===========

Icarus is aiming to support a functional subset as a proof of concept,
the initial target for this can be seen in `example/simple.ic` (reproduced below).

For this target the following systems are currently 'complete':

* lexer
* parser (syntactic analysis)
* analsye (semantic analysis)


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

    # hack to work around lack of constructors
    builtin fn Foo(a::Int b::String) -> Foo



We can see what Icarus makes of this by running:

    make example

Hidden in the output we see the new lexer output:

    lexer output:
    ----------------
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

    # hack to work around lack of constructors
    builtin fn Foo(a::Int b::String) -> Foo

    ----------------


Hidden elsewhere in the output we can see the parser reconstructing the program from it's current understanding (based on new lexer):

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
        let f::Foo = Foo(add_one(1) "hello")
        d(f)
    end

    builtin fn plus(a::Int b::Int) -> Int end
    ----------------

Finally the analyse step's outputs showing no errors, but making it clear that it isn't complete yet

    warning: main implementation pending, icarus is currently only partially functional
    analysis complete


