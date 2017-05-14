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
      pushstr "Hello world"
      call_builtin println(String) 1
      clean_stack
      return_void
      Hello world
      '
  },

  {
    input => '
      fn bar(a::Sint) -> Sint
        return 7 + a
      end

      fn foo(a::Sint) -> Sint
        return bar(a + 4)
      end

      fn baz(a::Sint) -> Sint
        let b = a + 3
        return bar(b)
      end

      fn main()
        println(foo(7))
        println(baz(7))
      end
      ',
    expected => '
      label entry
      call main() 0
      exit
      label bar(Sint)
      pushint 7
      copyarg 0
      call_builtin plus(Sint,Sint) 2
      store _t0
      load _t0
      save
      clean_stack
      restore
      return_value
      label foo(Sint)
      copyarg 0
      pushint 4
      call_builtin plus(Sint,Sint) 2
      store _t1
      load _t1
      call bar(Sint) 1
      store _t0
      load _t0
      save
      clean_stack
      restore
      return_value
      label baz(Sint)
      copyarg 0
      pushint 3
      call_builtin plus(Sint,Sint) 2
      store b
      load b
      call bar(Sint) 1
      store _t0
      load _t0
      save
      clean_stack
      restore
      return_value
      label main()
      pushint 7
      call foo(Sint) 1
      store _t0
      load _t0
      call_builtin println(Sint) 1
      pushint 7
      call baz(Sint) 1
      store _t1
      load _t1
      call_builtin println(Sint) 1
      clean_stack
      return_void
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

        println(6 < 15)
      end
      ',
    expected => '
      label entry
      call main() 0
      exit
      label main()
      pushbool 1
      call_builtin println(Bool) 1
      pushbool 0
      call_builtin println(Bool) 1
      pushint 6
      pushint 15
      call_builtin lessthan(Sint,Sint) 2
      store _t0
      load _t0
      call_builtin println(Bool) 1
      clean_stack
      return_void
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
        a::Sint
        b::Bar
        c::String
      end

      fn main()
        let b = Bar("Hello")
        let f = Foo(4, b, "World")
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
      pushstr "Bar"
      call_builtin print(String) 1
      pushstr "{"
      call_builtin print(String) 1
      copyarg 0
      load_offset_str 0
      call_builtin print(String) 1
      pushstr "}"
      call_builtin print(String) 1
      clean_stack
      return_void
      label println(Bar)
      copyarg 0
      call print(Bar) 1
      call_builtin println() 0
      clean_stack
      return_void
      label Foo(Sint,Bar,String)
      alloc 3
      copyarg 2
      store_offset 0
      copyarg 1
      store_offset 1
      copyarg 0
      store_offset 2
      save
      clean_stack
      restore
      return_value
      label print(Foo)
      pushstr "Foo"
      call_builtin print(String) 1
      pushstr "{"
      call_builtin print(String) 1
      copyarg 0
      load_offset_sint 0
      call_builtin print(Sint) 1
      load_offset_ref 1
      call print(Bar) 1
      load_offset_str 2
      call_builtin print(String) 1
      pushstr "}"
      call_builtin print(String) 1
      clean_stack
      return_void
      label println(Foo)
      copyarg 0
      call print(Foo) 1
      call_builtin println() 0
      clean_stack
      return_void
      label main()
      pushstr "Hello"
      call Bar(String) 1
      store b
      pushint 4
      load b
      pushstr "World"
      call Foo(Sint,Bar,String) 3
      store f
      load f
      call println(Foo) 1
      clean_stack
      return_void
      Foo{4Bar{Hello}World}
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
  my $input = cleanup $case->{input};
  my $expected = cleanup $case->{expected};
  run $input, $expected;
}

say "test_backend_pancake_end_to_end successs";

