
%{
    #include <stdio.h>
	#include <ctype.h>
	#define YYERROR_VERBOSE 1
	int cpt = 1;
	int c;
	int yylex(void);
	void yyerror(char *msg);

%}

%token TOK_A
%token TOK_X
%token TOK_Y

%%

G:		G S'\n'	{printf("%d ", cpt++);}
	|	/*epsilon*/;

S:		S TOK_A
	|	TOK_X
	|	TOK_Y;

%%
int main() {printf("%d ", cpt++); return yyparse(); }
extern int yychar;
int yylex(void) {
  do{
  	
     c = getchar();
  if (c=='a')
  	return TOK_A;
  if (c=='x')
  	return TOK_X;
  if (c=='y')
  	return TOK_Y;
  }
  while (c == ' ' || c == '\t');

}
void yyerror(char *msg) {
  fprintf(stderr, "ERROR: line = %d char = %c\n", cpt-1 , yychar);
}