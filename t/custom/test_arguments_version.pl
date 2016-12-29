#!/usr/bin/env perl
use strict;
use warnings;
use v5.10;

my $path = "./icarus";

die "Could not find '$path'\n" unless -e $path;

my $expected = <<EOF;
Icarus version: pre-alpha
EOF

my $output = `$path --version`;
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

