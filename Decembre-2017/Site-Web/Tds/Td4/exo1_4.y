%{
  #include <stdio.h>
  #include <ctype.h>

  void yyerror(const char* msg);
  int yylex(void);
  void prompt(void);

  int lineno = 1;
%}

%%
G:              G S '\n'                { printf("Analyse OK\n"); prompt();}
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

void yyerror(const char* msg) { fprintf(stderr, "Erreur: %s\n", msg); }

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
