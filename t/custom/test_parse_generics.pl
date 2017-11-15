#!/usr/bin/env perl
use strict;
use warnings;
use v5.10;

my $path = "./icarus";

die "Could not find '$path'\n" unless -e $path;

my $output = `$path parse example/generics.ic`;
my $exit_status = $?;
my $expected = <<EOF;

parser output:
----------------
fn id[T](t::T) -> T
    return t
end

type Foo[T]
    t::T
end

union Bar[T]
    t::T
end

fn print[T](f::Foo[T]) -> Unit
    print(f.t)
end

fn main() -> Unit
    let f = Foo[Sint](6s)
    let f2::Foo[Sint] = f
    println(id[Sint](10s))
    println(id[String]("Hello"))
    println(id[Foo[Sint]](Foo[Sint](1s)))
    println(id[Bar[Uint]](Bar[Uint](1u)))
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

say "test_parse_generics successs";

