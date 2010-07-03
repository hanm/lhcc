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
#include "ssc.h"
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
    t_symbol* sym_id = NULL;

    assert(exp);

    /* constant */
    if ((exp->kind >= AST_EXP_CONST_FLOAT_KIND && 
        exp->kind <= AST_EXP_CONST_UNSIGNED_LONG_LONG_KIND) ||
        exp->kind == AST_EXP_LITERAL_STRING_KIND ||
        exp->kind == AST_EXP_LITERAL_STRING_WIDE_KIND)
    {
        return exp;
    }

    /* identifier */
    assert(exp->kind == AST_EXP_IDENTIFIER_KIND);
    
    sym_id = find_symbol(exp->u.ast_id_exp.name, sym_table_identifiers);
   
    if (sym_id == NULL)
    {
       /* TODO - after declarations are parsed, check this id is in symbol table
        * For now leave the check otherwise too many false positives
        */
        return exp;
    }
    
    if (!sym_id->hidden_typedef && sym_id->storage ==  TK_TYPEDEF)
    {
        semantic_error("Identifier name can't be a type defined by typedef!", &exp->coord);
		return exp;
    }

	if (sym_id->storage == TK_ENUM)
	{
		/* enum has int type implicitly */
		t_ast_exp_val value;
		value.i = sym_id->value.i;

		/* constant folding */
		exp = make_ast_const_exp(value, AST_EXP_CONST_INTEGER_KIND);
		return exp;
	}
    
	/* identifier must have lvalue */
	exp->has_lvalue = 1;
	return exp;
}
    
static t_ast_exp* scc_postfix_expression(t_ast_exp* exp)
{
	assert(exp != NULL);

	switch (exp->kind)
	{
	case AST_EXP_SUBSCRIPT_KIND :
		{
			t_ast_exp* main_exp = ssc_implicit_conversion(ssc_expression(exp->u.ast_subscript_exp.main), 1);
			t_ast_exp* index_exp = ssc_implicit_conversion(ssc_expression(exp->u.ast_subscript_exp.index), 1);
			t_type* type = NULL;

			assert(main_exp && index_exp);

			/*
			 * Usually, the subscript expression has type ¡°pointer to <type>¡±, the expression within the
			 * square brackets has type int, and the type of the result is <type>. However, it is
			 * equally valid if the types of the postfix expression and the expression in brackets are
			 * reversed. This is because the expression E1[E2] is identical (by definition) to
			 * *((E1)+(E2)). Because addition is commutative, E1 and E2 can be interchanged.
			 *
			 * below code normalized the expression such that main is always type ptr
			 * and index is always type int
			*/
			if (IS_INTEGER_TYPE(main_exp->type))
			{
				t_ast_exp* temp = main_exp;
				main_exp = index_exp;
				index_exp = temp;
			}

			type = main_exp->type;

			if (IS_INTEGER_TYPE(index_exp->type) &&
				IS_PTR_TYPE(type) && 
				type->link &&
				!(IS_FUNCTION_TYPE(type->link)))
			{
				exp->type = type;
				/* array dereference yields a rvalue */
				exp->has_lvalue = 0;

				/* [TODO] 
				 1. pointer arithmetic
				 2. index checking (bounds check? no!)
				*/
			}
			else
			{
				semantic_error("subscript expression type error", &exp->coord);
			}

		}
	default:
		break;
	}


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

t_ast_exp* ssc_const_expression(t_ast_exp* exp)
{
	exp = ssc_expression(exp);

	assert(exp);

	/* const expression
	 * 1. type const
	 * 2. exp kind
	*/
	if (exp->kind >= AST_EXP_CONST_FLOAT_KIND && 
		exp->kind <= AST_EXP_CONST_UNSIGNED_LONG_LONG_KIND ||
		exp->kind == AST_EXP_IDENTIFIER_KIND ||
		exp->kind == AST_EXP_UNARY_KIND ||
		exp->kind == AST_EXP_BINARY_KIND) /* TODO - 
											  1. check expression type. this is not written yet.
											  2. remove check type == AST_EXP_IDENTIFIER_KIND
											  this is not neccessary after ssc_primary_expression is implemented.
											  as once it's implemented, the expression type could be turned into
											  something finely grained as constant type (instead of primary expression,
											  which is really no help at all - consider it a syntactic sugar only...).
											  */
	{
		return exp;
	}

	return NULL;
}

t_ast_exp* ssc_implicit_conversion(t_ast_exp* exp, int lvalue_to_rvalue)
{
	assert(exp != NULL);

	if (lvalue_to_rvalue)
	{
		exp->type = UNQUALIFY_TYPE(exp->type);
		exp->has_lvalue = 0;
	}

	if (IS_FUNCTION_TYPE(exp->type))
	{
		exp->type = pointer_type(exp->type);
	}
	
	if (IS_ARRAY_TYPE(exp->type))
	{
		exp->type = pointer_type(exp->type->link);

		/* per standard, except as noted, if an lvalue that has type ¡°array of <type>¡± appears as an operand,
			it is converted to an expression of the type ¡°pointer to <type>.¡± The resultant pointer
			points to the initial element of the array. In this case, the resultant pointer ceases to be
			an lvalue
	    */
		exp->has_lvalue = 0;
	}

	return exp;
}