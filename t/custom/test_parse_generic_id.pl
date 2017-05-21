#!/usr/bin/env perl
use strict;
use warnings;
use v5.10;

my $path = "./icarus";

die "Could not find '$path'\n" unless -e $path;

my $output = `$path parse example/generic_id.ic`;
my $exit_status = $?;
my $expected = <<EOF;

parser output:
----------------
# id[T](T)
fn id[T](t::T) -> T
    return t
end

type Foo
    a::Sint
end

# main()
fn main() -> Void
    println(id(10))
    println(id("Hello"))
    println(id(Foo(1)))
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

say "test_parse_generic_id successs";

