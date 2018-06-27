/*
 * automate.c        -- Automate déterministe pour ETF
 *
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 28-Sep-2016 20:13 (eg)
 * Last file update:  5-Oct-2017 13:09 (eg)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "analyseur-ll.h"


typedef int token;

typedef enum bool {false, true} bool;

//--- declarations
static int next;            // next token
int yylval;                 // value of previous encountered token

// Advance
static bool advance(void) {
  next = yylex();
  return true;
}

// Verify current token & advance
static bool verify(token tok) {
  if (next != tok) {
    fprintf(stderr, "Invalid token '%d'\n", next);
    return false;
  }
  return advance();
}

static bool E();

// ---------- F Non Terminal
static bool F() {
  switch (next) {
    case INT:
      return advance();
    case OPEN:
      return advance() && E()&& verify(CLOSE);
    default:
      fprintf(stderr, "Expected INT or OPEN\n");
      return false;
  }
}


// ---------- T Non Terminal
static bool Tp() {
  switch (next) {
    case DIV:
    case MULT:
      return advance() && F() && Tp();
    case PLUS:
    case MINUS:
    case CLOSE:
    case EOL:
      return true;
    default:
      fprintf(stderr, "Expected MULT, PLUS, or EOL\n");
      return false;
  }
}

static bool T() {
  switch (next) {
    case INT:
    case OPEN:
      return F() && Tp();
    default:
      fprintf(stderr, "Expected INT or OPEN\n");
      return false;
  }
}

// ---------- E Non Terminal
static bool Ep() {
  switch (next) {
    case PLUS:
    case MINUS:
      return advance() && T() && Ep();
    case CLOSE:
    case EOL:
      return true;
    default:
      fprintf(stderr, "Expected PLUS or CLOSE\n");
      return false;
  }
}

static bool E() {
  switch (next) {
    case INT:
    case OPEN:
      return T() && Ep();
    default:
      fprintf(stderr, "Expected INT or OPEN\n");
      return false;
  }
}

static void prompt(void) {
  printf("> "); fflush(stdout);
}

int main() {
  prompt();
  while (advance()) {
    if (next == EOF) break;
    printf("=> %scorrect.\n", E()?  "": "in");
    prompt();
  }
  printf("Bye.\n");

  return 0;
}
