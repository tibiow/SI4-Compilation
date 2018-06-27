%{
  #include <stdio.h>
  #include <ctype.h>

  void yyerror(const char* msg);
  int yylex(void);
  void prompt(void);

  #define  YYERROR_VERBOSE 1
  int lineno = 1;
  extern int yylval;
%}

%%
G:              G S '\n'                { printf("Analyse OK\n"); prompt();}
        |       error '\n'              { yyerrok; }
        |       /* empty */
        ;

S:              S 'a'                   { printf("Règle S -> Sa (r1)\n"); }
        |       'x'                     { printf("Règle S -> x  (r2)\n"); }
        |       'y'                     { printf("Règle S -> y  (r3)\n"); }
        ;

%%
int main() {
    prompt();
    return yyparse(); 
}

void yyerror(const char* msg) { 
    fprintf(stderr, "Erreur: ligne %d. Token: ", lineno);
    fprintf(stderr, (yychar < 256) ? "%c": "%d", yychar);
    fprintf(stderr, " %s\n", msg);
    prompt();
}

int yylex(void) {
    int c;

    do
        c =getchar();
    while (c == ' ' || c == '\t');

    if (c == '\n') lineno +=1;
    return c;
}

void prompt(void) {
  printf("[%d] ", lineno);
  fflush(stdout);
}
