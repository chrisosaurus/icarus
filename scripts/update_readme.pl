#!/usr/bin/env perl
use strict;
use warnings;
use v5.10;

use File::Slurp;

my $readme_path = "README.md";
my $example_path = "example/simple.ic";
my $command = "make example";

# make a backup of readme first
my $out = `cp $readme_path "$readme_path.bkp"`;
if( $? ){
    die "failed to cp : $out : $!\n";
}


# run our command (make example) and capture output
my $output = `$command`;
my $ret = $?;

if( $ret != 0 ){
    say "command '$command' failed to run, output was:";
    say $output;
    say "Got exit code '$ret'";
    die "command '$command' returned code '$ret'";
}


# get the contents of our readme
my $readme_contents = read_file($readme_path);
die "Failed to read readme file at '$readme_path'\n" unless defined $readme_contents;


# get the contents of example
my $example_contents = read_file($example_path);
die "Failed to read example file at '$example_path'\n" unless defined $example_contents;

# get the 3 sections of output we need from make example
die "Failed to match expected format\n" unless $output =~ m{

    ^lexer \s output:\n
    ^----------------\n
    (?<lexer_output> .*? )
    ^----------------\n

    \s*

    ^parser \s output:\n
    ^----------------\n
    (?<parser_output> .*? )
    ^----------------\n

    \s*

    ^analyse \s output:\n
    ^----------------\n
    (?<analyse_output> .*? )
    ^----------------\n

}xms;

my ($lexer_output, $parser_output, $analyse_output) = @+{qw/lexer_output parser_output analyse_output/};

# indent each of our outputs by one level
$example_contents   =~ s/^/    /xmsg;
$lexer_output       =~ s/^/    /xmsg;
$parser_output      =~ s/^/    /xmsg;
$analyse_output     =~ s/^/    /xmsg;

# remove space from lines that are nothing but
$example_contents   =~ s/^\s+$//xmsg;
$lexer_output       =~ s/^\s+$//xmsg;
$parser_output      =~ s/^\s+$//xmsg;
$analyse_output     =~ s/^\s+$//xmsg;

## built a new readme
die "Failed to build new README : example block\n" unless $readme_contents =~ s/
    ^\s+example\/simple.ic \s contents:\n
    ^\s+----------------\n
    .*?
    ^\s+----------------\n
/
     example\/simple.ic contents:
     ----------------
$example_contents;
     ----------------
/xms;

die "Failed to build new README : lexer block\n" unless $readme_contents =~ s/
    ^\s+lexer \s output:\n
    \s+----------------\n
    .*?
    \s+----------------\n
/
    lexer output:
    ----------------
$lexer_output
    ----------------
/xms;

die "Failed to build new README : parser block\n" unless $readme_contents =~ s/
    ^\s+parser \s output:\n
    \s+----------------\n
    .*?
    \s+----------------\n
/
    parser output:
    ----------------
$parser_output
    ----------------
/xms;

die "Failed to build new README : analyse block\n" unless $readme_contents =~ s/
    ^\s+analyse \s output:\n
    \s+----------------\n
    .*?
    \s+----------------\n
/
    analyse output:
    ----------------
$analyse_output
    ----------------
/xms;

open( my $fh, ">", $readme_path ) or die "failed to open '$readme_path' for writing output : $!\n";
print $fh $readme_contents;
close $fh;

# readme.md has 4 snippets we care about
# the first is just the contents of example/simple.ic.ic
# the last 3 of these are in the make example output
