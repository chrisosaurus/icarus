#!/usr/bin/env perl
use strict;
use warnings;
use v5.10;

my $path = "bin/t/custom/test_analyse_current";

die "Could not find '$path'\n" unless -e $path;

my $output = `$path`;

my $exit_status = $?;
if( $exit_status != 0 ){
    die "exit_status was '$exit_status', expected 0";
}

my $expected = <<EOF;
warning: ic_analyse_body: implementation pending
warning: ic_analyse_body: implementation pending
warning: ic_analyse_body: implementation pending
warning: ic_analyse_body: implementation pending
warning: ic_analyse_body: implementation pending
warning: ic_analyse_body: implementation pending
warning: ic_analyse_body: implementation pending
warning: ic_analyse_body: implementation pending
(partial) analyse was a success
EOF

unless( $output eq $expected ){
    say "Output was not as expected";
    say "=======\nExpected:\n$expected";
    say "=======\nGot:\n$output";
    say "=======\n";
    die "Output not as expected";
}

say "test_parse_example: successs";

