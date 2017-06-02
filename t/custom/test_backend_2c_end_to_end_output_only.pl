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
    expected_output => '
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
    expected_output => '
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
    expected_output => '
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
    expected_output => '
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
    expected_output => '
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
    expected_output => '
      Hello there Jennifer, very nice to meet you
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
    expected_output => '
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
      type Foo
        i::Sint
        b::Bar
      end

      type Bar
        s::String
      end

      fn main()
        let f = Foo(16s, Bar("Hello!!!"))
        println(f.i)
        println(f.b.s)
      end
    ',
    expected_output => '
      16
      Hello!!!
    '
  },
  {
    input =>'
      union Foo
          a::Sint
          b::String
      end

      fn main()
          let a = Foo(5s)
          println(a)
          let b = Foo("Hello")
          println(b)
      end
    ',
    expected_output =>'
      Foo{5}
      Foo{Hello}
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
  my $expected_output = shift // die;
  die if @_;

  my $in_tmp_file = `mktemp TESTING_BACKEND_2C_END_TO_END_OUTPUT_ONLY_XXX.ic`;
  chomp($in_tmp_file);
  open( my $fh, ">", "$in_tmp_file" ) or die "failed to open tmp file '$in_tmp_file'";
  print $fh $input;
  close $fh;

  # using mktemp here just to get name
  my $out_tmp_file = `mktemp TESTING_BACKEND_2C_END_TO_END_OUTPUT_ONLY_XXX.c`;
  # have to remove right away, as icarus 2c wants to create
  `rm $out_tmp_file`;

  # compile .ic to .c
  my $output = `$path 2c -i $in_tmp_file -o $out_tmp_file`;
  my $exit_status = $?;

  `rm $in_tmp_file`;

  if( $exit_status != 0 ){
    `rm $out_tmp_file`;
    say "Icarus 2c exit status was not as expected";
    say "=======\nGot:\n$output";
    die "exit_status was '$exit_status', expected 0";
  }

  # compile .c to a.out
  $output = `cc $out_tmp_file`;
  $exit_status = $?;

  `rm $out_tmp_file`;

  if( $exit_status != 0 ){
    say "cc exit status was not as expected";
    say "=======\nGot:\n$output";
    die "exit_status was '$exit_status', expected 0";
  }

  # finally run program
  $output = `./a.out`;

  `rm a.out`;

  if( $exit_status != 0 || $output ne $expected_output ){
      say "Output was not as expected";
      say "=======\nExpected:\n$expected_output";
      say "=======\nGot:\n$output";
      say "=======\n";
      if( $exit_status != 0 ){
          die "exit_status was '$exit_status', expected 0";
      } else {
          die "Output not as expected";
      }
  }

  say "test_backend_2c_end_to_end_output_only successs";
  say "=======\nGot correct output:\n$output";

}

for my $case (@$cases) {
  my $input = cleanup $case->{input};
  my $expected_output = cleanup $case->{expected_output};
  run $input, $expected_output;
}

say "test_backend_pancake_end_to_end_output_only successs";

