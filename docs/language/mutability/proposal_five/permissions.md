Prospoal five
============


Goals
====

at any interface / boundary we want an explicit contract where both ends know the following:

1. what am I allowed to do with this data
2. what can my 'donator' (caller) do with this data

permissions
===========


In this proposal we consider a more rich and complex set of permissions

Immutable / value (default):

- value semantics
- you cannot write
- you can read
- you can store
- 'donator' cannot write, read, and store
- this is guaranteed to not mutate 'under you', you can think of this as 'pass by value'


Readonly:

- shared reference
- you cannot write
- you can read
- you cannot store
- 'donator' can write, read, and store


Storable Readonly:

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


permission table
----------------

| Permission              | receiver can write | receiver can read | receiver can store | donator can read/write/store |
| ---                     | ---                | ---               | ---                | ---                          |
| **Immutable / value**   | no                 | yes               | yes                | no                           |
| **Readonly**            | no                 | yes               | no                 | yes                          |
| **Storable Readonly**   | no                 | yes               | yes                | yes                          |
| **Mutable**             | yes                | yes               | no                 | yes                          |
| **Storable Mutable**    | yes                | yes               | yes                | yes                          |


Conversions
===========

Permissions can be converted (downgraded) as long as the resulting type does not give away 'more' permissions to the same value

    immut x             -> immut x
                        -> readonly x
                        -> storable readonly x

    readonly x          -> readonly x


    storable readonly x -> readonly x
                        -> storable readonly x


    mut x               -> immut x
                        -> mut x


    storable mut x      -> immut x
                        -> shared immut x
                        -> mut x
                        -> storable mut x

conversion table
----------------

| Permission              | To  | Immutable | Readonly | Storable Readonly | Mutable | Storable Mutable |
| ---                     | --- | ---       | ---      | ---               | ---     | ---              |
| **From**                | --- | ---       | ---      | ---               | ---     | ---              |
| **Immutable**           | --- | yes       | yes      | yes               | no      | no               |
| **Readonly**            | --- | yes       | yes      | no                | no      | no               |
| **Storable Readonly**   | --- | yes       | yes      | yes               | no      | no               |
| **Mutable**             | --- | yes       | yes      | no                | yes     | no               |
| **Storable Mutable**    | --- | yes       | yes      | yes               | yes     | yes              |


fixme
-----

conversion to/from immut needs to be thought out more, it should be safe for any type to become an immut

it *should* be safe to convert an immut to any type as well, currently we disallow conversion to mutable and storable mutable, this needs thought.

Default
=======

the default permission is immut


Syntax
======

The syntax needs to capture these permissions, ideally in a clear and visually obvious way.

sigils
------

     x = immut x
    $x = readonly
    %x = storable readonly x
    &x = mut x
    @x = storable mut x


Downgrade
=========

By default variables Downgrade to immut

    fn bar(x::Int)
        print(x)
    end

    fn baz(&x::Int)
        print(x)
    end

    fn foo(&x::Int)
        bar(x)
        baz(&x)
    end

    fn main()
        let &x = 14
        foo(&x)
    end

the call `bar(x)` is giving an immut ref
the call `baz(&x)` is preventing this downgrade by explicitly passing a mutable reference


This is to say that an unqualified variable usage (`x`, `y`) is always using the default permissions
and you must qualify it (`$x`, `&x`, ...) for other permissions.


Minimal permissions
===================

The compiler will also try enforce that you don't do silly things with permissions / sigils

    fn main()
        let @x = 14
    end

here the `let @x` is needless, as we never store `x` or give it away for storing.
this should be an error or at the very least a warning.


    fn foo(@x::Int)
        print(x)
    end

here foo says it needs a storable mutable, however in NO BRANCH does it either store or mutate
this should be an error or at the very least a warning (warnings are useful when you want to compile partial code)


Aliasing
=======

We do allow aliasing, as long as the alias never violates the permissions

    fn foo(@x::Int)
        let &y = x
        &y = 14
    end

    fn main()
        let @x = 5
        foo(@x)
        print(x)
    end

this problem will output `14`

the `let &y = x` is making an alias to x with the mutable permission,
we then mutate the original value via `&y = 14`.

note that as per 'minimal permissions' the `foo(@x)` would raise a warning/error due to not using the storable perm.


Making mutation visible
======================

The above means that possible mutation is obvious

for example

    fn main()
        let @x = 14
        let &l = List()

        # here I am really calling append(&l, @x)
        # so I must make sure to pass a mutable l, otherwise append could not modify it
        # I am also saying that this function is allowed to store x
        # and thus may mutate it now or later
        # this also now means that anyone I give &l to may also store or mutate x through the
        # interface to &l
        &l.append(@x)

        print(x)

        add_one_to_all(&l)

        print(x)
    end

    fn add_one_to_all(&l::List)
        for &item in &l
            &item += 1
        end
    end

this program will output '14' and then '15'

note that `&l.append(@x)` is really saying 'append may store or mutate x, and a may now be stored and mutated through &l`



containers
==========

A container should not be able to give away any mutable permissions if it is itself immutable

a container that is mutable (but not storable) may be still able to hand out storable mutable references

I think this means we end up with a few different types, depending on the interface we want

we can do this by providing various get methods and dispatching on a combination
of type and permission (the Icarus analyse phase already handles this and correctly
pairs functions calls up with their declarations/definitions).

    # a List type for storing immutables
    builtin type ListImmut<T>
    builtin fn append<T>(&l::ListImmut<T>, %t::T)
    builtin fn get<T>(l::ListImmut<T>) -> t::T
    builtin fn getStorable<T>(l::ListImmut<T>) -> %t::T

    # a List type for storing mutables
    builtin type ListMut<T>
    builtin fn append<T>(&l::ListMut<T>, @t::T)
    builtin fn get<T>(&l::ListMut<T>) -> &t::T
    builtin fn get<T>(l::ListMut<T>) -> t::T
    builtin fn getStorable<T>(&l::ListMut<T>) -> @t::T
    builtin fn getStorable<T>(l::ListMut<T>) -> %t::T

example using ListMut

    fn main()
        let &list = ListMut<Int>

        # populate list
        for @i in [1..100]
            &list.append(@i)
        end

        # mutate list
        add_one_to_each(&list)

        # print out list
        print_list(list)

    end

    fn print_list(list::ListMut<Int>)
        for i in list
            print(i)
        end
    end

    fn add_one_to_each(&list::ListMut<Int>)
        for &i in &list
            &i += 1
        end
    end


Thoughts
========

Currently all the above guarantees except for immut are 'one way', they say was a receiver (callee) can do, but not what a donator (caller) can do.

We may want to suppport more 'two way' guarantees (like immut which prevents donator from doing things to the value).

This is approaching some of the ideas expressed in ponylang around capabilities http://tutorial.ponylang.org/capabilities/reference-capabilities/


