#!/usr/bin/env perl
use strict;
use warnings;
use v5.10;

my $path = "bin/t/custom/test_lex_simple";

die "Could not find '$path'\n" unless -e $path;

my $output = `$path`;
my $exit_status = $?;
my $expected = <<EOF;

lexer output:
----------------
# user defined type with 2 fields, an Int and a String
type Foo
    a::Int
    b::String
end

fn d(i::Int)
    print(i)
end

fn d(s::String)
    print(s)
end

# break apart a Foo and call d on each field
fn d(f::Foo)
    d(f.a)
    d(f.b)
end

# simple function to test return values
fn add_one(i::Int) -> Int
    let tmp = i
    tmp = i + 1
    return tmp
end

# entry point for program
fn main()
    let f::Foo = Foo(add_one(1) "hello")

    d(f)
end

# hack to work around lack of constructors
builtin fn Foo(a::Int b::String) -> Foo

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

