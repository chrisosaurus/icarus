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
18 : begin
19 : if
20 : elsif
21 : else
22 : return
23 : let
24 : for
25 : in
26 : while
27 : match
28 : case
29 : import
30 : builtin
31 : op
32 : fn
33 : type
34 : enum
35 : union
36 : ->
37 : ==
38 : =
39 : ::
40 : .
41 : ,
42 : and
43 : or
44 : /
45 : (
46 : )
47 : [
48 : ]
49 : <=
50 : >=
51 : <
52 : >
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
18 : IC_BEGIN
19 : IC_IF
20 : IC_ELSIF
21 : IC_ELSE
22 : IC_RETURN
23 : IC_LET
24 : IC_FOR
25 : IC_IN
26 : IC_WHILE
27 : IC_MATCH
28 : IC_CASE
29 : IC_IMPORT
30 : IC_BUILTIN
31 : IC_OP
32 : IC_FUNC
33 : IC_TYPE
34 : IC_ENUM
35 : IC_UNION
36 : IC_ARROW
37 : IC_EQUAL
38 : IC_ASSIGN
39 : IC_DOUBLECOLON
40 : IC_PERIOD
41 : IC_COMMA
42 : IC_AND
43 : IC_OR
44 : IC_DIVIDE
45 : IC_LRBRACKET
46 : IC_RRBRACKET
47 : IC_LSBRACKET
48 : IC_RSBRACKET
49 : IC_LESSTHAN_EQUAL
50 : IC_GREATERTHAN_EQUAL
51 : IC_LESSTHAN
52 : IC_GREATERTHAN
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

