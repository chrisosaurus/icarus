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
        let b = Bar(1s)
        let f = Foo(2s, "Hello", b, 3s)
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
        let b = Baz(Bar(Foo(4s)))
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
            let f1 = Foo(6s)
            println(f1)

            let f2 = Foo(Bar(4s))
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
          return Shape(Rectangle(Point(14u, 10u), Point(20u, 24u)))
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
        let f = Foo(Bar(4s))

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
  {
    input => '
      type Foo
          i::Sint
      end

      type Bar
          f::Foo
      end

      type Baz
          b::Bar
      end

      fn main()
          let b = Baz(Bar(Foo(4s)))
          let t = b.b.f.i
          println(t)
      end
    ',
    expected => '
      4
    ',
  },
  {
    input => '
      union Cons
        u::Uint
        cons::Cons
      end

      fn main()
        let c = Cons(Cons(Cons(4u)))
        println(c)
      end
    ',
    expected => '
      Cons{Cons{Cons{4}}}
    ',
  },
  {
    input => '
      type Zero
      end

      union Succ
          zero::Zero
          succ::Succ
      end

      fn value(_::Zero) -> Uint
          return 0u
      end

      fn value(s::Succ) -> Uint
          match s
              case zero::Zero
                  return 1u + 0u
              case succ::Succ
                  return 1u + value(succ)
              end
          end
      end

      fn print(s::Succ)
          let v = value(s)
          print(v)
      end

      fn print(z::Zero)
          let v = value(z)
          print(v)
      end

      fn plus(a::Zero, _::Zero) -> Zero
          return a
      end

      fn plus(_::Zero, b::Succ) -> Succ
          return b
      end

      fn plus(a::Succ, _::Zero) -> Succ
          return a
      end

      fn plus(a::Succ, b::Succ) -> Succ
          match a
              case succ::Succ
                  return Succ(succ + b)
              case zero::Zero
                  return Succ(b)
              end
          end
      end

      fn main()
          let three = Succ(Succ(Succ(Zero())))
          let two = Succ(Succ(Zero()))
          let five = three + two
          println(five)
      end
    ',
    expected => '
      5
    ',
  },
  {
    input => '
      fn main()
          assert(False)
      end
    ',
    expected => '
      Panic!
      assertion failed
    ',
    failure => 1,
  },
  {
    input => '
      fn main()
          if True
              if True
                  let a = 10u
                  println(a)
              end
              let a = 20u
              println(a)
          end
          if True
              let a = 30u
              println(a)
          end
      end
    ',
    expected => '
      10
      20
      30
    '
  },
  {
    input => '
      fn main()
          let a = 4u
          begin
              println(a)
          end
          println(a)
      end
    ',
    expected => '
      4
      4
    ',
  },
  {
    input => '
      fn main()
        let a = "..."
        a = "hello "
        let b = a
        a = "world"
        print(b)
        println(a)
      end
    ',
    expected => '
      hello world
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
  my $expect_failure = shift // die;
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

  if( $output_2c ne $expected ){
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

  # run program through pancake and capture output
  my $output_pancake = `$path pancake -i $in_tmp_file`;
  $exit_status = $?;

  if( $output_pancake ne $expected ){
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

  `rm $in_tmp_file`;

  say "test_backends_output_equivalence successs";
  say "=======\nGot correct output:\n$expected";

}

for my $case (@$cases) {
  my $input = cleanup $case->{input} // die;
  my $expected = cleanup $case->{expected} // die;
  my $expect_failure = $case->{failure} // 0;
  run $input, $expected, $expect_failure;
}

say "test_backends_output_equivalence successs";

