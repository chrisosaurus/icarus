
%{

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct string {
  char *string;
  unsigned int len;
} string;

void yyerror(const char *msg);

extern int yyline;
extern FILE* yyin;

%}

%%

%union {
  string str;
  long int integer;
  bool boolean;
}

%token <str>      IDENTIFIER
%token <str>      STRING
%token <str>      COMMENT

%token <integer>  INTEGER

%token <boolen>   TRUE        "True"
%token <boolen>   FALSE       "False"

%token            DOLLAR      "$"
%token            PERCENT     "%"
%token            AMPERSAND   "&"
%token            AT          "@"
%token            TIMES       "*"
%token            CARET       "^"
%token            PLUS        "+"
%token            MINUS       "-"
%token            DIVIDE      "/"

%token            END         "end"
%token            IF          "if"
%token            RETURN      "return"
%token            LET         "let"
%token            FUNC        "func"

%token            ARROW       "->"
%token            EQUAL       "=="
%token            ASSIGN      "="
%token            DOUBLECOLON "::"
%token            PERIOD      "."

%%

void die(const char *msg){
  printf("ERROR: %s\n", msg);
  exit(1);
}

void yyerror(const char *msg){
  die(msg);
}

int main(void){
  FILE *in_file = fopen("in.ic", "r");

  if( ! in_file ){
    puts("Failed to open file");
    return 1;
  }

  yyin = in_file;

  // keep parsing until there is no more
  do {
    if( ! yyparse() ){
      die("Parsing failed");
    }
  } while (!feof(yyin));


