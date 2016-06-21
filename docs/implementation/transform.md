Transform component
===================

notes
-----
notice that we are beginning to approximate SSA and linear typing.


example one
-----------
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


example two
------------

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

example three
-------------

we should also destructure returns

    fn bar() -> ???
      return baz("hello")
    end

to

    fn bar() -> ???
      let _l1::String = "hello"
      let _ret::???   = baz(_l1)
      return _ret
    end

using name `_ret` for return value to make it more obvious


example four
------------

    fn accum(in::???) -> ???
      let acc = []
      for each in foo(in)
        if cond(each)
          acc.append(bar(each))
        end
      end
      return acc
    end

should be transformed to

    fn accum(in::???) -> ???
      let acc::??? = []
      let _t1::??? = foo(in)
      for each in _t1
        let _t2::??? = cond(each)
        if _t2
          let _t3::??? = bar(each)
          acc.append(_t3)
        end
      end
      return acc
    end


example five
-------------

    fn assigns(a::???, b::???)
      a.x = b.y
    end

should be transformed to

    fn assigns(a::???, b::???)
      let _t1 = b.y
      a.x = _t1
    end

there are of course more complicated instances:

example six
------------

    fn more_complex(a::???, b::???)
      a.x.y.z = b.q.w.e
    end


should be transformed to

    fn more_complex(a::???, b::???)
      let _t1 = b.q
      let _t2 = _t1.w
      let _t3 = _t2.e

      let _t4 = a.x
      let _t5 = _t4.y

      _t5.z = _t3
    end


IR semantic grammar
-------------------

    stmt ::= let
         ::= assign
         ::= return
         ::= fcall
         ::= if
         ::= while
         ::= for

    let ::= constructor
        ::= fcall
        ::= faccess

    assign ::= faccess var
           ::= var faccess
           ::= var fcall
           ::= var var

    constructor ::= fcall literal

    fcall ::= func(var...)

    var ::= symbol

    literal ::= string
            ::= integer


MVP IR
------

removing faccess

    stmt ::= let
         ::= assign
         ::= return

    let ::= constructor
        ::= fcall

    assign ::= var fcall
           ::= var var

    constructor ::= fcall literal

    fcall ::= func(var...)

    var ::= symbol

    literal ::= string
            ::= integer


