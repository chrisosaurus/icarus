#!/usr/bin/env perl
use strict;
use warnings;
use v5.10;

my $path = "bin/t/custom/test_parse_simple";

die "Could not find '$path'\n" unless -e $path;

my $output = `$path`;
my $exit_status = $?;
my $expected = <<EOF;

parser output:
----------------
type Foo
    a::Sint
    b::String
end

# d(Sint)
fn d(i::Sint) -> Void
    print(i)
end

# d(String)
fn d(s::String) -> Void
    print(s)
end

# d(Foo)
fn d(f::Foo) -> Void
    d(f.a)
    d(f.b)
end

# add_one(Sint)
fn add_one(i::Sint) -> Sint
    let tmp = i
    tmp = i + 1
    return tmp
end

# maybe_add_one(Sint Bool)
fn maybe_add_one(i::Sint, b::Bool) -> Sint
    if b and i == 2
        i = i + 1
    end
    return i
end

# main()
fn main() -> Void
    let f::Foo = Foo(add_one(1), "hello")
    f.a = maybe_add_one(f.a, True)
    d(f)
end

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

say "test_parse_example: successs";

