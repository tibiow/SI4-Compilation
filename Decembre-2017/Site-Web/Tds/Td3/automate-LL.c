/*
 * automate.c        -- Automate déterministe pour ETF
 *
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 28-Sep-2016 20:13 (eg)
 * Last file update: 30-Sep-2016 12:04 (eg)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define OPEN  '('
#define CLOSE ')'
#define PLUS  '+'
#define MULT  '*'
#define INT   'i'
#define END   '$'

typedef char token;
typedef enum bool {false, true} bool;


char *next;             // The next token pointer


// Verify current token & advance
bool verify(token tok) {
  if (*next != tok) {
    fprintf(stderr, "Invalid token '%c'\n", *next);
    return false;
  }
  next++;
  return true;
}


bool E();


// ---------- F Non Terminal
bool F() {
  switch (*next) {
    case INT:
      return verify(INT);
    case OPEN:
      return verify(OPEN) && E()&& verify(CLOSE);
    default:
      fprintf(stderr, "Expected INT or OPEN\n");
      return false;
  }
}


// ---------- T Non Terminal
bool Tp() {
  switch (*next) {
    case MULT:
      return verify(MULT) && F() && Tp();
    case PLUS:
    case CLOSE:
    case END:
      return true;
    default:
      fprintf(stderr, "Expected MULT, PLUS, or EOF\n");
      return false;
  }
}

bool T() {
  switch (*next) {
    case INT:
    case OPEN:
      return F() && Tp(); 
    default:
      fprintf(stderr, "Expected INT or OPEN\n");
      return false;
  }
}

// ---------- E Non Terminal
bool Ep() {
  switch (*next) {
    case PLUS:
      return verify(PLUS) && T() && Ep();
    case CLOSE:
    case END:
      return true;
    default:
      fprintf(stderr, "Expected PLUS or CLOSE\n");
      return false;
  }
}

bool E() {
  switch (*next) {
    case INT:
    case OPEN:
      return T() && Ep(); 
    default:
      fprintf(stderr, "Expected INT or OPEN\n");
      return false;
  }
}



int main() {
  char word[100];                       // Example: "(i+i)*i". NO SPACES!!!

  while (fgets(word, sizeof word,stdin)) {
    word[strlen(word)-1] = END;        // to suppress '\n'
    next = word;
    printf("word = '%s' => %d\n", word, E()); 
  }
}
