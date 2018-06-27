/*
 *  exo2_etf.y:         Evaluation d'expressions (forme ETF)
 *
 *
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 16-Oct-2015 10:42 (eg)
 * Last file update:  2-Nov-2015 14:56 (eg)
 */

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

%token NUMBER

%%

lines:          lines expr '\n'         { printf("==> %d\n", $2); prompt();}
        |       lines '\n'
        |       error '\n'              { yyerrok; }
        |       /* empty */
        ;

expr:           expr '+' term           { $$ = $1 + $3; }
        |       expr '-' term           { $$ = $1 - $3; }
        |       term
        ;

term:           term '*' sfactor        { $$ = $1 * $3; }
        |       term '/' sfactor        { if ($3) $$ = $1 / $3;
                                          else fprintf(stderr, "division by 0\n"); }
        |       sfactor
        ;

sfactor  :      '-' factor              { $$ = -$2; }
        |       factor                  { $$ = $1; }
        ;

factor:         '(' expr ')'            { $$ = $2; }
        |       NUMBER
        ;




%%
void yyerror(const char* msg) {
    fprintf(stderr, "Error: Line %d. Token: ", yylineno);
    fprintf(stderr, (yychar < 256) ? "'%c'": "[%d]", yychar);
    fprintf(stderr, " %s\n", msg);
    prompt();
}

void prompt(void) {
  printf("[%d] ", yylineno);
  fflush(stdout);
}

int main() {
    prompt();
    return yyparse();
}
