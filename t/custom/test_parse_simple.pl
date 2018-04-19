#!/usr/bin/env perl
use strict;
use warnings;
use v5.10;

my $path = "./icarus";

die "Could not find '$path'\n" unless -e $path;

my $output = `$path parse example/simple.ic`;
my $exit_status = $?;
my $expected = <<EOF;

parser output:
----------------
type Foo
    a::Signed
    b::String
end

fn maybe_add_one(b::Bool, i::Signed) -> Signed
    if b
        return i + 1s
    else
        return i
    end
end

fn main() -> Unit
    let f::Foo = Foo(maybe_add_one(True, 1s), "hello")
    println(f)
    let x = 6s
    if x > 14s
        println("x > 14")
    else
        println("x <= 14")
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

say "test_parse_simple successs";

