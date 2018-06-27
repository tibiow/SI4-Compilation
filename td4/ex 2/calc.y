%{

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
%}

%token  NOMBRE
%token  PLUS  MOINS FOIS  DIVISE  PUISSANCE
%token  PARENTHESE_GAUCHE PARENTHESE_DROITE
%token  FIN

%left PLUS  MOINS
%left FOIS  DIVISE
%left NEG
%right  PUISSANCE

%start Input
%%

Input:
    /* Vide */
  | Input Ligne
  ;

Ligne:
    FIN
  | E FIN       { printf("==> %d\n",$1); }
  | error '\n'  {yyerrok; }
  ;

E:
    NOMBRE      { $$=$1; }
  | E PLUS E  { $$=$1+$3; }
  | E MOINS E { $$=$1-$3; }
  | E FOIS E  { $$=$1*$3; }
  | E DIVISE E  { $$=$1/$3; }
  | MOINS E %prec NEG  { $$=-$2; }
  | E PUISSANCE E { $$=pow($1,$3); }
  | PARENTHESE_GAUCHE E PARENTHESE_DROITE  { $$=$2; }
  ;

%%

int yyerror(char *s) {
  printf("%s\n",s);
}

int main(void) {
  yyparse();
}