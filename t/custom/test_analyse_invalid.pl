#!/usr/bin/env perl
use strict;
use warnings;
use v5.10;

my $path = "./icarus";

# filename to use for temp
my $filename = "analyse_invalid_testing.ic";

die "Could not find '$path'\n" unless -e $path;

# a list of invalid icarus programs
# and their expected outputs
# all programs must have an exit code of 1
my %invalid_programs = (
'fn main() foo() end'
=>
"ic_analyse_infer_fcall: error finding function declaration for function call:
        foo()
    I tried to lookup:
        foo()
    and failed
ic_analyse_infer: call to ic_analyse_infer_fcall failed
ic_analyse_body: expr: call to ic_analyse_infer failed
ic_analyse_body: unimplemented in error case
ic_analyse_decl_func: call to ic_analyse_body for validating body failed
ic_analyse_decl_func: error
ic_analyse: call to ic_analyse_decl_func failed
analysis failed
",

'fn main() let a::Sint = foo() end
fn foo() -> String end'
=>
"ic_analyse_let: let init type did not match declared type
ic_analyse_body: call to ic_analyse_let failed
ic_analyse_body: unimplemented in error case
ic_analyse_decl_func: call to ic_analyse_body for validating body failed
ic_analyse_decl_func: error
ic_analyse: call to ic_analyse_decl_func failed
analysis failed
",

'fn main() let a::String = 1s + 2s end'
=>
"ic_analyse_let: let init type did not match declared type
ic_analyse_body: call to ic_analyse_let failed
ic_analyse_body: unimplemented in error case
ic_analyse_decl_func: call to ic_analyse_body for validating body failed
ic_analyse_decl_func: error
ic_analyse: call to ic_analyse_decl_func failed
analysis failed
",

'fn foo() -> Sint return "hello" end'
=>
"ic_analyse_body: ret: returned type did not match declared
ic_analyse_body: unimplemented in error case
ic_analyse_decl_func: call to ic_analyse_body for validating body failed
ic_analyse_decl_func: error
ic_analyse: call to ic_analyse_decl_func failed
analysis failed
",

'fn foo() return "hello" end'
=>
"ic_analyse_body: ret: returned type did not match declared
ic_analyse_body: unimplemented in error case
ic_analyse_decl_func: call to ic_analyse_body for validating body failed
ic_analyse_decl_func: error
ic_analyse: call to ic_analyse_decl_func failed
analysis failed
",

'fn foo() let a::Sint = 5s a = "hello" end'
=>
'ic_analyse_body: assign: assignment between invalid types
ic_analyse_body: unimplemented in error case
ic_analyse_decl_func: call to ic_analyse_body for validating body failed
ic_analyse_decl_func: error
ic_analyse: call to ic_analyse_decl_func failed
analysis failed
',

'fn foo() let a::Sint = 5s a = 6s end'
=>
'ic_analyse: failed to find a main function
analysis failed
',

'fn main() foo[Sint](6s, 7s) end'
=>
"ic_analyse_resolve_type: unable to resolve type

    I tried to lookup both:
        foo[Sint]
        foo[_]
    and failed to find either

ic_analyse_infer_fcall: error finding function declaration for function call:
        foo[Sint](6s, 7s)

    I tried to lookup both:
        foo[Sint](Sint,Sint)
        foo[_](_,_)
    and failed to find either

    I also tried to consider this as a constructor
    but after attempting to instantiate this type, I still found no function matching the call:
        foo[Sint](Sint,Sint)

ic_analyse_infer: call to ic_analyse_infer_fcall failed
ic_analyse_body: expr: call to ic_analyse_infer failed
ic_analyse_body: unimplemented in error case
ic_analyse_decl_func: call to ic_analyse_body for validating body failed
ic_analyse_decl_func: error
ic_analyse: call to ic_analyse_decl_func failed
analysis failed
",

);

for my $key (keys %invalid_programs){
    my $source = $key;
    my $expected = $invalid_programs{$key};

    # write source to file
    open( my $fh, ">", $filename) or die "failed to open file '$filename' : $!\n";
    print $fh $source;
    close($fh);

    # run program
    my $output = `$path check $filename`;
    my $exit_status = $?;

    # check exit code and output
    if( $exit_status == 0 || $output ne $expected ){
        say "When running program";
        say "=======\nSource\n$source";
        say "=======\n";
        say "Output was not as expected";
        say "=======\nExpected:\n$expected";
        say "=======\nGot:\n$output";
        say "=======\n";
        if( $exit_status == 0 ){
            die "exit_status was '$exit_status', expected non zero";
        } else {
            die "Output not as expected";
        }
    }

}

unlink $filename;

say "test_analyse_invalid successs";

