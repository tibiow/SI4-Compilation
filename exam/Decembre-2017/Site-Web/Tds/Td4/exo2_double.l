%{
   #include <stdio.h>
   #define  YYSTYPE double
   #include "exo2_double.h"

%}

%option noyywrap
%option yylineno

%%
[ \t]                   {  /* skip  */  }
  /* réel commançant par un chiffre */
[0-9]+\.?[0-9]*(e[+-]?[0-9]+)?  { yylval = atof(yytext); return NUMBER; }
  /* réel commançant par un '.' */
\.[0-9]+(e[+-]?[0-9]+)?         { yylval = atof(yytext); return NUMBER; }
[^ \t]|\n               { return yytext[0]; }
%%
