Contextual typing
=================

I've sometimes referred to this as 'typing backpressure'.

Example
--------------

    fn foo() -> Uint
      let a = 14
      return a
    end

    fn bar() -> Sint
      let b = 15
      return b
    end

what is the type of `a`?
what is the type of `b`?

Currently the literal will default to some int type, currently Sint.
So `bar` will type, but currently `foo` is a type error.
This is non-ideal.


Obvious cases
------------


### direct return

    fn foo() -> Uint
      return 14
    end

here we can immediately see that `14` should be typed as `Uint`.


### indirection return - one layer

    fn foo() -> Uint
      let a = 14
      return a
    end

The hard part here is that the typing of `a` cannot be determined until it is
seen that it is returned later on, and therefore must be typed as `Uint`.


### indirect return - many layers

    fn foo() -> Uint
      let a = 14
      let b = 15 + a
      let c = a + b
      return c
    end

Here the typing for `a` and `b` must be inferred through `c` via the final
`return c`.


### type construction

    type Bar
      a::Uint
    end

    fn main()
      let b = Bar(15)
    end

likewise here, in the constructor call to `Bar(Uint)` we need to type `15`
as `Uint` to match the call.

This is a little bit odd, as currently Icarus dispatches function calls on type.

so `Bar(15)` will currently map to `Bar_Sint(15)` which is non-ideal.


