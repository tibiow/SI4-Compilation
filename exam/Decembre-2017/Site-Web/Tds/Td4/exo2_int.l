%{
   #include <stdio.h>
   #include "exo2_int.h"
%}

%option noyywrap
%option yylineno

%%
[ \t]           {  /* skip  */  }
[0-9]+          { yylval = atoi(yytext); return NUMBER; }
.|\n            { return yytext[0]; }
%%
