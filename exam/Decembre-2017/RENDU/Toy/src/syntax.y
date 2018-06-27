/*
 * syntax.y     -- Bison source of Toy syntaxic analyzer
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
 * Last file update: 18-Dec-2017 19:23 (eg)
 */


// ======================================================================
//              C code
// ======================================================================
%{
#include <list.h>
#include "toy.h"
#include "ast.h"
#include "syntax.h"
#include "prodcode.h"
#include "analysis.h"
#include "symbols.h"
#include "utils.h"


#define FREE_NODE       (((void (*)(void *)) free_node))  // to symplify writing

extern int yylex(void);         ///< the tokenizer build by Flex/Lex


#define YYERROR_VERBOSE 1
static void yyerror(const char *s) {
    error_msg(NULL, (char *) s);
}


static void analysis_and_code(ast_node *node) {
  analysis(node);
  if (!error_detected) produce_code(node);
  free_node(node);
  symbol_table_free_unused_tables();
}

static int exit_compiler(int errors) {
  if (errors)
    fprintf(stderr, "*** %d error%s detected\n", errors, (errors > 1) ? "s": "");

  return errors; // return code = number of seen errors
}

%}

// ======================================================================
//              Yacc/Bison declarations
// ======================================================================

%union {
    int int_value;              // constant integer or boolean value
    float float_value;          // constant float value
    char *string_value;         // constant string value
    char *ident;                // identifier
    List lst;                   // used for lists of parameters (formal or effective)
    ast_node *ast;              // node pointer
};

%token <int_value>    INTEGER BOOLEAN
%token <float_value>  FLOAT
%token <string_value> STRING
%token <ident> IDENTIFIER
%token KWHILE KIF KPRINT KRETURN KBREAK KFOR KREAD
%token TINT TFLOAT TBOOL TSTRING TVOID
%token KCLASS KNEW KEXTENDS KTHIS KNULL KSUPER KCONST
%token KTRY KCATCH KFINALLY KTHROW 


%type   <ast>   declarations declaration
%type   <ast>   stmt expr var_decl var_decl_readable type var identifier call prefix designator
%type   <ast>   class_decl attr_decl inherit
%type   <ast>   func_decl prototype expr_opt init_var
%type   <ast>   for1 for2 for3
%type   <ast>   try_catch catch finally
%type   <lst>   fparam_list eparam_list stmt_list var_list
%type   <lst>   members_list

%nonassoc IFX
%nonassoc KELSE

%right '='
%right '?' ':'
%left KOR
%left KAND
%left '>' '<' GE LE 
%left EQ NE
%nonassoc KINSTOF
%left '+' '-'
%left '*' '/' '%'
%left KNOT UMINUS
%left PP MM
%right POW


%%

// ======================================================================
//              Grammar
// ======================================================================
program:        declarations            { return exit_compiler(error_detected); }
        ;


declarations: declarations declaration  { analysis_and_code($2);}
        |     declaration               { analysis_and_code($1);}
        ;

declaration:    var_decl ';'            { }
        |       func_decl               { }
        |       class_decl              { }
        ;

stmt:           ';'                     { $$ = make_expr_statement(NULL); }
        |       expr ';'                { $$ = make_expr_statement($1); }
        |       KPRINT '(' eparam_list ')' ';'
                                        { $$ = make_print_statement($3); }
        |       KREAD '(' var_list ')' ';'
                                        { $$ = make_read_statement($3); }

        |       '{' stmt_list '}'       { $$ = make_block_statement($2); }
        |       KWHILE '(' expr ')' stmt
                                        { $$ = make_while_statement($3, $5); }
        |       KFOR '(' for1 ';' for2 ';' for3 ')' stmt
                                        { $$ = make_for_statement($3, $5, $7, $9); }
        |       try_catch               { $$ = $1; }
        |       KTHROW ';'              { $$ = make_throw_statement(); }
        |       KRETURN expr_opt ';'    { $$ = make_return_statement($2); }
        |       KBREAK ';'              { $$ = make_break_statement(); }
        |       KIF '(' expr ')' stmt %prec IFX
                                        { $$ = make_if_statement($3, $5, NULL); }
        |       KIF '(' expr ')' stmt KELSE stmt
                                        { $$ = make_if_statement($3, $5, $7); }
        |       var_decl ';'            { $$ = $1;}
        |       KCONST var_decl_readable ';'     { $$ = $2;}
        |       prototype ';'           { $$ = $1;}
        |       error ';'               { yyerrok; $$ = NULL; }
        ;


stmt_list:      stmt_list stmt          { list_append($1, $2, FREE_NODE); $$ = $1; }
        |       /* empty */             { $$ = list_create();  }
        ;

var_decl_readable:      var_decl_readable ',' var init_var { add_variable_to_decl($1, $3, $4); $$ = $1; }
        |       type var init_var       { $$ = make_var_decl_readable($2, $1, $3); }
        ;

var_decl:       var_decl ',' var init_var
                                        { add_variable_to_decl($1, $3, $4); $$ = $1; }
        |       type var init_var       { $$ = make_var_decl($2, $1, $3); }
        ;

init_var:       '=' expr                { $$ = $2; }
        |       /* empty */             { $$ = NULL; }
        ;

type:           TINT                    { $$ = int_type; }
        |       TFLOAT                  { $$ = float_type; }
        |       TBOOL                   { $$ = bool_type; }
        |       TSTRING                 { $$ = string_type; }
        |       TVOID                   { $$ = void_type; }
        |       IDENTIFIER              { $$ = make_type($1, "NULL"); }
        ;

// --- FOR

for1:           type var init_var       { $$ = make_var_decl($2, $1, $3); } 
        |       expr                    { $$ = $1; }
        |       /* empty */             { $$ = NULL; }
        ;

for2:           expr_opt                { $$ = $1; }
        ;

for3:           expr_opt                { $$ = $1; }
        ;

// --- TRY-CATCH-FINALLY & THROW
try_catch:      KTRY stmt catch finally { $$ = make_try_statement($2, $3, $4); }
         ;

catch:          KCATCH stmt             { $$ = $2; };
        |       /* empty */             { $$ = NULL; }
        ;

finally:        KFINALLY stmt           { $$ = $2; };
        |       /* empty */             { $$ = NULL; }
        ;


// ========== FUNCTIONS ==========
func_decl:      prototype ';'           { $$ = $1; }
        |       prototype '{' stmt_list '}'
                                        { add_body_to_function($1, $3); $$ = $1; }
        ;


prototype:      type  var '(' fparam_list ')'
                                        { $$ = make_function($2, $1, $4); }
        ;

fparam_list:    fparam_list ',' type var{ add_parameter_to_function($1,$4,$3); $$=$1;}
        |       type var                { $$ = list_create();
                                          add_parameter_to_function($$, $2, $1);}
        |       /* empty */             { $$ = list_create(); }
        ;

eparam_list:    eparam_list ',' expr    { list_append($1, $3, FREE_NODE); $$ = $1; }
        |       expr                    { $$ = list_create();
                                          list_append($$, $1, FREE_NODE); }
        |       /* empty */             { $$ = list_create(); }
        ;

var_list:       var_list ',' var       { list_append($1, $3, FREE_NODE); $$ = $1; }
        |       var                    { $$ = list_create();
                                          list_append($$, $1, FREE_NODE); }
        |       /* empty */             { $$ = list_create(); }
        ;


// ========== CLASSES ==========
class_decl:     KCLASS identifier ';'   { $$ = make_class($2, NULL, NULL); }
        |       KCLASS identifier  inherit '{' members_list '}' opt_semi
                                        { $$ = make_class($2, $3, $5); }
        ;

inherit:        KEXTENDS identifier     { $$ = $2; }
        |       /* empty */             { $$ = NULL; }
        ;

members_list:   members_list attr_decl
                                        { list_append($1, $2, FREE_NODE); $$ = $1; }
        |       /* empty */             { $$ = list_create();}
        ;

attr_decl:      var_decl ';'            { $$ = $1; }
        |       func_decl opt_semi      { $$ = $1; }
        ;

opt_semi:       ';'
        |       /* empty */
        ;


// ========== EXPRESSIONS ==========
expr:
                INTEGER               { $$ = make_integer_constant($1); }
        |       FLOAT                 { $$ = make_float_constant($1);   }
        |       BOOLEAN               { $$ = make_boolean_constant($1); }
        |       STRING                { $$ = make_string_constant($1);}
        |       KNULL                 { $$ = make_null_constant(); }
        |       designator            { $$ = $1; }
        |       var '=' expr          { $$ = make_expression("=",  assign, 2, $1,$3);}
        |       '-' expr %prec UMINUS { $$ = make_expression("-",  uarith, 1, $2); }
        |       '+' expr %prec UMINUS { $$ = $2; }
        |       expr '+' expr         { $$ = make_expression("+",  barith, 2, $1,$3);}
        |       expr '-' expr         { $$ = make_expression("-",  barith, 2, $1,$3);}
        |       expr '*' expr         { $$ = make_expression("*",  barith, 2, $1,$3);}
        |       expr '/' expr         { $$ = make_expression("/",  barith, 2, $1,$3);}
        |       expr '%' expr         { $$ = make_expression("%",  barith, 2, $1,$3);}
        |       expr POW expr         { $$ = make_expression("**", barith, 2, $1,$3);}
        |       expr '<' expr         { $$ = make_expression("<",  comp,   2, $1,$3);}
        |       expr '>' expr         { $$ = make_expression(">",  comp,   2, $1,$3);}
        |       expr EQ  expr         { $$ = make_expression("==", comp,   2, $1,$3);}
        |       expr GE  expr         { $$ = make_expression(">=", comp,   2, $1,$3);}
        |       expr LE  expr         { $$ = make_expression("<=", comp,   2, $1,$3);}
        |       expr NE  expr         { $$ = make_expression("!=", comp,   2, $1,$3);}
        |       expr KOR expr         { $$ = make_expression("||", blogic, 2, $1,$3);}
        |       expr KAND expr        { $$ = make_expression("&&", blogic, 2, $1,$3);}
        |       KNOT expr             { $$ = make_expression("!",  ulogic, 1, $2); }
        |       '('expr ')'           { $$ = make_expression("(", parenthesis,1,$2);}
        |       expr '?' expr ':' expr{ $$ = make_expression("?:",ternary,3,$1,$3,$5);}
        |       PP var                { $$ = make_expression("++", preincr,  1, $2); }
        |       MM var                { $$ = make_expression("--", preincr,  1, $2); }
        |       var PP                { $$ = make_expression("++", postincr, 1, $1); }
        |       var MM                { $$ = make_expression("--", postincr, 1, $1); }
        |       designator KINSTOF identifier
                                     { $$ = make_expression("isa", isa, 2, $1, $3); }
        ;


expr_opt:       expr                  { $$ = $1; }
        |       /* empty */           { $$ = NULL; }
        ;

var:            identifier            { $$ = $1; }
        |       prefix identifier     { $$ = add_prefix_to_identifier($2, $1); }
        ;

call:           var '(' eparam_list ')'
                                      { $$ = make_call($1, $3); }
        ;

identifier:     IDENTIFIER            { $$ = make_identifier($1); }
        ;

prefix:         designator '.'        { $$ = $1; }
        ;

designator:     var                   { $$ = $1; }
        |       call                  { $$ = $1; }
        |       KNEW IDENTIFIER       { $$ = make_instance_new(make_type($2, "NULL"));}
        |       KTHIS                 { $$ = make_this(); }
        |       KSUPER                { $$ = make_super(); }
        ;
%%
