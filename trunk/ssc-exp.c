/***************************************************************

Copyright (c) 2008-2010 Michael Liang Han

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

/* Semantic check for expressions */

#include "ast.h"


static t_ast_exp* scc_primary_expression(t_ast_exp* exp)
{
	return exp;
}
    
static t_ast_exp* scc_postfix_expression(t_ast_exp* exp)
{
	return exp;
}
      
static t_ast_exp* scc_unary_expression(t_ast_exp* exp)
{
	return exp;
}

static t_ast_exp* scc_cast_expression(t_ast_exp* exp)
{
	return exp;
}

static t_ast_exp* multiplicative_expression(t_ast_exp* exp)
{
	return exp;
}

static t_ast_exp* additive_expression(t_ast_exp* exp)
{
	return exp;
}

static t_ast_exp*shift_expression(t_ast_exp* exp)
{
	return exp;
}

static t_ast_exp* relational_expression(t_ast_exp* exp)
{
	return exp;
}

static t_ast_exp* equality_expression(t_ast_exp* exp)
{
	return exp;
}

static t_ast_exp* and_expression(t_ast_exp* exp)
{
	return exp;
}

static t_ast_exp* exclusive_or_expression(t_ast_exp* exp)
{
	return exp;
}

static t_ast_exp* inclusive_or_expression(t_ast_exp* exp)
{
	return exp;
}

static t_ast_exp* logical_and_expression(t_ast_exp* exp)
{
	return exp;
}

static t_ast_exp* logical_or_expression(t_ast_exp* exp)
{
	return exp;
}

static t_ast_exp* conditional_expression(t_ast_exp* exp)
{
	return exp;
}

static t_ast_exp* assignment_expression(t_ast_exp* exp)
{
	return exp;
}

t_ast_exp* ssc_expression(t_ast_exp* exp)
{
	return exp;
}