#include <stdio.h>
#include <stdlib.h>
#include "calc.h"

bool E() {
  switch (*next) {
    case NUMBER:   return T() && Ep();
    case OPEN:  return T() && Ep(); 
    default:    fprintf(stderr, "Expected NUMBER or OPEN\n"); return false;
  }
}

bool Ep() {
  switch (*next) {
    case PLUS:  return verify(PLUS) && T() && Ep();
    case MINUS: return verify(MINUS) && T() && Ep();
    case CLOSE: return true;
    case EOL:   return true;
    default: fprintf(stderr, "Expected PLUS or MINUS or CLOSE or EOL\n"); return false;
  }
}

bool T(){
  switch (*next) {
    case NUMBER:  return F() && Tp();
    case OPEN: return F() && Tp();
    default: fprintf(stderr, "Expected NUMBER or OPEN\n"); return false;
  }

}

bool Tp(){
    switch (*next) {
    case PLUS:  return true;
    case MINUS: return true;
    case MULT:  return verify(MULT) && F() && Tp();
    case DIV:   return verify(DIV) && F() && Tp();
    case CLOSE: return true;
    case EOL:   return true;
    default: fprintf(stderr, "Expected PLUS or MINUS or MULT or DIV or CLOSE or EOL\n"); return false;
  }
}

bool F(){
  switch (*next) {
    case NUMBER:   return verify(NUMBER);
    case OPEN:  return verify(OPEN) && E() && verify(CLOSE); 
    default:    fprintf(stderr, "Expected NUMBER or OPEN\n"); return false;
  }
}

bool verify(token tok){
  if (*next != tok) {
    fprintf(stderr, "Invalid token '%c'\n", *next);
    return false;
  }
  next++;
  return true;
}

int main(int argc, char const *argv[])
{

  return 0;
}