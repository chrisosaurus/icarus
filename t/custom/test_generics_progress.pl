#!/usr/bin/env perl
use strict;
use warnings;
use v5.10;
use IPC::Open3;

my $path = "./icarus";

die "Could not find '$path'\n" unless -e $path;

my $cases = [
  {
    # test that we are able to parse generics
    # not an error as we do not instantiate them
    input => '
      fn id[A](a::A) -> A
          return a
      end

      type Nothing
      end

      union Maybe[T]
          something::T
          nothing::Nothing
      end

      fn main()
      end
    ',
    expected => '
    ',
  },
  {
    # milestone 10
    input => '
      fn id[T](t::T) -> T
          return t
      end

      fn main()
          println(id[Sint](10s))
      end
    ',
    expected => "
      10
    ",
    failure => 0,
  },
  {
    # milestone 11
    input => '
      type Nothing
      end

      union Maybe[T]
          something::T
          nothing::Nothing
      end

      fn main()
          let m = Maybe[Sint](6s)

          println(m)

          match m
              case something::Sint
                  print("Found a Sint: ")
                  println(something)
              case nothing::Nothing
                  println("Nothing to see here")
              end
          end
      end
    ',
    expected => "
      Maybe[Sint]{6}
      Found a Sint: 6
    ",
    failure => 0,
  },
  {
    input => '
      type Nothing
      end

      union Maybe[T]
          something::T
          nothing::Nothing
      end

      fn maybe_id[T](t::Maybe[T]) -> Maybe[T]
        return t
      end

      fn flatten[T](m::Maybe[Maybe[T]]) -> Maybe[T]
        match m
          case something::Maybe[T]
            return someting
          case nothing::Nothing
            return Maybe[T](nothing)
          end
        end
      end

      fn boxup[T](t::T) -> Maybe[T]
        return Maybe[T](t)
      end

      fn main()
        let m = Maybe[Maybe[Sint]]( Maybe[Sint](6s) )
        println(m)

        let n = flatten[Sint](m)
        println(n)

        match n
          case something::Sint
            print("My Maybe contained: ")
            println(something)
          case nothing::Nothing
            println("My Maybe contained: Nothing")
          end
        end

        let o = boxup[String]("Hello Maybe world")
        println(o)
      end
      ',
    expected => '
      Maybe[Maybe[Sint]]{6}
      Maybe[Sint]{6}
      My Maybe contained: 6s
      Maybe[String]{"Hello Maybe world"}
    ',
    failure => 1,
  }
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

sub run_2c {
  my $input = shift // die;
  my $expected = shift // die;
  my $expect_failure = shift // die;
  my $in_tmp_file = shift // die;
  die if @_;

  # using mktemp here just to get name
  my $out_tmp_file = `mktemp TESTING_GENERICS_PROGRESS_XXX.c`;
  # have to remove right away, as icarus 2c wants to create
  `rm $out_tmp_file`;

  # compile .ic to .c
  my $output = `$path 2c -i $in_tmp_file -o $out_tmp_file`;
  my $exit_status = $?;

  if( $expect_failure == 1 && $exit_status != 0 ){
    # failed as expected, okay
    `rm -f $out_tmp_file`;
    return;
  }

  if( $exit_status != 0 ){
    `rm $out_tmp_file`;
    say "Icarus 2c exit status was not as expected";
    say "=======\nGot:\n$output";
    die "exit_status was '$exit_status', expected 0";
  }

  # compile .c to a.out
  $output = `cc $out_tmp_file`;
  $exit_status = $?;

  # compiling the C should never fail, regardless of expectation
  if( $exit_status != 0 ){
    say "cc exit status was not as expected";
    say "=======\nGot:\n$output";
    die "exit_status was '$exit_status', expected 0";
  }

  `rm $out_tmp_file`;

  # finally run program
  my $output_2c = `./a.out`;
  $exit_status = $?;

  `rm a.out`;

  if( $output_2c ne $expected ){
      if( $expect_failure == 1 ){
        # failed as expected, okay
        return;
      }
      say "2c output was not as expected";
      say "=======\nExpected:\n$expected";
      say "=======\nGot:\n$output_2c";
      say "=======\n";
      say "got exit code '$exit_status'";
      die "Output not as expected";
  }

  if( $expect_failure == 1 && $exit_status == 0 ){
    say "2c expected failure but was success";
    die "2c expected failure but was success";
  }

  if( $expect_failure == 0 && $exit_status != 0 ){
      die "2c exit_status was '$exit_status', expected 0";
  }
}

sub run_pancake {
  my $input = shift // die;
  my $expected = shift // die;
  my $expect_failure = shift // die;
  my $in_tmp_file = shift // die;
  die if @_;

  # run program through pancake and capture output
  my $output_pancake = `$path pancake -i $in_tmp_file`;
  my $exit_status = $?;

  if( $output_pancake ne $expected ){
      if( $expect_failure == 1 ){
        # failed as expected, okay
        return;
      }
      say "Pancake output was not as expected";
      say "=======\nExpected:\n$expected";
      say "=======\nGot:\n$output_pancake";
      say "=======\n";
      say "got exit code '$exit_status'";
      die "Output not as expected";
  }

  if( $expect_failure == 1 && $exit_status == 0 ){
  say "pancake expected failure but was success";
  die "pancake expected failure but was success";
  }

  if( $expect_failure == 0 && $exit_status != 0 ){
  die "pancake exit_status was '$exit_status', expected 0";
  }
}

sub run {
    my $input = shift // die;
    my $expected = shift // die;
    my $expect_failure = shift // die;
    die if @_;

    my $in_tmp_file = `mktemp TESTING_GENERICS_PROGRESS_XXX.ic`;
    chomp($in_tmp_file);
    open( my $fh, ">", "$in_tmp_file" ) or die "failed to open tmp file '$in_tmp_file'";
    print $fh $input;
    close $fh;

    run_2c $input, $expected, $expect_failure, $in_tmp_file;
    run_pancake $input, $expected, $expect_failure, $in_tmp_file;

    `rm $in_tmp_file`;

    say "test_generics_progress successs";
    say "=======\nGot correct output" . ($expect_failure && " (expected failure)" || '') . ":\n$expected";
}

for my $case (@$cases) {
  my $input = cleanup $case->{input} // die;
  my $expected = cleanup $case->{expected} // die;
  my $expect_failure = $case->{failure} // 0;
  run $input, $expected, $expect_failure;
}

say "test_generics_progress successs";

