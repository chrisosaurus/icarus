A quick overview of icarus' planned reference semantics

This is all theoretical at this point and still requires much discussion.

quick summary of rules
======================

    `=` always creates a new value and binds the variable on the left to it

Some example of application:

1. `let a = 5` creates a new variable `a` and a new value `5` and binds them
2. `a = 15` creates a new value `15` and rebinds a to this
3. `a += 3` is really `a = a + 3`, so no mutation occurs here
4. `f.a` is considered a variable, so `f.a = 4` will change the field `a` of `f` to the new value `4`, this will not otherwise mutate `f`
5. `&a` is considered a variable, this is mutating the variable referenced to from `a`


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


Further considerations
======================

Some food for further thought

* I quite like the idea of of the mutability contract where both caller and function have to explicitly agree on mutability of arguments
* I am not quite sure about mutability of values


One possible consideration is to make it illegal to pass a mutable and immutable reference
of the same object to the same function

    # potential: ILLEGAL cannot pass both mutable nad immutable reference of same value
    bar(&a a)


