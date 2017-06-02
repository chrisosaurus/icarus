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
9 : 14s
10 : 14u
11 : "hello"
12 : #hello
13 : True
14 : False
15 : 

16 :  
17 : end
18 : if
19 : elsif
20 : else
21 : return
22 : let
23 : for
24 : in
25 : while
26 : match
27 : case
28 : import
29 : builtin
30 : op
31 : fn
32 : type
33 : enum
34 : union
35 : ->
36 : ==
37 : =
38 : ::
39 : .
40 : ,
41 : and
42 : or
43 : /
44 : (
45 : )
46 : [
47 : ]
48 : <=
49 : >=
50 : <
51 : >
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
9 : IC_LITERAL_SIGNED_INTEGER
10 : IC_LITERAL_UNSIGNED_INTEGER
11 : IC_LITERAL_STRING
12 : IC_COMMENT
13 : IC_TRUE
14 : IC_TRUE
15 : IC_NEWLINE
16 : IC_WHITSPACE
17 : IC_END
18 : IC_IF
19 : IC_ELSIF
20 : IC_ELSE
21 : IC_RETURN
22 : IC_LET
23 : IC_FOR
24 : IC_IN
25 : IC_WHILE
26 : IC_MATCH
27 : IC_CASE
28 : IC_IMPORT
29 : IC_BUILTIN
30 : IC_OP
31 : IC_FUNC
32 : IC_TYPE
33 : IC_ENUM
34 : IC_UNION
35 : IC_ARROW
36 : IC_EQUAL
37 : IC_ASSIGN
38 : IC_DOUBLECOLON
39 : IC_PERIOD
40 : IC_COMMA
41 : IC_AND
42 : IC_OR
43 : IC_DIVIDE
44 : IC_LRBRACKET
45 : IC_RRBRACKET
46 : IC_LSBRACKET
47 : IC_RSBRACKET
48 : IC_LESSTHAN_EQUAL
49 : IC_GREATERTHAN_EQUAL
50 : IC_LESSTHAN
51 : IC_GREATERTHAN
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

