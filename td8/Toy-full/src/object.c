  /*
 * object.c     -- Object system bootstrap
 *
 * Copyright © 2015 Erick Gallesio - I3S-CNRS/Polytech Nice-Sophia <eg@unice.fr>
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
 *    Creation date: 16-Sep-2015 18:29 (eg)
 * Last file update: 18-Sep-2015 09:18 (eg)
 */

#include "toy.h"
#include "ast.h"
#include "symbols.h"
#include "object.h"

ast_node *Object_class;         ///< Class associated to symbol "Object"


/// Create a (predefined) Object method
static ast_node *create_Object_method(char *name, ast_node *rettype) {
  static int index = 0;
  ast_node *method = make_function(make_identifier(name), rettype, list_create());

  // fill the uninitialized method fields
  FUNCTION_IN_CLASS(method) = Object_class;
  FUNCTION_INDEX(method)    = index++;

  // add method to the Object class
  symbol_table_declare_member(name, method, Object_class);

  return method;
}

/// This function is in charge of bootstrapping the object system. It
/// declares:
///     * the Object class
///     * the Object::printobj method
///     * the Object::typename method
void init_object(void) {
  // ---- Bootstrap the Object system by forging the pseudo Object class
  Object_class = make_class(make_identifier("Object"), NULL, list_create());
  symbol_table_declare_class("Object", Object_class, NULL);

  // ----  Create the methods Object::printobj() and Object::typename()
  ast_node *read_method = create_Object_method("readobj", void_type);
  ast_node *print_method = create_Object_method("printobj", void_type);
  ast_node *type_method  = create_Object_method("typename", string_type);

  // --- make a list of these methods and place it in the Object members and vtable
  List methods = list_create();

  list_append(methods, print_method, NULL);
  list_append(methods, type_method,  NULL);
  list_append(methods, read_method,  NULL);


  CLASS_MEMBERS(Object_class) = CLASS_VTABLE(Object_class) = methods;
}
