No holes in the type system
===========================

This document will explain how the following are used in C and how Icarus plans to avoid their usage:

* null value
* void pointer type


Error/Base cases
===========
In C often functions that would return a pointer to an object (a reference) will instead return a null value on error or to signal a base case

In Icarus we plan to have `Maybe` type - derived form Haskell's `Maybe`, Julia's `Nullable`, and Rust's `Option`

A `Maybe` is just a built-in Union type

example syntax (work in progress):

    # defaults to Empty
    let m::Maybe<Sint>

    if ...
        m = 32

    if ...
        m = 16

    switch m
        case Sint
            print("Got number " m.contents)
        end
        case Empty
            print("Got nothing")
        end
    end

example syntax (work in progress):

    # returns position of needle within haystack on success
    # reutrns Empty on failure
    function search(haystack::String needle::Char) :: Maybe<Sint>
        let char::Char
        let pos::Sint

        for (char pos) in iterate(haystack)
            if char == needle
                return pos
            end
        end

        # failure case
        return Empty
    end

    let res::Maybe<Sint> = search("hello" 'e')
    switch res
        case Sint
            print("character found")
        end
        case Empty
            print("character not found")
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


Terminating a string
====================
In C a String is an array of characters terminated by a null value

In Icarus instead Strings will know their own length


