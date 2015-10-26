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
12 : import
13 : builtin
14 : op
15 : fn
16 : type
17 : enum
18 : union
19 : ->
20 : ==
21 : =
22 : ::
23 : .
24 : ,
25 : +
26 : -
27 : /
28 : *
29 : %
30 : (
31 : )
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
12 : IC_IMPORT
13 : IC_BUILTIN
14 : IC_OP
15 : IC_FUNC
16 : IC_TYPE
17 : IC_ENUM
18 : IC_UNION
19 : IC_ARROW
20 : IC_EQUAL
21 : IC_ASSIGN
22 : IC_DOUBLECOLON
23 : IC_PERIOD
24 : IC_COMMA
25 : IC_PLUS
26 : IC_MINUS
27 : IC_DIVIDE
28 : IC_MULTIPLY
29 : IC_MODULO
30 : IC_LRBRACKET
31 : IC_RRBRACKET
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

