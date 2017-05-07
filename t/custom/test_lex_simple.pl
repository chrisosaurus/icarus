#!/usr/bin/env perl
use strict;
use warnings;
use v5.10;

my $path = "./icarus";

die "Could not find '$path'\n" unless -e $path;

my $output = `$path lex example/simple.ic`;
my $exit_status = $?;
my $expected = <<EOF;

lexer output:
----------------
# user defined type with 2 fields, a Sint and a String
type Foo
    a::Sint
    b::String
end

# simple function to test return values and branching
fn maybe_add_one(b::Bool, i::Sint) -> Sint
    if b
        return i+1
    else
        return i
    end
end

# entry point for program
fn main()
    let f::Foo = Foo(maybe_add_one(True, 1), "hello")
    println(f)

    let x = 6
    if x > 14
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

say "test_lex_simple successs";

