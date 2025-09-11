%option noyywrap
%option nounput
%option noinput

%{
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "sysy.tab.h"
%}

WhiteSpace    [ \t\n\r]*
LineComment   "//".*
Identifier    [a-zA-Z_][a-zA-Z0-9_]*
Decimal       [1-9][0-9]*
Octal         0[0-7]*
Hexadecimal   0[xX][0-9a-fA-F]+

%%
{WhiteSpace}    {}
{LineComment}   {}
"int"           { return INT; }
"return"        { return RETURN; }

{Identifier}    { yylval.str_val = strdup(yytext); return IDENT; }
{Decimal}       { yylval.int_val = strtol(yytext, NULL, 0); return INT_CONST; }
{Octal}         { yylval.int_val = strtol(yytext, NULL, 0); return INT_CONST; }
{Hexadecimal}   { yylval.int_val = strtol(yytext, NULL, 0); return INT_CONST; }

.               { return yytext[0]; }
