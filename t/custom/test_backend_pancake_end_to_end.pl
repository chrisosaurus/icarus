#!/usr/bin/env perl
use strict;
use warnings;
use v5.10;
use IPC::Open3;

my $path = "./icarus";

die "Could not find '$path'\n" unless -e $path;

my $cases = [
  {
    input => '
      fn main()
        println("Hello world")
      end
      ',
    expected => '
      label entry
      call main() 0
      exit
      label main()
      push_str "Hello world"
      call_builtin println(String) 1
      clean_stack
      return_unit
      Hello world
      '
  },

  {
    input => '
      fn bar(a::Signed) -> Signed
        return 7s + a
      end

      fn foo(a::Signed) -> Signed
        return bar(a + 4s)
      end

      fn baz(a::Signed) -> Signed
        let b = a + 3s
        return bar(b)
      end

      fn main()
        println(foo(7s))
        println(baz(7s))
      end
      ',
    expected => '
      label entry
      call main() 0
      exit
      label bar(Signed)
      push_signed 7
      copyarg 0
      call_builtin plus(Signed,Signed) 2
      store _t0
      load _t0
      save
      clean_stack
      restore
      return_value
      label foo(Signed)
      copyarg 0
      push_signed 4
      call_builtin plus(Signed,Signed) 2
      store _t0
      load _t0
      call bar(Signed) 1
      store _t1
      load _t1
      save
      clean_stack
      restore
      return_value
      label baz(Signed)
      copyarg 0
      push_signed 3
      call_builtin plus(Signed,Signed) 2
      store b
      load b
      call bar(Signed) 1
      store _t0
      load _t0
      save
      clean_stack
      restore
      return_value
      label main()
      push_signed 7
      call foo(Signed) 1
      store _t0
      load _t0
      call_builtin println(Signed) 1
      push_signed 7
      call baz(Signed) 1
      store _t1
      load _t1
      call_builtin println(Signed) 1
      clean_stack
      return_unit
      18
      17
      ',
  },
  {
    input => '
      fn main()
        let b1 = True
        println(b1)
        let b2 = False
        println(b2)

        println(6s < 15s)
      end
      ',
    expected => '
      label entry
      call main() 0
      exit
      label main()
      push_bool 1
      call_builtin println(Bool) 1
      push_bool 0
      call_builtin println(Bool) 1
      push_signed 6
      push_signed 15
      call_builtin lessthan(Signed,Signed) 2
      store _t0
      load _t0
      call_builtin println(Bool) 1
      clean_stack
      return_unit
      True
      False
      True
      ',
  },
  {
    input => '
      type Bar
        s::String
      end

      type Foo
        a::Signed
        b::Bar
        c::String
      end

      fn main()
        let b = Bar("Hello")
        let f = Foo(4s, b, "World")
        println(f)
      end
    ',
    expected => '
      label entry
      call main() 0
      exit
      label Bar(String)
      alloc 1
      copyarg 0
      store_offset 0
      save
      clean_stack
      restore
      return_value
      label print(Bar)
      push_str "Bar"
      call_builtin print(String) 1
      push_str "("
      call_builtin print(String) 1
      copyarg 0
      load_offset_str 0
      call_builtin print(String) 1
      push_str ")"
      call_builtin print(String) 1
      clean_stack
      return_unit
      label println(Bar)
      copyarg 0
      call print(Bar) 1
      call_builtin println() 0
      clean_stack
      return_unit
      label Foo(Signed,Bar,String)
      alloc 3
      copyarg 0
      store_offset 0
      copyarg 1
      store_offset 1
      copyarg 2
      store_offset 2
      save
      clean_stack
      restore
      return_value
      label print(Foo)
      push_str "Foo"
      call_builtin print(String) 1
      push_str "("
      call_builtin print(String) 1
      copyarg 0
      load_offset_sint 0
      call_builtin print(Signed) 1
      push_str ", "
      call_builtin print(String) 1
      load_offset_ref 1
      call print(Bar) 1
      push_str ", "
      call_builtin print(String) 1
      load_offset_str 2
      call_builtin print(String) 1
      push_str ")"
      call_builtin print(String) 1
      clean_stack
      return_unit
      label println(Foo)
      copyarg 0
      call print(Foo) 1
      call_builtin println() 0
      clean_stack
      return_unit
      label main()
      push_str "Hello"
      call Bar(String) 1
      store b
      push_signed 4
      load b
      push_str "World"
      call Foo(Signed,Bar,String) 3
      store f
      load f
      call println(Foo) 1
      clean_stack
      return_unit
      Foo(4, Bar(Hello), World)
    ',
  },
  # testing copyarg ordering
  {
    input => '
      fn foo(a::Signed, b::String, c::String, d::Signed)
        println(a)
        println(b)
        println(c)
        println(d)
        println(d)
        println(c)
        println(b)
        println(a)
      end
      fn main()
        foo(1s, "Hello", "world", 2s)
      end
    ',
    expected => '
      label entry
      call main() 0
      exit
      label foo(Signed,String,String,Signed)
      copyarg 0
      call_builtin println(Signed) 1
      copyarg 1
      call_builtin println(String) 1
      copyarg 2
      call_builtin println(String) 1
      copyarg 3
      call_builtin println(Signed) 1
      copyarg 3
      call_builtin println(Signed) 1
      copyarg 2
      call_builtin println(String) 1
      copyarg 1
      call_builtin println(String) 1
      copyarg 0
      call_builtin println(Signed) 1
      clean_stack
      return_unit
      label main()
      push_signed 1
      push_str "Hello"
      push_str "world"
      push_signed 2
      call foo(Signed,String,String,Signed) 4
      clean_stack
      return_unit
      1
      Hello
      world
      2
      2
      world
      Hello
      1
    ',
  },
  {
    input => '
        type Bar
            a::Signed
        end

        union Foo
            a::Signed
            b::String
            c::Bar
        end

        fn main()
            let f = Foo(6s)
            println(f)
        end
    ',
    expected => '
      label entry
      call main() 0
      exit
      label Bar(Signed)
      alloc 1
      copyarg 0
      store_offset 0
      save
      clean_stack
      restore
      return_value
      label print(Bar)
      push_str "Bar"
      call_builtin print(String) 1
      push_str "("
      call_builtin print(String) 1
      copyarg 0
      load_offset_sint 0
      call_builtin print(Signed) 1
      push_str ")"
      call_builtin print(String) 1
      clean_stack
      return_unit
      label println(Bar)
      copyarg 0
      call print(Bar) 1
      call_builtin println() 0
      clean_stack
      return_unit
      label Foo(Signed)
      alloc 2
      push_unsigned 0
      store_offset 0
      copyarg 0
      store_offset 1
      save
      clean_stack
      restore
      return_value
      label Foo(String)
      alloc 2
      push_unsigned 1
      store_offset 0
      copyarg 0
      store_offset 1
      save
      clean_stack
      restore
      return_value
      label Foo(Bar)
      alloc 2
      push_unsigned 2
      store_offset 0
      copyarg 0
      store_offset 1
      save
      clean_stack
      restore
      return_value
      label print(Foo)
      push_str "Foo"
      call_builtin print(String) 1
      push_str "("
      call_builtin print(String) 1
      copyarg 0
      load_offset_uint 0
      push_unsigned 0
      call_builtin equal(Unsigned,Unsigned) 2
      jif_label print(Foo)0
      load_offset_uint 0
      push_unsigned 1
      call_builtin equal(Unsigned,Unsigned) 2
      jif_label print(Foo)1
      load_offset_uint 0
      push_unsigned 2
      call_builtin equal(Unsigned,Unsigned) 2
      jif_label print(Foo)2
      panic "impossible tag"
      label print(Foo)0
      load_offset_sint 1
      call_builtin print(Signed) 1
      jmp_label print(Foo)3
      label print(Foo)1
      load_offset_str 1
      call_builtin print(String) 1
      jmp_label print(Foo)3
      label print(Foo)2
      load_offset_ref 1
      call print(Bar) 1
      jmp_label print(Foo)3
      label print(Foo)3
      push_str ")"
      call_builtin print(String) 1
      clean_stack
      return_unit
      label println(Foo)
      copyarg 0
      call print(Foo) 1
      call_builtin println() 0
      clean_stack
      return_unit
      label main()
      push_signed 6
      call Foo(Signed) 1
      store f
      load f
      call println(Foo) 1
      clean_stack
      return_unit
      Foo(6)
    ',
  },
];

# whitespace sensitivity sucks
sub cleanup {
  my $str = shift // die;
  die if @_;

  # strip up to 6 leading (horizontal whitespace) spaces
  $str =~ s/^\h{0,6}//mg;

  # strip any leading newline
  $str =~ s/^\s*//mg;

  # strip any trailing newline
  $str =~ s/\s*$//mg;

  return $str;
}

sub run {
  my $input = shift // die;
  my $expected = shift // die;
  die if @_;

  my $in_tmp_file = `mktemp TESTING_BACKEND_PANCAKE_END_TO_END_XXX.ic`;
  chomp($in_tmp_file);
  open( my $fh, ">", "$in_tmp_file" ) or die "failed to open tmp file '$in_tmp_file'";
  print $fh $input;
  close $fh;

  my $output = `$path pancake $in_tmp_file -o -`;
  my $status = $?;

  `rm $in_tmp_file`;

  $output = cleanup $output;

  if( $status != 0 ){
      say "Command return non-zero exit code '${status}";
      say "Command gave output '${output}'";
      die "Command return non-zero exit code '${status}";
  }

  if( $output ne $expected ){
      say "Output was not as expected";
      say "=======\nExpected:\n$expected";
      say "=======\nGot:\n$output";
      say "=======\n";
      die "Output not as expected";
  }

  say "=======\nGot correct output:\n$output\n=======";
}

for my $case (@$cases) {
  my $input = cleanup $case->{input} // die;
  my $expected = cleanup $case->{expected} // die;
  run $input, $expected;
}

say "test_backend_pancake_end_to_end successs";

