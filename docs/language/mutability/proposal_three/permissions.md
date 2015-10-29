Goals
====

at any interface / boundary we want an explicit contract where both ends know the following:

1. what am I allowed to do with this data
2. what can my 'donator' (caller) do with this data

permissions
===========


In this proposal we consider a more rich and complex set of permissions

Frozen:

- unique copy (every time a frozen is passed it is copied)
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


permission table
----------------

| Permission              | receiver can write | receiver can read | receiver can store | donator can read/write/store |
| ---                     | ---                | ---               | ---                | ---                          |
| **Frozen**              | yes                | yes               | yes                | no                           |
| **Immutable**           | no                 | yes               | no                 | yes                          |
| **Storable Immutable**  | no                 | yes               | yes                | yes                          |
| **Mutable**             | yes                | yes               | no                 | yes                          |
| **Storable Mutable**    | yes                | yes               | yes                | yes                          |


Conversions
===========

Permissions can be converted as long as the resulting type does not give away 'more' permissions to the same value

note that any conversion to frozen is a copy, so all types can be converted to frozen safely (without giving away rights to the existing value, as frozen sets rights for a *new* value)

    frozen x         -> frozen x
                     -> immut x
                     -> storable immut x
                     -> mut x
                     -> storable mut x


    immut x          -> frozen x
                     -> immut x


    storable immut x -> frozen x
                     -> immut x
                     -> storable immut x


    mut x            -> frozen x
                     -> immut x
                     -> mut x


    storable mut x   -> frozen x
                     -> immut x
                     -> shared immut x
                     -> mut x
                     -> storable mut x

conversion table
----------------

| Permission              | To | Frozen | Immutable | Storable Immutable | Mutable | Storable Mutable |
| ---                     | ---| ---    | ---       | ---                | ---     | ---              |
| **From**                | ---| ---    | ---       | ---                | ---     | ---              |
| **Frozen**              | ---| yes    | yes       | yes                | yes     | yes              |
| **Immutable**           | ---| yes    | yes       | no                 | no      | no               |
| **Storable Immutable**  | ---| yes    | yes       | yes                | no      | no               |
| **Mutable**             | ---| yes    | yes       | no                 | yes     | no               |
| **Storable Mutable**    | ---| yes    | yes       | yes                | yes     | yes              |



Default
=======

the default permission given away in the other proposals was equivalent to the immut permission here,
for this proposal I think it makes more sense for frozen to be the default permissions


Syntax
======

The syntax needs to capture these permissions, ideally in a clear and visually obvious way.

sigils
------

     x = frozen x
    $x = immut x
    %x = storable immut x
    &x = mut x
    @x = storable mut x


Local
=====

Since our goal is only about capturing a contract between interfaces, what a body does to local only variables is not of great concern

    fn foo(&x::Int)
        let y = 15
        y += 1
        y += 8
        &x = y
    end

here this foo happily reassigns the internal variable y, mutating as happy as can be

until finally mutating x, at this point this mutation has external side effects so we must capture this via permissions.


Decay
=====

By default variables decay to frozen

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
        let x = 14
        foo(&x)
    end

the call `bar(x)` is giving a frozen variable
the call `baz(&x)` is preventing this decay by explicitly passing a mutable reference

I am unsure about the `let x = 14` and then `foo(&x)` within main


Minimal permissions
===================

The compiler will also try enforce that you don't do silly things with permissions / sigils

    fn main()
        let @x = 14
    end

here the `let @x` is needless, as the `x` is frozen and therefore I already have that permission,
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
        let x = 5
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
        let x = 14
        let l = List()

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



Automatic decay
==============

A function has to specify the minimum permissions that is requires to work, and a function cannot work beyond those

safe decay
----------
for example the builtin function print only needs to be able to read, it will never write or store

    # builtin print function for an Int
    builtin fn print($x::Int)

and thus a call to this function has to give at least the 'immut' right

    fn main()
        let x = 4
        print(x)
    end

note there that I am really passing a frozen in, as the contract for a frozen is at least the contract for an immut


I think it should be safe to pass in a frozen to (almost?) any function, regardless of the rights it wants

However I think passing in another explicit permission should be an error

    fn main2()
        let x = 4
        print(&x)
    end

here this should be an error or at the least a warning,
as I am explicitly handing in a mutable but it only needs an immut


maybe unsafe / useless decay
----------------------------

There is an interesting potential special case around containers

    fn main()
        let x = 5
        let l = List()

        l.append(x)
    end

here I am creating a list, but when I call append I pass in both the list and the value as frozen

this is interesting as it is technically not a violation of the permissions system, as a frozen l can still be mutated, and a frozen a can still be stored.

However as we know how append works, we know that this call is useless

it will take a frozen l and frozen x, it will then store that frozen x on the frozen l, it will then return

overall this makes no actual observable changes

we may want to allow a syntax to prevent this kind of useless decay,
however there may also be cases where we want to do this, to get around a broken interface asking for rights we don't want to give away.


Storage
======

There are also some potentially interesting cases around storage

multiple arguments
------------------

    foo(&w, &x, @y, @z)

as this means that `y` and `z` may be stored and mutated either on or through `w`, `x` and each other (`y` could be stored on `z` or vice versa)


unexplored cases
----------------

    &l.append(@x)

above we agreed this means `x` can now be mutated through `&l`

however, can `x` be mutated through `l` ?

    let &val = l.get(0)

here we have fetched out a stored value via get, get doesn't mutate so doesn't need `&l`

we now have a pointer to `x` which we can mutate through

and in doing so we are able to mutate the `x` value that is stored inside of `l`.

containers
==========

I think that a container should not be able to give away any mutable permissions if it is itself immutable

a container that is mutable (but not storable) may be still able to hand out storable mutable references

I think this means we end up with a few different types, depending on the interface we want

    # a List type for storing immutables
    builtin type ListImmut<T>
    builtin fn append<T>(&l::ListImmut<T>, %t::T)
    builtin fn get<T>(l::ListImmut<T>) -> $t::T

    # a List type for storing mutables
    builtin type ListMut<T>
    builtin fn append<T>(&l::ListMut<T>, @t::T)
    builtin fn get<T>(&l::ListMut<T>) -> &t::T

    # a List type for storing immutables
    builtin type ListStorableImmut<T>
    builtin fn append<T>(&l::ListStorableImmut<T>, %t::T)
    builtin fn get<T>(l::ListStorableImmut<T>) -> %t::T

    # a List type for storing mutables
    builtin type ListStorableMut<T>
    builtin fn append<T>(&l::ListStorableMut<T>, @t::T)
    builtin fn get<T>(&l::ListStorableMut<T>) -> @t::T

    # a List type for storing Frozens
    builtin type ListFrozen<T>
    builtin fn append<T>(&l::ListFrozen<T>, t::T)
    # but then what does it hand out?
    builtin fn get<T>(l::ListFrozen<T>) -> t::T
    builtin fn get<T>(l::ListFrozen<T>) -> $t::T
    builtin fn get<T>(l::ListFrozen<T>) -> %t::T
    builtin fn get<T>(&l::ListFrozen<T>) -> &t::T
    builtin fn get<T>(&l::ListFrozen<T>) -> @t::T

this has the potential to get very ugly....

another alternative is to provide different get methods,
and dispatch on permissions

    # a List type for storing immutables
    builtin type ListImmut<T>
    builtin fn append<T>(&l::ListImmut<T>, %t::T)
    builtin fn get<T>(l::ListImmut<T>) -> $t::T
    builtin fn getStorable<T>(l::ListImmut<T>) -> %t::T

    # a List type for storing mutables
    builtin type ListMut<T>
    builtin fn append<T>(&l::ListMut<T>, @t::T)
    builtin fn get<T>(&l::ListMut<T>) -> &t::T
    builtin fn get<T>(l::ListMut<T>) -> $t::T
    builtin fn getStorable<T>(&l::ListMut<T>) -> @t::T
    builtin fn getStorable<T>(l::ListMut<T>) -> %t::T


