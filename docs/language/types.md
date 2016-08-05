User defined types
==================

Record types
============
A record type is like a c struct, a set of names for data locations and their types

A record type introduces a default constructor which takes every field as an argument

A user can additionally define new constructors (to be described later)

    type Foo
        a::Sint
        s::String
    end

    # use of default constructor Foo(a::Sint s::String)
    let f::Foo = Foo(14 "hello")

    print(f.a) # 14
    print(f.b) # "hello


Enum types
==========
An enum type is a label and introduces no data storage

    enum Which
        Foo
        Bar
        Baz
    end

    let w::Which = function_that_returns_which()

    switch w
        case Foo
            print("function gave back a Which.Foo")
        end
        case Bar
            print("function gave back a Which.Bar")
        end
        case Baz
            print("function gave back a Which.Baz")
        end
    end

Union types
===========
A tagged enum, this is a combination of both Record and Enum

Like records it can store data and you specify multiple names and types

However like an Enum it can only ever hold *one* value at a time

    Union Foo
        s::String
        i::Sint
    end

    let f::Foo = function_returning_a_foo()

    # we must test *which* of the types it is before we can access any field
    switch f
        case String
            # do something with f.s
            # it is an error in here to reference f.i
        end
        case Sint
            # do something with f.i
            # it is an error in here to reference f.s
        end
    end


The above is roughly equivalent to the following C:

    struct Foo {
        enum {
            Foo_String,
            Foo_Sint
        } type;

        union {
            int i;
            char *c;
        } data;
    };

    struct Foo f = function_returning_a_foo();

    switch( f.type ){
        case Foo_String:
            // do something with f.data.s
            break;
        case Foo_Sint:
            // do something with f.data.i
            break;
    }

