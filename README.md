# Icarus [![Build Status](https://travis-ci.org/mkfifo/icarus.svg?branch=master)](https://travis-ci.org/mkfifo/icarus) [![Coverage Status](https://coveralls.io/repos/mkfifo/icarus/badge.svg?branch=master)](https://coveralls.io/r/mkfifo/icarus?branch=master) <a href="https://scan.coverity.com/projects/4854"> <img alt="Coverity Scan Build Status" src="https://scan.coverity.com/projects/4854/badge.svg"/> </a>

<img src="https://github.com/mkfifo/icarus/blob/master/resources/logo.jpg" alt="Logo" align="right" />

Icarus is a programming language designed as an experiment to explore mutation and aliasing control

Allows controlled mutation and aliasing, but only when both caller and callee agree to it.
No global mutable state.

I am currently exploring a few potential options for how Icarus' "permissions" (ownership, aliasing, mutation, capabilities) system will work.


Goals
=====

* Explicit, obvious, and statically verifiable mutability
* Strong statically verifiable type system
* Runtime memory safety (no pointer arithmetic, arrays are bounds checked, all memory is initialised)


Example
=======

An example showing fizzbuzz and some elements of Icarus' permissions system,
the below example will not yet run in Icarus

    fn is_div(a::Sint, b::Sint) -> Bool
        return (a % b) == 0s
    end

    fn fizzbuzz(from::Sint, to::Sint)
        for num in range(from, to)
            let &arr::Vector[String]

            if is_div(num, 3s)
                &arr.append("Fizz")
            end

            if is_div(num, 5s)
                &arr.append("Buzz")
            end

            if length(arr) == 0s
                &arr.append(num)
            end

            println(arr.join())
        end
    end

    fn main()
        fizzbuzz(1s, 100s)
    end

A simpler version is shown below which will run in Icarus


Progress
========

Icarus is very early in development and so is currently working towards a
minimal proof of concept, the surface syntax is purposefully minimal.


Icarus currently supports 2 backends: `2c` which compiles to C and
`pancake` which is a stack-based bytecode interpreter.

Pancake can be invoked directly as an interpreter, or it can be used to compile
icarus to pancake bytecode which can then be later interpreted.


Icarus currently lacks many of the basics you need would need in order to
write interesting programs.

Obvious glaring omissions which should prevent you from using Icarus:

 * Lack of generics (coming soon)
 * Lack of standard containers (coming soon)
 * Lack of garbage collection or memory management
 * Lack of mutation
 * Lack of iteration


The next milestone for Icarus is adding support for type polymorphic functions
and data types.

Work so far - 2c backend
========================

The life of a simple icarus program

if we input the following file (which can be found in `example/fizzer.ic`):

    fn is_div(a::Sint, b::Sint) -> Bool
        let rem = a % b
        return rem == 0s
    end

    fn fizzer(num::Sint)
        let str = ""

        if is_div(num, 3s)
            str = concat(str, "Fizz")
        end

        if is_div(num, 5s)
            str = concat(str, "Buzz")
        end

        if length(str) == 0s
            println(num)
        else
            println(str)
        end
    end

    fn fizzbuzz(from::Sint, to::Sint)
      if from < to
        fizzer(from)
        from = from + 1s
        fizzbuzz(from, to)
      end
    end

    fn main()
        # icarus currently lacks for loops and ranges
        # so this is a poor-mans fizzbuzz-derived demo
        fizzbuzz(1s, 20s)
    end

we can then compile to C this via:

    ./icarus 2c example/fizzer.ic -o fizzer.c --debug

which will show us the transformed IR version of this:

    fn is_div(a::Sint, b::Sint) -> Bool
        let rem::Sint = modulo(a, b)
        let _l0::Sint = 0s
        let _t0::Bool = equal(rem, _l0)
        return _t0
    end
    fn fizzer(num::Sint) -> Unit
        let str::String = ""
        let _l0::Sint = 3s
        let _t0::Bool = is_div(num, _l0)
        if _t0
            let _l1::String = "Fizz"
            str = concat(str, _l1)
        end
        let _l2::Sint = 5s
        let _t1::Bool = is_div(num, _l2)
        if _t1
            let _l3::String = "Buzz"
            str = concat(str, _l3)
        end
        let _t3::Uint = length(str)
        let _l4::Sint = 0s
        let _t2::Bool = equal(_t3, _l4)
        if _t2
            println(num)
        else
            println(str)
        end
    end
    fn fizzbuzz(from::Sint, to::Sint) -> Unit
        let _t0::Bool = lessthan(from, to)
        if _t0
            fizzer(from)
            let _l0::Sint = 1s
            from = plus(from, _l0)
            fizzbuzz(from, to)
        end
    end
    fn main() -> Unit
        let _l0::Sint = 1s
        let _l1::Sint = 20s
        fizzbuzz(_l0, _l1)
    end

and produce a c program `fizzer.c`.

if we compile and run this:

    gcc fizzer.c -o fizzer
    ./fizzer

we can see the output:

    1
    2
    Fizz
    4
    Buzz
    Fizz
    7
    8
    Fizz
    Buzz
    11
    Fizz
    13
    14
    FizzBuzz
    16
    17
    Fizz
    19

Work so far - pancake backend
========================

we can run this same example through the `pancake` backend which will
first compile it to bytecode and then interpret it

    ./icarus pancake example/fizzer.ic --debug

which shows us

    ...
    backend pancake selected (PENDING):
    Pancake bytecode:
    ==========================
    label entry
    call main() 0
    exit
    label is_div(Sint,Sint)
    copyarg 0
    copyarg 1
    call_builtin modulo(Sint,Sint) 2
    store rem
    load rem
    pushint 0
    call_builtin equal(Sint,Sint) 2
    store _t0
    load _t0
    save
    clean_stack
    restore
    return_value
    label fizzer(Sint)
    pushstr ""
    store str
    copyarg 0
    pushint 3
    call is_div(Sint,Sint) 2
    store _t0
    load _t0
    jnif_label fizzer(Sint)0
    load str
    pushstr "Fizz"
    call_builtin concat(String,String) 2
    store str
    label fizzer(Sint)0
    copyarg 0
    pushint 5
    call is_div(Sint,Sint) 2
    store _t1
    load _t1
    jnif_label fizzer(Sint)1
    load str
    pushstr "Buzz"
    call_builtin concat(String,String) 2
    store str
    label fizzer(Sint)1
    load str
    call_builtin length(String) 1
    store _t2
    load _t2
    pushint 0
    call_builtin equal(Uint,Sint) 2
    store _t3
    load _t3
    jnif_label fizzer(Sint)2
    copyarg 0
    call_builtin println(Sint) 1
    jmp_label fizzer(Sint)3
    label fizzer(Sint)2
    load str
    call_builtin println(String) 1
    label fizzer(Sint)3
    clean_stack
    return_unit
    label fizzbuzz(Sint,Sint)
    copyarg 0
    store from
    load from
    copyarg 1
    call_builtin lessthan(Sint,Sint) 2
    store _t0
    load _t0
    jnif_label fizzbuzz(Sint,Sint)0
    load from
    call fizzer(Sint) 1
    load from
    pushint 1
    call_builtin plus(Sint,Sint) 2
    store from
    load from
    copyarg 1
    call fizzbuzz(Sint,Sint) 2
    label fizzbuzz(Sint,Sint)0
    clean_stack
    return_unit
    label main()
    pushint 1
    pushint 20
    call fizzbuzz(Sint,Sint) 2
    clean_stack
    return_unit
    ==========================

    Pancake interpreter output
    ==========================
    1
    2
    Fizz
    4
    Buzz
    Fizz
    7
    8
    Fizz
    Buzz
    11
    Fizz
    13
    14
    FizzBuzz
    16
    17
    Fizz
    19

Example showing user-defined types
==================================

The file `example/shapes.ic` contains a simple example showing types and unions

    type Point
        x::Uint
        y::Uint
    end

    type Square
        topleft::Point
        width::Uint
    end

    type Rectangle
        topleft::Point
        bottomright::Point
    end

    type Circle
        center::Point
        radius::Uint
    end

    union Shape
        circle::Circle
        square::Square
        rectangle::Rectangle
    end

    fn generate_shape() -> Shape
        return Shape(Rectangle(Point(14u, 10u), Point(20u, 24u)))
    end

    fn main()
        let s = generate_shape()
        println(s)
    end


If we run this program we can see the output

    $ ./icarus example/shapes.ic
    Shape{Rectangle{Point{14, 10}, Point{20, 24}}}

Usage
=====

Icarus is written in C99 and only requires a C99 compiler and C99 standard
library, no other dependences are needed or used.

You can build Icarus by cloning this repository and running make,
any issues with this process should be reported here.

    make

You can run your first Icarus program through the interpreter `pancake`:

    ./icarus example/fizzer.ic

Or you can use the 2c compiler, compile the c, and then execute the generated
executable:

    ./icarus 2c example/fizzer.ic -o fizzer.c
    gcc fizzer.c -o fizzer
    ./fizzer

Icarus help is available directly via `--help` or via the man page,
you can view them like so:

    # view icarus builtin help
    ./icarus --help

    # view icarus man page
    man -l icarus.1


