#!/usr/bin/env perl
use strict;
use warnings;
use v5.10;

my $path = "./icarus";

die "Could not find '$path'\n" unless -e $path;

my $expected = <<EOF;
      icarus [command] [options]

        command:
          check          : perform static analysis on input file
          transform      : transform input file
          2c             : pass input file to backend 2c
          pancake        : (default) pass input file to backend pancake
          <default>      : defaults to `pancake`

        options:
          --output, -o   : output filename
          --input,  -i   : input filename
          --debug enable : icarus compiler debug options
          --version      : print version of icarus compiler
          --help, -h     : print this help message
          <default>      : will try to interpret as an input file, if none is already specified

        examples:
          icarus foo.ic                # run file `foo.ic` through `pancake`
          icarus check --input foo.ic  # perform static analysis on file `foo.ic`
          icarus 2c -i foo.ic -o foo.c # compile `foo.ic` to c and output that in new file `foo.c`


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

