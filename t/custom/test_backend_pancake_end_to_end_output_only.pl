#!/usr/bin/env perl
use strict;
use warnings;
use v5.10;
use IPC::Open3;

my $path = "bin/t/custom/test_backend_pancake_end_to_end_output_only";

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
      18
      17
      ',
  },
  {
    input => '
      fn main()
        println(True)
        println(False)
        println(6 < 15)
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
        if 4 < 5
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
        if 4 < 5
          println("4 < 5")
        end
        if 4 >= 5
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

  my $output = `$path $in_tmp_file`;
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

