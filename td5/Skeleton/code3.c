  /*
 * code2.c 	-- Production of code for a stack machine
 *
 * Copyright © 2015 Erick Gallesio - I3S-CNRS/Polytech Nice-Sophia <eg@unice.fr>
 *
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 22-Oct-2015 22:48 (eg)
 * Last file update:  4-Nov-2015 17:09 (eg)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "calc.h"
#include "syntax.h"

#define PRODB(name, color, label)		printf("box%d [label=\"%s\", fillcolor=\"%s\"]; \n", name, label, color)
#define PRODBF(name, color, label)		printf("box%d [label=\"%g\", fillcolor=\"%s\"]; \n", name, color, label)
#define PRODBT(from, to)					printf("box%d -> box%d;\n", from, to)
struct var_cell {		// Type for representing variables
  char *name;
  float value;
  struct var_cell *next;
};

static struct var_cell *all = NULL;

static int something = 0;

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
//		Code production
//			This version produces a dot file
// ---------------------------------------------------------------------
float eval(ast_node *n) {
	something++;
	int current = something;
  if (!n) return 0;

  switch (AST_KIND(n)) {
    case k_number:  PRODBF(something, NUMBER_VALUE(n), "tomato"); return 0;
    case k_ident:   PRODB(something, "peru",VAR_NAME(n)); return 0;
    case k_operator: {
      ast_node **op = OPER_OPERANDS(n);
      int arity     = OPER_ARITY(n);

      switch (OPER_OPERATOR(n)) {
	/* Expressions */
		case UMINUS: PRODB(something, "turquoise4", "[_]"); eval(op[0]); PRODBT(current, current+1); return 0;
        case '+':    PRODB(something, "turquoise4", "[+]"); eval(op[0]); PRODBT(current, current+1); eval(op[1]); PRODBT(current, current+2); return 0;
        case '-':    PRODB(something, "turquoise4", "[-]"); eval(op[0]); PRODBT(current, current+1); eval(op[1]); PRODBT(current, current+2); return 0;
        case '*':    PRODB(something, "turquoise4", "[*]"); eval(op[0]); PRODBT(current, current+1); eval(op[1]); PRODBT(current, current+2); return 0;
        case '/':    PRODB(something, "turquoise4", "[/]"); eval(op[0]); PRODBT(current, current+1); eval(op[1]); PRODBT(current, current+2); return 0;
        case '<':    PRODB(something, "turquoise4", "[<]"); eval(op[0]); PRODBT(current, current+1); eval(op[1]); PRODBT(current, current+2); return 0;
        case '>':    PRODB(something, "turquoise4", "[>]"); eval(op[0]); PRODBT(current, current+1); eval(op[1]); PRODBT(current, current+2); return 0;
        case GE:     PRODB(something, "turquoise4", "[<=]"); eval(op[0]); PRODBT(current, current+1); eval(op[1]); PRODBT(current, current+2); return 0;
        case LE:     PRODB(something, "turquoise4", "[>=]"); eval(op[0]); PRODBT(current, current+1); eval(op[1]); PRODBT(current, current+2); return 0;
        case NE:     PRODB(something, "turquoise4", "[!=]"); eval(op[0]); PRODBT(current, current+1); eval(op[1]); PRODBT(current, current+2); return 0;
        case EQ:     PRODB(something, "turquoise4", "[==]"); eval(op[0]); PRODBT(current, current+1); eval(op[1]); PRODBT(current, current+2); return 0;

	/* Statements */
      	case ';':	
		PRODB(something, "turquoise4", "[;]");
			if (arity == 0) 
				return 0;
			else {
			  eval(op[0]);
			  PRODBT(current, current+1);
			  PRODBT(current, something+1);
			  eval(op[1]);
			  return 0;
  			}

        case KIF:
      		PRODB(something, "turquoise4", "if");
      		eval(op[0]);
      		PRODBT(current, current+1);
      		PRODBT(current, something+1);
      		eval(op[1]);
      		if(arity==3){
      			PRODBT(current, something+1);
      			eval(op[2]);
      		}
      		return 0;

        case KWHILE :
        	PRODB(something, "turquoise4", "while");
        	eval(op[0]);
        	PRODBT(current, current+1);
        	PRODBT(current, something+1);
        	eval(op[1]);
        	return 0;

      	case KPRINT: 
      		PRODB(something, "turquoise4", "print");
      		eval(op[0]);
      		PRODBT(current, current+1);
      		return 0;
        case KREAD: 
          PRODB(something,"turquoise4", "read");
          eval(op[0]);
          PRODBT(current, current+1);
          return 0;
      	case '=': 
	      	PRODB(something, "turquoise4", "[=]");
	      	eval(op[0]);
	      	PRODBT(current, current+1);
	      	eval(op[1]);
	      	PRODBT(current, current+2);
	      	return 0;

      	default:
        	error_msg("Houston, we have a problem: unattended token %d|",
      		OPER_OPERATOR(n));
      		return 0;
      }
    }
    
    default:
    	return 0;
  }
}


void produce_code(ast_node *n) {
  printf("digraph E{\n");
  printf("\tnode [style=\"filled\"];\n");
  eval(n);
  printf("}\n");
   
}

