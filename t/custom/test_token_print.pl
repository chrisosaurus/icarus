#!/usr/bin/env perl
use strict;
use warnings;
use v5.10;

my $path = "bin/t/custom/test_token_print";

die "Could not find '$path'\n" unless -e $path;

my $output = `$path`;
my $exit_status = $?;
my $expected = <<EOF;
print testing
---------
0 : hello
1 : 14
2 : "hello"
3 : #hello
4 : 

5 :  
6 : end
7 : if
8 : else
9 : then
10 : return
11 : let
12 : builtin
13 : op
14 : fn
15 : type
16 : enum
17 : union
18 : ->
19 : ==
20 : =
21 : ::
22 : .
23 : ,
24 : +
25 : -
26 : /
27 : *
28 : (
29 : )
---------

print debug testing
---------
0 : IC_IDENTIFIER
1 : IC_LITERAL_INTEGER
2 : IC_LITERAL_STRING
3 : IC_COMMENT
4 : IC_NEWLINE
5 : IC_WHITSPACE
6 : IC_END
7 : IC_IF
8 : IC_ELSE
9 : IC_THEN
10 : IC_RETURN
11 : IC_LET
12 : IC_BUILTIN
13 : IC_OP
14 : IC_FUNC
15 : IC_TYPE
16 : IC_ENUM
17 : IC_UNION
18 : IC_ARROW
19 : IC_EQUAL
20 : IC_ASSIGN
21 : IC_DOUBLECOLON
22 : IC_PERIOD
23 : IC_COMMA
24 : IC_PLUS
25 : IC_MINUS
26 : IC_DIVIDE
27 : IC_MULTIPLY
28 : IC_LRBRACKET
29 : IC_RRBRACKET
---------
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

say "test_token_print successs";

