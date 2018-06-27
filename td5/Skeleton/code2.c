/*
 * code2.c  -- Production of code for a stack machine
 *
 * Copyright © 2015 Erick Gallesio - I3S-CNRS/Polytech Nice-Sophia <eg@unice.fr>
 *
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 21-Oct-2015 15:31 (eg)
 * Last file update:  4-Nov-2015 17:06 (eg)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "calc.h"
#include "syntax.h"

// ----------------------------------------------------------------------
//    Utilities
// ---------------------------------------------------------------------
#define LABEL(n)   printf("L%03d:\n", n);         // output a label

#define PROD0(op)  printf("\t%s\n", op)
#define PROD1F(op,v)   printf("\t%s\t%g\n", op, v)    // v is a float
#define PROD1S(op,v)   printf("\t%s\t%s\n", op, v)    // v is a string
#define PROD1L(op,v)   printf("\t%s\tL%03d\n", op, v) // v is a label

struct var_cell {   // Type for representing variables
  char *name;
  float value;
  struct var_cell *next;
};

static struct var_cell *all = NULL;

static int label = 0; //

static float get_ident_value(char *id) {
  for (struct var_cell *cur = all; cur; cur = cur->next) {
    if (strcmp(cur->name, id) == 0) return cur->value; 
  }
  error_msg("variable '%s' is unset.\n", id);
  return 0;
}

static float set_ident_value(char *id, float value) {
  for (struct var_cell *cur = all; cur; cur = cur->next) {
    if (strcmp(cur->name, id) == 0) { 
      cur->value = value;
      return value;
    }
  }

  // Var not found create a new cell and place it in front of the 'all' list
  struct var_cell * p = malloc(sizeof(struct var_cell));

  if (!p) {
    error_msg("cannot allocate memory for variable '%s'.\n", id);
    exit(1);
  }
  p->name  = strdup(id);
  p->value = value; 
  p->next  = all;
  all      = p;

  return value;
}


// ----------------------------------------------------------------------
//    Code production
//      This version produces code for a stack machine
// ---------------------------------------------------------------------


float eval(ast_node *n) {
  int current_label;

  if (!n) return 0;

  switch (AST_KIND(n)) {
    case k_number:  PROD1F("push", NUMBER_VALUE(n)); return 0;
    case k_ident:   PROD1S("load", VAR_NAME(n)); return 0;
    case k_operator: {
      ast_node **op = OPER_OPERANDS(n);
      int arity     = OPER_ARITY(n);

      switch (OPER_OPERATOR(n)) {
  /* Expressions */
        case UMINUS:    eval(op[0]); PROD0("negate"); return 0;
        case '+':       eval(op[0]); eval(op[1]); PROD0("add"); return 0;
        case '-':       eval(op[0]); eval(op[1]); PROD0("sub"); return 0;
        case '*':       eval(op[0]); eval(op[1]); PROD0("mul"); return 0;
        case '/':       eval(op[0]); eval(op[1]); PROD0("div"); return 0;
        case '<':       eval(op[0]); eval(op[1]); PROD0("cmplt"); return 0;
        case '>':       eval(op[0]); eval(op[1]); PROD0("cmpgt"); return 0;
        case GE:        eval(op[0]); eval(op[1]); PROD0("cmpge"); return 0;
        case LE:        eval(op[0]); eval(op[1]); PROD0("cmple"); return 0;
        case NE:        eval(op[0]); eval(op[1]); PROD0("cmpne"); return 0;
        case EQ:        eval(op[0]); eval(op[1]); PROD0("cmpeq"); return 0;

  /* Statements */
        case ';':
          if (arity == 0)
            return 0;
          else {
            eval(op[0]);
            return eval(op[1]);
          }

        case KIF:
          current_label = label;
          label+=(arity - 1);
          eval(op[0]);
          PROD1L("jumpz", current_label);
          eval(op[1]);
          if(arity==3)
            PROD1L("jump", current_label+1);
          LABEL(current_label);
          if(arity==3){
            eval(op[2]);
            LABEL(current_label+1);
          }
          return 0;

        case KWHILE :
          LABEL(label);
          eval(op[0]);
          PROD1L("jumpz", label+1);
          eval(op[1]);
          PROD1L("jump", label);
          LABEL(label+1);
          label+=2;
          return 0;

        case KPRINT:  
          eval(op[0]); 
          PROD0("print");
          return 0;
        case KREAD:
          PROD1S("read", VAR_NAME(op[0]));
          return 0;
        case '=': 
          eval(op[1]);
          PROD1S("store", VAR_NAME(op[0]));
          return 0;

        default:
          error_msg("Houston, we have a problem: unattended token %d",
          OPER_OPERATOR(n));
          return 0;
        }
    }
    default:
      return 0;
  }
}

void produce_code(ast_node *n) {
  eval(n);
}