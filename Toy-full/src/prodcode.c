/*
 * prodcode.c   -- Code Production
 *
 * Copyright © 2014-2017 Erick Gallesio - I3S-CNRS/Polytech'Nice <eg@unice.fr>
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
 *    Creation date: 21-Dec-2014 20:14 (eg)
 * Last file update: 16-Dec-2017 22:14 (eg)
 */

#include <string.h>
#include <stdarg.h>
#include <time.h>

#include "toy.h"
#include "ast.h"
#include "prodcode.h"

FILE *fout;             ///< The file where code is emitted


// ======================================================================
//      Static variables / functions
// ======================================================================
static int margin = 0;                  ///< used for indenting code

/// Increments or decrements the current margin and indents
static void indent(int val) {
  margin += val;
  if (margin>0)
    fprintf(fout, "%*s", 2*margin, " ");
}


/// Produce code for the given code
static void code(ast_node *p) {
  int m;

  if (p) {
    m = margin;
    p->produce_code(p);
    margin = m;
  }
}


/// Write code on the output file. The printf format conventions are accepted
static void emit(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  vfprintf(fout, fmt, ap);
  va_end(ap);
}


/// Produce code of expression eventually with a cast
inline static void code_expr_cast(ast_node *node) {
  if (AST_CAST(node)) {
    emit("(%s)(", TYPE_NAME(AST_CAST(node))); code(node); emit(")");
  } else
    code(node);
}


/// Produce the C name of a function/method
inline static void emit_function_name(ast_node *method) {
  ast_node* klass= FUNCTION_IN_CLASS(method);

  if (klass) emit("_%s_", IDENT_VAL(CLASS_NAME(klass))); // Add prefix if it's a method
  emit(IDENT_VAL(FUNCTION_NAME(method)));                // Toy name
}


// ======================================================================
//
//                              CONSTANTS
//
// ======================================================================
void produce_code_constant(ast_node *node){
  if (AST_TYPE(node) == string_type)
    emit("%s", CONSTANT_STRING_VALUE(node));
  else if (AST_TYPE(node) == float_type)
    emit("%f", CONSTANT_FLOAT_VALUE(node));
  else if (AST_TYPE(node) == null_type)
    emit("NULL");
  else
    emit("%d", CONSTANT_INT_VALUE(node));
}


// ======================================================================
//
//                              IDENTIFIERS
//
// ======================================================================
void produce_code_identifier(ast_node *node){
  ast_node* klass= IDENT_IN_CLASS(node);

  // Produce prefix if there is one
  if (IDENT_PREFIX(node)) {
    ast_node* prefix = IDENT_PREFIX(node);
    char* type       = TYPE_NAME(AST_TYPE(prefix));
    emit("_TOY_ACCESS("); code(prefix); emit(",%s)->", type);
  }

  // Produce code of the identifier
  if (klass) emit("_%s_", IDENT_VAL(CLASS_NAME(klass))); // Add prefix if it's a field
  emit("%s", IDENT_VAL(node));
}


// ======================================================================
//
//                              TYPES
//
// ======================================================================
void produce_code_type(ast_node *node){ emit("%s", TYPE_NAME(node)); }


// ======================================================================
//
//                              VARIABLE DECLARATIONS
//
// ======================================================================
static void produce_declarations(ast_node *node, bool emit_init) {
  /// This function is used for producing variable declaration and class
  /// members declarations.
  struct s_var_decl *n = (struct s_var_decl *) node;
  int len = list_size(n->vars);

  code(AST_TYPE(node)); emit(" ");
  FORLIST(p, n->vars) {
    ast_node *var = ((ast_node **)list_item_data(p))[0];
    ast_node *val = ((ast_node **)list_item_data(p))[1];

    code(var);
    if (emit_init) {      // Initialization code must be issued
      emit(" = ");
      if (val) code_expr_cast(val); else emit(TYPE_DEFAULT_INIT(AST_TYPE(node)));
    }
    if (--len) emit(", ");
  }
  emit(";\n");
}

void produce_code_var_decl(ast_node *node) {
  produce_declarations(node, true);  // Emit declarations and initialize the variables
}


// ======================================================================
//
//                              CLASSES
//
// ======================================================================

// ---- Class declaration ----------------------------------------
static void produce_class_fields(ast_node *klass) {
  if (klass) {
    produce_class_fields(CLASS_PARENT(klass));
    FORLIST(p, CLASS_MEMBERS(klass)) {
      if (AST_KIND(list_item_data(p)) == k_var_decl) {
        emit("  "); produce_declarations(list_item_data(p), false);
      }
    }
  }
}


static void produce_class_struct(ast_node *node, char *name) {
  emit("typedef struct %s *%s; // class %s implementation typedef\n", name,name,name);
  emit("struct %s {\n", name);
  emit("  _toy_metaclass *_instance_of;\n");
  produce_class_fields(node);
  emit("};\n\n");
  // Produce forward declaration of the metaclass sructure
  emit("extern _toy_metaclass _toy_meta_%s; // fully declared below\n\n", name);
}


static void produce_class_constructor(ast_node *node, char *name, List members) {
  // Emit function header
  emit("%s _init_%s(%s this) { // %s class constructor\n", name, name, name, name);

  // Emit a call to the super class initialization
  char *pname = IDENT_VAL(CLASS_NAME(CLASS_PARENT(node)));

  // Emit initialization of the super class and of the instance_of field
  emit("  _init_%s((%s) this);\n", pname, pname);
  emit("  this->_instance_of = &_toy_meta_%s;\n\n", name);

  // Emit direct members initialization
  FORLIST(p, members) {
    if (AST_KIND(list_item_data(p)) == k_var_decl) {
      struct s_var_decl *n = list_item_data(p);

      FORLIST(p, n->vars) {
        ast_node *var = ((ast_node **)list_item_data(p))[0];
        ast_node *val = ((ast_node **)list_item_data(p))[1];

        indent(+1);
        emit("this->"); code(var); emit(" = ");
        if (val) code(val); else emit(TYPE_DEFAULT_INIT(AST_TYPE(n)));
        emit(";\n");
        indent(-1);
      }
    }
  }

  // Emit the end of the constructor
  emit("  return this;\n}\n\n");
}


static void produce_class_metaclass(ast_node *node, char *name){
  char *pname = IDENT_VAL(CLASS_NAME(CLASS_PARENT(node)));
  List vtable = CLASS_VTABLE(node);
  int len     =  list_size(vtable);

  emit("// %s metaclass declaration\n", name);
  emit("_toy_metaclass _toy_meta_%s = {\n", name);
  emit("     .classname    = \"%s\",\n", name);
  emit("     .extends_meta = &_toy_meta_%s,\n", pname);
  emit("     .methods      = {\n");
  FORLIST(p, vtable) {
    emit("        "); emit_function_name(list_item_data(p));
    if (--len) emit(",\n") ;
  }
  emit("}\n};\n\n");
}

static void produce_class_methods(ast_node *node, char *name, List members) {
  emit("// Methods of the class %s\n", name);
  FORLIST(p, members) {
    if (AST_KIND(list_item_data(p)) != k_var_decl) {
      ast_node *method = list_item_data(p);
      code(method);
    }
    emit("\n");
  }
}

void produce_code_class(ast_node *node) {
  char *name   = IDENT_VAL(CLASS_NAME(node));
  List members = CLASS_MEMBERS(node);

  if (!members) {
    // We just have a class declaration (without definition)
    emit("typedef struct %s *%s;\n", name, name);
    return;
  }
  // We have a complete definition of the class
  emit("// ----------------------------------------\n");
  emit("// Class %s implementation\n", name);
  emit("// ----------------------------------------\n\n");
  produce_class_struct(node, name);
  produce_class_constructor(node, name, members);
  produce_class_methods(node, name, members);
  produce_class_metaclass(node, name);
  emit("// ---- End of class %s\n\n", name);
}

// ---- NEW call ----------------------------------------
void produce_code_instance_new(ast_node *node) {
  char *class_name = IDENT_VAL(INSTANCE_NEW_NAME(node));;

  emit("_init_%s(_TOY_NEW(%s))", class_name, class_name);
}

// ---- This designator ------------------------------------
void produce_code_this(ast_node *node) {
  emit("this");
}

// ---- Super designator ------------------------------------
void produce_code_super(ast_node *node) {
  emit("((%s) this)", TYPE_NAME(AST_TYPE(node)));
}

// ======================================================================
//
//                              EXPRESSIONS
//
// ======================================================================
void produce_code_expression(ast_node *node) {
  char *name = EXPRESSION_OPERATOR(node);

  switch (EXPRESSION_ARITY(node)) {
    case 1:
      if (EXPRESSION_KIND(node) == preincr) {
        emit(name); code(EXPRESSION_OP1(node));
      } else if (EXPRESSION_KIND(node) == postincr) {
        code(EXPRESSION_OP1(node)); emit(name);
      } else {
        switch(name[0]) {
          case '-':
          case '!':
            emit("%s", name);
            // NO BREAK HERE !!!!
          case '(':
            emit("("); code(EXPRESSION_OP1(node)); emit(")");
            break;
        }
      }
      break;
    case 2:
      if (EXPRESSION_KIND(node)==comp && AST_TYPE(EXPRESSION_OP1(node))==string_type) {
        // op2 is also a string (analysis has verified that point)
        // ⟹ We are comparing strings, generate a strcmp
        emit("(strcmp("); code(EXPRESSION_OP1(node));
        emit(", ");      code(EXPRESSION_OP2(node));
        emit(") %s 0)", name);
      } else if (strcmp(name, "**") == 0)  {
        // power operator
        emit("_toy_powint("); code(EXPRESSION_OP1(node));
        emit(", ");  code(EXPRESSION_OP2(node));
        emit(")");
      } else if (EXPRESSION_KIND(node)==isa) {
        ast_node * klass = EXPRESSION_OP2(node);
        emit("_toy_isa((Object) ("); code(EXPRESSION_OP1(node));
        emit("), \"%s\")", IDENT_VAL(AST_TYPE(klass)));
      } else {
        // simple binary operators
        code(EXPRESSION_OP1(node));
        emit(" %s ", name);
        code_expr_cast(EXPRESSION_OP2(node));
      }
      break;
    case 3: /* the only ternary operator is expr ? expr : expr */
      code(EXPRESSION_OP1(node)); emit (" ? "); code(EXPRESSION_OP2(node));
      emit(" : "); code(EXPRESSION_OP3(node));
      break;
  }
}

// ======================================================================
//
//                              FUNCTIONS / METHODS
//
// ======================================================================
void produce_code_function(ast_node *node) {
  struct s_function *n = (struct s_function *) node;
  char *class_name = (n->in_class) ?  IDENT_VAL(CLASS_NAME(n->in_class)): NULL;
  int comma = 0;

  // Emit function type and name
  code(AST_TYPE(node)); emit(" "); emit_function_name(node);

  // Produce parameters
  emit("(");
  if (!class_name && list_size(n->parameters) == 0)
    emit("void");
  else {
    // Eventually add the special this parameter
    if (class_name) {
      emit("%s this", class_name);
      comma += 1;
    }
    // produce "normal" parameters
    FORLIST(p, n->parameters) {
      ast_node *v = list_item_data(p);
      if (comma++) emit(", ");
      code(AST_TYPE(v)); emit(" "); code(v);
    }
  }
  emit(")");

  // Body or ';'
  if (n->body) { emit(" "); code(n->body); } else emit (";\n");
}


void produce_code_call(ast_node *node) {
  struct s_call *n = (struct s_call *) node;
  int comma = 0;

  if (IDENT_PREFIX(n->callee) == NULL) {                // ---- Function call
    code(n->callee); emit("(");
  }
  else {                                                // ---- Method call
    ast_node *method = IDENT_QUALIFIED(n->callee);

    if (AST_KIND(IDENT_PREFIX(n->callee)) == k_super) {
      // Static method call (e.g. super.foo(1,23))
      char *klass_id = TYPE_NAME(AST_TYPE(FUNCTION_IN_CLASS(method)));

      emit("_%s_%s(", klass_id, IDENT_VAL(FUNCTION_NAME(method)));
      emit("(%s) this", klass_id);
    } else {
      emit("_TOY_INVOKE("); code(AST_TYPE(method)); emit(", "); // Result type
      code(IDENT_PREFIX(n->callee));                            // this
      emit(", %d", FUNCTION_INDEX(method));                     // method index in vtable
      emit(", _this");                                          // First method parameter
    }
    comma++;                                                  // if method has params
  }

  // produce the parameters
  FORLIST(p, n->parameters) {
    if (comma++) emit(", ");
    code_expr_cast(list_item_data(p));
  }
  emit(")");
}

// ======================================================================
//
//                              STATEMENTS
//
// ======================================================================
void produce_code_if_statement(ast_node *node) {
  struct s_if_statement *n = (struct s_if_statement *) node;

  emit("if ("); code(n->cond); emit(")\n");
  indent(+1); code(n->then_stat);
  if (n->else_stat) {
    indent(-1); emit("else\n");
    indent(+1); code(n->else_stat);
  }
}


void produce_code_while_statement(ast_node *node) {
  struct s_while_statement *n = (struct s_while_statement *) node;

  emit("while ("); code(n->cond); emit(")\n");
  indent(+1); code(n->body_stat);
}


void produce_code_for_statement(ast_node *node) {
  struct s_for_statement *n = (struct s_for_statement *) node;

  emit("{\n");
  // Produce for1  (the code before the while)
  indent(+1); code(n->for1); emit(";\n");

  // produce for2 (the condition of the while
  indent(0); emit("while ("); if (n->for2) code(n->for2); else emit("1"); emit("){\n");

  // Produce the body of the for
  indent(+1); code(n->body_stat); emit("\n");

  // produce the for (incrementation code at end of body
  indent(0); if (n->for3) code(n->for3);  emit(";\n");

  // close the loop and the for block
  indent(-1); emit("}\n");
  indent(-1); emit("}\n");
}


void produce_code_block_statement(ast_node *node) {
  struct s_block_statement *n = (struct s_block_statement *)node;
  int sz = list_size(n->statements);

  emit("{\n"); indent(+1);
  FORLIST(p, n->statements) {
    code(list_item_data(p));
    if (--sz) indent(0);
  }
  indent(-1); emit("}\n");
}


void produce_code_expr_statement(ast_node *node) {
  struct s_expr_statement *n = (struct s_expr_statement *)node;

  if (n->stat) code(n->stat);
  emit(";\n");
}


void produce_code_print_statement(ast_node *node) {
  struct s_print_statement *n = (struct s_print_statement *) node;
  bool first_parameter = true;

  // Note: all the print must be in the same block (think to 'if (C) print(a, b)'
  emit("{\n"); indent(+1);
  FORLIST(p, n->parameters) {
    ast_node *item = list_item_data(p);

    if (! first_parameter) indent(0);
    if (AST_TYPE(item) == int_type) {                           // int
      emit("_toy_print_int(");
    } else if (AST_TYPE(item) == float_type) {                  // float
      emit("_toy_print_float(");
    } else if (AST_TYPE(item) == bool_type) {                   // bool
      emit("_toy_print_bool(");
    } else if (AST_TYPE(item) == string_type) {                 //string
      emit("_toy_print_string(");
    } else {                                                    // object
      emit("_toy_print_object((Object) ");
    }
    code(item);
    emit(");\n");
    first_parameter = false;
  }
  indent(0); emit("fflush(stdout);\n");
  indent(-1); emit("}\n");
}


void produce_code_read_statement(ast_node *node) {
  struct s_read_statement *n = (struct s_read_statement *) node;
  bool first_parameter = true;

  // Note: all the read must be in the same block (think to 'if (C) read(a, b)'
  emit("{\n"); indent(+1);
  FORLIST(p, n->parameters) {
    ast_node *item = list_item_data(p);

    if (! first_parameter) indent(0);
    if (AST_TYPE(item) == int_type) {                           // int
      emit("_toy_read_int(&");
    } else if (AST_TYPE(item) == float_type) {                  // float
      emit("_toy_read_float(&");
    } else if (AST_TYPE(item) == bool_type) {                   // bool
      emit("_toy_read_bool(&");
    } else if (AST_TYPE(item) == string_type) {                 //string
      emit("_toy_read_string(&");
    } else {                                                    // object
      emit("_toy_read_object((Object) ");
    }
    code(item);
    emit(");\n");
    first_parameter = false;
  }
  indent(-1); emit("}\n");
}


void produce_code_return_statement(ast_node *node) {
  struct s_return_statement *n = (struct s_return_statement *) node;
  emit("return");
  if (n->expr) {
    emit(" "); code_expr_cast(n->expr);
  }
  emit(";\n");
}

void produce_code_break_statement(ast_node *node) {
  emit("break;\n");
}

// ======================================================================
//
// produce_code
//
// ======================================================================

void produce_code(ast_node * node) {
  static bool initialized = false;

  if (!initialized) {
    time_t now = time(NULL);

    emit("// Code generated by toy compiler v%s on %s", TOY_VERSION, ctime(&now));
    emit("#include <toy-runtime.h>\n\n");
    initialized = true;
  }
  code(node);
}
