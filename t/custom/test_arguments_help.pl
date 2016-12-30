#!/usr/bin/env perl
use strict;
use warnings;
use v5.10;

my $path = "./icarus";

die "Could not find '$path'\n" unless -e $path;

my $expected = <<EOF;
icarus [command] [options]

  command:
    check                          : perform static analysis on input file
    transform                      : transform input file
    2c                             : run 2c - compiles icarus code to C
    pancake                        : run pancake - the icarus interpreter
    <default>                      : defaults to `pancake` if a command is omitted

  options:
    --debug                        : enable icarus internal debug output
    --help,     -h                 : print this help message
    --input,    -i     file        : input filename
    --output,   -o     file        : output filename
    --version                      : print version of icarus
    <default>                      : an argument will default to being an `input` file if the preceding option is omitted

  examples:
    icarus foo.ic                  # run file `foo.ic` through `pancake`
    icarus check --input foo.ic    # perform static analysis on file `foo.ic`
    icarus 2c -i foo.ic -o foo.c   # compile `foo.ic` to c and output that in new file `foo.c`
    icarus pancake foo.ic --debug  # run `foo.ic` through pancake with debug output
    icarus -h                      # print this help page


EOF

my $output = `$path -h`;
my $exit_status = $?;

if( $output ne $expected ){
    say "Output was not as expected";
    say "=======\nExpected:\n$expected";
    say "=======\nGot:\n$output";
    say "=======\n";
    die "Output not as expected";
}

if( $exit_status != 256 ){
    die "Exit status was '$exit_status', expected 256 (1)";
}

say "test_arguments_version successs";

