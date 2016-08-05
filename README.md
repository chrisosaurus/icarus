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


Work so far
===========

The life of a simple icarus program

if we input the following file (1.ic):

    fn get_str(name::String) -> String
        return concat(concat("Hello there ", name), ", very nice to meet you")
    end

    fn main()
      println(get_str("Jennifer"))
    end

we can then compile this via:

    ./icarus 1.ic 2c out.c

this will show us the parser outputting it's understanding of our code

    # get_str(String)
    fn get_str(name::String) -> String
        return concat(concat("Hello there ", name), ", very nice to meet you")
    end

    # main()
    fn main() -> Void
        println(get_str("Jennifer"))
    end

and show us the transformed IR version of this

    fn get_str(name::String) -> String
        let _l1::String = "Hello there "
        let _t2::String = concat(_l1, name)
        let _l2::String = ", very nice to meet you"
        let _t1::String = concat(_t2, _l2)
        return _t1
    end
    fn main() -> Void
        let _l1::String = "Jennifer"
        let _t1::String = get_str(_l1)
        println(_t1)
    end

which will output (to out.c):

    #include "backends/2c/builtins.c"
    /* get_str(String) -> String */
    String * i_get_str_a_String(String *name);
    /* main() -> Void */
    Void * i_main_a();
    /* get_str(String) -> String */
    String * i_get_str_a_String(String *name){
    String *_l1 = ic_string_new("Hello there ", 12);
    String *_t2 = i_concat_a_String_String(_l1, name);
    String *_l2 = ic_string_new(", very nice to meet you", 23);
    String *_t1 = i_concat_a_String_String(_t2, _l2);
    return _t1;
    }
    /* main() -> Void */
    Void * i_main_a(){
    String *_l1 = ic_string_new("Jennifer", 8);
    String *_t1 = i_get_str_a_String(_l1);
    i_println_a_String(_t1);
    }
    #include "backends/2c/entry.c"

if we compile and run this, we can see:

    gcc out.c
    ./a.out

the output:

    Hello there Jennifer, very nice to meet you


Current holes
=============

* The IR is incomplete
* The compiler (2c) is incomplete
* The interpreter (pancake) has only just started


Current focus
=============

The `transform` phase - which outputs the new Icarus Transform IR

The `pancake` interpreter - work is beginning

