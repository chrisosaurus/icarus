A quick overview of icarus' planned reference semantics

This is all theoretical at this point and still requires much discussion.

local aliasing
==============

Local variables are mutable and their values can be aliased and mutated

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


we can then create an alias to this

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

    function baz(&i::Int o::Int) ...


The view of the world is now during the function call to baz is now:

      +-var-----+  +-var-----+  +-var-----+
      | a       |  | i       |  | i       |
      | ::Int   |  | ::Int   |  | ::Int   |
      | mutable |  | mutable |  |immutable|
      +---------+  +---------+  +---------+
           \        |          /
            \       |         /
             \      |        /
              \     |       /
               \    |      /
                v   v     v
                +-value---+
                |  15     |
                |  ::Int  |
                +---------+


This value can be mutated only through `a` (from outside the function) or from `i` within the function,
any mutations to this value will be observable through all 3 variables,
this value can not be mutated via `i` though as `i` is immutable.



Further considerations
======================

Some food for further thought

* I quite like the idea of of the mutability contract where both caller and function have to explicitly agree on mutability of arguments
* I am not quite sure about all values being mutable
* I am also unsure about how assignments to variables are always writing through the variable to the value, what about variable reuse say in iterators?

