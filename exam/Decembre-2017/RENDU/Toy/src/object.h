/*
 * object.h     -- Object System Bootstrap
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
 *    Creation date: 16-Sep-2015 18:31 (eg)
 * Last file update: 18-Sep-2015 09:13 (eg)
 */

#ifndef _OBJECT_H_
#define _OBJECT_H_

extern ast_node *Object_class;          ///< Class associated to symbol "Object"
void init_object(void);                 ///< Bootstrap the Object System

#endif /* _OBJECT_H_ */
