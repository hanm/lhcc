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

/* Semantic check for statements */

#include "ssc.h"
#include "error.h"

static t_ast_stmt* ssc_label_stmt(t_ast_stmt* stmt)
{
	assert(stmt);

	return stmt;
}

static t_ast_stmt* ssc_expression_stmt(t_ast_stmt* stmt)
{
	assert(stmt);

	return stmt;
}

static t_ast_stmt* ssc_if_stmt(t_ast_stmt* stmt)
{
	assert(stmt);

	return stmt;
}

static t_ast_stmt* ssc_case_stmt(t_ast_stmt* stmt)
{
	assert(stmt);

	return stmt;
}

static t_ast_stmt* ssc_switch_stmt(t_ast_stmt* stmt)
{
	assert(stmt);

	return stmt;
}

static t_ast_stmt* ssc_default_stmt(t_ast_stmt* stmt)
{
	assert(stmt);

	return stmt;
}

static t_ast_stmt* ssc_do_stmt(t_ast_stmt* stmt)
{
	assert(stmt);

	return stmt;
}

static t_ast_stmt* ssc_while_stmt(t_ast_stmt* stmt)
{
	assert(stmt);

	return stmt;
}

static t_ast_stmt* ssc_for_stmt(t_ast_stmt* stmt)
{
	assert(stmt);

	return stmt;
}

static t_ast_stmt* ssc_goto_stmt(t_ast_stmt* stmt)
{
	assert(stmt);

	return stmt;
}

static t_ast_stmt* ssc_continue_stmt(t_ast_stmt* stmt)
{
	assert(stmt);

	return stmt;
}

static t_ast_stmt* ssc_break_stmt(t_ast_stmt* stmt)
{
	assert(stmt);

	return stmt;
}

static t_ast_stmt* ssc_return_stmt(t_ast_stmt* stmt)
{
	assert(stmt);

	return stmt;
}

t_ast_stmt* ssc_compound_stmt(t_ast_stmt* stmt)
{
	assert(stmt);

	return stmt;
}

t_ast_stmt* ssc_stmt(t_ast_stmt* stmt)
{
	t_ast_stmt* r = NULL;

	assert(stmt);

	switch(stmt->kind)
	{
	case AST_STMT_LABEL_KIND :
		{
			r = ssc_label_stmt(stmt);
			break;
		}
	case AST_STMT_CASE_KIND :
		{
			r = ssc_case_stmt(stmt);
			break;
		}
	case AST_STMT_DEFAULT_KIND :
		{
			r = ssc_default_stmt(stmt);
			break;
		}
	case AST_STMT_COMPOUND_KIND :
		{
			r = ssc_compound_stmt(stmt);
			break;
		}
	case AST_STMT_EXPRESSION_KIND : 
		{
			r = ssc_expression_stmt(stmt);
			break;
		}
	case AST_STMT_IF_KIND :
		{
			r = ssc_if_stmt(stmt);
			break;
		}
	case AST_STMT_SWITCH_KIND :
		{
			r = ssc_switch_stmt(stmt);
			break;
		}
	case AST_STMT_DO_KIND :
		{
			r = ssc_do_stmt(stmt);
			break;
		}
	case AST_STMT_FOR_KIND :
		{
			r = ssc_for_stmt(stmt);
			break;
		}
	case AST_STMT_WHILE_KIND :
		{
			r = ssc_while_stmt(stmt);
			break;
		}
	case AST_STMT_GOTO_KIND :
		{
			r = ssc_goto_stmt(stmt);
			break;
		}
	case AST_STMT_CONTINUE_KIND :
		{
			r = ssc_continue_stmt(stmt);
			break;
		}
	case AST_STMT_BREAK_KIND :
		{
			r = ssc_break_stmt(stmt);
			break;
		}
	case AST_STMT_RETURN_KIND :
		{
			r = ssc_return_stmt(stmt);
			break;
		}
	}

	return r;
}
