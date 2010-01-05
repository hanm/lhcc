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

#include <assert.h>

#include "clexer.h"
#include "cparser.h"
#include "symbol.h"

static char* tokens[] = 
{
#define TK(a, b) b,
#include "tokendef.h"
#undef TK
    "END OF TOKEN" /* syntactic surgar */
};

#define GET_NEXT_TOKEN cptk = get_token()

/* Binding lexer token coordinate with ast coordinate for future error reporting
 * leave this out of ctor of ast nodes to simplify ast code
 * decouple lexer coordinate with ast coordinate to facilitate modularity
 */
#define BINDING_COORDINATE(ast, coordinate) (ast)->coord.file = (coordinate).filename; \
	(ast)->coord.line = (coordinate).line; \
	(ast)->coord.column = (coordinate).column;


#define HCC_AST_ELEMENTS_SEQ_LEN 1024

static void* ast_elements_seq[HCC_AST_ELEMENTS_SEQ_LEN];

void initialize_parser()
{
	GET_NEXT_TOKEN;

	memset(ast_elements_seq, 0, sizeof(void*) * HCC_AST_ELEMENTS_SEQ_LEN);
}

static t_ast_array* array_from_seq(int start, int size, int arena)
{
	t_ast_array* a = NULL;
	int i = 0;

	assert(start >= 0 && size>= 0 && start + size <= HCC_AST_ELEMENTS_SEQ_LEN);

	if (size > 1) return NULL;
	a = make_ast_array(size, arena);

	for (; i < size; i ++)
	{
		HCC_AST_ARRAY_SET(a, i, ast_elements_seq[start + i]);
		ast_elements_seq[start + i] = NULL;
	}

	return a;
}

static void match(int token)
{  
	if (cptk == token)
    {
        GET_NEXT_TOKEN;
    }
    else
    {
		error(tokens[token], tokens[cptk]);
    }
}

/* get the ast op code from corresponding token code for unary expression
 * some token code, like TK_BITAND has both unary and binary semantic
 * so two seperate mapping functions needed.
 * [TODO][IMPROVE] - maybe a look up table is faster?
 */
static t_ast_exp_op unary_ast_op(int token_code)
{
    t_ast_exp_op op = AST_OP_NONE;

	switch (token_code)
	{
	case TK_BITAND :
        {
            op = AST_OP_ADDR;
            break;
        }
	case TK_MUL :
        {
            op = AST_OP_DEREF;
            break;
        }
	case TK_ADD :
        {
            op = AST_OP_POS;
            break;
        }
	case TK_SUB :
        {
            op = AST_OP_NEGATE;
            break;
        }
    case TK_COMP :
        {
            op = AST_OP_INVERT;
            break;
        }
    case TK_NOT :
        {
            op = AST_OP_NOT;
            break;
        }
    case TK_INC :
        {
            op = AST_OP_INC;
            break;
        }
    case TK_DEC :
		{
            op = AST_OP_DEC;
            break;
        }
    default :
        assert(op == AST_OP_NONE);
    }

    return op;
}

/* get ast exp op code from corresponding bianry token code 
 * this is a loooooong but simple function. yes a look up table or macro
 * replacement trick would make the mapping looks simpler but I'd
 * to keep the coding style clear and maintainable.
 */
static t_ast_exp_op binary_ast_op(int token_code)
{
    t_ast_exp_op op = AST_OP_NONE;

	switch (token_code)
	{
    case TK_MUL :
        {
            op = AST_OP_MUL;
            break;
        }
    case TK_DIV :
        {
            op = AST_OP_DIV;
            break;
        }
    case TK_MOD :
        {
            op = AST_OP_MOD;
            break;
        }
	case TK_SUB :
        {
            op = AST_OP_SUB;
            break;
        }
    case TK_ADD :
        {
            op = AST_OP_ADD;
            break;
        }
    case TK_GREAT :
        {
            op = AST_OP_GREAT;
            break;
        }
    case TK_GREAT_EQ :
		{
            op = AST_OP_GREAT_EQ;
            break;
        }
    case TK_LESS :
        {
            op = AST_OP_LESS;
            break;
        }
    case TK_LESS_EQ :
        {
            op = AST_OP_LESS_EQ;
            break;
        }
    case TK_LSHIFT :
        {
            op = AST_OP_LSHIFT;
            break;
        }
    case TK_RSHIFT :
        {
            op = AST_OP_RSHIFT;
            break;
        }
    case TK_EQUAL :
        {
            op = AST_OP_EQUAL;
            break;
        }
    case TK_UNEQUAL :
        {
            op = AST_OP_UNEQUAL;
            break;
        }
    case TK_BITAND :
        {
            op = AST_OP_BIT_AND;
            break;
        }
    case TK_BITOR :
        {
            op = AST_OP_BIT_OR;
            break;
        }
    case TK_BITXOR :
        {
            op = AST_OP_BIT_XOR;
            break;
        }
    case TK_AND :
        {
            op = AST_OP_AND;
            break;
        }
    case TK_OR :
        {
            op = AST_OP_OR;
            break;
        }
	case TK_ASSIGN :
		{
			op = AST_OP_ASSIGN;
			break;
		}
	case TK_BITOR_ASSIGN :
		{
			op = AST_OP_BIT_OR_ASSIGN;
			break;
		}
	case TK_BITXOR_ASSIGN :
		{
			op = AST_OP_BIT_XOR_ASSIGN;
			break;
		}
	case TK_BITAND_ASSIGN :
		{
			op = AST_OP_BIT_AND_ASSIGN;
			break;
		}
	case TK_LSHIFT_ASSIGN :
		{
			op = AST_OP_LSHIFT_ASSIGN;
			break;
		}
	case TK_RSHIFT_ASSIGN :
		{
			op = AST_OP_RSHIFT_ASSIGN;
			break;
		}
	case TK_ADD_ASSIGN :
		{
			op = AST_OP_ADD_ASSIGN;
			break;
		}
	case TK_SUB_ASSIGN :
		{
			op = AST_OP_SUB_ASSIGN;
			break;
		}
	case TK_MUL_ASSIGN :
		{
			op = AST_OP_MUL_ASSIGN;
			break;
		}
	case TK_DIV_ASSIGN :
		{
			op = AST_OP_DIV_ASSIGN;
			break;
		}
	case TK_MOD_ASSIGN :
		{
			op = AST_OP_MOD_ASSIGN;
			break;
		}
    default :
        assert(op == AST_OP_NONE);
    }

    return op;
}


/*
primary_expression
        : IDENTIFIER
        | CONSTANT
        | STRING_LITERAL
        | '(' expression ')'
        ;
*/
t_ast_exp* primary_expression()
{
    t_ast_exp* exp = NULL;
    t_ast_exp_val exp_val;
    memset(&exp_val, 0, sizeof(exp_val));

    switch (cptk)
    {
    case TK_ID :
        {
            exp = make_ast_id_exp(lexeme_value.string_value);
			BINDING_COORDINATE(exp, coord);

            GET_NEXT_TOKEN;
            break;
        }
    case TK_CONST_INTEGER :
    case TK_CONST_CHAR_LITERAL :
        {
            exp_val.i = lexeme_value.integer_value;
            exp = make_ast_const_exp(exp_val, AST_EXP_CONST_INTEGER_KIND);
			BINDING_COORDINATE(exp, coord);

            GET_NEXT_TOKEN;
            break;
        }
    case TK_CONST_FLOAT :
        {
            exp_val.f = lexeme_value.float_value;
            exp = make_ast_const_exp(exp_val, AST_EXP_CONST_FLOAT_KIND);
			BINDING_COORDINATE(exp, coord);
            
            GET_NEXT_TOKEN;
            break;
        }
    case TK_CONST_DOUBLE :
        {
            exp_val.d = (double)lexeme_value.double_value;
            exp = make_ast_const_exp(exp_val, AST_EXP_CONST_DOUBLE_KIND);
			BINDING_COORDINATE(exp, coord);
            
            GET_NEXT_TOKEN;
            break;
        }
    case TK_CONST_LONG_DOUBLE :
        {
            exp_val.ld = lexeme_value.double_value;
            exp = make_ast_const_exp(exp_val, AST_EXP_CONST_LONG_DOUBLE_KIND);
			BINDING_COORDINATE(exp, coord);
            
            GET_NEXT_TOKEN;
            break;
        }
	case TK_CONST_LONG_INTEGER :
        {
            exp_val.l = lexeme_value.integer_value;
            exp = make_ast_const_exp(exp_val, AST_EXP_CONST_LONG_INTEGER_KIND);
			BINDING_COORDINATE(exp, coord);
            
            GET_NEXT_TOKEN;
            break;
        }
	case TK_CONST_LONG_LONG :
        {
            /* [TODO] support long long*/
            exp = make_ast_const_exp(exp_val, AST_EXP_CONST_LONG_LONG_KIND);
			BINDING_COORDINATE(exp, coord);
            
            GET_NEXT_TOKEN;
            break;
        }
	case TK_CONST_UNSIGNED_INTEGER :
        {
            exp_val.ui = lexeme_value.integer_value;
            exp = make_ast_const_exp(exp_val, AST_EXP_CONST_UNSIGNED_INTEGER_KIND);
			BINDING_COORDINATE(exp, coord);
            
            GET_NEXT_TOKEN;
            break;
        }
	case TK_CONST_UNSIGNED_LONG_INTEGER :
        {
            exp_val.ul = lexeme_value.integer_value;
            exp = make_ast_const_exp(exp_val, AST_EXP_CONST_UNSIGNED_LONG_INTEGER_KIND);
			BINDING_COORDINATE(exp, coord);
            
            GET_NEXT_TOKEN;
            break;
        }
	case TK_CONST_UNSIGNED_LONG_LONG :
        {
            /* [TODO] support unsigned long long*/
            exp = make_ast_const_exp(exp_val, AST_EXP_CONST_UNSIGNED_LONG_LONG_KIND);
			BINDING_COORDINATE(exp, coord);
            
            GET_NEXT_TOKEN;
            break;
        }
    case TK_CONST_STRING_LITERAL:
        {
            /* [TODO] wide string in lexer */
			exp_val.p = lexeme_value.string_value;
            exp = make_ast_const_exp(exp_val, AST_EXP_LITERAL_STRING_KIND);
			BINDING_COORDINATE(exp, coord);

            GET_NEXT_TOKEN;
            break;
        }
    case TK_LPAREN :
        {
            GET_NEXT_TOKEN;
            exp = expression();
            match(TK_RPAREN);
            break;
        }
    default :
        error(&coord, "expect identifier, constant, string literal or (");
    }

	return exp;
}

/*
postfix_expression
        : primary_expression
        | postfix_expression '[' expression ']'
        | postfix_expression '(' ')'
        | postfix_expression '(' argument_expression_list ')'
        | postfix_expression '.' IDENTIFIER
        | postfix_expression PTR_OP IDENTIFIER
        | postfix_expression INC_OP
        | postfix_expression DEC_OP
        ;
argument_expression_list : assignment_expression
                                        | argument_expression_list ',' assignment_expression
                                        ;
*/
t_ast_exp* postfix_expression()
{
    t_ast_exp* exp = primary_expression();
    t_ast_list *arguments = make_ast_list_entry(), *c_arg = arguments;
	t_coordinate saved_coord = coord;
    
	assert(exp);

    for (;;)
    {
        switch (cptk)
        {
        case TK_LBRACKET :
            {
                GET_NEXT_TOKEN;

				exp = make_ast_subscript_exp(exp, expression());
				BINDING_COORDINATE(exp, saved_coord);

				match(TK_RBRACKET);
                break;
            }
        case TK_LPAREN :
            {
                GET_NEXT_TOKEN;
                if (cptk != TK_RPAREN)
                {
                    HCC_AST_LIST_APPEND(c_arg, assignment_expression());
                    while (cptk == TK_COMMA)
                    {
                        GET_NEXT_TOKEN;
                        HCC_AST_LIST_APPEND(c_arg, assignment_expression());
                    }
                }

				exp = make_ast_call_exp(exp, arguments);
				BINDING_COORDINATE(exp, saved_coord);

                match(TK_RPAREN);
                break;
            }
        case TK_DOT :
        case TK_ARROW :
            {
				t_ast_exp_op op;
				if (cptk == TK_DOT)
				{
					op = AST_OP_DOT;
				}
				else
				{
					op = AST_OP_PTR;
				}

                GET_NEXT_TOKEN;
				if (cptk != TK_ID)
				{
					syntax_error("expect an identifier as a member of struct or union");
					break;
				}
				
				exp = make_ast_indir_exp(exp, op, lexeme_value.string_value);
				BINDING_COORDINATE(exp, saved_coord);
                
				GET_NEXT_TOKEN;
                break;
            }
        case TK_INC :
        case TK_DEC :
            {
				t_ast_exp_op op = unary_ast_op(cptk);
				exp = make_ast_postop_exp(exp, op);
				BINDING_COORDINATE(exp, saved_coord);

                GET_NEXT_TOKEN;
                break;
            }
        default:
            return exp;
        }
    }
}

/*
unary_expression
        : postfix_expression
        | unary_operator unary_expression
        | sizeof_expression
        | '(' type_name ')' unary_expression
        ;

sizeof_expression
    : SIZEOF unary_expression
	| SIZEOF '(' type_name ')'

unary_operator
        : '&'
        | '*'
        | '+'
        | '-'
        | '~'
        | '!'
        | '++'
        | '--'
        ;
*/
t_ast_exp* unary_expression()
{
    t_ast_exp* exp = NULL;
	t_coordinate saved_coord = coord;
    
	switch (cptk)
	{
	case TK_BITAND :
	case TK_MUL :
	case TK_ADD :
	case TK_SUB :
    case TK_COMP :
    case TK_NOT :
    case TK_INC :
    case TK_DEC :
		{
            t_ast_exp_op op = unary_ast_op(cptk);
            assert(op != AST_OP_NONE);

            GET_NEXT_TOKEN;

            exp = make_ast_unary_exp(unary_expression(), op);
			BINDING_COORDINATE(exp, saved_coord);

            break;
        }
    case TK_LPAREN :
        {
            /*
             * Two possibilites here - 
             * 1. (type name) unary_expression
             * 2. postfix_expression
             * The trick here is postfix_expression requires parsing the '(' itself. 
             * So here we need to peek next token instead of consume TK_LPAREN
             */
            int peek_token_code = peek_token();

            if (is_token_typename_token(peek_token_code, peek_lexeme_value.string_value))
            {
                GET_NEXT_TOKEN; 
                
                /* [TODO][AST] hook type declaration ast with unary ast */
                type_name();
                match(TK_RPAREN);

                exp = make_ast_cast_exp(make_ast_typename_exp(), unary_expression());
				BINDING_COORDINATE(exp, saved_coord);
            }
            else
            {
                exp = postfix_expression();
            }

            break;
        }
    case TK_SIZEOF :
        {
            exp = sizeof_expression();
            break;
        }
    default :
        exp = postfix_expression();
	}

    return exp;
}

/*
sizeof_expression
    : SIZEOF unary_expression
	| SIZEOF '(' type_name ')'
*/
t_ast_exp* sizeof_expression()
{
    t_ast_exp* exp = NULL;
    t_ast_exp* type = NULL;
	t_coordinate saved_coord = coord;

    GET_NEXT_TOKEN;
    
    if (cptk == TK_LPAREN)
    {
        GET_NEXT_TOKEN;

        if (is_token_typename_token(cptk, lexeme_value.string_value))
        {
            type_name();
            type = make_ast_typename_exp();
			BINDING_COORDINATE(type, saved_coord);
        }
        else
        {
            exp = unary_expression();
        }

        match(TK_RPAREN);
    }
    else
    {
        exp = unary_expression();
    }

    exp = make_ast_sizeof_exp(type, exp);
	BINDING_COORDINATE(exp, saved_coord);

	return exp;
}

/*
multiplicative_expression
        : cast_expression
        | multiplicative_expression '*' cast_expression
        | multiplicative_expression '/' cast_expression
        | multiplicative_expression '%' cast_expression
        ;
*/
t_ast_exp* mul_expression()
{
    t_ast_exp* exp = unary_expression();
	t_coordinate saved_coord = coord;

    while (cptk == TK_MUL ||
        cptk == TK_DIV ||
        cptk == TK_MOD)
    {
        t_ast_exp_op op = binary_ast_op(cptk);
		saved_coord = coord;
        GET_NEXT_TOKEN;

        exp = make_ast_binary_exp(exp, op, unary_expression());
		BINDING_COORDINATE(exp, saved_coord);
    }

    return exp;
}

/*
additive_expression
        : multiplicative_expression
        | additive_expression '+' multiplicative_expression
        | additive_expression '-' multiplicative_expression
        ;
*/
t_ast_exp* add_expression()
{
    t_ast_exp* exp = mul_expression();
	t_coordinate saved_coord = coord;

    while (cptk == TK_ADD || cptk == TK_SUB)
    {
        t_ast_exp_op op = binary_ast_op(cptk);
		saved_coord = coord;
        GET_NEXT_TOKEN;

        exp = make_ast_binary_exp(exp, op, mul_expression());
		BINDING_COORDINATE(exp, saved_coord);
    }

    return exp;
}

/*
shift_expression
        : additive_expression
        | shift_expression '<<' additive_expression
        | shift_expression '>>' additive_expression
        ;
*/
t_ast_exp* shift_expression()
{
	t_ast_exp* exp = add_expression();
	t_coordinate saved_coord = coord;

    while (cptk == TK_LSHIFT || cptk == TK_RSHIFT)
    {
		t_ast_exp_op op = binary_ast_op(cptk);
		saved_coord = coord;
        GET_NEXT_TOKEN;

		exp = make_ast_binary_exp(exp, op, add_expression());
		BINDING_COORDINATE(exp, saved_coord);
    }

	return exp;
}

/*
relational_expression
        : shift_expression
        | relational_expression '<' shift_expression
        | relational_expression '>' shift_expression
        | relational_expression '<=' shift_expression
        | relational_expression '>=' shift_expression
        ;
*/
t_ast_exp* rel_expression()
{
	t_ast_exp* exp = shift_expression();
	t_coordinate saved_coord = coord;

    while (cptk == TK_GREAT ||
        cptk == TK_GREAT_EQ ||
        cptk == TK_LESS ||
        cptk == TK_LESS_EQ)
    {
		t_ast_exp_op op = binary_ast_op(cptk);
		saved_coord = coord;
        GET_NEXT_TOKEN;

		exp = make_ast_binary_exp(exp, op, shift_expression());
		BINDING_COORDINATE(exp, saved_coord);
    }

	return exp;
}

/*
equality_expression
        : relational_expression
        | equality_expression '==' relational_expression
        | equality_expression '!=' relational_expression
        ;
*/
t_ast_exp* eql_expression()
{
	t_ast_exp* exp = rel_expression();
	t_coordinate saved_coord = coord;

    while (cptk == TK_EQUAL || cptk == TK_UNEQUAL)
    {
		t_ast_exp_op op = binary_ast_op(cptk);
		saved_coord = coord;
        GET_NEXT_TOKEN;

		exp = make_ast_binary_exp(exp, op, rel_expression());
		BINDING_COORDINATE(exp, saved_coord);
    }

	return exp;
}

/*
and_expression
        : equality_expression
        | and_expression '&' equality_expression
        ;
*/
t_ast_exp* and_expression()
{
	t_ast_exp* exp = eql_expression();
	t_coordinate saved_coord = coord;

    while (cptk == TK_BITAND)
    {
		saved_coord = coord;
        GET_NEXT_TOKEN;

		exp = make_ast_binary_exp(exp, AST_OP_BIT_AND, eql_expression());
		BINDING_COORDINATE(exp, saved_coord);
    }

	return exp;
}

/*
exclusive_or_expression
        : and_expression
        | exclusive_or_expression '^' and_expression
        ;
*/
t_ast_exp* xor_expression()
{
	t_ast_exp* exp = and_expression();
	t_coordinate saved_coord = coord;

    while (cptk == TK_BITXOR)
    {
		saved_coord = coord;
        GET_NEXT_TOKEN;

		exp = make_ast_binary_exp(exp, AST_OP_BIT_XOR, and_expression());
		BINDING_COORDINATE(exp, saved_coord);
    }

	return exp;
}

/*
inclusive_or_expression
        : exclusive_or_expression
        | inclusive_or_expression '|' exclusive_or_expression
        ;
*/
t_ast_exp* or_expression()
{
	t_ast_exp* exp = xor_expression();
	t_coordinate saved_coord = coord;

    while (cptk == TK_BITOR)
    {
		saved_coord = coord;
        GET_NEXT_TOKEN;

		exp = make_ast_binary_exp(exp, AST_OP_BIT_OR, xor_expression());
		BINDING_COORDINATE(exp, saved_coord);
    }

	return exp;
}

/*
logical_and_expression
        : inclusive_or_expression
        | logical_and_expression '&&' inclusive_or_expression
        ;
*/
t_ast_exp* logical_and_expression()
{
    t_ast_exp* exp = or_expression();
	t_coordinate saved_coord = coord;

    while (cptk == TK_AND)
    {
		saved_coord = coord;
        GET_NEXT_TOKEN;

		exp = make_ast_binary_exp(exp, AST_OP_AND, or_expression());
		BINDING_COORDINATE(exp, saved_coord);
    }

	return exp;
}  

/*
logical_or_expression
        : logical_and_expression
        | logical_or_expression '||' logical_and_expression
        ;
*/
t_ast_exp* logical_or_expression()
{
    t_ast_exp* exp = logical_and_expression();
	t_coordinate saved_coord = coord;

    while (cptk == TK_OR)
    {
		saved_coord = coord;
        GET_NEXT_TOKEN;

		exp = make_ast_binary_exp(exp, AST_OP_OR, logical_and_expression());
		BINDING_COORDINATE(exp, saved_coord);
    }

	return exp;
}

/*
conditional_expression
        : logical_or_expression
        | logical_or_expression '?' expression ':' conditional_expression
        ;
*/
t_ast_exp* conditional_expression()
{
    t_ast_exp* exp = logical_or_expression();

    if (cptk == TK_QUESTION)
    {
		t_ast_exp* true_exp = NULL;
		t_ast_exp* false_exp = NULL;
		t_coordinate saved_coord = coord;

        GET_NEXT_TOKEN;
        true_exp = expression();
        match(TK_COLON);
        false_exp = conditional_expression();

		assert(true_exp && false_exp);

		exp = make_ast_conditional_exp(exp, true_exp, false_exp);
		BINDING_COORDINATE(exp, saved_coord);
    }

	return exp;
}

t_ast_exp* constant_expression()
{
    return conditional_expression(); 
}

/*
assignment_expression
        : conditional_expression
        | unary_expression assignment_operator assignment_expression
        ;
*/
t_ast_exp* assignment_expression()
{
    /* here is anothe twist - parsing conditional expression always
     * yield parsing a unary expression and there is no trailing parsing needed
     * so it's absolutely safe to start with parsing a conditional expression first
    */
	t_ast_exp* exp = conditional_expression();

    if (cptk >= TK_ASSIGN && cptk <= TK_MOD_ASSIGN)
    {
		t_ast_exp_op op = binary_ast_op(cptk);
		t_coordinate saved_coord = coord;

        GET_NEXT_TOKEN;
		exp = make_ast_assignment_exp(exp, op, assignment_expression());
		BINDING_COORDINATE(exp, saved_coord);
    }

	return exp;
}

/**
 *  expression:
 *      assignment-expression
 *      expression , assignment-expression
 */
t_ast_exp* expression()
{
	t_ast_exp* exp = NULL;
	t_coordinate saved_coord = coord;

    exp = assignment_expression();
	BINDING_COORDINATE(exp, saved_coord);

    while (cptk == TK_COMMA)
    {
		saved_coord = coord;
        GET_NEXT_TOKEN;
		exp = make_ast_comma_exp(exp, assignment_expression());
		BINDING_COORDINATE(exp, saved_coord);
    }

    return exp;
}

/*
statement
        : labeled_statement
        | compound_statement
        | expression_statement
        | selection_statement
        | iteration_statement
        | jump_statement
        ;
*/
t_ast_stmt* statement()
{
	t_ast_stmt* stmt = NULL;

    switch (cptk)
    {
    case TK_ID :
        stmt = expression_statement();
        break;
    case TK_SWITCH :
        stmt = switch_statement();
        break;
    case TK_CASE :
        stmt = case_statement();
        break;
    case TK_DEFAULT :
        stmt = default_statement();
        break;
    case TK_IF :
        stmt = if_statement();
        break;
    case TK_DO :
        stmt = do_while_statement();
        break;
    case TK_WHILE :
        stmt = while_statement();
        break;
    case TK_FOR :
        stmt = for_statement();
        break;
    case TK_BREAK :
        stmt = break_statement();
        break;
    case TK_CONTINUE :
        stmt = continue_statement();
        break;
    case TK_RETURN :
        stmt = return_statement();
        break;
    case TK_GOTO :
        stmt = goto_statement();
        break;
    case TK_LBRACE :
        stmt = compound_statement();
        break;
    default:
        stmt = expression_statement();
    }

	return stmt;
}

/*
expression_statement
        : ';'
        | expression ';'
        ;
*/
t_ast_stmt* expression_statement()
{
	t_ast_stmt* stmt = make_ast_empty_stmt();

    if (cptk == TK_ID && peek_token() == TK_COLON)
    {
        labeled_statement();
    }
    else if (cptk == TK_ID && strcmp(lexeme_value.string_value, "__asm") == 0)
    {
        /* [FIX ME][ASSEMBLY] 
         * this is a temp workaround to skip inline assembly parsing 
         * parser should invoke assembler to deal with these snippets
        */
        GET_NEXT_TOKEN;

        if (cptk == TK_LBRACE)
        {
            /* skip __asm {} block */
            while (cptk != TK_RBRACE) GET_NEXT_TOKEN;
            match(TK_RBRACE); 
        }
        else
        {
            /* skip single __asm statement */
             while (cptk != TK_RBRACE) GET_NEXT_TOKEN;
        }
        /* else skip multiple single asm stmt.. bang! it can't do that now! [TODO] */
    }
	else if (cptk == TK_SEMICOLON)
	{
		/* empty expression statement */
		GET_NEXT_TOKEN;
	}
    else
    {
        expression();
        match(TK_SEMICOLON);
    }

	return stmt;
}

/*
labeled_statement
        : IDENTIFIER ':' statement
        | CASE constant_expression ':' statement
        | DEFAULT ':' statement
        ;

NOTE - the case and default statement are stripped out of this deriveration.
Both case are dealt explicitly.
*/
t_ast_stmt* labeled_statement()
{
	t_ast_stmt* stmt = NULL;
	t_coordinate saved_coord = coord;

    match(TK_ID); 
	match(TK_COLON);
	statement();
	
	/* [FIX ME] hook with statment instead of empty place holder!*/
	stmt = make_ast_label_stmt(lexeme_value.string_value, make_ast_empty_stmt());
	BINDING_COORDINATE(stmt, saved_coord);
	
	return stmt;
}

t_ast_stmt* case_statement()
{
	t_ast_stmt *stmt = NULL, *body_stmt = NULL;
	t_ast_exp* exp = NULL;
	t_coordinate saved_coord = coord;

    match(TK_CASE); 
	exp = constant_expression();
    match(TK_COLON);
	body_stmt = statement();

	stmt = make_ast_case_stmt(exp, body_stmt);
	BINDING_COORDINATE(stmt, saved_coord);

	return stmt;
}

t_ast_stmt* default_statement()
{
	t_ast_stmt *stmt = NULL, *body_stmt = NULL;
	t_coordinate saved_coord = coord;

    match(TK_DEFAULT);
    match(TK_COLON);
	body_stmt = statement();
	
	stmt = make_ast_default_stmt(body_stmt);
	BINDING_COORDINATE(stmt, saved_coord);

	return stmt;
}

/*
selection_statement
        : IF '(' expression ')' statement
        | IF '(' expression ')' statement ELSE statement
        | SWITCH '(' expression ')' statement
        ;
*/
t_ast_stmt* switch_statement()
{
	t_ast_stmt *stmt = NULL, *switch_stmt = NULL;
	t_ast_exp* test_exp = NULL;
	t_coordinate saved_coord = coord;

    match(TK_SWITCH);
    match(TK_LPAREN);
	test_exp = expression();
    match(TK_RPAREN);
	switch_stmt = statement();
	
	stmt = make_ast_switch_stmt(test_exp, switch_stmt);
	BINDING_COORDINATE(stmt, saved_coord);

	return stmt;
}

t_ast_stmt* if_statement()
{
	t_ast_stmt *stmt = NULL, *then_stmt = NULL, *else_stmt = NULL;
	t_ast_exp* test_exp = NULL;
	t_coordinate saved_coord = coord;
	
    match(TK_IF);
    match(TK_LPAREN);
	test_exp = expression();
    match(TK_RPAREN);
	then_stmt = statement();
    if (cptk == TK_ELSE)
    {
        GET_NEXT_TOKEN;
		else_stmt = statement();
    }

	stmt = make_ast_if_stmt(test_exp, then_stmt, else_stmt);
	BINDING_COORDINATE(stmt, saved_coord);

	return stmt;
}

/*
iteration_statement
        : WHILE '(' expression ')' statement
        | DO statement WHILE '(' expression ')' ';'
        | FOR '(' expression_statement expression_statement ')' statement
        | FOR '(' expression_statement expression_statement expression ')' statement
        ;
*/
t_ast_stmt* do_while_statement()
{
	t_ast_stmt *stmt = NULL, *body_stmt = NULL;
	t_ast_exp* test_exp = NULL;
	t_coordinate saved_coord = coord;

    match(TK_DO);
	body_stmt = statement();
    match(TK_WHILE);
    match(TK_LPAREN);
	test_exp = expression();
    match(TK_RPAREN);
    match(TK_SEMICOLON);

	stmt = make_ast_do_stmt(body_stmt, test_exp);
	BINDING_COORDINATE(stmt, saved_coord);

	return stmt;
}

t_ast_stmt* while_statement()
{
	t_ast_stmt *stmt = NULL, *body_stmt = NULL;
	t_ast_exp* test_exp = NULL;
	t_coordinate saved_coord = coord;

    match(TK_WHILE);
    match(TK_LPAREN);
	test_exp = expression();
    match(TK_RPAREN);
	body_stmt = statement();

	stmt = make_ast_while_stmt(test_exp, body_stmt);
	BINDING_COORDINATE(stmt, saved_coord);

	return stmt;
}

t_ast_stmt* for_statement()
{
	t_ast_stmt *stmt = NULL, *init_exp_stmt = NULL, *test_stmt = NULL, *body_stmt = NULL;
	t_ast_exp* post_test_exp = NULL;
	t_coordinate saved_coord = coord;

    match(TK_FOR);
    match(TK_LPAREN);
	init_exp_stmt = expression_statement();
	test_stmt = expression_statement();
    if (cptk != TK_RPAREN)
    {
		post_test_exp = expression();
    }
    match(TK_RPAREN);
	body_stmt = statement();

	stmt = make_ast_for_stmt(init_exp_stmt, test_stmt, post_test_exp, body_stmt);
	BINDING_COORDINATE(stmt, saved_coord);

	return stmt;
}

/*
jump_statement
        : GOTO IDENTIFIER ';'
        | CONTINUE ';'
        | BREAK ';'
        | RETURN ';'
        | RETURN expression ';'
        ;
*/

t_ast_stmt* goto_statement()
{
	t_ast_stmt* stmt = NULL;

    match(TK_GOTO);
	if (cptk == TK_ID)
	{
		stmt = make_ast_goto_stmt(lexeme_value.string_value);
		BINDING_COORDINATE(stmt, coord);
		GET_NEXT_TOKEN;
	}
	else
	{
		match(TK_ID);
	}
	match(TK_SEMICOLON);

	return stmt;
}

t_ast_stmt* continue_statement()
{
	t_ast_stmt* stmt = make_ast_continue_stmt();
	BINDING_COORDINATE(stmt, coord);

    match(TK_CONTINUE);
    match(TK_SEMICOLON);

	return stmt;
}

t_ast_stmt* break_statement()
{
	t_ast_stmt* stmt = make_ast_break_stmt();
	BINDING_COORDINATE(stmt, coord);

    match(TK_BREAK);
    match(TK_SEMICOLON);

	return stmt;
}

t_ast_stmt* return_statement()
{
	t_ast_stmt* stmt = NULL;
	t_ast_exp* exp = NULL;
	t_coordinate saved_coord = coord;

    match(TK_RETURN);
    if (cptk != TK_SEMICOLON)
    {
		exp = expression();
    }
    match(TK_SEMICOLON);
	
	stmt = make_ast_return_stmt(exp);
	BINDING_COORDINATE(stmt, saved_coord);

	return stmt;
}

/*
compound_statement
        : '{' '}'
        | '{' statement_list '}'
        | '{' declaration_list '}'
        | '{' declaration_list statement_list '}'
        ;
*/
t_ast_stmt* compound_statement()
{
	t_ast_stmt* stmt = NULL;
    t_ast_list *stmts = make_ast_list_entry(), *declrs = make_ast_list_entry();
    t_ast_list *c_stmt = stmts, *c_declr = declrs;
    t_coordinate saved_coord = coord;
    (stmts, declrs, c_stmt, c_declr);

	match(TK_LBRACE);
    enter_scope();

	while (cptk != TK_RBRACE && cptk != TK_END)
	{
		if (is_current_token_declaration_specifier_token())
		{
			if (cptk == TK_ID && peek_token() == TK_COLON)
			{
                HCC_AST_LIST_APPEND(c_stmt, statement());
			}
			else
			{
                declaration();
            }
        }
		else
		{
            HCC_AST_LIST_APPEND(c_stmt, statement());
		}
	}

    match(TK_RBRACE);
    exit_scope();

    stmt = make_ast_compound_stmt(stmts, declrs);
    BINDING_COORDINATE(stmt, saved_coord);

	return stmt;
}

/*
declaration
        : declaration_specifiers ';'
        | declaration_specifiers init_declarator_list ';'
        ;
*/
void declaration()
{
	int storage_class = declaration_specifiers();
	if (cptk == TK_SEMICOLON)
	{
		GET_NEXT_TOKEN;
		return;
	}

	init_declarator(storage_class);

	while (cptk == TK_COMMA)
	{
		GET_NEXT_TOKEN;
		init_declarator(storage_class);
	}

    /* beginning of a compound statement */ 
    if (cptk == TK_LBRACE)
    {
        return;
    }

	match(TK_SEMICOLON);
}

int declaration_specifiers()
{
    int storage_specifier = TK_AUTO;

	int alien_type_engaged = 0;

    for(;;)
    {
        switch(cptk)
        {
        case TK_AUTO:
        case TK_REGISTER:
        case TK_EXTERN:
        case TK_STATIC:
        case TK_TYPEDEF:
            storage_specifier = cptk;
            GET_NEXT_TOKEN;
            break;
        case TK_CONST:
        case TK_VOLATILE:
            GET_NEXT_TOKEN;
            break;
        case TK_FLOAT:
        case TK_DOUBLE:
        case TK_CHAR:
        case TK_SHORT:
        case TK_INT:
        case TK_SIGNED:
        case TK_UNSIGNED:
        case TK_VOID:
        case TK_LONG:
		case TK_INT64:
            GET_NEXT_TOKEN;
            break;
        case TK_ID:
			if (!alien_type_engaged && is_typedef_id(lexeme_value.string_value))
            {
				alien_type_engaged = 1;
                GET_NEXT_TOKEN;
				break;
            }
            return storage_specifier;
        case TK_STRUCT:
        case TK_UNION:
            struct_or_union_specifier();
			alien_type_engaged = 1;
            break;
        case TK_ENUM:
            enum_specifier();
			alien_type_engaged = 1;
            break;
        default:
            return storage_specifier;
        }
    }
}

/*
init_declarator
	: declarator
	| declarator '=' initializer
	;
*/
void init_declarator(int storage_class)
{
    declarator(storage_class);

	if (cptk == TK_ASSIGN)
	{
		GET_NEXT_TOKEN;
		initializer();
	}
}

/*
initializer
	: assignment_expression
	| '{' initializer_list '}'
	| '{' initializer_list ',' '}'
	;

initializer_list
	: initializer
	| initializer_list ',' initializer
	;
*/
void initializer()
{
	if (cptk == TK_LBRACE)
	{
		GET_NEXT_TOKEN;
		
		initializer();
		while (cptk == TK_COMMA)
		{
			GET_NEXT_TOKEN;
			if (cptk == TK_RBRACE) break;
			initializer();
		}

		match(TK_RBRACE);
	}
	else
	{
		assignment_expression();
	}
}

/*
parameter_type_list
	: parameter_list
	| parameter_list ',' ELLIPSIS
	;
parameter_list
	: parameter_declaration
	| parameter_list ',' parameter_declaration
	;
*/
void parameter_type_list()
{
	parameter_declaration();

	while (cptk == TK_COMMA)
	{
		GET_NEXT_TOKEN;
		if (cptk == TK_ELLIPSE)
		{
			GET_NEXT_TOKEN;
			break;
		}

		parameter_declaration();
	}
}

/*
parameter_declaration
	: declaration_specifiers declarator
	| declaration_specifiers abstract_declarator
	| declaration_specifiers
	;
*/
void parameter_declaration()
{
	int storage_class = declaration_specifiers();

    if (cptk == TK_MUL)
    {
        pointer();
	}  

    if (!is_current_token_declarator_token() && cptk != TK_LBRACKET)
    {
        if (cptk == TK_ID && is_typedef_id(lexeme_value.string_value))
        {
            /* typedef name is hidden by redeclaring the name as a normal identifier */
            t_symbol* sym = find_symbol(lexeme_value.string_value, sym_table_identifiers);
            sym->hidden_typedef = 1;
            record_hidden_typedef_name(sym);
        }
        else
        {
            /*
            * look ahead is neither a possible start of a direct declarator, nor
            * an direct abstract direclarator. 
            * This is a case where the parameter declaration is just typename*
            */
            return;
        }
    }
    
    /*
     * sample torture:
     * int foo(const void*, int (__cdecl * _PtFuncCompare)(void *, const void *, const void *), const void*);
     *
	 */
	/*
	direct_declarator
	: IDENTIFIER
	| '(' declarator ')'
	;
	*/
	/*
	direct_abstract_declarator
	: '(' abstract_declarator ')'
	: suffix_declarator
	*/

	if (cptk == TK_ID)
	{
		direct_declarator(storage_class);
	}
	else if (cptk == TK_LBRACKET)
	{
		direct_abstract_declarator();
	}
	else
	{
		assert(cptk == TK_LPAREN);
		GET_NEXT_TOKEN;
		
		if (cptk == TK_RPAREN)
		{
			/* empty suffix declarator */
			return;
		}
		else if (is_current_token_declaration_specifier_token())
		{
			/* ( parameter_type_list) in suffix declarator */
            enter_scope();
			parameter_type_list();
			match(TK_RPAREN);
            if (cptk != TK_LBRACE)
            {
                exit_scope();
            }
		}
		else
		{
            all_declarator(storage_class);
			match(TK_RPAREN);
		}
	}

	while (cptk == TK_LPAREN || cptk == TK_LBRACKET)
	{
		suffix_declarator();
	}
}

/*
pointer
	: '*'
	| '*' type_qualifier_list
	| '*' pointer
	| '*' type_qualifier_list pointer
	;
*/
void pointer()
{
	while (cptk == TK_MUL)
	{
		GET_NEXT_TOKEN;
		while (cptk == TK_CONST || cptk == TK_VOLATILE)
		{
			GET_NEXT_TOKEN;
		}
	}
}

/*
suffix_declarator
	| '[' constant_expression ']'
	| '[' ']'
	|  '(' parameter_type_list ')'
	|  '(' identifier_list ')'
	|  '(' ')'
*/
void suffix_declarator()
{
    if (cptk == TK_LBRACKET)
    {
        GET_NEXT_TOKEN;
        if (cptk != TK_RBRACKET)
        {
            constant_expression();
        }
        match(TK_RBRACKET);
    }
    else if (cptk == TK_LPAREN)
    {
        int new_scope = 0;

        GET_NEXT_TOKEN;
        
        /* parameter type list always starts with declaration specifiers */
		if (is_current_token_declaration_specifier_token())
        {
            enter_scope();
            new_scope = 1;
            parameter_type_list();
        }
        else
        {
            if (cptk == TK_ID)
            {
                GET_NEXT_TOKEN;
                while (cptk == TK_COMMA)
                {
                    GET_NEXT_TOKEN;
                    match(TK_ID);
                }
            }
        }

        match(TK_RPAREN);

        if (new_scope)
        {
            if (cptk != TK_LBRACE)
            {
                exit_scope();
            }
        }
    }
}

/*
declarator
	: pointer direct_declarator
	| direct_declarator
	;
*/
void declarator(int storage_class)
{
	if (cptk == TK_MUL)
	{
		pointer();
	}
    direct_declarator(storage_class);

    while (cptk == TK_LPAREN || cptk == TK_LBRACKET)
    {
        suffix_declarator();
    }
}

/*
direct_declarator
	: IDENTIFIER
	| '(' declarator ')'
	;
*/
void direct_declarator(int storage_class)
{
    if (cptk == TK_LPAREN)
    {
        GET_NEXT_TOKEN;
        declarator(storage_class);
        match(TK_RPAREN);
    }
    else
    {
		if (cptk != TK_ID)
		{
			syntax_error("direct declarator must end with an identifier");
		}

		/*
		 * parser has to know if an ID is typedefined type name or not to make some decisions
		 * this is the place where C grammar's LL(0) is violated
		 * otherwise symbol management could all be done in semantic checking phase.
		*/
		if (storage_class == TK_TYPEDEF)
		{
			t_symbol* symbol = add_symbol(lexeme_value.string_value, &sym_table_identifiers, symbol_scope, FUNC);
			symbol->storage = TK_TYPEDEF;
		}
		else
		{
			/* [DEBUG] */
			(lexeme_value.string_value);
			(storage_class);
		}

        /* DEBUG */
        if (strcmp("CreatePrivateObjectSecurity"/*"PRKCRM_MARSHAL_HEADER"*/, lexeme_value.string_value) == 0)
        {
			int a = 0;
            (a);
        }

		GET_NEXT_TOKEN;
    }
}

/*
abstract_declarator
	: pointer
	| direct_abstract_declarator
	| pointer direct_abstract_declarator
	;
*/
void abstract_declarator()
{
	if (cptk == TK_MUL)
	{
		pointer();

		/*
		 * look ahead is neither '(' nor '[', we are done.
		 * In this case abstract declarator is just a plain pointer.
         */
		if (cptk != TK_LPAREN && cptk != TK_LBRACKET)
		{
			return;
		}
	}

	direct_abstract_declarator();

	while (cptk == TK_LPAREN || cptk == TK_LBRACKET)
	{
		suffix_declarator();
	}
}

/*
direct_abstract_declarator
	: '(' abstract_declarator ')'
	: suffix_declarator
*/
void direct_abstract_declarator()
{
	if (cptk == TK_LPAREN)
	{
		GET_NEXT_TOKEN;

		/*
	     * look ahead is either ')' or parameter_type_list, so we must processing the suffix declarator.
		 */
		if (cptk == TK_RPAREN || is_current_token_declaration_specifier_token())
		{
			suffix_declarator();
		}
		else
		{
			abstract_declarator();
			match(TK_RPAREN);
		}
	}
	else
	{
		syntax_error("illegal token found in abstract declarator!");
	}
}

/* this function parse declarator regardless of its type - that is, it could parse
 * both declarator and abstract declarator. it should only be used when absolutely neccessary
 * because a function should only do one thing well. 
 */
void all_declarator(int storage_class)
{
    if (cptk == TK_MUL)
    {
        pointer();

        if (cptk != TK_LPAREN && 
            cptk != TK_LBRACKET &&
            cptk != TK_ID) 
        {
            return;
        }
    }

    if (cptk == TK_ID)
    {
		/* only typedef symbol should be recorded at parsing stage */
		if (storage_class == TK_TYPEDEF)
		{
			t_symbol* symbol = add_symbol(lexeme_value.string_value, &sym_table_identifiers, symbol_scope, FUNC);
			symbol->storage = TK_TYPEDEF;
		}

        GET_NEXT_TOKEN;
    }
    else if (cptk == TK_LPAREN)
    {
        all_declarator(storage_class);
    }
    else
    {
        if (cptk != TK_LBRACKET) syntax_error("declarator error");
    }
    
    while (cptk == TK_LPAREN || cptk == TK_LBRACKET)
    {
        suffix_declarator();
    }
}

/*
struct_or_union_specifier
	: struct_or_union IDENTIFIER '{' struct_declaration_list '}'
	| struct_or_union '{' struct_declaration_list '}'
	| struct_or_union IDENTIFIER
	;
*/
void struct_or_union_specifier()
{
    assert(cptk == TK_STRUCT || cptk == TK_UNION);

    GET_NEXT_TOKEN;
    if (cptk  == TK_ID)
    {
        /* [TODO] [SYMBOL MANAGE] - install tag name into the types table? */
        GET_NEXT_TOKEN;
    }

    if (cptk == TK_LBRACE)
    {
        GET_NEXT_TOKEN;
        struct_declaration_list();
        match(TK_RBRACE);
    }
}

/*
struct_declaration_list
	: struct_declaration
	| struct_declaration_list struct_declaration
	;
struct_declaration
	: specifier_qualifier_list struct_declarator_list ';'
	;
struct_declarator_list
	: struct_declarator
	| struct_declarator_list ',' struct_declarator
	;
*/
void struct_declaration_list()
{
    do
    {
		specifiers_qualifier_list();
        struct_declarator();
        
        while (cptk == TK_COMMA)
        {
            GET_NEXT_TOKEN;
            struct_declarator();
        }

        match(TK_SEMICOLON);
    }
    while (cptk != TK_RBRACE);
}

/*
struct_declarator
	: declarator
	| ':' constant_expression
	| declarator ':' constant_expression
	;
*/
void struct_declarator()
{
    /* short circuit anonymous struct / union */
    if (cptk == TK_SEMICOLON)
    {
        return;
    }
   
    if (cptk != TK_COLON)
    {
        declarator(TK_AUTO);
    }

    if (cptk == TK_COLON)
    {
        GET_NEXT_TOKEN;
        constant_expression();
    }
}

/*
specifier_qualifier_list
	: type_specifier specifier_qualifier_list
	| type_specifier
	| type_qualifier specifier_qualifier_list
	| type_qualifier
	;
*/
void specifiers_qualifier_list()
{
    int alien_type_engaged = 0;

    for(;;)
    {
        switch(cptk)
        {
        case TK_CONST:
        case TK_VOLATILE:
            GET_NEXT_TOKEN;
            break;
        case TK_FLOAT:
        case TK_DOUBLE:
        case TK_CHAR:
        case TK_SHORT:
        case TK_INT:
        case TK_SIGNED:
        case TK_UNSIGNED:
        case TK_VOID:
        case TK_LONG:
		case TK_INT64:
            GET_NEXT_TOKEN;
            break;
        case TK_ID:
			if (!alien_type_engaged && is_typedef_id(lexeme_value.string_value))
            {
                GET_NEXT_TOKEN;
                alien_type_engaged = 1; 
                break;
            }
            return;
        case TK_STRUCT:
        case TK_UNION:
            struct_or_union_specifier();
			alien_type_engaged = 1;
            break;
        case TK_ENUM:
            enum_specifier();
			alien_type_engaged = 1;
            break;
        case TK_AUTO:
        case TK_REGISTER:
        case TK_EXTERN:
        case TK_STATIC:
        case TK_TYPEDEF:
            {
                syntax_error("illegal storage class appears");
                return;
            }
        default:
            return;
        }
    }
}

/*
enum_specifier
	: ENUM '{' enumerator_list '}'
	| ENUM IDENTIFIER '{' enumerator_list '}'
	| ENUM IDENTIFIER
	;
enumerator_list
	: enumerator
	| enumerator_list ',' enumerator
	;
*/
void enum_specifier()
{
    //
    // this flag is to indicate that at least we have parsed either { , or ID
    // which are the only legal suffix following enum keyword..
    //
    int flag = 0;

    match(TK_ENUM);

    if (cptk == TK_ID)
    {
        GET_NEXT_TOKEN;
        flag = 1;
    }

    if (cptk == TK_LBRACE)
    {
        GET_NEXT_TOKEN;
        enumerator();
        while (cptk == TK_COMMA)
        {
            GET_NEXT_TOKEN;
            enumerator();
        }

        match(TK_RBRACE);
        flag = 1;
    }

    if (0 == flag)
    {
        syntax_error("Error when parsing enum specifier : { or identifier expected");
    }
}

/*
enumerator
	: IDENTIFIER
	| IDENTIFIER '=' constant_expression
	;
*/
void enumerator()
{
    if (cptk != TK_ID)
    {
		/*
		 * deal with "unclosed" enum member declaration like
		 * enum { a, b, c, }
		 */
		if (cptk == TK_RBRACE)
		{
			return;
		}

        syntax_error("enumerator must be identifier!");
        return;
    }

    GET_NEXT_TOKEN;
    if (cptk == TK_ASSIGN)
    {
        GET_NEXT_TOKEN;
        constant_expression();
    }
}

/*
translation_unit
	: external_declaration
	| translation_unit external_declaration
	;
*/
void translation_unit()
{
	while (cptk != TK_END)
	{
		external_declaration();
	}
}

/*
external_declaration
	: function_definition  
	| declaration
	;
function_definition
	: declaration_specifiers declarator declaration_list compound_statement
	| declaration_specifiers declarator compound_statement
	| declarator declaration_list compound_statement
	| declarator compound_statement
	;

declaration_list
	: declaration
	| declaration_list declaration
	;

declaration
	: declaration_specifiers ';'
	| declaration_specifiers init_declarator_list ';'
	;

init_declarator_list
	: init_declarator
	| init_declarator_list ',' init_declarator
	;

init_declarator
	: declarator
	| declarator '=' initializer
	;
*/
void external_declaration()
{
    int storage_class = TK_AUTO;

    if (is_current_token_declarator_token())
    {
        /* [FIX ME] - IS THAT NEEDED? function omit types have extern int assumed... */
        declarator(TK_AUTO);

        if (cptk != TK_LBRACE)
        {
            /* declaration list */
            while (is_current_token_declaration_specifier_token())
            {
                declaration();
            }
        }
        
        /* function body */
        compound_statement();
        return;
    }

    storage_class = declaration_specifiers();
    
    if (cptk == TK_SEMICOLON)
    {
		/* for non struct/union declaration this should issue a warning */
		/* TODO - for example, int; long; is by itself not meaningful declarations */
        GET_NEXT_TOKEN;
        return;
    }

    declarator(storage_class);
    
	if (cptk == TK_SEMICOLON)
	{
        /* declaration_specifiers declarator ;*/
		GET_NEXT_TOKEN;
        return;
	}
    else if (cptk == TK_LBRACE)
    {
        /* declaration_specifiers declarator compound_statement */
        compound_statement();
        return;
    }
    else if (is_current_token_declaration_specifier_token())
    {
        /* declaration_specifiers declarator declaration_list compound_statement*/
        while (is_current_token_declaration_specifier_token())
        {
            declaration();
        }

        compound_statement();
        return;
    }
  
    /*
     * survive crossfire
	 * declaration_specifiers init_declarator_list
     */

    /* init_declarator_list */
    if (cptk == TK_COMMA || cptk == TK_ASSIGN)
    {
        if (cptk == TK_ASSIGN)
        {
            GET_NEXT_TOKEN;
            initializer();
        }

        while (cptk == TK_COMMA)
        {
            GET_NEXT_TOKEN;
            init_declarator(storage_class);
        }

        match(TK_SEMICOLON);
    }
    else
    {
		syntax_error("illegal token detected in declaration parsing");
    }
}

/*
type_name
        : specifier_qualifier_list
        | specifier_qualifier_list abstract_declarator
        ;
*/
void type_name()
{
    specifiers_qualifier_list();

    if (cptk == TK_MUL || cptk == TK_LPAREN || cptk == TK_LBRACKET)
    {
        abstract_declarator();
    }
}


int is_typedef_id(char* token_name)
{
    t_symbol* sym = find_symbol(token_name, sym_table_identifiers);

    return (sym != NULL) && (sym->storage == TK_TYPEDEF) && (sym->scope <= symbol_scope);
}

int is_current_token_declaration_specifier_token()
{
    if (cptk == TK_AUTO ||
        cptk == TK_EXTERN ||
        cptk == TK_REGISTER ||
        cptk == TK_STATIC ||
        cptk == TK_TYPEDEF ||
        cptk == TK_CONST ||
        cptk == TK_VOLATILE ||
        cptk == TK_SIGNED ||
        cptk == TK_UNSIGNED ||
        cptk == TK_SHORT ||
        cptk == TK_LONG ||
        cptk == TK_CHAR ||
        cptk == TK_INT ||
        cptk == TK_FLOAT ||
        cptk == TK_DOUBLE ||
        cptk == TK_ENUM ||
        cptk == TK_STRUCT ||
        cptk == TK_UNION ||
        cptk == TK_VOID ||
		cptk == TK_INT64)
    {
        return 1;
    }
    else if (cptk == TK_ID)
    {
        return is_typedef_id(lexeme_value.string_value);
    }
    else
    {
        return 0;
    }
}

int is_current_token_declarator_token()
{
	if (cptk == TK_MUL ||
		cptk == TK_LPAREN ||
		(cptk == TK_ID && !is_typedef_id(lexeme_value.string_value)))
	{
		return 1;
	}
	else
	{
        return 0;
	}
}

int is_token_typename_token(int token_code, char* token_symbol)
{
    if (token_code == TK_CONST ||
        token_code == TK_VOLATILE ||
        token_code == TK_FLOAT ||
        token_code == TK_DOUBLE ||
        token_code == TK_CHAR ||
        token_code == TK_SHORT ||
        token_code == TK_INT ||
        token_code == TK_SIGNED ||
        token_code == TK_UNSIGNED ||
        token_code == TK_VOID ||
        token_code == TK_LONG ||
        token_code == TK_INT64 ||
        token_code == TK_STRUCT ||
        token_code == TK_UNION ||
        token_code == TK_ENUM)
    {
        return 1;
    }
    else if (token_symbol != NULL)
    {
        return is_typedef_id(token_symbol);
    }
      
    return 0;
}
