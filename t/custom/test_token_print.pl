#!/usr/bin/env perl
use strict;
use warnings;
use v5.10;

my $path = "bin/t/custom/test_token_print";

die "Could not find '$path'\n" unless -e $path;

my $output = `$path`;

my $exit_status = $?;
if( $exit_status != 0 ){
    die "exit_status was '$exit_status', expected 0";
}

my $expected = <<EOF;
print testing
---------
4 : IC_NEWLINE
5 : IC_WHITSPACE
6 : IC_END
7 : IC_IF
8 : IC_ELSE
9 : IC_THEN
10 : IC_RETURN
11 : IC_LET
12 : IC_BUILTIN
13 : IC_FUNC
14 : IC_TYPE
15 : IC_ENUM
16 : IC_UNION
17 : IC_ARROW
18 : IC_EQUAL
19 : IC_ASSIGN
20 : IC_DOUBLECOLON
21 : IC_PERIOD
22 : IC_COMMA
23 : IC_PLUS
24 : IC_MINUS
25 : IC_DIVIDE
26 : IC_MULTIPLY
27 : IC_LRBRACKET
28 : IC_RRBRACKET
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
13 : IC_FUNC
14 : IC_TYPE
15 : IC_ENUM
16 : IC_UNION
17 : IC_ARROW
18 : IC_EQUAL
19 : IC_ASSIGN
20 : IC_DOUBLECOLON
21 : IC_PERIOD
22 : IC_COMMA
23 : IC_PLUS
24 : IC_MINUS
25 : IC_DIVIDE
26 : IC_MULTIPLY
27 : IC_LRBRACKET
28 : IC_RRBRACKET
---------
EOF

unless( $output eq $expected ){
    say "Output was not as expected";
    say "=======\nExpected:\n$expected";
    say "=======\nGot:\n$output";
    say "=======\n";
    die "Output not as expected";
}

say "test_lex_example: successs";

