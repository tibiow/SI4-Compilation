%{
   #define MAXBUF 1000     // un peu moche car on ne s'en sert pas....

   #include <stdio.h>
   #include "exo3.h"
%}

%option noyywrap
%option yylineno

%%
[ \t]           {  /* skip  */  }
[0-9]+          { yylval.val = atoi(yytext); return NUMBER; }
[^ \t]|\n       { return yytext[0]; }
%%
