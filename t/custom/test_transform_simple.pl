#!/usr/bin/env perl
use strict;
use warnings;
use v5.10;

my $path = "bin/t/custom/test_transform_simple";

die "Could not find '$path'\n" unless -e $path;

my $input = <<EOF;
fn get_greeting(name::String) -> String
  return concat(concat("Hello there ", name), ", very nice to meet you")
end

fn main()
    println(get_greeting("Jennifer"))
end
EOF

my $expected = <<EOF;
ic_transform_stmt: transform implementation pending
ic_transform_stmt: transform implementation pending
ic_transform: transform implementation pending
fn get_greeting(name::String) -> String
    let _l1::String = "Hello there "
    let _t2::String = concat(_l1, name)
    let _l2::String = ", very nice to meet you"
    let _t1::String = concat(_t2, _l2)
    return _t1
end
fn main() -> Void
    let _l1::String = "Jennifer"
    let _t1::String = get_greeting(_l1)
    println(_t1)
end
EOF

my $tmp_file = `mktemp TESTING_TRANSFORM_SIMPLE_XXX.ic`;
chomp($tmp_file);
open( my $fh, ">", "$tmp_file" ) or die "failed to open tmp file '$tmp_file'";
print $fh $input;
close $fh;

my $output = `$path $tmp_file`;
my $exit_status = $?;

`rm $tmp_file`;

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
