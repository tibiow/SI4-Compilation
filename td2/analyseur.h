#include <stdio.h>
#include <stdbool.h>

#define NUMBER 258
#define PLUS 260
#define MINUS 261
#define MULT 262
#define DIV 263
#define OPEN 264
#define CLOSE 265
#define EOL 266
#define UMINUS 268


int next;

extern int yylex();
bool E(void);
bool Ep(void);
bool T(void);
bool Tp(void);
bool F(void);
bool verify(int);
