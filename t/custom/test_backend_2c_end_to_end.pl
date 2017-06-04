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
      type Foo
        a::Sint
        b::String
      end

      fn main()
        let f = Foo(6s, "Hello")
        println(f)
      end
      ',
    expected_c =>'
      #include "backends/2c/builtins.c"
      typedef struct Foo * Foo;
      Foo i_Foo_a_Sint_String(Sint a, String b);
      void i_print_a_Foo(Foo f);
      void i_println_a_Foo(Foo f);
      /* main() -> Void */
      Void i_main_a();
      struct Foo {
        Sint a;
        String b;
      };
      Foo i_Foo_a_Sint_String(Sint a, String b) {
        Foo tmp = ic_alloc(sizeof(struct Foo));
        tmp->a = a;
        tmp->b = b;
        return tmp;
      }
      void i_print_a_Foo(Foo f){
        fputs("Foo{", stdout);
        i_print_a_Sint(f->a);
        fputs(", ", stdout);
        i_print_a_String(f->b);
        fputs("}", stdout);
      }
      void i_println_a_Foo(Foo f){
        i_print_a_Foo(f);
        i_println_a();
      }
      /* main() -> Void */
      Void i_main_a(){
        Sint _l0 = ic_sint_new(6);
        String _l1 = ic_string_new("Hello", 5);
        Foo f = i_Foo_a_Sint_String(_l0, _l1);
        i_println_a_Foo(f);
      }
      #include "backends/2c/entry.c"
    ',
    expected_output => '
      Foo{6, Hello}
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
    expected_c =>'
      #include "backends/2c/builtins.c"
      enum Foo_tag;
      typedef struct Foo * Foo;
      Foo i_Foo_a_Sint(Sint a);
      Foo i_Foo_a_String(String b);
      void i_print_a_Foo(Foo f);
      void i_println_a_Foo(Foo f);
      /* main() -> Void */
      Void i_main_a();
      enum Foo_tag {
        Foo_tag_Sint_a,
        Foo_tag_String_b,
      };
      struct Foo {
        enum Foo_tag _tag;
        union {
          Sint a;
          String b;
        } u;
      };
      Foo i_Foo_a_Sint(Sint a){
        Foo tmp = ic_alloc(sizeof(struct Foo));
        tmp->_tag = Foo_tag_Sint_a;
        tmp->u.a = a;
        return tmp;
      }
      Foo i_Foo_a_String(String b){
        Foo tmp = ic_alloc(sizeof(struct Foo));
        tmp->_tag = Foo_tag_String_b;
        tmp->u.b = b;
        return tmp;
      }
      void i_print_a_Foo(Foo f){
        fputs("Foo{", stdout);
        switch (f->_tag) {
          case Foo_tag_Sint_a:
            i_print_a_Sint(f->u.a);
            break;
          case Foo_tag_String_b:
            i_print_a_String(f->u.b);
            break;
          default:
            panic("impossible tag on Foo");
        }
        fputs("}", stdout);
      }
      void i_println_a_Foo(Foo f){
        i_print_a_Foo(f);
        i_println_a();
      }
      /* main() -> Void */
      Void i_main_a(){
        Sint _l0 = ic_sint_new(5);
        Foo a = i_Foo_a_Sint(_l0);
        i_println_a_Foo(a);
        String _l1 = ic_string_new("Hello", 5);
        Foo b = i_Foo_a_String(_l1);
        i_println_a_Foo(b);
      }
      #include "backends/2c/entry.c"
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
  my $expected_c = shift // die;
  my $expected_output = shift // die;
  die if @_;

  my $in_tmp_file = `mktemp TESTING_BACKEND_2C_END_TO_END_XXX.ic`;
  chomp($in_tmp_file);
  open( my $fh, ">", "$in_tmp_file" ) or die "failed to open tmp file '$in_tmp_file'";
  print $fh $input;
  close $fh;

  # using mktemp here just to get name
  my $out_tmp_file = `mktemp TESTING_BACKEND_2C_END_TO_END_XXX.c`;
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

  if( $exit_status != 0 ){
    say "cc exit status was not as expected";
    say "=======\nGot:\n$output";
    die "exit_status was '$exit_status', expected 0";
  }

  my $generated_c = `cat $out_tmp_file`;
  `rm $out_tmp_file`;

  if ( $expected_c ne $generated_c ){
      say "Generated c was not as expected";
      say "=======\nExpected:\n$expected_c";
      say "=======\nGot:\n$generated_c";
      say "=======\n";
      die "Generated c was not as expected";
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

  say "test_backend_pancake_end_to_end successs";
  say "=======\nGot correct output:\n$output";

}

for my $case (@$cases) {
  my $input = cleanup $case->{input};
  my $expected_c = cleanup $case->{expected_c} // die;
  my $expected_output = cleanup $case->{expected_output} // die;
  run $input, $expected_c, $expected_output;
}

say "test_backend_pancake_end_to_end successs";

