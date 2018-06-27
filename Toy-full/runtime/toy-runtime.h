/*
 * toy-runtime.h        -- The Toy runtime header
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
 *    Creation date:  5-Jan-2015 18:23 (eg)
 * Last file update: 16-Dec-2017 22:17 (eg)
 */

#ifndef _TOY_RUNTIME_H_
#define _TOY_RUNTIME_H_
#include <stdio.h>
#include <string.h>

typedef const char *_toy_string;
typedef _toy_string string;


// ----------------------------------------------------------------------
//
//                       Classes stuff
//
// ----------------------------------------------------------------------

// ---- Structure used for toy metaclasses
typedef struct _toy_metaclass {
  char *classname;                      // name of the class
  struct _toy_metaclass *extends_meta;  // metaclass of inherited class
  void* methods [];                     // array of methods addresses
} _toy_metaclass;

extern _toy_metaclass _toy_meta_Object; // The metaclass of Object

// ---- Definition of the Object class
typedef struct Object {
  _toy_metaclass *_instance_of;
} *Object;

// ---- The initialization code for an instance of the "Object" class
Object _init_Object(Object this);


// ---- Object allocation
Object _toy_allocate_object(size_t sz, char*name,  int lineno);
#define _TOY_NEW(_C)                    \
  ((_C) _toy_allocate_object(sizeof(struct _C), #_C, __LINE__))


// ---- Field access
Object _toy_nullaccess(char *file, int line);

#define _TOY_ACCESS(_obj, _klass)                       \
  ({ _klass _tmp_ =  _obj;                              \
    if (! _tmp_) _toy_nullaccess(__FILE__, __LINE__);   \
    _tmp_; })


// ---- Method invocation
#define _TOY_INVOKE(_rettype, _obj, _idx, ...)                             \
  ({ Object _this = (Object) _obj;                                         \
    if (! _this) _toy_nullaccess(__FILE__, __LINE__);                      \
    ((_rettype (*)())(_this->_instance_of->methods[_idx]))(__VA_ARGS__);})

// ---- Object predefined methods
void _Object_printobj(Object this);
void _Object_readobj(Object this);
_toy_string _Object_typename(Object this);

// ---- instanceof operator runtime utility
int _toy_isa(Object this, _toy_string klass_name);



// ----------------------------------------------------------------------
//
//                       I/O functions
//
// ----------------------------------------------------------------------

// ---------- P R I N T ----------
void _toy_print_int(int o);
void _toy_print_float(float o);
void _toy_print_bool(char o);
void _toy_print_string(_toy_string o);
void _toy_print_object(Object o);

// ---------- R E A D ----------
void _toy_read_int(int *o);
void _toy_read_float(float *o);
void _toy_read_bool(char *o);
void _toy_read_string(_toy_string *o);
void _toy_read_object(Object o);

// ----------------------------------------------------------------------
//
//                       Arithmetic functions
//
// ----------------------------------------------------------------------
int _toy_powint(int a, int b);    ///<  computes a^b


#endif /* _TOY_RUNTIME_H_ */
