Icarus
======

Icarus is a toy language with static type checking

Inspiration has been drawn from (in alphabetical order):

* C
* C++
* Go
* Haskell
* Julia
* Python
* Rust
* Scheme

Plans
=====

* no undefined behavior
* strong statically checked type system
* no 'holes' in the type system (null and void pointer type are all banished) - see `docs/noholes.md`
* managed memory (automatic garbage collection)
* multiple dispatch (dispatch to functions is based on function name and type of arguments passed)
* operator overloading
* parametrized types (derived from C++ and Julia)
* union types (tagged enums, staticly checked before being taken apart)
    * like Rust's enums
    * like haskell's ADTs (Algebraic Data Types)
* including both an interpreter and compiler (currently considering both C and LLVM bitcode as compiler backend outputs) - see `docs/components.md`

Work so far
===========

So far all Icarus can do is lex it's `example/simple.ic`

    make example

outputs:

    type Foo a :: Int b :: String end function d ( i :: Int ) print ( i ) end function d ( s :: String ) print ( s ) end function d ( f :: Foo ) d ( f . a ) d ( f . b ) end function main ( ) let f :: Foo = Foo ( 1 " hello " ) d ( f ) end 

