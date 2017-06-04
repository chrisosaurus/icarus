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
      Hello world
      '
  },

  {
    input => '
      fn bar(a::Sint) -> Sint
        return 7s + a
      end

      fn foo(a::Sint) -> Sint
        return bar(a + 4s)
      end

      fn baz(a::Sint) -> Sint
        let b = a + 3s
        return bar(b)
      end

      fn main()
        println(foo(7s))
        println(baz(7s))
      end
      ',
    expected => '
      18
      17
      ',
  },
  {
    input => '
      fn main()
        println(True)
        println(False)
        println(6s < 15s)
      end
      ',
    expected => '
      True
      False
      True
      ',
  },
  {
    input => '
      fn main()
        if 4s < 5s
          println("4 < 5")
        else
          println("4 >= 5")
        end
      end
      ',
    expected => '
      4 < 5
      ',
  },
  {
    input => '
      fn main()
        if 4s < 5s
          println("4 < 5")
        end
        if 4s >= 5s
          println("4 >= 5")
        end
      end
      ',
    expected => '
      4 < 5
      ',
  },
  {
    input => '
      fn get_str(name::String) -> String
          return concat(concat("Hello there ", name), ", very nice to meet you")
      end

      fn main() -> Void
          println(get_str("Jennifer"))
      end
      ',
    expected => '
      Hello there Jennifer, very nice to meet you
      ',
  },
  {
    input => '
      fn foo(i::Sint) -> Sint
        return i + 1s
      end

      fn main()
        let a = 5s
        a = foo(a)
        println(a)
      end
      ',
    expected => '
      6
      ',
  },
  {
    input => '
      fn foo(a::Sint, b::Sint, c::Sint)
        println(a)
        println(b)
        println(c)
      end

      fn main()
        foo(4s, 5s, 6s)
      end
      ',
    expected => '
      4
      5
      6
      ',
  },
  {
    input => '
      fn is_div(a::Sint, b::Sint) -> Bool
          let rem = a % b
          return rem == 0s
      end

      fn fizzer(num::Sint)
          let str = ""

          if is_div(num, 3s)
              str = concat(str, "Fizz")
          end

          if is_div(num, 5s)
              str = concat(str, "Buzz")
          end

          if length(str) == 0s
              println(num)
          else
              println(str)
          end
      end

      fn fizzbuzz(from::Sint, to::Sint)
        if from < to
          fizzer(from)
          from = plus(from, 1s)
          fizzbuzz(from, to)
        end
      end

      fn main()
          # icarus currently lacks for loops and ranges
          # so this is a poor-mans fizzbuzz-derived demo
          fizzbuzz(1s, 20s)
      end
      ',
    expected => '
      1
      2
      Fizz
      4
      Buzz
      Fizz
      7
      8
      Fizz
      Buzz
      11
      Fizz
      13
      14
      FizzBuzz
      16
      17
      Fizz
      19
      '
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
        let f = Foo(4s, b, "World")
        println(f)
      end
    ',
    expected => '
      Foo{4, Bar{Hello}, World}
    ',
  },
];

# whitespace sensitivity sucks
sub cleanup {
  my $str = shift // die;
  die if @_;

  # strip first blank line
  $str =~ s/^\n//;

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

  my $output = `$path pancake $in_tmp_file`;
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
  my $input = cleanup $case->{input} // die;
  my $expected = cleanup $case->{expected} // die;
  run $input, $expected;
}

say "test_backend_pancake_end_to_end successs";

