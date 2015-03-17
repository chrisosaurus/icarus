# syntax and semantics still in the works

# largely inspired by:
#   c (pointer vs pointee difference)
#   c# (`out` is the biggest influence here)
#   rust (default immutability)

# the goals here are:
#    to have mutability visible at the call site
#    to have mutability visible at the interface level (function declaration)

type Foo
  a::Int
end

# function bar takes a mutable reference to a Foo
function bar(&f::Foo)
  f.a = 14
end

# function bar takes a mutable reference to an Int
function bar(&a::Int)
  a = 14
end

function main()
  let x::Foo = Foo(0)

  bar(&x) # one - function can mutate x

  bar(&x.a) # two - function can mutate x.a


  bar(14) # three - ERROR cannot pass constant as mutable ref
  bar(x.a) # ERROR passing an immutable reference, function wants mutable


  let i::Int = 0;

  bar(&i) # four - function can mutate i

end

