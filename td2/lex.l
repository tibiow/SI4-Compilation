%{


#include <stdio.h> 
#include <stdlib.h>
#include "calc.h"

%}

%%
"+"         {return PLUS;}
"-"         {return MINUS;}
"*"         {return MULT;}
"/"         {return DIV;}
"="         {return EQUAL;}
"("         {return OPEN;}
")"         {return CLOSE;}
"\n"        {return EOL;}
[a-zA-Z]+   {yylval.var = strdup(yytext); return IDENT;}
[0-9]+      {yylval.val = atoi(yytext); return NUMBER;}

%%

int yywrap(){
    return 1;
    }   