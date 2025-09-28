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
%token LE GE EQ NE AND OR
%token <str_val> IDENT
%token <int_val> INT_CONST

%type <ast_val> FuncDef FuncType Block Stmt Exp PrimaryExp UnaryExp MulExp AddExp RelExp EqExp LAndExp LOrExp Number

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
  : LOrExp { $$ = $1; }
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

MulExp
  : UnaryExp { $$ = $1; }
  | MulExp '*' UnaryExp { $$ = create_binary_ast('*', $1, $3); }
  | MulExp '/' UnaryExp { $$ = create_binary_ast('/', $1, $3); }
  | MulExp '%' UnaryExp { $$ = create_binary_ast('%', $1, $3); }
  ;

AddExp
  : MulExp { $$ = $1; }
  | AddExp '+' MulExp { $$ = create_binary_ast('+', $1, $3); }
  | AddExp '-' MulExp { $$ = create_binary_ast('-', $1, $3); }
  ;

RelExp
  : AddExp { $$ = $1; }
  | RelExp '<' AddExp { $$ = create_binary_ast('<', $1, $3); }
  | RelExp '>' AddExp { $$ = create_binary_ast('>', $1, $3); }
  | RelExp LE AddExp { $$ = create_binary_ast('l', $1, $3); }
  | RelExp GE AddExp { $$ = create_binary_ast('g', $1, $3); }
  ;

EqExp
  : RelExp { $$ = $1; }
  | EqExp EQ RelExp { $$ = create_binary_ast('e', $1, $3); }
  | EqExp NE RelExp { $$ = create_binary_ast('n', $1, $3); }
  ;

LAndExp
  : EqExp { $$ = $1; }
  | LAndExp AND EqExp { $$ = create_binary_ast('&', $1, $3); }
  ;

LOrExp
  : LAndExp { $$ = $1; }
  | LOrExp OR LAndExp { $$ = create_binary_ast('|', $1, $3); }
  ;

%%

void yyerror(BaseAST **ast, const char *s) {
  fprintf(stderr, "error: %s\n", s);
}
