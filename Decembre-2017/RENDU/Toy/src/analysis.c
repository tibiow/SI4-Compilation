/*
 * analysis.c   -- AST analysis and typing
 *
 * Copyright © 2015-2017 Erick Gallesio - I3S-CNRS/Polytech Nice-Sophia <eg@unice.fr>
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 * USA.
 *
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 14-Jan-2015 16:55 (eg)
 * Last file update: 18-Dec-2017 19:17 (eg)
 */

#include <string.h>
#include "toy.h"
#include "ast.h"
#include "utils.h"
#include "symbols.h"
#include "object.h"
#include "analysis.h"

/// \file
///
/// This module is in charge of analyzing the ASTs built by the
/// compiler. The main function of this module is analysis() which
/// take an AST as parameter and descends recursively it to fully
/// analyze this AST. Each node of the AST embeds a pointer on a
/// specialized function in charge of the node analysis.


static ast_node *current_function = NULL;
static ast_node *current_class    = NULL;
static int current_loop           = 0;

/// Test if a node is a number (that is an integer or a float
static bool is_number(ast_node *n){
  return AST_TYPE(n) == int_type || AST_TYPE(n) == float_type;
}


///  A type can be a predefined type or a class identifier denoting a
/// type. When the analysis is not able to determine a type
/// (e.g. undeclared class), it is generally set to NULL. the function
/// valid_type() returns true if a type is valid (that is if it is not
/// NULL or if its type is not null when it is an identifier.
static bool valid_type(ast_node *type) {
  return (type && AST_TYPE(type));
}


/// The function compatible_types tests if e2 is compatible with e1.
/// Generally, e1 is on the left and e2 on the right (e.g "e1 = e2").
/// Let t1 and t2 be the types of e1 and e2, the function return true
/// iff:
///       - t1 and t2 are the same standard type
///       - t1 and t2 denotes classes names  and t2 has t1 in its
///         inheritance tree
///
/// **Warning**: This function may change the AST_CAST field of e2. In effect,
/// we try  also to see here if a cast is needed. If it it is the case,
/// e2 has its AST_CAST member which is changed to denotes the AST_TYPE of e1.
/// The code production pass will probably use this information


static bool compatible_types_for_classes(ast_node *t1, ast_node *t2) {
  // If type names are identical, t1 and t2 are compatible (ugly and not very
  // efficient. We should use a same descriptor in this case). Must be fixed?
  if (strcmp(TYPE_NAME(t1), TYPE_NAME(t2)) == 0) return true;

  // If t1 is exactly an Object, everybody is compatible to it (but needs a cast)
  if (AST_TYPE(t1) == Object_class) return true;

  // if t2 is exactly an Object return false since t1 is not exactly an Object
  if (AST_TYPE(t2) == Object_class) return false;

  // See if t2 has t1 in its inheritance list
  ast_node *klass  = symbol_table_search(TYPE_NAME(t2));
  ast_node *parent = symbol_table_search(IDENT_VAL(CLASS_NAME(CLASS_PARENT(klass))));

  if (parent!=Object_class) return compatible_types_for_classes(t1, AST_TYPE(parent));

  return false;
}


static bool compatible_types(ast_node *e1, ast_node *e2) {
  ast_node *t1 = AST_TYPE(e1), *t2 = AST_TYPE(e2);

  if (!valid_type(t1) || !valid_type(t2)) // to avoid error cascades => true
    return true;

  if (t1 == t2) return true;               // same types => true

  // Testing a mix of integer and float computations
  if ((t1 == int_type   && t2 == float_type) ||
      (t1 == float_type && t2 == int_type))    return true;

  // Testing if t1 is an object and t2 is null
  if (!TYPE_IS_STANDARD(t1) && (t2 == null_type)) return true;

  // Testing objects
  if (! TYPE_IS_STANDARD(t1) && !TYPE_IS_STANDARD(t2)) { // both are objects
    if (compatible_types_for_classes(t1, t2)) {
      if (t1 != t2) AST_CAST(e2) = t1;
      return true;
    }
  }

  // Since we could not return true, return false
  return false;
}



// ======================================================================
//
//                              CONSTANTS
//
// ======================================================================
void analysis_constant(ast_node *node){ /* constants are typed when created */ }

// ======================================================================
//
//                              IDENTIFIERS
//
// ======================================================================

static void prefix_analysis(ast_node *node, char *id){
  // Type the prefix
  analysis(IDENT_PREFIX(node));

  // Verify that the prefix is a class instance
  ast_node *prefix_type = AST_TYPE(IDENT_PREFIX(node));

  AST_TYPE(node) = NULL;        // will be changed if no error is detected

  if (!valid_type(prefix_type)) return;

  if (TYPE_IS_STANDARD(prefix_type)) {
    error_msg(node, "prefix of '%s' is not a class instance", id);
    return;
  }

  // Verify that the class has the given identifier as field
  ast_node *member = symbol_table_search_member(id, TYPE_NAME(prefix_type));

  if (!member) {
    error_msg(node, "member '%s' does not exist in class '%s'", id,
                    TYPE_NAME(prefix_type));
    return;
  }

  // Copy the class definition of member in the identifier
  IDENT_IN_CLASS(node) = IDENT_IN_CLASS(member);

  // Fill the member slot for further analyzes and type the node
  IDENT_QUALIFIED(node) = member;
  AST_TYPE(node)        = AST_TYPE(member);
}


static void simple_identifier_analysis(ast_node *node, char *id){
  // Try to see if "id" is a field of the current class.
  // If so, add "this" as (an implicit prefix)
  if (current_class) {
    char * class_name = IDENT_VAL(CLASS_NAME(current_class));
    ast_node *member  = symbol_table_search_member(id, class_name);
    ast_node *local   = symbol_table_search_blocks(id);
    if (!local && member) {
      // add the prefix "this" to current object
      IDENT_PREFIX(node) = make_this();
      prefix_analysis(node, id); // A little bit overkill, but safer
      return;
    }
  }
  // Identifier is really a non qualified one 
  ast_node *var = symbol_table_search(id);

  AST_TYPE(node) = int_type; // for now
  if (!var) {
    // var is undeclared. Declare it as an integer to avoid further errors
    symbol_table_declare_object(id, node);
    error_msg(node, "%s is not declared", id);
  }
  else if (AST_KIND(var) == k_function) {
    // var is declared but as a function.
    error_msg(node, "%s function used as variable", id);
  }
  else if (AST_KIND(var) == k_class) {
    // var is declared as a class
    AST_TYPE(node) = AST_TYPE(var);
  }
  else if (AST_KIND(var) == k_identifier) {
    // Set type of the node to the type of the identifier in the table
    AST_TYPE(node) = AST_TYPE(var);
  }
  else {
    die("fatal error when analyzing an identifier");
  }
}

void analysis_identifier(ast_node * node) {
  char *id = IDENT_VAL(node);

  if (IDENT_PREFIX(node)) prefix_analysis(node, id );
  else                    simple_identifier_analysis(node, id);
}

// ======================================================================
//
//                              TYPES
//
// ======================================================================
void analysis_type(ast_node *node){
  if (TYPE_IS_STANDARD(node)) return;  // Standard type are correct

  // Type is an ident. It must denote a class
  ast_node *k = symbol_table_search(TYPE_NAME(node));

  if (!k) {
    error_msg(node, "class '%s' is not declared", TYPE_NAME(node));
    AST_TYPE(node) = NULL;
  } else if (AST_KIND(k) != k_class) {
    error_msg(node, "%s does not denote a class", TYPE_NAME(node));
    AST_TYPE(node)  = NULL;
  } else
    AST_TYPE(node) = k;
}


// ======================================================================
//
//                              VARIABLE DECLARATIONS
//
// ======================================================================
static int is_const(ast_node *node) {
  /// Determine if node denotes a constant expression
  if (AST_KIND(node) == k_constant)
    return 1;
  if (AST_KIND(node) == k_expression) {
    int c1, c2=1, c3=1;
    switch (EXPRESSION_ARITY(node)) {
        case 3: c3 = is_const(EXPRESSION_OP3(node)); // no break !!!
        case 2: c2 = is_const(EXPRESSION_OP2(node)); // no break !!!
        case 1: c1 = is_const(EXPRESSION_OP1(node)); break;
        default: die("weird operator arity");
    }
    return c1 && c2 && c3;
  }
  // If we are here, node doesn't denote a constant
  return 0;
}


static void decl_locals(ast_node *node, ast_node *in_class) {
  analysis(AST_TYPE(node));

  // Verify that we don't declare void vars/members
  if (AST_TYPE(node) == void_type)
    error_msg(node, "void is an improper type here");

  // Enter all the vars/members in the symbol table
  bool global_decl = (!current_class && !current_function);

  FORLIST(p, VARDECL_VARS(node)) {
    // p denote a couple with a variable identifier and initial value
    ast_node *var = ((ast_node **)list_item_data(p))[0];
    ast_node *val = ((ast_node **)list_item_data(p))[1];

    // If in_class is NULL we declare a variable. Otherwise, node denotes a
    // member symbol which must be declared in the class hash table
    if (in_class) {
      symbol_table_declare_member(IDENT_VAL(var), var, in_class);
      IDENT_IN_CLASS(var) = in_class;
    }
    else
      symbol_table_declare_object(IDENT_VAL(var), var);

    // type the expression and verify that the type is conform to declaration
    if (val) {
      analysis(val);
      if (!compatible_types(node, val))
        error_msg(node, "initializer type incompatible with '%s'", IDENT_VAL(var));
      if (global_decl && !is_const(val))
        error_msg(node, "initializer for '%s' is not constant", IDENT_VAL(var));
    }
 }
}


void analysis_var_decl(ast_node *node) {
  decl_locals(node, NULL);
}

// ======================================================================
//
//                              CLASSES
//
// ======================================================================

//  ---- Class declaration ----------------------------------------
static void build_class_vtable(ast_node *klass);

void analysis_class(ast_node *node) {
  ast_node *klass  = CLASS_NAME(node);
  ast_node *parent = NULL;
  List members     = CLASS_MEMBERS(node);
  char *name       = IDENT_VAL(klass);

  // Search the given name in the symbol table
  ast_node *prev = symbol_table_search(name);

  // Verify that class was not already defined
  if (prev) {
    if (AST_KIND(prev) == k_class) {
      if (CLASS_MEMBERS(prev))
        error_msg(node, "class '%s' was already defined", name);
    } else
      error_msg(node, "class '%s' cannot be defined (symbol already used)", name);
    return;
  }

  // Class was not defined before
  if (CLASS_PARENT(node)) {
    // Verify that the parent class exists
    char *pname = IDENT_VAL(CLASS_PARENT(node));

    parent = symbol_table_search(pname);

    if (!parent)
      error_msg(node, "parent class '%s' does not exist", pname);
    else if (AST_KIND(parent) != k_class) {
      error_msg(node, "%s is not a class", pname);
      parent = NULL;            // to continue analysis further
    } else {
      // Correct the parent with the class (instead of its name)
      free_node(CLASS_PARENT(node));  // it was a simple identifier
      CLASS_PARENT(node) = parent;    // Set the parent to the class itself instead
    }
  }

  if (!parent)
    // No class was givent (or error detected) => parent is "Object"
    CLASS_PARENT(node) = parent = Object_class;

  // declare the class and its members
  symbol_table_declare_class(name, node, parent);
  if (members) {
    current_class = node;
    FORLIST(p, CLASS_MEMBERS(node)) {
      ast_node *member = list_item_data(p);

      if (AST_KIND(member) == k_var_decl)
        decl_locals(member, node);      // member is a field list
      else
        analysis_function(member);      // member is a method
    }

    // Build the class vtable
    build_class_vtable(current_class);

    current_class = NULL;
  }
}

// ---- Creation of the class vtable ------------------------------
static void build_class_vtable_unsorted(ast_node *klass, List lst) {
  if (klass) {
    // Recurse
    build_class_vtable_unsorted(CLASS_PARENT(klass), lst);
    FORLIST(p, CLASS_MEMBERS(klass)) {
      if (AST_KIND(list_item_data(p)) == k_function) {
        list_append(lst, list_item_data(p), NULL);
      }
    }
  }
}

static void build_class_vtable(ast_node *klass) {
  List tmp = list_create();
  int index = 0;

  build_class_vtable_unsorted(klass, tmp);
  // We had running down the hierarchy and tmp may contain duplicates.
  // Build a sorted set of methods
  List vtable = list_create();

  FORLIST(p, tmp) {
    ast_node *method  = list_item_data(p);
    char *method_name = IDENT_VAL(FUNCTION_NAME(method));
    ast_node *last    = NULL;
    int occurence     = 0;

    FORLIST(q, tmp) {
      ast_node *candidate = list_item_data(q);

      if (strcmp(method_name, IDENT_VAL(FUNCTION_NAME(candidate))) == 0) {
        last = candidate;
        occurence += 1;
      }
    }
    if ((occurence == 1) || (occurence > 1 && method != last)) {
      FUNCTION_INDEX(last) = index++;
      list_append(vtable, last, NULL);
    }
  }

  list_destroy(tmp);
  CLASS_VTABLE(klass) = vtable;
}


//  ---- Instance creation ----------------------------------------
void analysis_instance_new(ast_node *node) {
  analysis(AST_TYPE(node));
}

//  ---- This designator -------------------------------------------
void analysis_this(ast_node *node) {
  if (!current_class) {
    error_msg(node, "usage of 'this' outside of a method");
    AST_TYPE(node) = NULL;
  } else {
    AST_TYPE(node) = make_type(IDENT_VAL(CLASS_NAME(current_class)), "NULL");
  }
}

//  ---- Super designator -------------------------------------------
void analysis_super(ast_node *node) {
  if (!current_class) {
    error_msg(node, "usage of 'super' outside of a method");
    AST_TYPE(node) = NULL;
  } else {
    AST_TYPE(node) = make_type(IDENT_VAL(CLASS_NAME(CLASS_PARENT(current_class))), "NULL");
  }
}

// ======================================================================
//
//                              EXPRESSIONS
//
// ======================================================================
void analysis_expression(ast_node *node) {
  char     *name = EXPRESSION_OPERATOR(node);
  ast_node  *op1 = EXPRESSION_OP1(node);
  ast_node  *op2 = EXPRESSION_OP2(node);  /* may be NULL */
  ast_node  *op3 = EXPRESSION_OP3(node);  /* may be NULL */

  switch (EXPRESSION_ARITY(node)) {
   case 1: analysis(op1);
            switch(EXPRESSION_KIND(node)) {
              case preincr:
              case postincr:
              case uarith:
                if (!is_number(op1))
                  error_msg(node, "operand of `%s' must be a number", name);
                AST_TYPE(node) = int_type;
                break;
              case ulogic:
                if (AST_TYPE(op1) != bool_type)
                  error_msg(node, "operand of 'not' must be bool");
                AST_TYPE(node) = bool_type;
                break;
              case parenthesis:
                AST_TYPE(node) = AST_TYPE(op1);
                break;
              default: die("unhandled unary expression");
            }
            break;

    case 2: analysis(op1); analysis(op2);
            if (!AST_TYPE(op1) || !AST_TYPE(op2)) //
              // if one of the types is NULL, we consider that this is compatible
              // It arises generally when a variable is auto-declared
              break;

            // Special test for isa
            if (EXPRESSION_KIND(node) == isa) {
              if (TYPE_IS_STANDARD(AST_TYPE(op1))) {
                error_msg(node, "first operand of instanceof is not an object");
              }
              if (TYPE_IS_STANDARD(AST_TYPE(op2))) {
                error_msg(node, "second operand of instanceof is not an object");
              }

              AST_TYPE(node) = bool_type;
              break;
            }

            if (! compatible_types(op1, op2))
              error_msg(node, "operands of %s have incompatible types",
                              !strcmp(name, "=")? "assignment" : name);

            switch(EXPRESSION_KIND(node)) {
              case assign:
                AST_TYPE(node)= AST_TYPE(op2);
                break;
              case barith:
                if (!is_number(op1) || !is_number(op2))
                  error_msg(node, "operands of %s must be numbers", name);
                AST_TYPE(node) = (AST_TYPE(op1)== float_type ||
                                  AST_TYPE(op2) == float_type)? float_type: int_type;
                break;
              case blogic:
                if (AST_TYPE(op1) != bool_type || AST_TYPE(op2) != bool_type)
                  error_msg(node, "operands of %s must be bool", name);
                AST_TYPE(node) = bool_type;
                break;
              case comp:
                AST_TYPE(node) = bool_type;
                break;
              default: die("unhandled binary expression");
            }
            break;
    case 3: // the only ternary operator is expr ? expr : expr
            analysis(op1);analysis(op2); analysis(op3);
            if (AST_TYPE(op1) != bool_type)
              error_msg(node, "test is not boolean in a '?:' expression");
            else {
              if (AST_TYPE(op2) != AST_TYPE(op3))
                // NOTE: en fait c'est bien plus difficle que ça....
                error_msg(node, "branches of '?:' are of different types");
            }
            AST_TYPE(node) = AST_TYPE(op2);
            break;
  }
}


// ======================================================================
//
//                              FUNCTIONS
//
// ======================================================================
static int parameter_cmp(List formal, List effective, char *id, ast_node *node) {
  List_item  p1 = list_head(formal);
  List_item  p2 = list_head(effective);
  int       err = 0;

  for (int idx=1; p1 && p2; p1 = list_item_next(p1), p2 = list_item_next(p2), idx++){
    if (!compatible_types(list_item_data(p1), list_item_data(p2))) {
      error_msg(node, "type of parameter #%d is not compatible with "
                      "previous definition of '%s'", idx, id);
      err += 1;
    }
  }

  if (p1 && !p2) {
    error_msg(node, "call or definition of '%s' has not enough parameters", id);
    err++;
  }
  if (!p1 && p2) {
    error_msg(node, "call or definition of '%s' has too much parameters", id);
    err++;
  }
  return !err;
}


static void analysis_formal(List formal) {
  int pos = 0, len  = list_size(formal);
  char *names[len];

  FORLIST(p, formal) {
    ast_node *param= list_item_data(p);

    // verify formal parameter type
    if (AST_TYPE(param) == void_type)
      error_msg(param, "type of parameter cannot be void");
    else
      analysis(AST_TYPE(param));

    // Verify that this parameter was not previously used
    for (int i = 0; i < pos; i ++) {
      if (strcmp(IDENT_VAL(param), names[i]) == 0) {
        error_msg(param, "name '%s' was already used for paramater #%d",
                         IDENT_VAL(param), i+1);
        break;
      }
    }
    // Retain the current parameter name for further controls
    names[pos++] = IDENT_VAL(param);
  }
}

// ----------------------------------------------------------------------
// Function / Method Declaration
// ----------------------------------------------------------------------
static void analysis_function_body(ast_node *node) {
  struct s_function *n = (struct s_function *) node;

  // Declare the parameters in a new scope
  enter_scope();
  FORLIST(p, n->parameters) {
    ast_node *param= list_item_data(p);
    symbol_table_declare_object(IDENT_VAL(param), param);
  }

  // Analyze the statements of the function (don't use analysis_block_statement
  // which enter a new scope)
  struct s_block_statement *body = (struct s_block_statement *) n->body;

  current_function = node;
  FORLIST(p, body->statements) analysis(list_item_data(p));
  current_function = NULL;

  leave_scope();
}


void analysis_function(ast_node *node) {                // Used also for methods
  struct s_function *n = (struct s_function *) node;
  char *id             = IDENT_VAL(n->name);

  n->in_class = current_class;                  // Set the embedding class (if any)
  analysis(AST_TYPE(node));                     // Verify the return type
  analysis_formal(FUNCTION_PARAMETERS(node));   // Verify the formal parameter list

  // Declare  the function/method
  if (current_class) {                          // **** This is a METHOD declaration
    char *kid      = IDENT_VAL(CLASS_NAME(current_class));
    bool error     = false;
    ast_node *prev = symbol_table_search_member(id, kid);

    if (prev) {
      if (FUNCTION_IN_CLASS(prev) == current_class) {
        // Verify that the previous method was not declared in the current class
        error_msg(node, "method %s was already declared in this class", id);
        error = true;
      } else if (AST_KIND(prev) != k_function) {
        // Verify that previous declaration was not a field
        error_msg(node, "inherited %s was not declared as a method", id);
        error = true;
      } else if (! parameter_cmp(FUNCTION_PARAMETERS(node),FUNCTION_PARAMETERS(prev),
                                 id,node)){
        // Verify that previous and current prototypes are compatible
        error_msg(node, "parameters of %s incompatible with previous declaration", id);
        error = true;
      }
      else if (! compatible_types(prev, node)) {
        // Verify that return type of current method is compatible with inherited one
        error_msg(node, "method return type is not compatible with overridden method");
        error = true;
      }
    }
    if (!error) // No error → declare the method in the current class
      symbol_table_declare_member(id, node, current_class);
  }
  else {                                        // **** This is a FUNCTION declaration
    ast_node *prev = symbol_table_search(id);
    if (prev && (AST_KIND(prev) == k_function) && (FUNCTION_BODY(prev) == NULL)) {
      // prev was a prototype
      parameter_cmp(FUNCTION_PARAMETERS(prev),FUNCTION_PARAMETERS(node),id,node);
      if (strcmp(TYPE_NAME(AST_TYPE(prev)), TYPE_NAME(AST_TYPE(node))) != 0)
        error_msg(node, "function return type is different from '%s' prototype", id);
      // Keep the new  definition rather than old prototype
      free_node(prev);
      symbol_table_delete(id);
    }
    symbol_table_declare_object(id, node);
  }

  // Body analysis
  if (n->body) analysis_function_body(node);
}


// ----------------------------------------------------------------------
// Function / Method Call
// ----------------------------------------------------------------------
static void analysis_method_call(ast_node *node) {
  struct s_call *n = (struct s_call *) node;
  ast_node *callee = n->callee;

  // Type the callee and the parameter list
  analysis(callee);
  FORLIST(p, n->parameters) analysis(list_item_data(p));

  if (!AST_TYPE(callee)) {
    // Error in previous analysis
    AST_TYPE(node) = NULL;
    return;
  }

  ast_node *func = IDENT_QUALIFIED(callee);
  char *id       = IDENT_VAL(callee);

  if (!func || AST_KIND(func) != k_function)
    // The callee was a member of the class but not a method
    error_msg(node, "member '%s' is not a method of class '%s'", id,
                    TYPE_NAME(AST_TYPE(IDENT_PREFIX(callee))));
  else {
    parameter_cmp(FUNCTION_PARAMETERS(func), CALL_PARAMETERS(n), id, node);
    AST_TYPE(node) = AST_TYPE(callee);
  }
}


static void analysis_function_call(ast_node *node) {
  struct s_call *n = (struct s_call *) node;

  // Simple function call: Search function name
  char *id = IDENT_VAL(n->callee);
  struct ast_node *func = symbol_table_search(id);

  if (!func || (func && AST_KIND(func) != k_function))
    error_msg(node, "function %s is undefined", id);
  // if !func, we could declare it. Refrain to do so: which type?, which params?
  else
    // func is declared as a function. Set type of node to its return type
    AST_TYPE(n) = AST_TYPE(func);

  // Type the parameter list
  FORLIST(p, n->parameters) analysis(list_item_data(p));

  if (func)
    // Compare formal parameter list with effective list
    parameter_cmp(FUNCTION_PARAMETERS(func), CALL_PARAMETERS(n), id, node);
}


void analysis_call(ast_node *node) {
  if (IDENT_PREFIX(CALL_CALLEE(node)))
    analysis_method_call(node);
  else
    analysis_function_call(node);
}

// ======================================================================
//
//                              STATEMENTS
//
// ======================================================================
void analysis_if_statement(ast_node *node) {
  struct s_if_statement *n = (struct s_if_statement *) node;

  // Analyze condition
  analysis(n->cond);
  if (AST_TYPE(n->cond) != bool_type) error_msg(node, "condition must be boolean");

  // Analyze if and then parts
  analysis(n->then_stat);
  analysis(n->else_stat);
}


void analysis_while_statement(ast_node *node) {
  struct s_while_statement *n = (struct s_while_statement *) node;

  // Analyze condition
  analysis(n->cond);
  if (AST_TYPE(n->cond) != bool_type) error_msg(n->cond, "condition must be boolean");

  // Retain that we are in a loop (for breaks) and analyze loop body
  current_loop += 1;
  analysis(n->body_stat);
  current_loop -= 1;
}


void analysis_for_statement(ast_node *node) {
  struct s_for_statement *n = (struct s_for_statement *) node;

  enter_scope();
  // Analyze the three components of the "condition"
  analysis(n->for1); analysis(n->for2); analysis(n->for3);
  if (n->for2 && (AST_TYPE(n->for2) != bool_type))
    error_msg(n->for2, "condition must be boolean");

  // Retain that we are in a loop (for breaks) and analyze loop body
  current_loop += 1;
  analysis(n->body_stat);
  current_loop -= 1;
  leave_scope();
}


void analysis_block_statement(ast_node *node) {
  enter_scope();
  list_for_each(((struct s_block_statement *) node)->statements,
                (list_iterator) analysis);
  leave_scope();
}


void analysis_expr_statement(ast_node *node) {
  analysis(((struct s_expr_statement *)node)->stat);
}

void analysis_print_statement(ast_node *node) {
  struct s_print_statement *n = (struct s_print_statement *) node;
  // Type the parameter list
  FORLIST(p, n->parameters) analysis(list_item_data(p));
}

void analysis_read_statement(ast_node *node) {
  struct s_read_statement *n = (struct s_read_statement *) node;
  // Type the parameter list
  FORLIST(p, n->parameters) analysis(list_item_data(p));
}

void analysis_return_statement(ast_node *node) {
  struct s_return_statement *n = (struct s_return_statement *) node;
  analysis(n->expr);

  if (n->expr) {
    // Return with an expression
    if (AST_TYPE(current_function) == void_type)
      error_msg(node, "current function is void; expression unexpected here");
    else
      if (!compatible_types(current_function, n->expr))
        error_msg(node, "type of expression is incompatible with function type");
  } else {
    // No expression in the return
    if (AST_TYPE(current_function) != void_type)
      error_msg(node, "expression expected after return");
  }
}

void analysis_break_statement(ast_node *node) {
  if (!current_loop)
    error_msg(node, "break used outside a loop");
}

void analysis_try_statement(ast_node *node) {
  // ...
}

void analysis_throw_statement(ast_node *node) {
  // ....
}


// ======================================================================

/// The main analysis function. This function will launch the analysis
/// on the abstract tree given as parameter. The analysis will travel
/// recursively the tree, by launching an analysis on the encountered
/// nodes.
void analysis(ast_node *node) {
  if (!node) return;
  if (!AST_ANALYSIS(node)) {
    fprintf(stderr, "No analysis function on node %p (%d) \n", node, AST_KIND(node));
    exit(1);
  }
  AST_ANALYSIS(node)(node);
}
