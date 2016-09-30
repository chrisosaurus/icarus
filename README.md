# Icarus [![Build Status](https://travis-ci.org/mkfifo/icarus.svg?branch=master)](https://travis-ci.org/mkfifo/icarus) [![Coverage Status](https://coveralls.io/repos/mkfifo/icarus/badge.svg?branch=master)](https://coveralls.io/r/mkfifo/icarus?branch=master) <a href="https://scan.coverity.com/projects/4854"> <img alt="Coverity Scan Build Status" src="https://scan.coverity.com/projects/4854/badge.svg"/> </a>

<img src="https://github.com/mkfifo/icarus/blob/master/resources/logo.jpg" alt="Logo" align="right" />

Icarus is a programming language designed as an experiment to explore static verification of mutation contracts

A hybrid imperative/functional language designed to explore the boundary between these 2 paradigms - trying to port some of the lessons and advantages from functional programming back to a more traditional imperative style.

Allows controlled mutation and aliasing, but only when both caller and callee agree to it. 
No global mutable state.

I am currently exploring a few potential options for how Icarus' "permissions" (ownership, aliasing,mutation) system will work.

The current proposal can be seen in [docs/language/mutability/proposal_five/permissions.md](https://github.com/mkfifo/icarus/blob/master/docs/language/mutability/proposal_five/permissions.md)


Goals
=====

* Explicit, obvious and statically verifiable mutability
* Strong statically verified type system
* Runtime memory safety (no pointer arithmetic, arrays are bounds checked, all memory is intialised)


Example
=======

An example showing fizzbuzz and some elements of Icarus' permissions system,
the below example will not yet run in Icarus

    fn is_div(a::Sint, b::Sint) -> Bool
        return (a % b) == 0
    end

    fn fizzbuzz(from::Sint, to::Sint)
        for num in range(from, to)
            let &arr::Vector<String>

            if is_div(num, 3)
                &arr.append("Fizz")
            end

            if is_div(num, 5)
                &arr.append("Buzz")
            end

            if length(arr) == 0
                &arr.append(num)
            end

            print(join(" ", arr))
        end
    end

    fn main()
        fizzbuzz(1, 100)
    end

A simpler version is shown below which will run in Icarus


Progress
========

Icarus is currently working towards a minimal proof of concept, in that context:

Complete to a basic level (good enough for minimal proof of concept):

* lexer
* parser
* analyser (semantic analysis phase)

In progress (currently lacking for minimal proof of concept):

* transform (convert to IR)
* backends (compiler, interpreter)


Work so far - 2c backend
========================

The life of a simple icarus program

if we input the following file (`example/fizzer.ic)):

    fn is_div(a::Sint, b::Sint) -> Bool
        let rem = a % b
        return rem == 0
    end

    fn fizzer(num::Sint)
        let str = ""

        if is_div(num, 3)
            str = concat(str, "Fizz")
        end

        if is_div(num, 5)
            str = concat(str, "Buzz")
        end

        if length(str) == 0
            println(num)
        else
            println(str)
        end
    end

    fn fizzbuzz(from::Sint, to::Sint)
      if from < to
        fizzer(from)
        from = plus(from, 1)
        fizzbuzz(from, to)
      end
    end

    fn main()
        # icarus currently lacks for loops and ranges
        # so this is a poor-mans fizzbuzz-derived demo
        fizzbuzz(1, 20)
    end

we can then compile this via:

    ./icarus example/fizzer.ic 2c fizzer.c

which will show us the transformed IR version of this:

    fn is_div(a::Sint, b::Sint) -> Bool
        let rem::Sint = modulo(a, b)
        let _l0::Sint = 0
        let _t0::Bool = equal(rem, _l0)
        return _t0
    end
    fn fizzer(num::Sint) -> Void
        let str::String = ""
        let _l0::Sint = 3
        let _t0::Bool = is_div(num, _l0)
        if _t0
            let _l1::String = "Fizz"
            str = concat(str, _l1)
        end
        let _l2::Sint = 5
        let _t1::Bool = is_div(num, _l2)
        if _t1
            let _l3::String = "Buzz"
            str = concat(str, _l3)
        end
        let _t3::Uint = length(str)
        let _l4::Sint = 0
        let _t2::Bool = equal(_t3, _l4)
        if _t2
            println(num)
        else
            println(str)
        end
    end
    fn fizzbuzz(from::Sint, to::Sint) -> Void
        let _t0::Bool = lessthan(from, to)
        if _t0
            fizzer(from)
            let _l0::Sint = 1
            from = plus(from, _l0)
            fizzbuzz(from, to)
        end
    end
    fn main() -> Void
        let _l0::Sint = 1
        let _l1::Sint = 20
        fizzbuzz(_l0, _l1)
    end

and it will produce a c program `fizzer.c`.

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

    ./icarus example/fizzer.ic pancake

which shows us

    ...
    backend pancake selected (PENDING):
    Pancake bytecode:
    ==========================
    label entry
    call main() 0
    exit
    label is_div(Sint,Sint)
    copyarg 1
    copyarg 0
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
    clean_stack
    return_void
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
    store _t3
    load _t3
    pushint 0
    call_builtin equal(Uint,Sint) 2
    store _t2
    load _t2
    jnif_label fizzer(Sint)2
    copyarg 0
    call_builtin println(Sint) 1
    jmp_label fizzer(Sint)3
    label fizzer(Sint)2
    load str
    call_builtin println(String) 1
    label fizzer(Sint)3
    clean_stack
    return_void
    label fizzbuzz(Sint,Sint)
    copyarg 1
    store from
    load from
    copyarg 0
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
    copyarg 0
    call fizzbuzz(Sint,Sint) 2
    label fizzbuzz(Sint,Sint)0
    clean_stack
    return_void
    label main()
    pushint 1
    pushint 20
    call fizzbuzz(Sint,Sint) 2
    clean_stack
    return_void
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

Current holes
=============

* The IR is incomplete
* The compiler (2c) is incomplete
* The interpreter (pancake) is incomplete


Current focus
=============

The two backends are roughly now on-par with each other,
so the focus has now shifted to adding some more of the basic language
features.

