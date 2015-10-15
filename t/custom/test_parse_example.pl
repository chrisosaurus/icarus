#!/usr/bin/env perl
use strict;
use warnings;
use v5.10;

my $path = "bin/t/custom/test_parse_example";

die "Could not find '$path'\n" unless -e $path;

my $output = `$path`;

my $exit_status = $?;
if( $exit_status != 0 ){
    die "exit_status was '$exit_status', expected 0";
}

my $expected = <<EOF;

parser output:
----------------
type Foo
    a::Int
    b::String
end

# d(Int)
fn d(i::Int) -> Void
    print(i)
end

# d(String)
fn d(s::String) -> Void
    print(s)
end

# d(Foo)
fn d(f::Foo) -> Void
    d(f . a)
    d(f . b)
end

# add_one(Int)
fn add_one(i::Int) -> Int
    return i + 1
end

# main()
fn main() -> Void
    let f::Foo = Foo(add_one(1) "hello")
    d(f)
end

# print(String)
fn print(s::String) -> Void
end

# print(Int)
fn print(i::Int) -> Void
end
----------------

EOF

unless( $output eq $expected ){
    say "Output was not as expected";
    say "=======\nExpected:\n$expected";
    say "=======\nGot:\n$output";
    say "=======\n";
    die "Output not as expected";
}

say "test_parse_example: successs";

