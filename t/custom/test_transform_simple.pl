#!/usr/bin/env perl
use strict;
use warnings;
use v5.10;

my $path = "./icarus transform example/simple.ic";

my $output = `$path`;
my $exit_status = $?;
my $expected = <<EOF;
fn println(f::Foo) -> Void
    let _l0::String = "Foo{"
    print(_l0)
    let _l1::String = "a:"
    print(_l1)
    let _t0::Sint = f.a
    print(_t0)
    let _l2::String = ", b:"
    print(_l2)
    let _t1::String = f.b
    print(_t1)
    let _l3::String = "}"
    println(_l3)
end
fn maybe_add_one(b::Bool, i::Sint) -> Sint
    let _l0::Bool = True
    let _t0::Bool = equal(b, _l0)
    if _t0
        let _l1::Sint = 1
        let _t1::Sint = plus(i, _l1)
        return _t1
    else
        return i
    end
end
fn main() -> Void
    let _l0::Bool = True
    let _l1::Sint = 1
    let _t0::Sint = maybe_add_one(_l0, _l1)
    let _l2::String = "hello"
    let f::Foo = Foo(_t0, _l2)
    println(f)
    let x::Sint = 6
    let _l3::Sint = 14
    let _t1::Bool = greaterthan(x, _l3)
    if _t1
        let _l4::String = "x > 14"
        println(_l4)
    else
        let _l5::String = "x <= 14"
        println(_l5)
    end
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

say "test_transform_simple successs";

