/*
 * lexical.l    -- Flex source of Toy lexical analyzer
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
 *    Creation date: 13-Dec-2014 13:19 (eg)
 * Last file update: 13-Dec-2017 18:40 (eg)
 */

%{
#include <stdlib.h>
#include "toy.h"
#include "ast.h"
#include "utils.h"
#include "syntax.h"


%}
%x COMMENT
%option yylineno

hexint          0x[0-9a-fA-F]+
decint          [0-9]+

pointnum        [0-9]+\.[0-9]*|\.[0-9]+
exponent        [Ee][+-]?[0-9]+
floatnum        {pointnum}{exponent}?

%%

{hexint}|{decint}      {
                          yylval.int_value = strtol(yytext, NULL, 0);
                          return INTEGER;
                        }
{floatnum}              {
                          yylval.float_value = strtof(yytext, NULL);
                          return FLOAT;
                        }

\"([^\"]|\\.)*\"        { yylval.string_value = strdup(yytext); return STRING; }
[-()<>=+*/;{}.%,?:]     { return *yytext; }

">="            return GE;
"<="            return LE;
"=="            return EQ;
"!="            return NE;
"++"            return PP;
"--"            return MM;
"\*\*"          return POW;

"or"            return KOR;
"and"           return KAND;
"not"           return KNOT;
"while"         return KWHILE;
"if"            return KIF;
"else"          return KELSE;
"print"         return KPRINT;
"read"          return KREAD;
"return"        return KRETURN;
"break"         return KBREAK;
"for"           return KFOR;
"class"         return KCLASS;
"extends"       return KEXTENDS;
"new"           return KNEW;
"this"          return KTHIS;
"super"         return KSUPER;
"null"          return KNULL;
"instanceof"    return KINSTOF;

"int"           return TINT;
"float"         return TFLOAT;
"bool"          return TBOOL;
"string"        return TSTRING;
"void"          return TVOID;

"false"         { yylval.int_value = 0; return BOOLEAN; }
"true"          { yylval.int_value = 1; return BOOLEAN; }



                        /* ==== Identifiers ==== */
[a-zA-Z][_a-zA-Z0-9]*       {
                                yylval.ident = strdup(yytext);
                                return IDENTIFIER;
                             }

                        /* ==== Comments ==== */
"/*"                    { BEGIN COMMENT; }
<COMMENT>(.|\n)         ;
<COMMENT>"*/"           { BEGIN 0; }
"//".*          ;       /* monoline comment */





[ \t\r\n]+      ;       /* ignore whitespace */


.               {
                  error_msg(NULL, "Unknown character '%c' (value=%d)\n",
                            *yytext, *yytext);
                }
%%
int yywrap(void) {
    return 1;
}
