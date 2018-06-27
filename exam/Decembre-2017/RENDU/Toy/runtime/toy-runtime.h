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
 * Last file update: 19-Dec-2017 12:26 (eg)
 */

#ifndef _TOY_RUNTIME_H_
#define _TOY_RUNTIME_H_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>

typedef const char *_toy_string;
typedef _toy_string string;


// ----------------------------------------------------------------------
//
//                       Debug macros
//
// ----------------------------------------------------------------------
extern int _trace_lev;           // Trace indentation


// Code added when entering a function in debug mode
#define ENTER_VOID          printf("%*s>> Enter procedure '%s'\n",    \
                                   _trace_lev++, " ", __func__)

#define ENTER_FUNC(_type)   _type _res;                               \
                            printf("%*s>> Enter function '%s'\n",    \
                                   _trace_lev++, " ", __func__)

// Code added when leaving a function in debug mode (void function)
#define LEAVE_VOID        goto _out; /* to avoid warnings if no return */   \
                          _out: printf("%*s<< Leave procedure '%s'\n",      \
                                       --_trace_lev, " ", __func__)

#define LEAVE_FUNC(_type) goto _out; /* to avoid warnings if no return */   \
                          _out: printf("%*s<< Leave function '%s' <-",      \
                                       --_trace_lev, " ", __func__);        \
                          _toy_print_##_type((_type) _res);                 \
                          printf("\n");                                     \
                          return _res;

// Code for returning from a function in debug mode
#define RETURN                     goto _out
#define RETURN_VALUE(_type, _val)  do {_res = (_type) (_val); goto _out; } while(0)


// ----------------------------------------------------------------------
//
//                       TRY-CATCH-FINALLY & THROW
//
// ----------------------------------------------------------------------
extern jmp_buf _env[10];        // FIXME: No control on the length !!!!!!
extern int _index_jmpbuf;

#define THROW    longjmp(_env[_index_jmpbuf-1], 1);

#define TRY      { if (setjmp(_env[_index_jmpbuf++]) == 0) {
#define CATCH    } else {
#define FINALLY  } { _index_jmpbuf--;
#define ENDTRY   } }


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
