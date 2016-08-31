#!/usr/bin/env perl
use strict;
use warnings;
use v5.10;

my $path = "bin/t/custom/test_backend_pancake_simple";

die "Could not find '$path'\n" unless -e $path;

my $input = <<EOF;
fn foo(s::String)
  println(s)
end

fn main()
  foo("Hello world")
end
EOF

my $expected = <<EOF;
label entry
call main() 0
exit
label foo(String)
copyarg 0
call println(String) 1
pop 1
return_void
label main()
pushstr Hello world
call foo(String) 1
pop 0
return_void
EOF

my $in_tmp_file = `mktemp TESTING_BACKEND_PANCAKE_SIMPLE_XXX.ic`;
chomp($in_tmp_file);
open( my $fh, ">", "$in_tmp_file" ) or die "failed to open tmp file '$in_tmp_file'";
print $fh $input;
close $fh;

# using mktemp here just to get name
my $out_tmp_file = `mktemp TESTING_BACKEND_PANCAKE_SIMPLE_XXX.out`;

my $output = `$path $in_tmp_file $out_tmp_file`;
my $exit_status = $?;

`rm $in_tmp_file`;

if( $exit_status != 0 ){
  `rm $out_tmp_file`;
  say "Icarus 2c exit status was not as expected";
  say "=======\nGot:\n$output";
  die "exit_status was '$exit_status', expected 0";
}

# finally run program
$output = `cat $out_tmp_file`;
`rm $out_tmp_file`;

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

say "test_backend_pancake_simple successs";
say "=======\nGot correct output:\n$output";

