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

# print(Foo)
fn print(f::Foo) -> Void
    print(f.a)
    print(f.b)
end

# maybe_add_one(Bool,Sint)
fn maybe_add_one(b::Bool, i::Sint) -> Sint
    if b == True
        return i + 1
    else
        return i
    end
end

# main()
fn main() -> Void
    let f::Foo = Foo(maybe_add_one(True, 1), "hello")
    print(f)
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

say "test_parse_example: successs";

