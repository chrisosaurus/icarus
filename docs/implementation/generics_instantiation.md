Generics
========

Icarus is in the progress of adding type generics to types and functions,
these most closely match templates from `C_++`.

Icarus currently only supports whole-program compilation,
there is no distinct link phase.

Types
=====

Types cannot be overloaded, which makes this a little easier.
Type inferring of type params is also harder, so skip for now.

    type Nothing
    end

    union Maybe[T]
      something::T
      nothing::Nothing
    end

Type instantiation
------------------

When the user uses the type `Maybe[Signed]` via:

    let q = Maybe[Signed](6)

or

    let q::Maybe[Signed] = ...

or

    fn foo(m::Maybe[Signed]) -> Signed
      ...
    end

or

    fn bar() -> Maybe[Signed]
      ...
    end

or

    type Foo
      m::Maybe[Signed]
    end

or

    union Baz
      m::Maybe[Signed]
    end

We have to check if `Maybe[Signed]` has already been instantiated,
if so we use that,
if not we trigger an instantiation.

Functions
=========

Functions can be overloaded, which is where one of the difficulties comes from,
the other would be the desire for inference.

    fn id[T](t::T) -> T
      return t
    end

    fn main()
      println(id(10))
    end

we don't want to have to always write `id[Signed](10}` since this can be inferred
from the argument to `id`.

In the presence of non-generic overloading we always want an exact match.
In the presence of generic overloading we always try an exact match first,
then a generic one.
It becomes problematic if we allow multiple generics at the same time,
at least within any arity.

We could possibly look at allowing overloading of the non-generic parameters,
e.g.

    fn foo[A](one::Signed, two::A) -> A ... end
    fn foo[A](one::String, two::A) -> A ... end
    fn foo[B](one::Bar, two::A) -> A ... end

are all distinct and do not interfere

whereas these cannot be told apart trivially

    fn foo[A](one::Signed, two::A) -> A ... end
    fn foo[A, B](one::A, two::B) -> A ... end

Possible solution spaces:

- disallow overloading of functions with generic instances = overly strict but safe
- allow only one generic version of each function with a given arity
- allow overloaded generic versions as long as no generic param clashes with a non-generic param

Function lookup
---------------

    foo(14, "Hello", "World")

could match any of

    fn foo(Signed,String,String)
    fn foo[A](A,String,String)
    fn foo[B](Signed,B,B)
    fn foo[A,B](A,B,B)

and this blows up as we have more complex generics and more arguments.

We also want to provide good error messages:

    'function foo(Signed,String) could not be found, perhaps you meant [list of similar functions)'

For now:

- there can be any number of fully-typed overloaded functions
- there can only be one generic version of a function for each function arguments arity
- there can only be one generic version of a function for each type_params arity
- functions will first try an exact match for a fully-typed, then try the only available generic at that function arguments arity


    baz(14, "Hello")

will therefore try lookup (in order)

1. `baz(Signed,String)`    -> at most one function
2. `baz[*](_,_)`         -> at most one function
3. error, no function found

since in Icarus we can use a function's name to refer to it, we also need to be able to lookup

    foo
    foo[A]
    foo[A,B]
    etc.

therefore at some point we will want maps for each of

1. `foo(Signed,String)` -> at most one function
2. `foo(_,_)`         -> list of functions
3. `foo[*](_,_)`      -> at most one function
4. `foo[_,_,_](_,_)`  -> at most one function - can we fold this into (3) ?
5. `foo[_,_,_]`       -> at most one function - is this desired ?
6. `foo`              -> list of functions

we may therefore want to force function names to be fully typed on usage,
there is instead of `foo` as a function value
me may want to require `foo(Signed,String)` as a function value.

