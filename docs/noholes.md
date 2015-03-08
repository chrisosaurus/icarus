No holes in the type system
===========================

This document will explain how the following are used in C and how Icarus plans to avoid their usage:

* null value
* void pointer type


Terminating a string
====================
In C a String is an array of characters terminated by a null value

In Icarus instead Strings will known their own length


Error cases
===========
In C often functions that would return a pointer to an object (a reference) will instead return a null value on error

In Icarus we plan to have `Maybe` type (see `Base cases` below for more) - derived form Haskell's `Maybe`, Julia's `Nullable`, and Rust's `Option`

A `Maybe` is just a built-in Enum type (see `Base cases` below for more)

example syntax:

    # defaults to Empty
    let m::Maybe<Int>

    if ...
        m = 32

    if ...
        m = 16

    switch m
        case Int
            print("Got number " m.contents)
        end
        case Empty
            print("Got nothing")
        end
    end


Base cases
==========
In C you generally return a null to signal a base case

Icarus plans to address this instead using Union types (tagged enums) - more on this coming soon, for now see Haskell's ADT and Rust's Enums

example syntax:

    enum Container
        i::Int
        s::String
        f::Foo
    end

    function bar(c::Container)
        # unwrap Container and perform action based on type
        # compiler to enforce that *all* possible types are handled
        # compiler also enforces that any 'unwrapping' is safe
        switch c
            case Int
                # can only unwrap c.i as we know we must be an Int
                c.i += 14
            end
            case String
                # can only unwrap c.s as we know we must be a String
                print(c.s)
            end
            case Foo
                # can only unwrap c.f as we know we must be a Foo
                do-something-with-foo(c.f)
            end
    end

Generics
========
In C if you want to write a generic data structure or function you often have to pass things around as void pointers

Icarus plans to avoid this using parametric types (C++ templates)

example syntax (work in progress):

    type Foo<T>
        contents::Array<T>
    end

    function bar<T>(a::T)
        ...
    end

