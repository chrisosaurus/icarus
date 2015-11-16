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
16 : elsif
17 : else
18 : return
19 : let
20 : for
21 : in
22 : while
23 : import
24 : builtin
25 : op
26 : fn
27 : type
28 : enum
29 : union
30 : ->
31 : ==
32 : =
33 : ::
34 : .
35 : ,
36 : &&
37 : ||
38 : /
39 : (
40 : )
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
16 : IC_ELSIF
17 : IC_ELSE
18 : IC_RETURN
19 : IC_LET
20 : IC_FOR
21 : IC_IN
22 : IC_WHILE
23 : IC_IMPORT
24 : IC_BUILTIN
25 : IC_OP
26 : IC_FUNC
27 : IC_TYPE
28 : IC_ENUM
29 : IC_UNION
30 : IC_ARROW
31 : IC_EQUAL
32 : IC_ASSIGN
33 : IC_DOUBLECOLON
34 : IC_PERIOD
35 : IC_COMMA
36 : IC_AND
37 : IC_OR
38 : IC_DIVIDE
39 : IC_LRBRACKET
40 : IC_RRBRACKET
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

