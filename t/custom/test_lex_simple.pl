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
# user defined type with 2 fields, an Sint and a String
type Foo
    a::Sint
    b::String
end

fn d(i::Sint)
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
fn add_one(i::Sint) -> Sint
    let tmp = i
    tmp = i + 1
    return tmp
end

fn maybe_add_one(i::Sint, b::Bool) -> Sint
    # FIXME this doesn't work `if i == 2 and b`
    # due to lack of operator precedence
    if b andi == 2
        i = i + 1
    end

    return i
end

# entry point for program
fn main()
    let f::Foo = Foo(add_one(1) "hello")

    f.a = maybe_add_one(f.a, True)

    d(f)
end

# hack to work around lack of constructors
builtin fn Foo(a::Sint, b::String) -> Foo

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

