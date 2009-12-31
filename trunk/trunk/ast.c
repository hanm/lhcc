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

#define ALLOCATE_GENERIC_AST_EXP  t_ast_exp* exp; \
	CALLOC(exp, PERM)

#define ALLOCATE_GENERIC_AST_STMT t_ast_stmt* stmt; \
	CALLOC(stmt, PERM)


t_ast_exp* make_ast_id_exp(char* name)
{
	ALLOCATE_GENERIC_AST_EXP;

	assert(name);

	exp->kind = AST_EXP_IDENTIFIER_KIND;
	exp->u.ast_id_exp.name = name;
	
	return exp;
}

t_ast_exp* make_ast_const_exp(t_ast_exp_val val, t_ast_exp_kind kind)
{
	ALLOCATE_GENERIC_AST_EXP;

    assert(kind == AST_EXP_CONST_FLOAT_KIND ||
              kind == AST_EXP_CONST_DOUBLE_KIND ||
              kind == AST_EXP_CONST_LONG_DOUBLE_KIND ||
              kind == AST_EXP_CONST_INTEGER_KIND ||
	          kind == AST_EXP_CONST_LONG_INTEGER_KIND ||
	          kind == AST_EXP_CONST_LONG_LONG_KIND ||
	          kind == AST_EXP_CONST_UNSIGNED_INTEGER_KIND ||
	          kind == AST_EXP_CONST_UNSIGNED_LONG_INTEGER_KIND ||
	          kind == AST_EXP_CONST_UNSIGNED_LONG_LONG_KIND ||
              kind == AST_EXP_LITERAL_STRING_KIND ||
              kind == AST_EXP_LITERAL_STRING_WIDE_KIND);

	exp->kind = kind;
	exp->u.ast_const_exp.val = val;

	return exp;
}

t_ast_exp* make_ast_subscript_exp(t_ast_exp* main, t_ast_exp* index)
{
	ALLOCATE_GENERIC_AST_EXP;

	assert(main && index);

	exp->kind = AST_EXP_SUBSCRIPT_KIND;
	exp->u.ast_subscript_exp.main = main;
	exp->u.ast_subscript_exp.index = index;

	return exp;
}

t_ast_exp* make_ast_call_exp(t_ast_exp* func, t_ast_list args)
{
	ALLOCATE_GENERIC_AST_EXP;

	assert(func);

	exp->kind = AST_EXP_FUNCTION_CALL_KIND;
	exp->u.ast_call_exp.func = func;
	exp->u.ast_call_exp.args = args;

	return exp;
}

t_ast_exp* make_ast_indir_exp(t_ast_exp* expression, t_ast_exp_op op, char* id)
{
    ALLOCATE_GENERIC_AST_EXP;

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
    ALLOCATE_GENERIC_AST_EXP;

	assert(expression);
    assert(op == AST_OP_INC || op == AST_OP_DEC);

    exp->kind = AST_EXP_POSTOP_KIND;
    exp->u.ast_postop_exp.exp = expression;
    exp->u.ast_postop_exp.op = op;

    return exp;
}

t_ast_exp* make_ast_unary_exp(t_ast_exp* expression, t_ast_exp_op op)
{
    ALLOCATE_GENERIC_AST_EXP;

    assert(expression);
    assert(op == AST_OP_ADDR ||
	          op == AST_OP_DEREF ||
              op == AST_OP_POS ||
              op == AST_OP_NEGATE ||
              op == AST_OP_INVERT ||
              op == AST_OP_NOT ||
              op == AST_OP_INC ||
              op == AST_OP_DEC);

    exp->kind = AST_EXP_UNARY_KIND;
    exp->u.ast_unary_exp.exp = expression;
    exp->u.ast_unary_exp.op = op;

    return exp;
}

t_ast_exp* make_ast_cast_exp(t_ast_exp* type, t_ast_exp* expression)
{
    ALLOCATE_GENERIC_AST_EXP;
    
    assert(type && expression);

    exp->kind = AST_EXP_CAST_KIND;
    exp->u.ast_cast_exp.type = type;
    exp->u.ast_cast_exp.exp = expression;

    return exp;
}

t_ast_exp* make_ast_sizeof_exp(t_ast_exp* type, t_ast_exp* expression)
{
    ALLOCATE_GENERIC_AST_EXP;

    assert(type || exp); 

    exp->kind = AST_EXP_SIZEOF_KIND;
    exp->u.ast_sizeof_exp.type = type;
    exp->u.ast_sizeof_exp.exp = expression;

    return exp;
}

t_ast_exp* make_ast_binary_exp(t_ast_exp* left, t_ast_exp_op op, t_ast_exp* right)
{
    ALLOCATE_GENERIC_AST_EXP;

    assert(left && right);

    exp->kind = AST_EXP_BINARY_KIND;
    exp->u.ast_binary_exp.left = left;
    exp->u.ast_binary_exp.op = op;
    exp->u.ast_binary_exp.right = right;

    return exp;
}

t_ast_exp* make_ast_conditional_exp(t_ast_exp* cond_exp, t_ast_exp* true_exp, t_ast_exp* false_exp)
{
    ALLOCATE_GENERIC_AST_EXP;

    assert(cond_exp);
    assert(!(!true_exp && !false_exp));
	
    exp->kind = AST_EXP_CONDITION_KIND;
    exp->u.ast_conditional_exp.cond_exp = cond_exp;
    exp->u.ast_conditional_exp.true_exp = true_exp;
    exp->u.ast_conditional_exp.false_exp =  false_exp;

    return exp;
}

t_ast_exp* make_ast_assignment_exp(t_ast_exp* cond_exp, t_ast_exp_op op, t_ast_exp* assign_exp)
{
    ALLOCATE_GENERIC_AST_EXP;

    assert(cond_exp);

    exp->kind = AST_EXP_ASSIGNMENT_KIND;
    exp->u.ast_assignment_exp.cond_exp = cond_exp;
    exp->u.ast_assignment_exp.assign_exp = assign_exp;
    exp->u.ast_assignment_exp.op = op;

    return exp;
}

t_ast_exp* make_ast_comma_exp(t_ast_exp* comma_exp, t_ast_exp* assign_exp)
{
    ALLOCATE_GENERIC_AST_EXP;

    assert(assign_exp);

    exp->kind = AST_EXP_COMMA_KIND;
    exp->u.ast_comma_exp.assign_exp = assign_exp;
    exp->u.ast_comma_exp.comma_exp = comma_exp; /* optional, may be NULL */

    /* note comma expression could decay to just single assignment expression 
     * which is the normal expression so called.
    */
    return exp;
}

t_ast_exp* make_ast_typename_exp()
{
    ALLOCATE_GENERIC_AST_EXP;
    exp->kind = AST_EXP_GENERIC_EXP_KIND;

    return exp;
}

t_ast_exp* make_ast_generic_exp()
{
	ALLOCATE_GENERIC_AST_EXP;
	exp->kind = AST_EXP_GENERIC_EXP_KIND;

	return exp;
}

t_ast_stmt* make_ast_label_stmt(char* label_name, t_ast_stmt* label_stmt)
{
	ALLOCATE_GENERIC_AST_STMT;

	assert(label_name && label_stmt);

	stmt->kind = AST_STMT_LABEL_KIND;
	stmt->u.ast_label_stmt.label_name = label_name;
	stmt->u.ast_label_stmt.stmt = label_stmt;
	
	return stmt;
}

t_ast_stmt* make_ast_expression_stmt(t_ast_exp* exp)
{
	ALLOCATE_GENERIC_AST_STMT;

	assert(exp);

	stmt->kind = AST_STMT_EXPRESSION_KIND;
	stmt->u.ast_expression_stmt.exp = exp;

	return stmt;
}

t_ast_stmt* make_ast_if_stmt(t_ast_exp* test_exp, t_ast_stmt* then_stmt, t_ast_stmt* else_stmt)
{
	ALLOCATE_GENERIC_AST_STMT;

	assert(test_exp && then_stmt);

	stmt->kind = AST_STMT_IF_KIND;
	stmt->u.ast_if_stmt.test_exp = test_exp;
	stmt->u.ast_if_stmt.then_stmt = then_stmt;
	stmt->u.ast_if_stmt.else_stmt = else_stmt;

	return stmt;
}

t_ast_stmt* make_ast_switch_stmt(t_ast_exp* test_exp, t_ast_stmt* switch_stmt)
{
	ALLOCATE_GENERIC_AST_STMT;

	assert(test_exp && switch_stmt);

	stmt->kind = AST_STMT_SWITCH_KIND;
	stmt->u.ast_switch_stmt.test_exp = test_exp;
	stmt->u.ast_switch_stmt.stmt = switch_stmt;

	return stmt;
}

t_ast_stmt* make_ast_do_stmt(t_ast_stmt* body_stmt, t_ast_exp* test_exp)
{
	ALLOCATE_GENERIC_AST_STMT;

	assert(body_stmt && test_exp);

	stmt->kind = AST_STMT_DO_KIND;
	stmt->u.ast_do_stmt.body_stmt = body_stmt;
	stmt->u.ast_do_stmt.test_exp = test_exp;

	return stmt;
}

t_ast_stmt* make_ast_while_stmt(t_ast_exp* test_exp, t_ast_stmt* body_stmt)
{
	ALLOCATE_GENERIC_AST_STMT;

	assert(test_exp && body_stmt);

	stmt->kind = AST_STMT_WHILE_KIND;
	stmt->u.ast_while_stmt.body_stmt = body_stmt;
	stmt->u.ast_while_stmt.test_exp = test_exp;

	return stmt;
}

t_ast_stmt* make_ast_for_stmt(t_ast_stmt* init_exp_stmt, t_ast_stmt* test_exp_stmt, t_ast_exp* post_test_exp, t_ast_stmt* body_stmt)
{
	ALLOCATE_GENERIC_AST_STMT;

	assert(init_exp_stmt && test_exp_stmt && body_stmt);

	stmt->kind = AST_STMT_FOR_KIND;
	stmt->u.ast_for_stmt.init_exp_stmt = init_exp_stmt;
	stmt->u.ast_for_stmt.test_exp_stmt = test_exp_stmt;
	stmt->u.ast_for_stmt.post_test_exp = post_test_exp;
	stmt->u.ast_for_stmt.body_stmt = body_stmt;

	return stmt;
}

t_ast_stmt* make_ast_goto_stmt(char* label_name)
{
	ALLOCATE_GENERIC_AST_STMT;

	assert(label_name);

	stmt->kind = AST_STMT_GOTO_KIND;
	stmt->u.ast_goto_stmt.label_name = label_name;

	return stmt;
}

t_ast_stmt* make_ast_continue_stmt(t_ast_stmt* target)
{
	ALLOCATE_GENERIC_AST_STMT;

	assert(target);
	
	stmt->kind = AST_STMT_CONTINUE__KIND;
	stmt->u.ast_continue_stmt.target = target;

	return stmt;
}

t_ast_stmt* make_ast_break_stmt(t_ast_stmt* target)
{
	ALLOCATE_GENERIC_AST_STMT;

	assert(target);

	stmt->kind = AST_STMT_BREAK_KIND;
	stmt->u.ast_break_stmt.target = target;

	return stmt;
}

t_ast_stmt* make_ast_return_stmt(t_ast_exp* return_exp)
{
	ALLOCATE_GENERIC_AST_STMT;

	stmt->kind = AST_STMT_RETURN_KIND;
	stmt->u.ast_return_stmt.exp = return_exp;

	return stmt;
}

t_ast_stmt* make_ast_compound_stmt(t_ast_stmt_list stmts)
{
	ALLOCATE_GENERIC_AST_STMT;

	if (stmts.current == NULL) 
	{
		stmt->kind = AST_STMT_EMPTY_KIND;
	}
	else
	{
		stmt->kind = AST_STMT_COMPOUND_KIND;
		stmt->u.ast_compound_stmt.stmts = stmts;
	}

	return stmt;
}

t_ast_stmt* make_ast_empty_stmt()
{
	ALLOCATE_GENERIC_AST_STMT;

	stmt->kind = AST_STMT_EMPTY_KIND;

	return stmt;
}