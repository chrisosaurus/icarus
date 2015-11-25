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
12 : True
13 : False
14 : 

15 :  
16 : end
17 : if
18 : elsif
19 : else
20 : return
21 : let
22 : for
23 : in
24 : while
25 : import
26 : builtin
27 : op
28 : fn
29 : type
30 : enum
31 : union
32 : ->
33 : ==
34 : =
35 : ::
36 : .
37 : ,
38 : and
39 : or
40 : /
41 : (
42 : )
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
12 : IC_TRUE
13 : IC_TRUE
14 : IC_NEWLINE
15 : IC_WHITSPACE
16 : IC_END
17 : IC_IF
18 : IC_ELSIF
19 : IC_ELSE
20 : IC_RETURN
21 : IC_LET
22 : IC_FOR
23 : IC_IN
24 : IC_WHILE
25 : IC_IMPORT
26 : IC_BUILTIN
27 : IC_OP
28 : IC_FUNC
29 : IC_TYPE
30 : IC_ENUM
31 : IC_UNION
32 : IC_ARROW
33 : IC_EQUAL
34 : IC_ASSIGN
35 : IC_DOUBLECOLON
36 : IC_PERIOD
37 : IC_COMMA
38 : IC_AND
39 : IC_OR
40 : IC_DIVIDE
41 : IC_LRBRACKET
42 : IC_RRBRACKET
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

