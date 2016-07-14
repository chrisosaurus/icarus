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

    fn say_hi() -> Void
      let _t1 = get_str()
      println(_t1)
    end

    fn main() -> Void
      say_hi()
    end


the bytecode:

    mapping:
      get_str -> 5
      say_hi -> 2
      main -> 0

    literals:
      lit_1 -> "Hello world"

    instructions:
      0 call say_hi     # main
      1 exit success
      2 call get_str    # say_hi
      3 call println
      4 return
      5 pushstr lit_1   # get_str
      6 return


the return address during runtime:

    return address  after 0:

      0

    return address after 2

      0
      2

    return address after 6

      0

    return address after 1

      <empty>


