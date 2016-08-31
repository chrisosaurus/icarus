#!/usr/bin/env perl
use strict;
use warnings;
use v5.10;
use IPC::Open3;

my $path = "bin/t/custom/test_backend_pancake_interpret_instructions";

die "Could not find '$path'\n" unless -e $path;

my $input = <<EOF;
label dummy
pushuint 6
pushuint 7
call_builtin plus(Uint,Uint) 2
exit
EOF

my $expected = <<EOF;
uint: 13
EOF

my $pid = open3(my $write, my $read, my $error, "$path") or die "Failed to open $path";
print $write $input;
close $write;
waitpid($pid, 0);
my $output = "";
{
    local $/ = undef;
    $output = <$read>;
}

if( $output ne $expected ){
    say "Output was not as expected";
    say "=======\nExpected:\n$expected";
    say "=======\nGot:\n$output";
    say "=======\n";
    die "Output not as expected";
}

say "test_backend_pancake_interpret_simple successs";
say "=======\nGot correct output:\n$output";

