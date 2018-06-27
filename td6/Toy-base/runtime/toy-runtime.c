/*
 * toy-runtime.c        -- The Toy programming language runtime library
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
 *    Creation date:  2-Jul-2015 09:44 (eg)
 * Last file update: 18-Sep-2015 12:18 (eg)
 */


#include <stdio.h>
#include <stdlib.h>
#include "toy-runtime.h"


// ----------------------------------------------------------------------
//
//                       Print functions
//
// ----------------------------------------------------------------------
void _toy_print_bool(char o) {
  printf("%s", o? "true": "false");
}

int _toy_powint(int a, int b) {         // a and b should be unsigned
  unsigned int res = 1;

  while (b) {
    if (b & 1) res *= a;
    a *= a;
    b >>= 1;
  }
  return res;
}

char* _my_strcat(char* s1, char* s2){
	char* reponse = malloc(strlen(s1)+strlen(s2)+1);	
	strcpy(reponse,s1);
	strcat(reponse,s2);
	return reponse;
}
