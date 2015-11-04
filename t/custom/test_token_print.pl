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
0 : \$
1 : %
2 : &
3 : @
4 : *
5 : hello
6 : 14
7 : "hello"
8 : #hello
9 : 

10 :  
11 : end
12 : if
13 : else
14 : then
15 : return
16 : let
17 : import
18 : builtin
19 : op
20 : fn
21 : type
22 : enum
23 : union
24 : ->
25 : ==
26 : =
27 : ::
28 : .
29 : ,
30 : +
31 : -
32 : /
33 : (
34 : )
---------

print debug testing
---------
0 : IC_DOLLAR
1 : IC_PERCENT
2 : IC_AMPERSAND
3 : IC_AT
4 : IC_ASTERISK
5 : IC_IDENTIFIER
6 : IC_LITERAL_INTEGER
7 : IC_LITERAL_STRING
8 : IC_COMMENT
9 : IC_NEWLINE
10 : IC_WHITSPACE
11 : IC_END
12 : IC_IF
13 : IC_ELSE
14 : IC_THEN
15 : IC_RETURN
16 : IC_LET
17 : IC_IMPORT
18 : IC_BUILTIN
19 : IC_OP
20 : IC_FUNC
21 : IC_TYPE
22 : IC_ENUM
23 : IC_UNION
24 : IC_ARROW
25 : IC_EQUAL
26 : IC_ASSIGN
27 : IC_DOUBLECOLON
28 : IC_PERIOD
29 : IC_COMMA
30 : IC_PLUS
31 : IC_MINUS
32 : IC_DIVIDE
33 : IC_LRBRACKET
34 : IC_RRBRACKET
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

