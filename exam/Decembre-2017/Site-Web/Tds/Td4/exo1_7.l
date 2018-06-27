%{
   #include <stdio.h>
   #include "exo1_7.h"
%}
%option noyywrap
%option yylineno

%%

[ \t]           { /* skip  */ }
"\n"            { return '\n'; }
[aA]            { return TOK_A;}
[xX]            { return TOK_X;}
[yY]            { return TOK_Y;}
.               { return yytext[0]; } /* cela provoquera une erreur syntaxique */
%%
