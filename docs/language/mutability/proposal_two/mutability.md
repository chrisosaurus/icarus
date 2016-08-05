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
    | ::Sint    |    | ::Sint    |
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
        | ::Sint  |
        +---------+


we can make further immutable reference to this type

    let c = a
    let d = b

    +-var-------+    +-var-------+
    | a         |    | b         |
    | ::Sint    |    | ::Sint    |
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
        | ::Sint  |                      | ::Sint    |
        +---------+                      | immutable |
            ^                            +-----------+
            |
            |
        +-var-------+
        | d         |
        | ::Sint    |
        | immutable |
        +-----------+


we can then mutate the value `5` and it will be visible through all the reference

    &a = 142

    +-var-------+    +-var-------+
    | a         |    | a         |
    | ::Sint    |    | ::Sint    |
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
        | ::Sint  |                      | ::Sint    |
        +---------+                      | immutable |
            ^                            +-----------+
            |
            |
        +-var-------+
        | d         |
        | ::Sint    |
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
    | ::Sint    |    | ::Sint    |
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
        | ::Sint  |                      | ::Sint    |
        +---------+                      | immutable |
            ^                            +-----------+
            |
            |
        +-var-------+
        | d         |
        | ::Sint    |
        | immutable |
        +-----------+



        +-var-------+
        | g         |
        | ::Sint    |
        | mutable   |
        +-----------+
            |
            |
            v
          +-value---+
          | 142     |
          | ::Sint  |
          +---------+


notice that we now have 2 different 'graphs', each still respecting the rule of 'only 1 mutable reference'


functions
==========

the rules are not violated by functions taking mutable copies

    fn foo(&bar::Sint)
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
        &i::Sint
    end

    fn insert(&n::Node, &a::Sint)
        &n->i = a
    end

    let &mine = 5
    let &n = Node()
    insert(&n, &mine)

the line `&n->i = a` is illegal, as it is mutable to mutable

this instead insert would have to be

    fn insert(&n::Node, &a::Sint)
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
        i::Box<&Sint>
    end

    fn insert(&n::Node, a::Box<&Sint>)
        &n->i = a
    end

    let &mine::Box<&Sint> = 5
    let &n = Node()
    insert(&n, &mine)

we now end up with


    +-var---------+                         +-var---------+
    | mine        |                         | n           |
    | ::Box<&Sint>|                         | ::Node      |
    | immutable   |                         | mutable     |
    +-------------+                         +-------------+
             \                                |
              \                               |
               v                              v
                +-Box-------+         +-Node--------+
                | ::&Sint   |         |             |
                | mutable   |<--------| immutable   |
                +-----------+         +-------------+
                    |
                    |
                    |
                    |
                    v
                 +-value---+
                 | 5       |
                 | ::Sint  |
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
    | ::Box<&Sint>|                         | ::Node      |
    | immutable   |                         | mutable     |
    +-------------+                         +-------------+
             \                                |
              \                               |
               v                              v
                +-Box-------+         +-Node--------+
                | ::&Sint   |         |             |
                | mutable   |<--------| immutable   |
                +-----------+         +-------------+
                    |
                    |
                    |
                    |
                    v
                 +-value---+
                 | 5       |
                 | ::Sint  |
                 +---------+


