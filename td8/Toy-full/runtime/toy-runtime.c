/*
 * toy-runtime.c        -- The Toy programming language runtime library
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
 *    Creation date:  2-Jul-2015 09:44 (eg)
 * Last file update: 13-Dec-2017 20:16 (eg)
 */


#include <stdio.h>
#include <stdlib.h>
#include "toy-runtime.h"


// ----------------------------------------------------------------------
//
//                       Classes stuff
//
// ----------------------------------------------------------------------
Object _init_Object(Object this) {
  this->_instance_of = &_toy_meta_Object;
  return this;
}

static int _toy_instanceof(Object o, char* name){
  if (strcmp(o->_instance_of->classname,name)==0)
    return 1;
  if (o->_instance_of->extends_meta == NULL)
    return 0;
  return _toy_instanceof(o->_instance_of->extends_meta,name);
}

Object _toy_allocate_object(size_t sz, char *name, int lineno) {
  Object res = malloc(sz);

  if (!res) {
    fprintf(stderr, "**** %s instance allocation failed (size=%ld, line = %d)\n",
            name, (long) sz, lineno);
    exit(1);
  }
  return res;
}


Object _toy_nullaccess(char *file, int line) {
  fprintf(stderr, "Null pointer exception in file %s (line %d)\n", file, line);
  exit(1);
  return NULL;  // for the C compiler
}

void _Object_readobj(Object this) {
  // Implementation of Object::printobj()
  printf("not implemented");
}

void _Object_printobj(Object this) {
  // Implementation of Object::printobj()
  printf("<%s instance %p>", this->_instance_of->classname, this);
}

_toy_string _Object_typename(Object this) {
  // Implementation of Object::typename()
  return this->_instance_of->classname;
}


// ---- Object Metaclass -----------------------------------------------
_toy_metaclass _toy_meta_Object = {
  .classname     = "Object",
  .extends_meta  = NULL,
  .methods       = { _Object_printobj, _Object_typename, _Object_readobj }
};


// ----------------------------------------------------------------------
//
//                       I/O functions
//
// ----------------------------------------------------------------------

// ---------- P R I N T ----------

void _toy_print_bool(char o) {
  printf("%s", o? "true": "false");
}

void _toy_print_object(Object o) {
  if (!o)
    printf("<null>");
  else {
    // Simulate the toy call "o.printobj()".
    // We know here that the print method is at index 0. This is a little bit fragile
    _TOY_INVOKE(void, o, 0, _this);
  }
}

// ---------- R E A D ----------
#define READ_BUFF_SIZE 256

void _toy_read_int(int *o) {
  *o = 0;   // Set result to 0 by default
  if (!feof(stdin)) {
    int i;
    if (fscanf(stdin, "%d", &i) == 1) {
      *o = i;
    }
  }
}

void _toy_read_float(float *o) {
  *o = 0.0;   // Set result to 0.0 by default
  if (!feof(stdin)) {
    float  f;
    if (fscanf(stdin, "%f", &f) == 1) {
      *o = f;
    }
  }
}

void _toy_read_bool(char *o) {
  *o = 0;  // Set result to false by default

  if (!feof(stdin)) {
    char *p;
    if (fscanf(stdin, " %m[01a-zA-Z]", &p) == 1) {
      if ((strcasecmp(p, "true")==0) || (strcasecmp(p, "t")==0) || (strcmp(p, "1")==0))
        *o = 1;
      if (!feof(stdin)) fgetc(stdin); // to skip the terminator
      free(p);
    }
  }
}

void _toy_read_string(_toy_string *o) {
  *o = NULL;            // set result to "null" by default

  if (!feof(stdin)) {
    char buffer[READ_BUFF_SIZE];

    if (fgets(buffer, READ_BUFF_SIZE, stdin)) {
      int len = strlen(buffer);
      // Suppress a possible final newline
      if (buffer[len-1] == '\n') buffer[len-1] = '\0';
      *o = strdup(buffer);
    }
  }
}

void _toy_read_object(Object o) {
  _TOY_INVOKE(void, o, 2, _this);
  //fprintf(stderr, "function _toy_read_object on %p must be implemented!!!!\n", o);
  //exit(1);
}

void _toy_instanceof_object(Object o) {
  _TOY_INVOKE(void, o, 2, _this);
  //fprintf(stderr, "function _toy_read_object on %p must be implemented!!!!\n", o);
  //exit(1);
}


// ----------------------------------------------------------------------
//
//                       Arithmetic functions
//
// ----------------------------------------------------------------------
int _toy_powint(int a, int b) {         // function  and b should be unsigned
  unsigned int res = 1;

  while (b) {
    if (b & 1) res *= a;
    a *= a;
    b >>= 1;
  }
  return res;
}
