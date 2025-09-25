%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

int yylex();
void yyerror(BaseAST **ast, const char *s);
%}

%parse-param { BaseAST **ast }

%union {
  char *str_val;
  int int_val;
  BaseAST *ast_val;
}

%token INT RETURN
%token <str_val> IDENT
%token <int_val> INT_CONST

%type <ast_val> FuncDef FuncType Block Stmt Exp PrimaryExp UnaryExp Number

%%

CompUnit
  : FuncDef {
    *ast = create_comp_unit_ast($1);
  }
  ;

FuncDef
  : FuncType IDENT '(' ')' Block {
    $$ = create_func_def_ast($1, $2, $5);
  }
  ;

FuncType
  : INT {
    $$ = create_func_type_ast();
  }
  ;

Block
  : '{' Stmt '}' {
    $$ = create_block_ast($2);
  }
  ;

Stmt
  : RETURN Exp ';' {
    $$ = create_stmt_ast($2);
  }
  ;

Number
  : INT_CONST { $$ = create_number_ast($1); }
  ;

Exp
  : UnaryExp { $$ = $1; }
  ;

PrimaryExp
  : '(' Exp ')' { $$ = $2; }
  | Number { $$ = $1; }
  ;

UnaryExp
  : PrimaryExp { $$ = $1; }
  | '+' UnaryExp { $$ = $2; }
  | '-' UnaryExp { $$ = create_unary_ast('-', $2); }
  | '!' UnaryExp { $$ = create_unary_ast('!', $2); }
  ;

%%

void yyerror(BaseAST **ast, const char *s) {
  fprintf(stderr, "error: %s\n", s);
}
