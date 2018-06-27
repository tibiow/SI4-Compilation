
%{
  #include <stdio.h>
	#include <ctype.h>
	#define YYERROR_VERBOSE 1
	int cpt = 1;
	int c;
	int yylex(void);
	void yyerror(char *msg);

%}

chiffre   [0-9]
entier    {chiffre}+
%token NUMBER 

%%

G:		G E'\n'	{printf("%d ", cpt++); printf("Resultat : %d\n",$2); }
	|	/*epsilon*/;

E:    E'+'E   { $$ = $1 + $3; }
  |   E'-'E   { $$ = $1 - $3; }
  |   E'*'E   { $$ = $1 * $3; }
  |   E'/'E   { $$ = $1 / $3; }
  |   '('E')' { $$ = $2; } 
  | NUMBER    { $$ = $1; }
  ;
%%

int main() {printf("%d ", cpt++); return yyparse(); }
extern int yychar;

int yylex(void) {
  do{
    c = getchar();
  }
  while (c == ' ' || c == '\t');

}
void yyerror(char *msg) {
  //fprintf(stderr, "ERROR: line = %d char = %c\n", cpt-1 , yychar);
  printf("%s\n", msg);
}