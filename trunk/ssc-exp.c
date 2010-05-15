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
#include "ssc-all.h"
#include "error.h"

/* Define NULL pointer value */
#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

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

static t_ast_exp* ssc_multiplicative_expression(t_ast_exp* exp)
{
	return exp;
}

static t_ast_exp* ssc_additive_expression(t_ast_exp* exp)
{
	return exp;
}

static t_ast_exp* ssc_shift_expression(t_ast_exp* exp)
{
	return exp;
}

static t_ast_exp* ssc_relational_expression(t_ast_exp* exp)
{
	return exp;
}

static t_ast_exp* ssc_equality_expression(t_ast_exp* exp)
{
	return exp;
}

static t_ast_exp* ssc_and_expression(t_ast_exp* exp)
{
	return exp;
}

static t_ast_exp* ssc_exclusive_or_expression(t_ast_exp* exp)
{
	return exp;
}

static t_ast_exp* ssc_inclusive_or_expression(t_ast_exp* exp)
{
	return exp;
}

static t_ast_exp* ssc_logical_and_expression(t_ast_exp* exp)
{
	return exp;
}

static t_ast_exp* ssc_logical_or_expression(t_ast_exp* exp)
{
	return exp;
}

static t_ast_exp* ssc_conditional_expression(t_ast_exp* exp)
{
	return exp;
}

static t_ast_exp* ssc_assignment_expression(t_ast_exp* exp)
{
	return exp;
}

static t_ast_exp* scc_comma_expression(t_ast_exp* exp)
{
	return exp;
}

static t_ast_exp* scc_binary_expression(t_ast_exp* exp)
{
	t_ast_exp* r = NULL;
	
	assert(exp && exp->kind == AST_EXP_BINARY_KIND);

	switch (exp->u.ast_binary_exp.op)
	{
	case AST_OP_ADD :
	case AST_OP_SUB :
		{
			r = ssc_additive_expression(exp);
			break;
		}
	case AST_OP_MUL :
	case AST_OP_DIV :
	case AST_OP_MOD :
		{
			r = ssc_multiplicative_expression(exp);
			break;
		}
	case AST_OP_BIT_AND :
		{
			r = ssc_and_expression(exp);
			break;
		}
	case AST_OP_BIT_OR :
		{
			r = ssc_inclusive_or_expression(exp);
			break;
		}
	case AST_OP_BIT_XOR :
		{
			r = ssc_exclusive_or_expression(exp);
			break;
		}
	case AST_OP_LSHIFT :
	case AST_OP_RSHIFT :
		{
			r = ssc_shift_expression(exp);
			break;
		}
	case AST_OP_LESS :
	case AST_OP_LESS_EQ :
	case AST_OP_GREAT :
	case AST_OP_GREAT_EQ :
		{
			r = ssc_relational_expression(exp);
			break;
		}
	case AST_OP_EQUAL :
	case AST_OP_UNEQUAL :
		{
			r = ssc_equality_expression(exp);
			break;
		}
	case AST_OP_AND :
		{
			r = ssc_logical_and_expression(exp);
			break;
		}
	case AST_OP_OR :
		{
			r = ssc_logical_or_expression(exp);
			break;
		}
	case AST_OP_ASSIGN :
    case AST_OP_MUL_ASSIGN :
    case AST_OP_DIV_ASSIGN :
    case AST_OP_MOD_ASSIGN :
    case AST_OP_ADD_ASSIGN :
    case AST_OP_SUB_ASSIGN :
    case AST_OP_LSHIFT_ASSIGN :
    case AST_OP_RSHIFT_ASSIGN :
    case AST_OP_BIT_AND_ASSIGN :
    case AST_OP_BIT_OR_ASSIGN :
    case AST_OP_BIT_XOR_ASSIGN :
		{
			r = ssc_assignment_expression(exp);
			break;
		}
	default:
		assert(0);
	}

	return r;
}

t_ast_exp* ssc_expression(t_ast_exp* exp)
{
	t_ast_exp* r = NULL;

	assert(exp);

	switch (exp->kind)
	{
	case AST_EXP_BINARY_KIND :
		{
			r = scc_binary_expression(exp);
			break;
		}
	case AST_EXP_UNARY_KIND :
		{
			r = scc_unary_expression(exp);
			break;
		}
	case AST_EXP_IDENTIFIER_KIND :
		{
			r = scc_primary_expression(exp);
			break;
		}
	case AST_EXP_FUNCTION_CALL_KIND :
	case AST_EXP_SUBSCRIPT_KIND :
	case AST_EXP_INDIR_KIND :
	case AST_EXP_POSTOP_KIND :
		{
			r = scc_postfix_expression(exp);
			break;
		}
	case AST_EXP_CAST_KIND :
	case AST_EXP_SIZEOF_KIND :
		{
			r = scc_unary_expression(exp);
			break;
		}
	case AST_EXP_CONDITION_KIND :
		{
			r = ssc_conditional_expression(exp);
			break;
		}
	case AST_EXP_ASSIGNMENT_KIND : 
		{
			r = ssc_assignment_expression(exp);
			break;
		}
	case AST_EXP_COMMA_KIND :
		{
			r = scc_comma_expression(exp);
			break;
		}
	case AST_EXP_CONST_FLOAT_KIND :
	case AST_EXP_CONST_DOUBLE_KIND :
	case AST_EXP_CONST_LONG_DOUBLE_KIND :
	case AST_EXP_CONST_INTEGER_KIND :
	case AST_EXP_CONST_LONG_INTEGER_KIND :
	case AST_EXP_CONST_LONG_LONG_KIND :
	case AST_EXP_CONST_UNSIGNED_INTEGER_KIND :
	case AST_EXP_CONST_UNSIGNED_LONG_INTEGER_KIND :
	case AST_EXP_CONST_UNSIGNED_LONG_LONG_KIND : 
	case AST_EXP_LITERAL_STRING_KIND :
	case AST_EXP_LITERAL_STRING_WIDE_KIND :
		{
			r = scc_primary_expression(exp);
			break;
		}
	default:
		assert(0);
	}

	return r;
}