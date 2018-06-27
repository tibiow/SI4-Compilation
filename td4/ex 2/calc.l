%{

#include "calc.h"
#include <stdlib.h>


%}

chiffre   [0-9]
entier    {chiffre}+

%%


{entier}    {
      yylval=atoi(yytext);
      return(NOMBRE);
    }

"+"   return(PLUS);
"-"   return(MOINS);

"*"   return(FOIS);
"/"   return(DIVISE);

"^"   return(PUISSANCE);

"("   return(PARENTHESE_GAUCHE);
")"   return(PARENTHESE_DROITE);

"\n"  return(FIN);

