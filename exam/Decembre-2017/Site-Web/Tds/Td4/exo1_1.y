%{
  #include <stdio.h>
  #include <ctype.h>

  void yyerror(const char* msg);
  int yylex(void);
%}

%%
G:              S                       { printf("Analyse OK\n"); }
        ;

S:              S 'a'                   { printf("Règle S -> Sa (r1)\n"); }
        |       'x'                     { printf("Règle S -> x  (r2)\n"); }
        |       'y'                     { printf("Règle S -> y  (r3)\n"); }
        ;

%%
int main() { return yyparse(); }

void yyerror(const char* msg) { fprintf(stderr, "Erreur: %s\n", msg); }

int yylex(void) {
    int c;

    do  c =getchar(); while (isspace(c));
    return c;
}
