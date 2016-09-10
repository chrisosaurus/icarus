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
      lexer output:
      ----------------

      fn main()
        println("Hello world")
      end
      ----------------


      parser output:
      ----------------
      # main()
      fn main() -> Void
          println("Hello world")
      end
      ----------------


      analyse output:
      ----------------
      warning: main implementation pending, icarus is currently only partially functional
      analysis complete
      ----------------


      transform output (PENDING):
      ----------------
      ----------------
      fn main() -> Void
          let _l1::String = "Hello world"
          println(_l1)
      end

      backend pancake selected (PENDING):
      Pancake bytecode:
      ==========================
      label entry
      call main() 0
      exit
      label main()
      pushstr "Hello world"
      call_builtin println(String) 1
      clean_stack
      return_void
      ==========================

      Pancake interpreter output
      ==========================
      Hello world
      '
  },

  {
    input => '
      fn bar() -> Sint
        return 14
      end

      fn foo() -> Sint
        return bar()
      end

      fn main()
        println(foo())
      end
      ',
    expected => '
      lexer output:
      ----------------

      fn bar() -> Sint
        return 14
      end

      fn foo() -> Sint
        return bar()
      end

      fn main()
        println(foo())
      end
      ----------------


      parser output:
      ----------------
      # bar()
      fn bar() -> Sint
          return 14
      end

      # foo()
      fn foo() -> Sint
          return bar()
      end

      # main()
      fn main() -> Void
          println(foo())
      end
      ----------------


      analyse output:
      ----------------
      warning: main implementation pending, icarus is currently only partially functional
      analysis complete
      ----------------


      transform output (PENDING):
      ----------------
      ----------------
      fn bar() -> Sint
          let _l1::Sint = 14
          return _l1
      end
      fn foo() -> Sint
          let _t1::Sint = bar()
          return _t1
      end
      fn main() -> Void
          let _t1::Sint = foo()
          println(_t1)
      end

      backend pancake selected (PENDING):
      Pancake bytecode:
      ==========================
      label entry
      call main() 0
      exit
      label bar()
      pushint 14
      save
      clean_stack
      restore
      return_value
      clean_stack
      return_void
      label foo()
      call bar() 0
      store _t1
      load _t1
      save
      clean_stack
      restore
      return_value
      clean_stack
      return_void
      label main()
      call foo() 0
      store _t1
      load _t1
      call_builtin println(Sint) 1
      clean_stack
      return_void
      ==========================

      Pancake interpreter output
      ==========================
      14
      ',
  },
];

# whitespace sensitivity sucks
sub cleanup {
  my $str = shift // die;
  die if @_;

  # strip up to 6 leading (horizontal whitespace) spaces
  $str =~ s/^\h{0,6}//mg;

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

  my $output = `$path $in_tmp_file pancake`;
  my $status = $?;

  `rm $in_tmp_file`;

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

