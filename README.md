Icarus
======

Icarus is a toy typed language with static type checking


Plans
=====

* strong statically checked type system
* no 'holes' in the type system (null pointers be gone!)
* garbage collected
* multiple dispatch (dispatch to functions is based on function name and type of arguments passed)
* parametrized types
* union types (syntactic sugar for tagged enums, staticly checked before being taken apart)

Work so far
===========

So far all Icarus can do it lex it's `example/simple.ic`

    make example

outputs:

    type Foo a :: int b :: string end function d ( i :: Int ) print ( i ) end function d ( s :: String ) print ( s ) end function d ( f :: Foo ) d ( f . a ) d ( f . b ) end function main ( ) let f :: Foo = Foo ( 1 " hello " ) d ( f ) end 

