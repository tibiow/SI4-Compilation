%{


#include <stdio.h> 
#include <stdlib.h>
#include "calc.h"
#include "analyseur.h"
%}

%%
"+"         {return PLUS;}
"-"         {return MINUS;}
"*"         {return MULT;}
"/"         {return DIV;}
"("         {return OPEN;}
")"         {return CLOSE;}
"\n"        {return EOL;}
" "         {}
[0-9]+      {return NUMBER;}

%%



int yywrap(){
    return 1;
    }   