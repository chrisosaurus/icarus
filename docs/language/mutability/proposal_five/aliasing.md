Alising
========


What is working well
====================

I like the behavior as can be seen in the heapsort example,
where we do an inplace heapsort

    fn main()
        let &arr = MakeArray()
        heapsort(&arr)
        print(arr)
    end

this is fine as once heapsort returns there is still only 1 reference to our array.


Safe aliasing
===============

    fn safe(&x::Int, y::Int)
        print(y)
        &x += 1
        print(y)
    end

    fn main()
        let &z = 1
        safe(&z, z)
    end

safe is given two references here, but the second one `y` has
immutable permissions, this gives it value semantics.

This guarantee on `y` means `safe` knows it cannot mutate under it for the duration of the call,
so the mutation through `x` cannot mutate `y`.

so this program will output

    1
    1

Explicit 'unsafe' aliasing
==========================

if however you purposefully want to allow a mutation through x to be seen through y

    fn unsafe(&x::Int, $y::Int)
        print(y)
        &x += 1
        print(y)
    end

    fn main()
        let &z = 1
        safe(&z, $z)
    end

here y is now `$y` which is a readonly reference,
this does not give guarantees about other mutations.

this program will output

    1
    2


