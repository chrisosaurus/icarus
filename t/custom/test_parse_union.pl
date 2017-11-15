#!/usr/bin/env perl
use strict;
use warnings;
use v5.10;

my $path = "./icarus";

die "Could not find '$path'\n" unless -e $path;

my $output = `$path parse example/union.ic`;
my $exit_status = $?;
my $expected = <<EOF;

parser output:
----------------
union Foo
    a::Sint
    b::String
end

fn main() -> Unit
    let f = Foo("Hello")
    match f
        case a::Sint
            println(a)
        case b::String
            println(b)
        end
    end
end
----------------

EOF

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

say "test_parse_union successs";

