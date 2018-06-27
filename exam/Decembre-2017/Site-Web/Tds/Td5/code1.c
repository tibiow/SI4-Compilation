/*
 * code1.c      -- Production of code for calc
 *
 * Copyright © 2015-2017 Erick Gallesio - I3S-CNRS/Polytech Nice-Sophia <eg@unice.fr>
 *
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 21-Oct-2015 13:31 (eg)
 * Last file update: 15-Nov-2017 19:30 (eg)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "calc.h"
#include "syntax.h"

// ----------------------------------------------------------------------
//              Variable management
// ----------------------------------------------------------------------
struct var_cell {               // Type for representing variables
  char *name;
  float value;
  struct var_cell *next;
};

static struct var_cell *all = NULL;

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
//              Tree evaluation
// ----------------------------------------------------------------------
float eval(ast_node *n) {
  if (!n) return 0;

  switch (AST_KIND(n)) {
    case k_number:
      return NUMBER_VALUE(n);
    case k_ident:
      return get_ident_value(VAR_NAME(n));
    case k_operator: {
      ast_node **op = OPER_OPERANDS(n);
      int arity     = OPER_ARITY(n);

      switch (OPER_OPERATOR(n)) {
        /* Expressions */
        case UMINUS:    return -eval(op[0]);
        case '+':       return eval(op[0]) +  eval(op[1]);
        case '-':       return eval(op[0]) -  eval(op[1]);
        case '*':       return eval(op[0]) *  eval(op[1]);
        case '/':       return eval(op[0]) /  eval(op[1]);
        case '<':       return eval(op[0]) <  eval(op[1]);
        case '>':       return eval(op[0]) >  eval(op[1]);
        case GE:        return eval(op[0]) >= eval(op[1]);
        case LE:        return eval(op[0]) <= eval(op[1]);
        case NE:        return eval(op[0]) != eval(op[1]);
        case EQ:        return eval(op[0]) == eval(op[1]);

        /* Statements */
        case ';':       if (arity == 0) return 0;
                        else {
                          eval(op[0]);
                          return eval(op[1]);
                        }
        case KPRINT:    printf("%g\n", eval(op[0])); return 0;
        case KREAD:     {
                          float value;
                          printf("%s? ",  VAR_NAME(op[0]));
                          fflush(stdout);
                          scanf("%f", &value);
                          set_ident_value(VAR_NAME(op[0]), value);
                          return 0;
                        }
        case '=':       return set_ident_value(VAR_NAME(op[0]), eval(op[1]));
        case KWHILE:    while (eval(op[0])) eval(op[1]); return 0;
        case KIF:       if (eval(op[0])) eval(op[1]);
                        else
                          if (arity > 2) eval(op[2]);
                        return 0;
        default:        error_msg("Houston, we have a problem: unattended token %d",
                                  OPER_OPERATOR(n));
                        return 0;
        }
    }
    default:
      return 0;
  }
}

// ----------------------------------------------------------------------
//              Code production
//                      This version doesn't produce code in fact.
//                      It directly "interprets" the tree given as
//                      argument
// ---------------------------------------------------------------------
void produce_code(ast_node *n) {
  eval(n);
}
