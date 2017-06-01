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
      type Bar
        a::Sint
      end

      type Foo
        a::Sint
        b::String
        c::Bar
        d::Sint
      end

      fn work()
        println(concat("Hello", " world"))
      end

      fn main()
        let b = Bar(1)
        let f = Foo(2, "Hello", b, 3)
        println(f)
        work()
      end
    ',
    expected => '
      Foo{2, Hello, Bar{1}, 3}
      Hello world
    '
  },
  {
    input => '
      type Foo
        a::Sint
      end

      type Bar
        f::Foo
      end

      type Baz
        b::Bar
      end

      fn main()
        let b = Baz(Bar(Foo(4)))
        println(b)
        println(b.b.f.a)
      end
    ',
    expected => '
      Baz{Bar{Foo{4}}}
      4
    ',
  },
  {
    input => '
        type Bar
            a::Sint
        end

        union Foo
            a::Sint
            b::String
            c::Bar
        end

        fn main()
            let f1 = Foo(6)
            println(f1)

            let f2 = Foo(Bar(4))
            println(f2)
        end
    ',
    expected => '
      Foo{6}
      Foo{Bar{4}}
    ',
  },
  {
    input => '
      type Point
          x::Uint
          y::Uint
      end

      type Square
          topleft::Point
          width::Uint
      end

      type Rectangle
          topleft::Point
          bottomright::Point
      end

      type Circle
          center::Point
          radius::Uint
      end

      union Shape
          circle::Circle
          square::Square
          rectangle::Rectangle
      end

      fn generate_shape() -> Shape
          return Shape(Rectangle(Point(Uint(14), Uint(10)), Point(Uint(20), Uint(24))))
      end

      fn main()
          let s = generate_shape()
          println(s)
      end
    ',
    expected => '
      Shape{Rectangle{Point{14, 10}, Point{20, 24}}}
    ',
  },
  {
    input => '
      union Foo
        a::Sint
        b::String
        c::Bar
      end

      type Bar
        b::Sint
      end

      fn main()
        let f = Foo(Bar(4))

        match f
          else
            println(f)
          end
        end

        match f
          case c::Bar
            println(c)
            println(c.b)
          else
            println("NOPE")
          end
        end
      end
    ',
    expected => '
      Foo{Bar{4}}
      Bar{4}
      4
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

  my $in_tmp_file = `mktemp TESTING_BACKENDS_OUTPUT_EQUIVALENCE_XXX.ic`;
  chomp($in_tmp_file);
  open( my $fh, ">", "$in_tmp_file" ) or die "failed to open tmp file '$in_tmp_file'";
  print $fh $input;
  close $fh;

  # using mktemp here just to get name
  my $out_tmp_file = `mktemp TESTING_BACKENDS_OUTPUT_EQUIVALENCE_XXX.c`;
  # have to remove right away, as icarus 2c wants to create
  `rm $out_tmp_file`;

  # compile .ic to .c
  my $output = `$path 2c -i $in_tmp_file -o $out_tmp_file`;
  my $exit_status = $?;


  if( $exit_status != 0 ){
    `rm $out_tmp_file`;
    say "Icarus 2c exit status was not as expected";
    say "=======\nGot:\n$output";
    die "exit_status was '$exit_status', expected 0";
  }

  # compile .c to a.out
  $output = `cc $out_tmp_file`;
  $exit_status = $?;

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

  if( $exit_status != 0 || $output_2c ne $expected ){
      say "2c output was not as expected";
      say "=======\nExpected:\n$expected";
      say "=======\nGot:\n$output_2c";
      say "=======\n";
      if( $exit_status != 0 ){
          die "exit_status was '$exit_status', expected 0";
      } else {
          die "Output not as expected";
      }
  }

  # run program through pancake and capture output
  my $output_pancake = `$path pancake -i $in_tmp_file`;
  $exit_status = $?;

  if( $exit_status != 0 || $output_pancake ne $expected ){
      say "Pancake output was not as expected";
      say "=======\nExpected:\n$expected";
      say "=======\nGot:\n$output_pancake";
      say "=======\n";
      if( $exit_status != 0 ){
          die "exit_status was '$exit_status', expected 0";
      } else {
          die "Output not as expected";
      }
  }

  `rm $in_tmp_file`;

  say "test_backends_output_equivalence successs";
  say "=======\nGot correct output:\n$expected";

}

for my $case (@$cases) {
  my $input = cleanup $case->{input};
  my $expected = cleanup $case->{expected};
  run $input, $expected;
}

say "test_backends_output_equivalence successs";

