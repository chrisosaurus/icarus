Prospoal four
============


Goals
====

at any interface / boundary we want an explicit contract where both ends know the following:

1. what am I allowed to do with this data
2. what can my 'donator' (caller) do with this data

permissions
===========


In this proposal we consider a more rich and complex set of permissions

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
| **Immutable**           | no                 | yes               | no                 | yes                          |
| **Storable Immutable**  | no                 | yes               | yes                | yes                          |
| **Mutable**             | yes                | yes               | no                 | yes                          |
| **Storable Mutable**    | yes                | yes               | yes                | yes                          |


Conversions
===========

Permissions can be converted as long as the resulting type does not give away 'more' permissions to the same value

note that any conversion to frozen is a copy, so all types can be converted to frozen safely (without giving away rights to the existing value, as frozen sets rights for a *new* value)

    immut x          -> immut x


    storable immut x -> immut x
                     -> storable immut x


    mut x            -> immut x
                     -> mut x


    storable mut x   -> immut x
                     -> shared immut x
                     -> mut x
                     -> storable mut x

conversion table
----------------

| Permission              | To  | Immutable | Storable Immutable | Mutable | Storable Mutable |
| ---                     | --- | ---       | ---                | ---     | ---              |
| **From**                | --- | ---       | ---                | ---     | ---              |
| **Immutable**           | --- | yes       | no                 | no      | no               |
| **Storable Immutable**  | --- | yes       | yes                | no      | no               |
| **Mutable**             | --- | yes       | no                 | yes     | no               |
| **Storable Mutable**    | --- | yes       | yes                | yes     | yes              |

Default
=======

the default permission is immut


Syntax
======

The syntax needs to capture these permissions, ideally in a clear and visually obvious way.

sigils
------

     x = immut x
    %x = storable immut x
    &x = mut x
    @x = storable mut x


Decay
=====

By default variables decay to immut

    fn bar(x::Sint)
        print(x)
    end

    fn baz(&x::Sint)
        print(x)
    end

    fn foo(&x::Sint)
        bar(x)
        baz(&x)
    end

    fn main()
        let &x = 14
        foo(&x)
    end

the call `bar(x)` is giving an immut ref
the call `baz(&x)` is preventing this decay by explicitly passing a mutable reference


Minimal permissions
===================

The compiler will also try enforce that you don't do silly things with permissions / sigils

    fn main()
        let @x = 14
    end

here the `let @x` is needless, as we never store `x` or give it away for storing.
this should be an error or at the very least a warning.


    fn foo(@x::Sint)
        print(x)
    end

here foo says it needs a storable mutable, however in NO BRANCH does it either store or mutate
this should be an error or at the very least a warning (warnings are useful when you want to compile partial code)


Aliasing
=======

We do allow aliasing, as long as the alias never violates the permissions

    fn foo(@x::Sint)
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


Storage
======

There are also some potentially interesting cases around storage

multiple arguments
------------------

    foo(&w, &x, @y, @z)

as this means that `y` and `z` may be stored and mutated either on or through `w`, `x` and each other (`y` could be stored on `z` or vice versa)


leakage
-------

    &l.append(@x)

above we agreed this means `x` can now be mutated through `&l`

however, can `x` be mutated through `l` ?

    let &val = l.get(0)

here we have fetched out a stored value via get, get doesn't mutate so only needs l.
we now have a pointer to `x` which we can mutate through
and in doing so we are able to mutate the `x` value that is stored inside of `l`.

I think this is a violation of the permission system, I do not think you should be able to take
a type with a given permission and extract a type from it with `greater` permissions.

Note that `greater permissions` has to have a specific definition immediately discussed below under `containers`.

containers
==========

I think that a container should not be able to give away any mutable permissions if it is itself immutable

a container that is mutable (but not storable) may be still able to hand out storable mutable references

I think this means we end up with a few different types, depending on the interface we want

we can do this by providing various get methods and dispatching on a combination
of type and permission

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
        let &list = ListMut<Sint>

        # populate list
        for i in [1..100]
            &list.append(@i)
        end

        # mutate list
        add_one_to_each(&list)

        # print out list
        print_list(list)

    end

    fn print_list(list::ListMut<Sint>)
        for i in list
            print(i)
        end
    end

    fn add_one_to_each(&list::ListMut<Sint>)
        for &i in &list
            &i += 1
        end
    end


greater permissions
--------------------

here is a table showing what permissions we can `get` out from a container of a given permission

| Permission              | Can get out | Immutable | Storable Immutable | Mutable | Storable Mutable |
| ---                     | ---         | ---       | ---                | ---     | ---              |
| **Container**           | ---         | ---       | ---                | ---     | ---              |
| **Immutable**           | ---         | yes       | yes                | no      | no               |
| **Storable Immutable**  | ---         | yes       | yes                | no      | no               |
| **Mutable**             | ---         | yes       | yes                | yes     | yes              |
| **Storable Mutable**    | ---         | yes       | yes                | yes     | yes              |

This is mostly the same as the type conversion table, the only differences are:

- a 'immutable container' may give away a 'storable immutable reference', as long as it was stored in it with those permissions
- a 'mutable container' may give away a 'storable mutable reference', as long as it was stored in it with those permissions


Considerations
==============

immutable not (always) being interesting
----------------------------------------

If you are only reading from variables then I think your code should be 'sigil less',
below `print_list` only reads and prints.

    fn main()
        let list = ListMut<Sint>

        # populate list
        for i in [1..100]
            &list.append(@i)
        end

        # print out list
        print_list(list)

    end

    fn print_list(list::ListMut<Sint>)
        for i in list
            print(i)
        end
    end


Thoughts
========

Currently the above guarantees are 'one way', they say was a receiver (callee) can do, but not what a donator (caller) can do.

I think we need to at least support 'two way' guarantees, possibly around uniqueness,
say "This is now yours, no one else can touch it, you can do anything", or "this is not unique, but no one can write to it".

This is approaching some of the ideas expressed in ponylang around capabilities http://tutorial.ponylang.org/capabilities/reference-capabilities/


