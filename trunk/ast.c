/***************************************************************

Copyright (c) 2008 Michael Liang Han

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

****************************************************************/

#include "ast.h"

// guide line on creating ast http://lambda.uta.edu/cse5317/notes/node25.html
// also Abel's book has similar ideas on building ast on top of c structs..
/*
typedef struct Exp {
  enum { int_exp, true_exp, false_exp, variable_exp,
         binary_op, unary_op, function_call,
         record_construction, projection } tag;
  union { int                                      integer;
          string                                   variable;
          struct { string           oper;
                   struct Exp*      left;
                   struct Exp*      right; }       binary;
          struct { string           oper;
                   struct Exp*      uexp; }        unary;
          struct { string           name;
                   struct Exp_list* arguments; }   call;
          struct rec { string       attribute;
                       struct Exp*  value;
                       struct rec*  next; }        record;
          struct { struct Exp*  value;
                   string attribute; }             project;
      } op;
} ast;
where Exp_list is a list of ASTs:
typedef struct Exp_list { 
  ast*             elem;
  struct Exp_list* next;
} ast_list;

It's a good idea to define a constructor for every kind of expression to simplify the task of constructing ASTs:
ast* make_binary_op ( string oper, ast* left, ast* right ) {
  ast* e = (ast*) malloc(sizeof(ast));
  e->tag = binary_op;
  e->op.binary.oper = make_string(oper);
  e->op.binary.left = left;
  e->op.binary.right = right;
  return e;
};

For example,
make_binary_op("+",make_binary_op("-",make_variable("x"),make_integer(2)),
               make_integer(3))
constructs the AST for the input (x-2)+3.

Unfortunately, when constructing a compiler, we need to define many tree-like data structures to capture ASTs for many different constructs, 
such as expressions, statements, declarations, programs etc, as well as type structures, intermediate representation (IR) trees, etc. 
This would require hundreds of recursive structs in C or classes in Java. 
An alternative method is to use just one generic tree structure to capture all possible tree structures. 
*/

static int ast_place_holder()
{
    return 0;
}