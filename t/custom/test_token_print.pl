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
5 : ^
6 : +
7 : -
8 : hello
9 : 14
10 : "hello"
11 : #hello
12 : 

13 :  
14 : end
15 : if
16 : else
17 : return
18 : let
19 : import
20 : builtin
21 : op
22 : fn
23 : type
24 : enum
25 : union
26 : ->
27 : ==
28 : =
29 : ::
30 : .
31 : ,
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
5 : IC_CARET
6 : IC_PLUS
7 : IC_MINUS
8 : IC_IDENTIFIER
9 : IC_LITERAL_INTEGER
10 : IC_LITERAL_STRING
11 : IC_COMMENT
12 : IC_NEWLINE
13 : IC_WHITSPACE
14 : IC_END
15 : IC_IF
16 : IC_ELSE
17 : IC_RETURN
18 : IC_LET
19 : IC_IMPORT
20 : IC_BUILTIN
21 : IC_OP
22 : IC_FUNC
23 : IC_TYPE
24 : IC_ENUM
25 : IC_UNION
26 : IC_ARROW
27 : IC_EQUAL
28 : IC_ASSIGN
29 : IC_DOUBLECOLON
30 : IC_PERIOD
31 : IC_COMMA
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

