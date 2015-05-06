This document is a rough attempt to start clarifying variable and value semantics some more

Local let variables
===================

Immutable value references
---------------------------

In Icarus mutability and variables have some interesting properties

    let a::Int = 5

here we create a mutable variable a that holds a reference to the immutable value 5

there is no way to mutate 5 from a

however we can reassign the variable, assignment (`=`) never mutates values but does mutate (reassign) variables

    a = 12


Say if instead we create a user defined type and make a variable of it

    type Foo
        a::Int
    end

    let f::Foo = Foo(1)

this is again a mutable variable f holding a reference to the immutable value Foo(1)

we cannot mutate Foo(1) through this

    # compile time error, cannot mutate immutable value
    f.a = 14


Mutable value references
---------------------------

if instead we create a mutable reference we can get the desired behavior

    type Foo
        a::Int
    end

    let &f::Foo = Foo(1)

    &f.a = 14

notice that I use `&f.a` to specify mutability

I can still use both `f` to refer to `Foo(1)` and `f.a` to refer to `1` as non-mutables (say to pass to another function)

    # don't give print a mutable reference, we do not allow print to modify `Foo(1)`
    print(f)


Function arguments
==================

these rules apply to function arguments as well, with one minor exceptions

    let a::Int = 5

here `a` is a **mutable** local variable a holding a reference to an **immutable** value 5

however if we have

    fn foo(b::Int)

here `b` is an **immutable** local variable holding a reference to an **immutable** Integer value

and

    fn foo(&c::Int)

here c is an **immutable** local variable holding a reference to a **mutable** Integer value

we can refer to this as both a mutable value `&c` and an immutable value `c`, we can mutate the value `&c = 4`,
but any attempt to modify the variable `c = 4` directly is an error



That is to say inside a function there is no way to reuse an argument variable to refer to a new value

with an argument holding a mutable reference we get pointer semantics

