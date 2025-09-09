%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int yylex();
void yyerror(char **ast, const char *s);
%}

%parse-param { char **ast }

%union {
  char *str_val;
  int int_val;
}

%token INT RETURN
%token <str_val> IDENT
%token <int_val> INT_CONST

%type <str_val> FuncDef FuncType Block Stmt Number

%%

CompUnit
  : FuncDef {
    *ast = $1;
  }
  ;

FuncDef
  : FuncType IDENT '(' ')' Block {
    size_t len = strlen($1) + 1 + strlen($2) + 2 + 1 + strlen($5) + 1;
    char *result = (char *)malloc(len);
    sprintf(result, "%s %s() %s", $1, $2, $5);
    $$ = result;
    free($1);
    free($2);
    free($5);
  }
  ;

FuncType
  : INT {
    $$ = strdup("int");
  }
  ;

Block
  : '{' Stmt '}' {
    size_t len = strlen($2) + 4 + 1; // for "{ ", " }", and '\0'
    char *result = (char *)malloc(len);
    sprintf(result, "{ %s }", $2);
    $$ = result;
    free($2);
  }
  ;

Stmt
  : RETURN Number ';' {
    size_t len = strlen("return ") + strlen($2) + 1 + 1; // for ";" and '\0'
    char *result = (char *)malloc(len);
    sprintf(result, "return %s;", $2);
    $$ = result;
    free($2);
  }
  ;

Number
  : INT_CONST {
    char buffer[50];
    sprintf(buffer, "%d", $1);
    $$ = strdup(buffer);
  }
  ;

%%

void yyerror(char **ast, const char *s) {
  fprintf(stderr, "error: %s\n", s);
}
