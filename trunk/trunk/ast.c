/***************************************************************

Copyright (c) 2008-2009 Michael Liang Han

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

#include "hcc.h"
#include "arena.h"
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

#define ALLOCATE_GENERIC_AST  t_ast_exp* exp; \
	CALLOC(exp, PERM)


t_ast_exp* make_ast_id_exp(char* name)
{
	ALLOCATE_GENERIC_AST;

	assert(name);

	exp->kind = AST_EXP_IDENTIFIER_KIND;
	exp->u.ast_id_exp.name = name;
	
	return exp;
}

t_ast_exp* make_ast_const_exp(t_ast_exp_val val)
{
	ALLOCATE_GENERIC_AST;

	exp->kind = AST_EXP_CONST_KIND;
	exp->u.ast_const_exp.val = val;

	return exp;
}

t_ast_exp* make_ast_subscript_exp(t_ast_exp* main, t_ast_exp* index)
{
	ALLOCATE_GENERIC_AST;

	assert(main && index);

	exp->kind = AST_EXP_SUBSCRIPT_KIND;
	exp->u.ast_subscript_exp.main = main;
	exp->u.ast_subscript_exp.index = index;

	return exp;
}

t_ast_exp* make_ast_call_exp(t_ast_exp* func, t_ast_list args)
{
	ALLOCATE_GENERIC_AST;

	assert(func);

	exp->kind = AST_EXP_FUNCTION_CALL_KIND;
	exp->u.ast_call_exp.func = func;
	exp->u.ast_call_exp.args = args;

	return exp;
}

t_ast_exp* make_ast_indir_exp(t_ast_exp* expression, t_ast_exp_op op, char* id)
{
    ALLOCATE_GENERIC_AST;

    assert(expression && id);
    assert(op == AST_OP_PTR || op == AST_OP_DOT);

    exp->kind = AST_EXP_INDIR_KIND;
    exp->u.ast_indir_exp.exp = expression;
    exp->u.ast_indir_exp.op = op;
    exp->u.ast_indir_exp.id = id;

    return exp;
}

t_ast_exp* make_ast_postop_exp(t_ast_exp* expression, t_ast_exp_op op)
{
    ALLOCATE_GENERIC_AST;

    assert(expression);
    assert(op == AST_OP_INC || op == AST_OP_DEC);

    exp->kind = AST_EXP_POSTOP_KIND;
    exp->u.ast_postop_exp.exp = expression;
    exp->u.ast_postop_exp.op = op;

    return exp;
}

t_ast_exp* make_ast_unary_exp(t_ast_exp* expression, t_ast_exp_op op)
{
    ALLOCATE_GENERIC_AST;

    assert(expression);
    assert(op == AST_OP_ADDR ||
	          op == AST_OP_DEREF ||
              op == AST_OP_POS ||
              op == AST_OP_NEGATE ||
              op == AST_OP_INVERT ||
              op == AST_OP_NOT);

    exp->kind = AST_EXP_UNARY_KIND;
    exp->u.ast_unary_exp.exp = expression;
    exp->u.ast_unary_exp.op = op;

    return exp;
}

t_ast_exp* make_ast_cast_exp(t_ast_exp* type, t_ast_exp* expression)
{
    ALLOCATE_GENERIC_AST;
    
    assert(type && expression);

    exp->kind = AST_EXP_CAST_KIND;
    exp->u.ast_cast_exp.type = type;
    exp->u.ast_cast_exp.exp = expression;

    return exp;
}

t_ast_exp* make_ast_sizeof_exp(t_ast_exp* type, t_ast_exp* expression)
{
    ALLOCATE_GENERIC_AST;

    assert(type || exp); 

    exp->kind = AST_EXP_SIZEOF_KIND;
    exp->u.ast_sizeof_exp.type = type;
    exp->u.ast_sizeof_exp.exp = expression;

    return exp;
}

t_ast_exp* make_ast_binary_exp(t_ast_exp* left, t_ast_exp_op op, t_ast_exp* right)
{
    ALLOCATE_GENERIC_AST;

    assert(left && right);

    exp->kind = AST_EXP_BINARY_KIND;
    exp->u.ast_binary_exp.left = left;
    exp->u.ast_binary_exp.op = op;
    exp->u.ast_binary_exp.right = right;

    return exp;
}
