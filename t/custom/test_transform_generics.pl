#!/usr/bin/env perl
use strict;
use warnings;
use v5.10;

my $path = "./icarus transform example/generics.ic";

my $output = `$path`;
my $exit_status = $?;
my $expected = <<EOF;
type Foo[T::Sint]
    t::Sint
end
union Bar[T::Uint]
    t::Uint
end
fn main() -> Unit
    let _l0::Sint = 6s
    let f::Foo[Sint] = Foo(_l0)
    let f2::Foo[Sint] = f
    let _l1::Sint = 10s
    let _t0::Sint = id(_l1)
    println(_t0)
    let _l2::String = "Hello"
    let _t1::String = id(_l2)
    println(_t1)
    let _l3::Sint = 1s
    let _t2::Foo[Sint] = Foo(_l3)
    let _t3::Foo[Sint] = id(_t2)
    println(_t3)
    let _l4::Uint = 1u
    let _t4::Bar[Uint] = Bar(_l4)
    let _t5::Bar[Uint] = id(_t4)
    println(_t5)
end
fn id[T::Sint](t::Sint) -> Sint
    return t
end
fn id[T::String](t::String) -> String
    return t
end
fn id[T::Foo](t::Foo) -> Foo
    return t
end
fn id[T::Bar](t::Bar) -> Bar
    return t
end
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

say "test_transform_generics successs";

