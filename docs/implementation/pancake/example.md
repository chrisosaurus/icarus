Bytecode transformation and runtime example
============================================

Small example icarus program:

    fn get_str() -> String
      return "hello world"
    end

    fn say_hi()
      println(get_str())
    end

    fn main()
      say_hi()
    end


the same program in TIR:

    fn get_str() -> String
      let _l1 = "Hello world"
      return _l1
    end

    fn say_hi() -> Unit
      let _t1 = get_str()
      println(_t1)
    end

    fn main() -> Unit
      say_hi()
    end


the bytecode:

    mapping:
      get_str -> 6
      say_hi -> 3
      main -> 1

    literals:
      lit_1 -> "Hello world"

    instructions:
      0 exit failure    # always exit, error case
      1 call say_hi     # main
      2 exit success
      3 call get_str    # say_hi
      4 call println
      5 return
      6 pushstr lit_1   # get_str
      7 return


the return address during runtime:

    return address  after 0:

      1

    return address after 2

      1
      3

    return address after 6

      1

    return address after 1

      [empty]

note that the address 0 is special, it is always exit
