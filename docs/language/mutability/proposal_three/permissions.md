Goal
====

at any interface / boundary we want an explicit contract where both ends know the following:

1. what am I allowed to do with this data
2. what can my 'donator' (caller) do with this data

permissions
===========


In this proposal we consider a more rich and complex set of permissions

Frozen:

- unique copy
- you can write
- you can read
- you can store
- 'donator' cannot write, read, or store.


Immutable:

- shared reference
- you cannot write
- you can read
- you cannot store
- 'donator' can write, read, and store


Storable Immutable:

- shared reference
- you cannot write
- you can read
- you can store
- 'donator' can write, read, and store


Mutable:

- shared reference
- you can write
- you can read
- you cannot store
- 'donator' can write, read, and store


Storable Mutable:

- shared reference
- you can write
- you can read
- you can store
- 'donator' can write, read, and store



Conversions
===========

Permissions can be converted as long as the resulting type does not give away 'more' permissions to the same value

note that any conversion to frozen is a copy, so all types can be converted to frozen safely (without giving away rights to the existing value, as frozen sets rights for a *new* value)

    frozen a         -> frozen a
                     -> immut a
                     -> storable immut a
                     -> mut a
                     -> storable mut a


    immut a          -> frozen a
                     -> immut a


    storable immut a -> frozen a
                     -> immut a
                     -> storable immut a


    mut a            -> frozen a
                     -> immut a
                     -> mut a


    storable mut a   -> frozen a
                     -> immut a
                     -> shared immut a
                     -> mut a
                     -> storable mut a


Default
=======

the default permission given away in the other proposals was equivalent to the immut permission here,
for this proposal I think it makes more sense for frozen to be the default permissions


Syntax
======

The syntax needs to capture these permissions, ideally in a clear and visually obvious way.

sigils
------

     a = frozen a
    $a = immut a
    %a = storable immut a
    &a = mut a
    @a = storable mut a


Local
=====

Since our goal is only about capturing a contract between interfaces, what a body does to local only variables is not of great concern

    fn foo(&a::Int)
        let b = 15
        b += 1
        b += 8
        &a = b
    end

here this foo happily reassigns the internal variable b, mutating as happy as can be

until finally mutating a, at this point this mutation has external side effects so we must capture this via permissions.


Decay
=====

By default variables decay to frozen

    fn bar(a::Int)
        print(a)
    end

    fn baz(&a::Int)
        print(a)
    end

    fn foo(&a::Int)
        bar(a)
        baz(&a)
    end

    fn main()
        let a = 14
        foo(&a)
    end

the call `bar(a)` is giving a frozen variable
the call `baz(&a)` is preventing this decay by explicitly passing a mutable reference

I am unsure about the `let a = 14` and then `foo(&a)` within main


Minimal permissions
===================

The compiler will also try enforce that you don't do silly things with permissions / sigils

    fn main()
        let @a = 14
    end

here the `let @a` is needless, as the `a` is frozen and therefore I already have that permission,
this should be an error or at the very least a warning.


    fn foo(@a::Int)
        print(a)
    end

here foo says it needs a storable mutable, however in NO BRANCH does it either store or mutate
this should be an error or at the very least a warning (warnings are useful when you want to compile partial code)


Aliasing
=======

We do allow aliasing, as long as the alias never violates the permissions

    fn foo(@a::Int)
        let &b = a
        &b = 14
    end

    fn main()
        let a = 5
        foo(@a)
        print(a)
    end

this problem will output `14`

the `let &b = a` is making an alias to a with the mutable permission,
we then mutate the original value via `&b = 14`.

note that as per 'minimal permissions' the `foo(@a)` would raise a warning/error due to not using the storable perm.


