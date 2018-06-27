%{
  #include <stdio.h>
  #include <ctype.h>
  #include <unistd.h>

  void yyerror(const char* msg);
  int yylex(void);
  void prompt(void);

  #define  YYERROR_VERBOSE 1
  extern int yylineno, yylval;
%}

%token TOK_A TOK_X TOK_Y

%%
gram:           gram s '\n'             { printf("Analyse OK\n"); prompt();}
        |       error '\n'              { yyerrok; }
        |       /* empty */
        ;

s:              s TOK_A                 { printf("Règle S -> Sa (r1)\n"); }
        |       TOK_X                   { printf("Règle S -> x  (r2)\n"); }
        |       TOK_Y                   { printf("Règle S -> y  (r3)\n"); }
        ;

%%
int main() {
    prompt();
    return yyparse();
}

void yyerror(const char* msg) {
    fprintf(stderr, "Erreur: ligne %d. Token: ", yylineno);
    // afficher la valeur de yychar. L'affichage se fait sous forme de
    // caractère si yychar est < 256, sous la forme d'une chaîne
    // (après conversion) sinon
    if  (yychar < 256)
        fprintf(stderr, "'%c' (soit 0x%x)", yychar, yychar);
    else
        switch(yychar) {
            case TOK_A: fprintf(stderr, "'A' ou 'a'"); break;
            case TOK_X: fprintf(stderr, "'X' ou 'x'"); break;
            case TOK_Y: fprintf(stderr, "'Y' ou 'y'"); break;
        }
    fprintf(stderr, "\nmsg='%s'\n", msg);
    prompt();
}

void prompt(void) {
  printf("[%d] ", yylineno);
  fflush(stdout);
}
