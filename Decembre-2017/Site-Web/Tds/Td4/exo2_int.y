/*
 *  exo2_int.y:         Evaluation d'expressions (forme E+E)
 *
 *
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 16-Oct-2015 10:42 (eg)
 * Last file update:  2-Nov-2015 14:57 (eg)
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

%left '+' '-'
%left '*' '/'
%right UMINUS

%%

lines:          lines expr '\n'         { printf("==> %d\n", $2); prompt();}
        |       lines '\n'
        |       error '\n'              { yyerrok; }
        |       /* empty */
        ;

expr:           expr '+' expr           { $$ = $1 + $3; }
        |       expr '-' expr           { $$ = $1 - $3; }
        |       expr '*' expr           { $$ = $1 * $3; }
        |       expr '/' expr           { if ($3) $$ = $1 / $3;
                                          else fprintf(stderr, "division by 0\n"); }
        |       '(' expr ')'            { $$ = $2; }
        |       '-' expr %prec UMINUS   { $$ = -$2; }
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
