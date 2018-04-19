pitfalls
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


Bad aliasing
===============

here is an example where I dislike the default semantics

    fn naughty(&x::Signed, y::Signed)
        print(y)
        &x += 1
        print(y)
    end

    fn main()
        let &z = 1
        naughty(&z, z)
    end

since `naughty` is given two reference to the same object,
one mutable and one immut,
any mutations through the first reference are visible via the second

so this program will output

    1
    2


Bad aliasing within containers
==============================

    type Node
        x::Signed
    end

    fn store(y::Signed)
        return Node(y)
    end

    fn main()
        let &z = 1
        let n = store(%z)
        print(n.x)
        &z += 1
        print(n.x)
    end

likewise this program will also print

    1
    2


