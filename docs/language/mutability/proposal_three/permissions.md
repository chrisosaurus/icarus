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


explicit
--------

conversions must be explicit

for example this is wrong

    fn foo($y::Int) ... end

    let x = 5
    foo(x)

this is illegal, despite a conversion from frozen x to immutable x being allowed

we can instead do this as long as we make it explicit

    fn foo($y::Int) ... end

    let x = 5
    foo($x)

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

and thus a call to this function has to give the immut right

here we call it (illegally) but supplying a frozen

    fn main()
        let x = 4
        print(x)
    end

we can fix this either by explicitely converting to immut

    fn main()
        let x = 4
        print($x)
    end

or we can make a wrapper function which does this for us

    fn print(y::Int) print($y) end

This is safe as a conversion from frozen to immmut is safe



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

    let &val = $l.get(0)

here we have fetched out a stored value via get, get doesn't mutate so only needs $l.
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
    builtin fn get<T>($l::ListImmut<T>) -> $t::T
    builtin fn getStorable<T>($l::ListImmut<T>) -> %t::T

    # a List type for storing mutables
    builtin type ListMut<T>
    builtin fn append<T>(&l::ListMut<T>, @t::T)
    builtin fn get<T>(&l::ListMut<T>) -> &t::T
    builtin fn get<T>($l::ListMut<T>) -> $t::T
    builtin fn getStorable<T>(&l::ListMut<T>) -> @t::T
    builtin fn getStorable<T>($l::ListMut<T>) -> %t::T


example using ListMut

    fn main()
        let list = ListMut<Int>

        # populate list
        for i in [1..100]
            &list.append(@i)
        end

        # mutate list
        add_one_to_each(&list)

        # print out list
        print_list($list)

    end

    fn print_list($list::ListMut<Int>)
        for $i in $list
            print($i)
        end
    end

    fn add_int_to_each(&list::ListMut<Int>)
        for &i in &list
            &i += 1
        end
    end


greater permissions
--------------------

| Permission              | Contents | Frozen | Immutable | Storable Immutable | Mutable | Storable Mutable |
| ---                     | ---      | ---    | ---       | ---                | ---     | ---              |
| **Container**           | ---      | ---    | ---       | ---                | ---     | ---              |
| **Frozen**              | ---      | yes    | yes       | yes                | yes     | yes              |
| **Immutable**           | ---      | yes    | yes       | yes                | no      | no               |
| **Storable Immutable**  | ---      | yes    | yes       | yes                | no      | no               |
| **Mutable**             | ---      | yes    | yes       | yes                | yes     | yes              |
| **Storable Mutable**    | ---      | yes    | yes       | yes                | yes     | yes              |

This is mostly the same as the type conversion table, the only differences are:

- a 'immutable container' may give away a 'storable immutable reference', as long as it was stored in it with those permissions
- a 'mutable container' may give away a 'storable mutable reference', as long as it was stored in it with those permissions


Considerations
==============

the immutable permission is not very interesting, yet it has a sigil and thus visible noise

obviously a storable immutable should require a sigil, as you are giving away a permanent immut reference

however an immutable reference is only temporary, for the duration of the callee

Consider a snippet similar to above

    fn main()
        let list = ListMut<Int>

        # populate list
        for i in [1..100]
            &list.append(@i)
        end

        # print out list
        print_list($list)

    end

    fn print_list($list::ListMut<Int>)
        for $i in $list
            print($i)
        end
    end

there are a lot of immutable sigils (`$`), however as a caller of print_list you could argue that you don't actually care

forcing the user to also expose a conversion function `fn print_list(list::ListMut<Int>) print_list($list) end` is gross

the above snippet is not really more explicit than

    fn main()
        let list = ListMut<Int>

        # populate list
        for i in [1..100]
            &list.append(@i)
        end

        # print out list
        print_list(list)

    end

    fn print_list(list::ListMut<Int>)
        for i in list
            print(i)
        end
    end


