/*
 * symbols.h    -- Symbol Table Management
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
 *    Creation date: 12-Jan-2015 11:16 (eg)
 * Last file update:  9-Dec-2015 12:59 (eg)
 */

#ifndef _SYMBOLS_H_
#define _SYMBOLS_H_


typedef struct symbol_table *Symbol_table;   ///<  The Toy symbol table

/// Create a new symbol table for current scope.
void enter_scope(void);

/// Mark the table for current scope for destruction.
void leave_scope(void);

/// Declare an object (ident or function) in the current block.
void symbol_table_declare_object(char *id, ast_node *ident);

/// Declare a new class.
void symbol_table_declare_class(char *id, ast_node *obj, ast_node *parent);

/// Declare a class member.
void symbol_table_declare_member(char *id, ast_node *obj, ast_node *klass);

/// Search a symbol starting from current scope.
ast_node *symbol_table_search(char *id);

/// Search a symbol starting from current scope but not in global scope.
ast_node *symbol_table_search_blocks(char *id);

/// Search a member of  given class.
ast_node *symbol_table_search_member(char *id, char *class_id);

/// Remove a symbol current scope
void symbol_table_delete(char *id);

/// Free the old (no more used) tables.
void symbol_table_free_unused_tables(void);

/// Initialization of the symbol table module.
void init_symbols(void);

#endif /* _SYMBOLS_H_ */
