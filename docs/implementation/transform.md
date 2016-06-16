Transform component
===================

Current after analyse we are left with functions of the form

  fn say_hi()
    println("hello world")
  end

notice that we are constructing a literal, which could fail, and then
calling println on the result.

we should first convert this to the form

  fn say_hi()
    let _l1::String = "hello world"
    println(_l1)
  end


likewise if we have a more complex function

  fn foo()
    let res = do_something(x(y(z("hello"))))
    println(1 + res + 3)
  end

we should first convert to the form

  fn foo()
    let _l1::String = "hello"
    let _t1::???    = z(_l1)
    let _t2::???    = y(_t1)
    let _t3::???    = x(_t2)
    let res::???    = do_someting(_t3)
    let _l2::Int    = 1
    let _l3::Int    = 3
    let _t4::???    = _l2 + res
    let _t5::???    = _t4 + _l3
    println(_t5)
  end

the idea here is that every line is a single operation
and we can then automate the insertion of error checking between each line

here each `_lx` stands for 'literal x'
and each `_tx` stands for 'temporary x'

we should also destructure returns

  fn bar()
    return baz("hello")
  end

to

  fn bar()
    let _l1::String = "hello"
    let _ret::???   = baz(_l1)
    return _ret
  end

using name `_ret` for return value to make it more obvious


notice that we are beginning to approximate SSA and linear typing.

