This document tries to capture an alternative proposal for typing and mutability

Rules
=====

The principles here are different:

* values are mutable
* only one mutable reference may exist to any value at a time
* you may have many immutable reference to a value
* assignment from mutable to mutable is a copy

Basics
=======

illustration example:

    let &a = 5
    let b = a

we now have a single value `5` with two reference to it



    +-var-------+    +-var-------+
    | a         |    | b         |
    | ::Int     |    | ::Int     |
    | mutable   |    | immutable |
    +-----------+    +-----------+
       \              /
        \            /
         \          /
          \        /
           \      /
            v    v
        +-value---+
        | 5       |
        | ::Int   |
        +---------+


we can make further immutable reference to this type

    let c = a
    let d = b

    +-var-------+    +-var-------+
    | a         |    | b         |
    | ::Int     |    | ::Int     |
    | mutable   |    | immutable |
    +-----------+    +-----------+
       \              /
        \            /
         \          /
          \        /
           \      /
            v    v
        +-value---+ <--------------------+-var-------+
        | 5       |                      | c         |
        | ::Int   |                      | ::Int     |
        +---------+                      | immutable |
            ^                            +-----------+
            |
            |
        +-var-------+
        | d         |
        | ::Int     |
        | immutable |
        +-----------+


we can then mutate the value `5` and it will be visible through all the reference

    &a = 142

    +-var-------+    +-var-------+
    | a         |    | a         |
    | ::Int     |    | ::Int     |
    | mutable   |    | immutable |
    +-----------+    +-----------+
       \              /
        \            /
         \          /
          \        /
           \      /
            v    v
        +-value---+ <--------------------+-var-------+
        | 142     |                      | c         |
        | ::Int   |                      | ::Int     |
        +---------+                      | immutable |
            ^                            +-----------+
            |
            |
        +-var-------+
        | d         |
        | ::Int     |
        | immutable |
        +-----------+

copying
=======

we cannot just assign a mutable reference to another mutable reference

    # this is ILLEGAL
    let &e = a
    # this is also ILLEGAL
    let &f = a

instead we must perform a copy

    let &g := a

    +-var-------+    +-var-------+
    | a         |    | a         |
    | ::Int     |    | ::Int     |
    | mutable   |    | immutable |
    +-----------+    +-----------+
       \              /
        \            /
         \          /
          \        /
           \      /
            v    v
        +-value---+ <--------------------+-var-------+
        | 142     |                      | c         |
        | ::Int   |                      | ::Int     |
        +---------+                      | immutable |
            ^                            +-----------+
            |
            |
        +-var-------+
        | d         |
        | ::Int     |
        | immutable |
        +-----------+



        +-var-------+
        | g         |
        | ::Int     |
        | mutable   |
        +-----------+
            |
            |
            v
          +-value---+
          | 142     |
          | ::Int   |
          +---------+


notice that we now have 2 different 'graphs', each still respecting the rule of 'only 1 mutable reference'


functions
==========

the rules are not violated by functions taking mutable copies

    fn foo(&bar::Int)
        ...
    end

    foo(&a)

as in any scope there is only one single mutable value, the caller and callee
do not both execute simultaneously so no two people hold simultaneous mutable
reference.

This the guarantee is preserved.


shot comings
=============

the principles so far are limited, they lack the power to express certain ideas around storage

    type Node
        &i::Int
    end

    fn insert(&n::Node, &a::Int)
        &n->i = a
    end

    let &mine = 5
    let &n = Node()
    insert(&n, &mine)

the line `&n->i = a` is illegal, as it is mutable to mutable

this instead insert would have to be

    fn insert(&n::Node, &a::Int)
        &n->i := a
    end

which means we no longer have shared state, which may be desirable

boxing
======

this is where the `Box` types comes in

a Box type allows you to share mutable reference, the normal rules apply:

* there can only be one mutable reference to a box type
* there can be any number of immutable reference to a box type
* you can mutate the value inside the box type, even through an immutable ref

the above rewritten with a box type is now:

    type Node
        i::Box<&Int>
    end

    fn insert(&n::Node, a::Box<&Int>)
        &n->i = a
    end

    let &mine::Box<&Int> = 5
    let &n = Node()
    insert(&n, &mine)

we now end up with


    +-var---------+                         +-var---------+
    | mine        |                         | n           |
    | ::Box<&Int> |                         | ::Node      |
    | immutable   |                         | mutable     |
    +-------------+                         +-------------+
             \                                |
              \                               |
               v                              v
                +-Box-------+         +-Node--------+
                | ::&Int    |         |             |
                | mutable   |<--------| immutable   |
                +-----------+         +-------------+
                    |
                    |
                    |
                    |
                    v
                 +-value---+
                 | 5       |
                 | ::Int   |
                 +---------+


this is interesting as, if you could all the references:

*    the node only has a single mutable reference from `&n`
*    the box has no mutable reference to it
*    the value only has a single mutable reference from the Box

in order for this to be useful we must allow mutation *through* an immutable reference

    &mine->value = 16

which then gives us

    +-var---------+                         +-var---------+
    | mine        |                         | n           |
    | ::Box<&Int> |                         | ::Node      |
    | immutable   |                         | mutable     |
    +-------------+                         +-------------+
             \                                |
              \                               |
               v                              v
                +-Box-------+         +-Node--------+
                | ::&Int    |         |             |
                | mutable   |<--------| immutable   |
                +-----------+         +-------------+
                    |
                    |
                    |
                    |
                    v
                 +-value---+
                 | 5       |
                 | ::Int   |
                 +---------+


