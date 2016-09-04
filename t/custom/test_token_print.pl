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
17 : then
18 : if
19 : elsif
20 : else
21 : return
22 : let
23 : for
24 : in
25 : while
26 : import
27 : builtin
28 : op
29 : fn
30 : type
31 : enum
32 : union
33 : ->
34 : ==
35 : =
36 : ::
37 : .
38 : ,
39 : and
40 : or
41 : /
42 : (
43 : )
44 : <=
45 : >=
46 : <
47 : >
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
17 : IC_THEN
18 : IC_IF
19 : IC_ELSIF
20 : IC_ELSE
21 : IC_RETURN
22 : IC_LET
23 : IC_FOR
24 : IC_IN
25 : IC_WHILE
26 : IC_IMPORT
27 : IC_BUILTIN
28 : IC_OP
29 : IC_FUNC
30 : IC_TYPE
31 : IC_ENUM
32 : IC_UNION
33 : IC_ARROW
34 : IC_EQUAL
35 : IC_ASSIGN
36 : IC_DOUBLECOLON
37 : IC_PERIOD
38 : IC_COMMA
39 : IC_AND
40 : IC_OR
41 : IC_DIVIDE
42 : IC_LRBRACKET
43 : IC_RRBRACKET
44 : IC_LESSTHAN_EQUAL
45 : IC_GREATERTHAN_EQUAL
46 : IC_LESSTHAN
47 : IC_GREATERTHAN
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

