#!/usr/bin/env perl
use strict;
use warnings;
use v5.10;

my $path = "./icarus";

die "Could not find '$path'\n" unless -e $path;

my $expected = <<EOF;
Foo{a:2, b:hello}
x <= 14
EOF

# using mktemp here just to get name
my $out_tmp_file = `mktemp TESTING_BACKEND_2C_HELLO_XXX.c`;
# have to remove right away, as icarus 2c wants to create
`rm $out_tmp_file`;

# compile .ic to .c
my $output = `$path 2c -i example/simple.ic -o $out_tmp_file`;
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

`rm $out_tmp_file`;

if( $exit_status != 0 ){
  say "cc exit status was not as expected";
  say "=======\nGot:\n$output";
  die "exit_status was '$exit_status', expected 0";
}

# finally run program
$output = `./a.out`;

`rm a.out`;

if( $exit_status != 0 || $output ne $expected ){
    say "Output was not as expected";
    say "=======\nExpected:\n$expected";
    say "=======\nGot:\n$output";
    say "=======\n";
    if( $exit_status != 0 ){
        die "exit_status was '$exit_status', expected 0";
    } else {
        die "Output not as expected";
    }
}

say "test_backend_2c_simple successs";
say "=======\nGot correct output:\n$output";

