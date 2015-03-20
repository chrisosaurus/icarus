A quick overview of icarus' planned reference semantics

This is all theoretical at this point and still requires much discussion.

local mutation
==============

Local variables are mutable and they can be reassigned new values

    let a = 5

this allocated an integer value `5` and created a mutable variable `a` referencing this

      +-var-----+
      | a       |
      | ::Int   |
      | mutable |
      +---------+
           \
            \
             \
              \
               \
                v
               +-value---+
               |  5      |
               |  ::Int  |
               +---------+


we can mutate this freely

    a = 14

      +-var-----+
      | a       |
      | ::Int   |
      | mutable |
      +---------+
           \
            \
             \
              \
               \
                v
               +-value---+
               |  14     |
               |  ::Int  |
               +---------+



local aliasing
==============

this section is still TO BE DECIDED

option 1
---------

we can then create aliases

    let a = 5
    let b = a

we now have

      +-var-----+     +-var-----+
      | a       |     | b       |
      | ::Int   |     | ::Int   |
      | mutable |     | mutable |
      +---------+     +---------+
           \           |
            \          |
             \         |
              \        |
               \       |
                v      v
               +-value---+
               |  5      |
               |  ::Int  |
               +---------+

mutation either variable will be reflected in the other

    b += 3

yields:

      +-var-----+     +-var-----+
      | a       |     | b       |
      | ::Int   |     | ::Int   |
      | mutable |     | mutable |
      +---------+     +---------+
           \           |
            \          |
             \         |
              \        |
               \       |
                v      v
               +-value---+
               |  8      |
               |  ::Int  |
               +---------+

option 2
--------

we can then create aliases

    let a = 5
    let b = a

we now have

      +-var-----+     +-var-----+
      | a       |     | b       |
      | ::Int   |     | ::Int   |
      | mutable |     | mutable |
      +---------+     +---------+
           \           |
            \          |
             \         |
              \        |
               \       |
                v      v
               +-value---+
               |  5      |
               |  ::Int  |
               +---------+

however mutation will create a new value

    b += 3

yields:

      +-var-----+     +-var-----+
      | a       |     | b       |
      | ::Int   |     | ::Int   |
      | mutable |     | mutable |
      +---------+     +---------+
          |               |
          |               |
          |               |
          |               |
          |               |
          v               v
     +-value---+        +-value---+
     |  5      |        |  8      |
     |  ::Int  |        |  ::Int  |
     +---------+        +---------+



function arguments
==================
Arguments passed to function by default pass an immutable reference

    function foo(a::Int) ...

    ....

    let x = 5
    foo(5)

is a function taking an immutable reference to an int,
any attempts to mutate a will result in a compile time error,
any attempts to upgrade a to a mutable reference will also result in a compile time error.


A function can also ask for a mutable refernece

    function bar(&a::Int) ...

this function is now free to mutate a,
this function can also pass on this reference to another function as either a mutable or immutable reference,
that is the function can 'downgrade' to an immutable reference.

Call sites must also 'agree' to this mutable contract by also using the & operator on the variable being passed

The following is a compile time error as the caller is not agreeing to the mutable contract

    let x = 5
    bar(x) # compile time error, bar wanted a mutable reference

To correctly call this function the caller must acknowledge the mutable contract

    let x = 5
    bar(&x)

Conversely trying to call pass a mutable argument when a function is expecting an immutable reference is also an error

    let x = 5
    foo(&x) # compile time error, foo wanted an immutable reference



Let's look at an example of how this looks:

    let a = 15
    baz(&a a)

    function baz(&m::Int i::Int) ...


The view of the world during the function call to baz:


      +-var-----+
      | m       |
      | ::int   |
      | mutable |
      +---------+
           |
           v
      +-var-----+          +-var-----+
      | a       |          | i       |
      | ::int   |          | ::int   |
      | mutable |          |immutable|
      +---------+          +---------+
           \                   /
            \                 /
             \               /
              \             /
               \           /
                v         v
                +-value---+
                |  15     |
                |  ::int  |
                +---------+



If inside our function baz we mutate through m

    let a = 15
    baz(&a a)

    function baz(&m::Int i::Int)
        &m = 123
    end

we then get this view of the world

      +-var-----+
      | m       |
      | ::Int   |
      | mutable |
      +---------+
           |
           V
      +-var-----+       +-var-----+
      | a       |       | i       |
      | ::Int   |       | ::Int   |
      | mutable |       |immutable|
      +---------+       +---------+
           |                 |
           |                 |
           |                 |
           v                 v
      +-value---+       +-value---+
      |  123    |       |  15     |
      |  ::Int  |       |  ::Int  |
      +---------+       +---------+

unsolved cases
--------------

The above is currently not solved for composite types (user defined structs)

One possible solution is to make it illegal to pass a mutable and immutable reference
of the same object to the same function

    # potential: ILLEGAL cannot pass both mutable nad immutable reference of same value
    bar(&a a)


Further considerations
======================

Some food for further thought

* I quite like the idea of of the mutability contract where both caller and function have to explicitly agree on mutability of arguments
* I am not quite sure about mutability of values
* what about references to instances of types


