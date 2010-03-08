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


static t_ast_native_type_kind token_to_ast_native_type(int token_code)
{
	t_ast_native_type_kind kind;

	switch (token_code)
	{
		case TK_FLOAT:
			kind = AST_NTYPE_FLOAT;
			break;
        case TK_DOUBLE:
			kind = AST_NTYPE_DOUBLE;
			break;
        case TK_CHAR:
			kind = AST_NTYPE_CHAR;
			break;
        case TK_SHORT:
			kind = AST_NTYPE_SHORT;
			break;
        case TK_INT:
			kind = AST_NTYPE_INT;
			break;
        case TK_SIGNED:
			kind = AST_NTYPE_SIGNED;
			break;
        case TK_UNSIGNED:
			kind = AST_NTYPE_UNSIGNED;
			break;
        case TK_VOID:
			kind = AST_NTYPE_VOID;
			break;
        case TK_LONG:
			kind = AST_NTYPE_LONG;
			break;
		case TK_INT64:
			kind = AST_NTYPE_INT64;
			break;
		default:
			assert(0);
	}

	return kind;
}

static t_ast_storage_specifier_kind token_to_ast_storage_kind(int token_code)
{
	t_ast_storage_specifier_kind kind;

	switch (token_code)
	{
		case TK_AUTO:
			kind = AST_STORAGE_AUTO;
			break;
        case TK_REGISTER:
			kind = AST_STORAGE_REGISTER;
			break;
        case TK_EXTERN:
			kind = AST_STORAGE_EXTERN;
			break;
        case TK_STATIC:
			kind = AST_STORAGE_STATIC;
			break;
        case TK_TYPEDEF:
			kind = AST_STORAGE_TYPEDEF;
			break;
		default:
			assert(0);
	}

	return kind;
}
static t_ast_type_qualifier_kind token_to_ast_type_qualifier_kind(int token_code)
{
    if (token_code == TK_CONST)
    {
        return AST_TYPE_CONST;
    }
    else if (token_code == TK_VOLATILE)
    {
        return AST_TYPE_VOLATILE;
    }
    else
    {
        /* restrict etc C99, for now no..*/
        assert(0);
        return AST_TYPE_CONST;
    }
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
t_ast_declaration* declaration()
{
	t_ast_declaration_specifier* declr_specifiers = declaration_specifiers();
	int storage_class = declr_specifiers->storage_class;
    t_ast_list *init_declr_list = make_ast_list_entry(), *c_list = init_declr_list;
    t_coordinate saved_coord = coord;
    t_ast_declaration* declr = NULL;
   
	if (cptk == TK_SEMICOLON)
	{
        declr = make_ast_declaration(declr_specifiers, init_declr_list);
        BINDING_COORDINATE(declr, saved_coord);

		GET_NEXT_TOKEN;
        return declr;
	}

    HCC_AST_LIST_APPEND(c_list, init_declarator(storage_class));

	while (cptk == TK_COMMA)
	{
		GET_NEXT_TOKEN;
        HCC_AST_LIST_APPEND(c_list, init_declarator(storage_class));
	}

    declr = make_ast_declaration(declr_specifiers, init_declr_list);
    BINDING_COORDINATE(declr, saved_coord);

    /* beginning of a compound statement */ 
    if (cptk == TK_LBRACE)
    {
        return declr;
    }

	match(TK_SEMICOLON);

    return declr;
}

/*
declaration_specifiers
	: storage_class_specifier
	| storage_class_specifier declaration_specifiers
	| type_specifier
	| type_specifier declaration_specifiers
	| type_qualifier
	| type_qualifier declaration_specifiers
	;
*/
t_ast_declaration_specifier* declaration_specifiers()
{
    int storage_specifier = TK_AUTO;
	t_ast_storage_specifier_kind storage_kind = AST_STORAGE_AUTO;
	t_ast_type_specifier* s = NULL;
	t_ast_list* list = make_ast_list_entry();
	t_ast_declaration_specifier* declr_specifiers = make_ast_declaration_specifier(list);
	int alien_type_engaged = 0;
	int storage_specifier_engaged = 0;

	BINDING_COORDINATE(declr_specifiers, coord);
	declr_specifiers->storage_kind = storage_kind;
	declr_specifiers->storage_class = storage_specifier;

    for(;;)
    {
        switch(cptk)
        {
        case TK_AUTO:
        case TK_REGISTER:
        case TK_EXTERN:
        case TK_STATIC:
        case TK_TYPEDEF:
			{
				t_ast_storage_specifier* s = NULL;
				if (storage_specifier_engaged)
				{
					/* another place to do semantic checking durin parsing. storage kind is
					 * a critical value for parser to make parsing decision so got to make it correct
					 * in the first place.
					 */
					syntax_error("more than one storage class specified");
					return declr_specifiers;
				}

				storage_specifier_engaged = 1;
				storage_specifier = cptk;
				storage_kind = token_to_ast_storage_kind(cptk);
				s = make_ast_storage_specifier(storage_kind);
				BINDING_COORDINATE(s, coord);
				HCC_AST_LIST_APPEND(list, s);

				declr_specifiers->storage_kind = storage_kind;
				declr_specifiers->storage_class = storage_specifier;
				
				GET_NEXT_TOKEN;
				break;
			}
        case TK_CONST:
        case TK_VOLATILE:
			{
				t_ast_type_qualifier_kind kind = (cptk == TK_CONST)? AST_TYPE_CONST : AST_TYPE_VOLATILE;
				t_ast_type_qualifier* q = make_ast_type_qualifer(kind);
				BINDING_COORDINATE(q, coord);
				HCC_AST_LIST_APPEND(list, q);

				GET_NEXT_TOKEN;
				break;
			}
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
			{
				s = make_ast_type_specifier_native_type(token_to_ast_native_type(cptk));
				BINDING_COORDINATE(s, coord);
				HCC_AST_LIST_APPEND(list, s);

				GET_NEXT_TOKEN;
				break;
			}
        case TK_ID:
			if (!alien_type_engaged && is_typedef_id(lexeme_value.string_value))
            {
				s = make_ast_type_specifier_typedef(lexeme_value.string_value);
				BINDING_COORDINATE(s, coord);
				HCC_AST_LIST_APPEND(list, s);

				alien_type_engaged = 1;
                GET_NEXT_TOKEN;
				break;
            }

            return declr_specifiers;
        case TK_STRUCT:
		case TK_UNION:
			{
				s = make_ast_type_specifier_struct_union(struct_or_union_specifier());
				HCC_AST_LIST_APPEND(list, s);

				alien_type_engaged = 1;
				break;
			}
        case TK_ENUM:
			{
				s = make_ast_type_specifier_enum(enum_specifier());
				HCC_AST_LIST_APPEND(list, s);

				alien_type_engaged = 1;
				break;
			}
        default:
            return declr_specifiers;
        }
    }
}

/*
init_declarator
	: declarator
	| declarator '=' initializer
	;
*/
t_ast_init_declarator* init_declarator(int storage_class)
{
    t_ast_init_declarator* init_declr = NULL;
    t_coordinate saved_coord = coord;
    t_ast_initializer* init = NULL;

    t_ast_declarator* declr = declarator(storage_class);

	if (cptk == TK_ASSIGN)
	{
		GET_NEXT_TOKEN;
        init = initializer();
	}

    init_declr = make_ast_init_declarator(declr, init);
    BINDING_COORDINATE(init_declr, saved_coord);

    return init_declr;
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
t_ast_initializer* initializer()
{
    t_ast_list *initializer_list = make_ast_list_entry(), *c_list = initializer_list;
    t_ast_exp* exp = NULL;
    t_coordinate saved_coord = coord;
    t_ast_initializer* r = NULL;
    int comma_ending = 0;

	if (cptk == TK_LBRACE)
	{
		GET_NEXT_TOKEN;
		
        HCC_AST_LIST_APPEND(c_list, initializer());
		while (cptk == TK_COMMA)
		{
			GET_NEXT_TOKEN;
			if (cptk == TK_RBRACE) 
            {
                comma_ending = 1;
                break;
            }

            HCC_AST_LIST_APPEND(c_list, initializer());
		}

		match(TK_RBRACE);
	}
	else
	{
		exp = assignment_expression();
	}

    r = make_ast_initializer(exp, initializer_list, comma_ending);
    BINDING_COORDINATE(r, saved_coord);

    return r;
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
t_ast_param_type_list* parameter_type_list()
{
	t_ast_list *list = make_ast_list_entry();
	t_ast_param_type_list* param_type_list = make_ast_parameter_type_list(list, 0);
	BINDING_COORDINATE(param_type_list, coord);

	HCC_AST_LIST_APPEND(list, parameter_declaration());

	while (cptk == TK_COMMA)
	{
		GET_NEXT_TOKEN;
		if (cptk == TK_ELLIPSE)
		{
			GET_NEXT_TOKEN;

			param_type_list->has_ellipsis = 1;

			break;
		}

		HCC_AST_LIST_APPEND(list, parameter_declaration());
	}

	return param_type_list;
}

/*
parameter_declaration
	: declaration_specifiers declarator
	| declaration_specifiers abstract_declarator
	| declaration_specifiers
	;
*/
t_ast_parameter_declaration* parameter_declaration()
{
    t_ast_parameter_declaration* param_delcr = NULL;
	t_ast_declaration_specifier* declr_specifiers = declaration_specifiers();
	int storage_class = declr_specifiers->storage_class;
    t_ast_pointer* ptr = NULL;
    t_ast_list* suffix_declr_list = make_ast_list_entry();
    t_ast_direct_declarator* direct_declr = NULL;
    t_ast_direct_abstract_declarator* direct_abstract_declr = NULL;
	t_ast_all_declarator* all_declr = NULL;
	t_coordinate saved_coord = coord;
    
    if (cptk == TK_MUL)
    {
        ptr = pointer();
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
            param_delcr = make_ast_parameter_declaration(declr_specifiers, direct_declr, direct_abstract_declr, all_declr, ptr, suffix_declr_list);
            BINDING_COORDINATE(param_delcr, saved_coord);
            
            return param_delcr;
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
        direct_declr = direct_declarator(storage_class);
	}
	else if (cptk == TK_LBRACKET)
	{
        direct_abstract_declr = direct_abstract_declarator();
	}
	else
	{
		assert(cptk == TK_LPAREN);
		GET_NEXT_TOKEN;
		
		if (cptk == TK_RPAREN)
		{
			/* empty suffix declarator */
			param_delcr = make_ast_parameter_declaration(declr_specifiers, direct_declr, direct_abstract_declr, all_declr, ptr, suffix_declr_list);
            BINDING_COORDINATE(param_delcr, saved_coord); 
    
            return param_delcr;
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
			all_declr = all_declarator(storage_class);
			match(TK_RPAREN);
		}
	}

	param_delcr = make_ast_parameter_declaration(declr_specifiers, direct_declr, direct_abstract_declr, all_declr, ptr, suffix_declr_list);
    BINDING_COORDINATE(param_delcr, saved_coord);

	while (cptk == TK_LPAREN || cptk == TK_LBRACKET)
	{
        HCC_AST_LIST_APPEND(suffix_declr_list, suffix_declarator());
	}

    return param_delcr;
}

/*
pointer
	: '*'
	| '*' type_qualifier_list
	| '*' pointer
	| '*' type_qualifier_list pointer
	;
*/
t_ast_pointer* pointer()
{
    /* personally I think this code is ugly. the cost of not using recursion. */
    t_ast_list* type_qualifier_list =  make_ast_list_entry();
    t_ast_pointer* pointer = make_ast_pointer(type_qualifier_list, NULL);
    t_ast_pointer* c_pointer = pointer;
    BINDING_COORDINATE(pointer, coord);

    assert(cptk == TK_MUL);

    GET_NEXT_TOKEN;
    while (cptk == TK_CONST || cptk == TK_VOLATILE)
    {
        t_ast_type_qualifier_kind kind = token_to_ast_type_qualifier_kind(cptk);
        t_ast_type_qualifier* qualifier = make_ast_type_qualifer(kind);
        BINDING_COORDINATE(qualifier, coord);
        HCC_AST_LIST_APPEND(type_qualifier_list, qualifier);

        GET_NEXT_TOKEN;
    }

	while (cptk == TK_MUL)
	{
        type_qualifier_list = make_ast_list_entry();
        c_pointer->pointer = make_ast_pointer(type_qualifier_list, NULL);

		GET_NEXT_TOKEN;
		while (cptk == TK_CONST || cptk == TK_VOLATILE)
		{
            t_ast_type_qualifier_kind kind = token_to_ast_type_qualifier_kind(cptk);
            t_ast_type_qualifier* qualifier = make_ast_type_qualifer(kind);
            BINDING_COORDINATE(qualifier, coord);
            HCC_AST_LIST_APPEND(type_qualifier_list, qualifier);

			GET_NEXT_TOKEN;
		}

        c_pointer = c_pointer->pointer;
	}

    return pointer;
}

/*
suffix_declarator
	| '[' constant_expression ']'
	| '[' ']'
	|  '(' parameter_type_list ')'
	|  '(' identifier_list ')'
	|  '(' ')'
*/
t_ast_suffix_declarator* suffix_declarator()
{
    t_ast_suffix_declarator* suffix_declr = NULL;
    t_ast_exp* exp = NULL;
    t_ast_list *ids = make_ast_list_entry();
    t_coordinate saved_coord = coord;

    if (cptk == TK_LBRACKET)
    {
        GET_NEXT_TOKEN;
        if (cptk != TK_RBRACKET)
        {
            exp = constant_expression();
        }
        match(TK_RBRACKET);

        suffix_declr = make_ast_subscript_declarator(exp);
        BINDING_COORDINATE(suffix_declr, saved_coord);
    }
    else if (cptk == TK_LPAREN)
    {
        int new_scope = 0;

		suffix_declr = make_ast_parameter_list_declarator(NULL, ids);
		BINDING_COORDINATE(suffix_declr, saved_coord);

        GET_NEXT_TOKEN;
        
        /* parameter type list always starts with declaration specifiers */
		if (is_current_token_declaration_specifier_token())
        {
            enter_scope();
            new_scope = 1;
			suffix_declr->u.parameter.param_type_list = parameter_type_list();
        }
        else
        {
            if (cptk == TK_ID)
            {
				HCC_AST_LIST_APPEND(ids, lexeme_value.string_value);

                GET_NEXT_TOKEN;
                while (cptk == TK_COMMA)
                {
                    GET_NEXT_TOKEN;

					HCC_AST_LIST_APPEND(ids, lexeme_value.string_value);

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

	assert(suffix_declr);

    return suffix_declr;
}

/*
declarator
	: pointer direct_declarator
	| direct_declarator
	;
*/
t_ast_declarator* declarator(int storage_class)
{
    t_ast_declarator* d = NULL;
    t_ast_pointer* ptr = NULL;
    t_ast_direct_declarator* dir_declr = NULL;
    t_ast_list* suffix_list = make_ast_list_entry();
    t_coordinate saved_coord = coord;

	if (cptk == TK_MUL)
	{
        ptr = pointer();
	}

	dir_declr = direct_declarator(storage_class);

	d = make_ast_declarator(ptr, dir_declr, suffix_list);
    BINDING_COORDINATE(d, saved_coord);

    while (cptk == TK_LPAREN || cptk == TK_LBRACKET)
    {
		HCC_AST_LIST_APPEND(suffix_list, suffix_declarator());
    }

    return d;
}

/*
direct_declarator
	: IDENTIFIER
	| '(' declarator ')'
	;
*/
t_ast_direct_declarator* direct_declarator(int storage_class)
{
    t_ast_direct_declarator* dir_declr = NULL;
    t_ast_declarator* declr = NULL;
    t_coordinate saved_coord = coord;
    char* id = NULL;

    if (cptk == TK_LPAREN)
    {
        GET_NEXT_TOKEN;
        declr = declarator(storage_class);
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

        id = lexeme_value.string_value;

		GET_NEXT_TOKEN;
    }

    dir_declr = make_ast_direct_declarator(id, declr);
    BINDING_COORDINATE(dir_declr, saved_coord);

    return dir_declr;
}

/*
abstract_declarator
	: pointer
	| direct_abstract_declarator
	| pointer direct_abstract_declarator
	;
*/
t_ast_abstract_declarator* abstract_declarator()
{
    t_ast_abstract_declarator* abstract_declr = NULL;
    t_ast_pointer* ptr = NULL;
    t_ast_direct_abstract_declarator* dir_abstract_declr = NULL;
    t_ast_list *suffix_list = make_ast_list_entry();
    t_coordinate saved_coord = coord;

	if (cptk == TK_MUL)
	{
		ptr = pointer();

		/*
		 * look ahead is neither '(' nor '[', we are done.
		 * In this case abstract declarator is just a plain pointer.
         */
		if (cptk != TK_LPAREN && cptk != TK_LBRACKET)
		{
            abstract_declr = make_ast_abstract_declarator(ptr, dir_abstract_declr, suffix_list);
            BINDING_COORDINATE(abstract_declr, saved_coord);

			return abstract_declr;
		}
	}

    dir_abstract_declr = direct_abstract_declarator();

	abstract_declr = make_ast_abstract_declarator(ptr, dir_abstract_declr, suffix_list);
    BINDING_COORDINATE(abstract_declr, saved_coord);

	while (cptk == TK_LPAREN || cptk == TK_LBRACKET)
	{
		HCC_AST_LIST_APPEND(suffix_list, suffix_declarator());
	}

    return abstract_declr;
}

/*
direct_abstract_declarator
	: '(' abstract_declarator ')'
	: suffix_declarator
*/
t_ast_direct_abstract_declarator* direct_abstract_declarator()
{
    t_ast_direct_abstract_declarator* dir_abstract_declr = NULL;
    t_ast_suffix_declarator* suffix_declr = NULL;
    t_ast_abstract_declarator* abstract_declr = NULL;
    t_coordinate saved_coord = coord;

	if (cptk == TK_LPAREN)
	{
		GET_NEXT_TOKEN;

		/*
	     * abstract_declarator doesn't start with either ) or declr specifier
		 * so this must be a suffix declarator (either (), or (parameter type))
		 */
		if (cptk == TK_RPAREN || is_current_token_declaration_specifier_token())
		{
			suffix_declr = suffix_declarator();
		}
		else
		{
            abstract_declr = abstract_declarator();
			match(TK_RPAREN);
		}
	}
	else
	{
		syntax_error("illegal token found in abstract declarator!");
	}

    dir_abstract_declr = make_ast_direct_abstract_declarator(suffix_declr, abstract_declr);
    BINDING_COORDINATE(dir_abstract_declr, saved_coord);

    return dir_abstract_declr;
}

/* this function parse declarator regardless of its type - that is, it could parse
 * both declarator and abstract declarator. it should only be used when absolutely neccessary
 * because a function should only do one thing well. 
 */
t_ast_all_declarator* all_declarator(int storage_class)
{
    t_ast_all_declarator *all_declr = NULL, *sub_all_declr = NULL;
    t_ast_pointer* ptr = NULL;
    t_ast_list* suffix_declr_list = make_ast_list_entry();
    char* id = NULL;
    t_coordinate saved_coord = coord;

    if (cptk == TK_MUL)
    {
        ptr = pointer();

        if (cptk != TK_LPAREN && 
            cptk != TK_LBRACKET &&
            cptk != TK_ID) 
        {
            all_declr = make_ast_all_declarator(ptr, id, sub_all_declr, suffix_declr_list);
            BINDING_COORDINATE(all_declr, saved_coord);

            return all_declr;
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

        id = lexeme_value.string_value;

        GET_NEXT_TOKEN;
    }
    else if (cptk == TK_LPAREN)
    {
        sub_all_declr = all_declarator(storage_class);
    }
    else
    {
        if (cptk != TK_LBRACKET) syntax_error("declarator error");
    }
    
    all_declr = make_ast_all_declarator(ptr, id, sub_all_declr, suffix_declr_list);
    BINDING_COORDINATE(all_declr, saved_coord);

    while (cptk == TK_LPAREN || cptk == TK_LBRACKET)
    {
        HCC_AST_LIST_APPEND(suffix_declr_list, suffix_declarator());
    }

    return all_declr;
}

/*
struct_or_union_specifier
	: struct_or_union IDENTIFIER '{' struct_declaration_list '}'
	| struct_or_union '{' struct_declaration_list '}'
	| struct_or_union IDENTIFIER
	;
*/
t_ast_struct_or_union_specifier* struct_or_union_specifier()
{
	t_ast_struct_or_union_specifier* s = NULL;
	t_ast_list* struct_declr_list = NULL;
	char* id = NULL;
	t_coordinate saved_coord = coord;
	int is_struct = (cptk == TK_STRUCT)? 1 : 0;

    assert(cptk == TK_STRUCT || cptk == TK_UNION);

    GET_NEXT_TOKEN;
    if (cptk  == TK_ID)
    {
        /* [TODO] [SYMBOL MANAGE] - install tag name into the types table? */
		id = lexeme_value.string_value;
        GET_NEXT_TOKEN;
    }

    if (cptk == TK_LBRACE)
    {
        GET_NEXT_TOKEN;
		struct_declr_list = struct_declaration_list();
        match(TK_RBRACE);
    }

	s = make_ast_struct_union_specifier(is_struct, id, struct_declr_list);
	BINDING_COORDINATE(s, saved_coord);

	return s;
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
t_ast_list* struct_declaration_list()
{
	t_ast_list *list = make_ast_list_entry(), *c_list = list, *specifier_qualifier_list = NULL;
	t_ast_struct_declaration* struct_declaration = NULL;

    do
    {
		t_ast_list* struct_declarator_list = make_ast_list_entry();
		specifier_qualifier_list = specifiers_qualifier_list();
		
		struct_declaration = make_ast_struct_declaration(specifier_qualifier_list, struct_declarator_list);
		BINDING_COORDINATE(struct_declaration, coord);

		HCC_AST_LIST_APPEND(struct_declarator_list, struct_declarator());
        
        while (cptk == TK_COMMA)
        {
            GET_NEXT_TOKEN;
			HCC_AST_LIST_APPEND(struct_declarator_list, struct_declarator());
        }

        match(TK_SEMICOLON);

		HCC_AST_LIST_APPEND(c_list, struct_declaration);
    }
    while (cptk != TK_RBRACE);

	return list;
}

/*
struct_declarator
	: declarator
	| ':' constant_expression
	| declarator ':' constant_expression
	;
*/
t_ast_struct_declarator* struct_declarator()
{
    t_ast_struct_declarator* d = NULL;
    t_coordinate saved_coord = coord;
    t_ast_declarator* declr = NULL;
    t_ast_exp* exp = NULL;

    /* short circuit anonymous struct / union */
    if (cptk == TK_SEMICOLON)
    {
        d = make_ast_struct_declarator(declr, exp);
        BINDING_COORDINATE(d, coord);

        return d;
    }
   
    if (cptk != TK_COLON)
    {
        declr = declarator(TK_AUTO);
    }

    if (cptk == TK_COLON)
    {
        GET_NEXT_TOKEN;
        exp = constant_expression();
    }

    d = make_ast_struct_declarator(declr, exp);
    BINDING_COORDINATE(d, saved_coord);

    return d;
}

/*
specifier_qualifier_list
	: type_specifier specifier_qualifier_list
	| type_specifier
	| type_qualifier specifier_qualifier_list
	| type_qualifier
	;
*/
t_ast_list* specifiers_qualifier_list()
{
	t_ast_list *list = make_ast_list_entry(), *c_list = list;
    int alien_type_engaged = 0;
	t_ast_type_specifier* s = NULL;

    for(;;)
    {
        switch(cptk)
        {
        case TK_CONST:
		case TK_VOLATILE:
			{
				t_ast_type_qualifier_kind kind = (cptk == TK_CONST)? AST_TYPE_CONST : AST_TYPE_VOLATILE;
				t_ast_type_qualifier* q = make_ast_type_qualifer(kind);
				BINDING_COORDINATE(q, coord);
				HCC_AST_LIST_APPEND(c_list, q);

				GET_NEXT_TOKEN;
				break;
			}
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
			{
				s = make_ast_type_specifier_native_type(token_to_ast_native_type(cptk));
				BINDING_COORDINATE(s, coord);
				HCC_AST_LIST_APPEND(c_list, s);
				
				GET_NEXT_TOKEN;
				break;
			}
        case TK_ID:
			if (!alien_type_engaged && is_typedef_id(lexeme_value.string_value))
            {
				s = make_ast_type_specifier_typedef(lexeme_value.string_value);
				BINDING_COORDINATE(s, coord);
				HCC_AST_LIST_APPEND(c_list, s);

                GET_NEXT_TOKEN;
                alien_type_engaged = 1; 
                break;
            }

            return list;
        case TK_STRUCT:
        case TK_UNION:
			{
				s = make_ast_type_specifier_struct_union(struct_or_union_specifier());
				HCC_AST_LIST_APPEND(c_list, s);
				alien_type_engaged = 1;
				break;
			}
        case TK_ENUM:
			{
				s = make_ast_type_specifier_enum(enum_specifier());
				HCC_AST_LIST_APPEND(c_list, s);
				alien_type_engaged = 1;
				break;
			}
        case TK_AUTO:
        case TK_REGISTER:
        case TK_EXTERN:
        case TK_STATIC:
        case TK_TYPEDEF:
            {
                syntax_error("illegal storage class appears");
                return list;
            }
        default:
            return list;
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
t_ast_enum_specifier* enum_specifier()
{
    /*
     * this flag is to indicate that at least we have parsed either { , or ID
     * which are the only legal suffix following enum keyword..
    */
    int flag = 0;
	t_ast_list *enumerator_list = make_ast_list_entry();
	t_ast_enum_specifier* e = make_ast_enum_specifier(NULL, enumerator_list);
	BINDING_COORDINATE(e, coord);

    match(TK_ENUM);

    if (cptk == TK_ID)
    {
		e->id = lexeme_value.string_value;
		
        GET_NEXT_TOKEN;
        flag = 1;
    }

    if (cptk == TK_LBRACE)
    {
        GET_NEXT_TOKEN;

        if (cptk == TK_RBRACE)
        {
            GET_NEXT_TOKEN;
            return e;
        }

		HCC_AST_LIST_APPEND(enumerator_list, enumerator());
        while (cptk == TK_COMMA)
        {
            GET_NEXT_TOKEN;

            if (cptk == TK_RBRACE)
            {
                GET_NEXT_TOKEN;
                return e;
            }

			HCC_AST_LIST_APPEND(enumerator_list, enumerator());
        }

        match(TK_RBRACE);
        flag = 1;
    }

    if (0 == flag)
    {
        syntax_error("Error when parsing enum specifier : { or identifier expected");
    }

	return e;
}

/*
enumerator
	: IDENTIFIER
	| IDENTIFIER '=' constant_expression
	;
*/
t_ast_enumerator* enumerator()
{
    t_ast_enumerator* e = NULL;
    t_ast_exp* exp = NULL;
    char* id = lexeme_value.string_value;
    t_coordinate saved_coord = coord;

    if (cptk != TK_ID)
    {
        syntax_error("enumerator must be identifier!");
        return NULL;
    }

    GET_NEXT_TOKEN;
    if (cptk == TK_ASSIGN)
    {
        GET_NEXT_TOKEN;
        exp = constant_expression();
    }

    e = make_ast_enumerator(id, exp);
    BINDING_COORDINATE(e, saved_coord);

    return e;
}

/*
translation_unit
	: external_declaration
	| translation_unit external_declaration
	;
*/
t_ast_translation_unit* translation_unit()
{
    t_ast_list* ext_declaration_list = make_ast_list_entry();
    t_ast_translation_unit* t = make_ast_translation_unit(ext_declaration_list);
    BINDING_COORDINATE(t, coord);

	while (cptk != TK_END)
	{
        HCC_AST_LIST_APPEND(ext_declaration_list, external_declaration());
	}

    return t;
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
t_ast_external_declaration* external_declaration()
{
    int storage_class = TK_AUTO;
	t_ast_declarator* declrtor = NULL;
    t_ast_init_declarator* init_declrtor = NULL;
	t_ast_list *declr_list = make_ast_list_entry(), *c_declr_list = declr_list;
    t_ast_list *init_declr_list = make_ast_list_entry(), *c_init_declr_list = init_declr_list;
	t_ast_declaration_specifier* declr_specifier = NULL;
	t_ast_stmt* compound_stmt = NULL;
    t_ast_external_declaration* ext_declr = NULL;
	t_coordinate saved_coord = coord;

	t_ast_function_definition* func_def = NULL;
	t_ast_declaration* declare = NULL;

    if (is_current_token_declarator_token())
    {
        /* [FIX ME] - IS THAT NEEDED? function omit types have extern int assumed... */
		declrtor = declarator(TK_AUTO);

        if (cptk != TK_LBRACE)
        {
            /* declaration list */
            while (is_current_token_declaration_specifier_token())
            {
				HCC_AST_LIST_APPEND(c_declr_list, declaration());
            }
        }
        
        /* function body */
		compound_stmt = compound_statement();
		
		func_def = make_ast_function_definition(declr_specifier, declrtor, declr_list, compound_stmt);
        ext_declr = make_ast_external_declaration(func_def, declare);
        BINDING_COORDINATE(func_def, saved_coord);
        BINDING_COORDINATE(ext_declr, saved_coord);

        return ext_declr;
    }

	declr_specifier = declaration_specifiers();
	storage_class = declr_specifier->storage_class;
    
    if (cptk == TK_SEMICOLON)
    {
		/* for non struct/union declaration this should issue a warning */
		/* TODO - for example, int; long; is by itself not meaningful declarations */
        GET_NEXT_TOKEN;

		(declare);

        /* GETH - this return NULL! */
        return ext_declr;
    }

	declrtor = declarator(storage_class);
    HCC_AST_LIST_APPEND(c_declr_list, declrtor);
    
	if (cptk == TK_SEMICOLON)
	{
        /* declaration_specifiers declarator ;*/
        /* this is declaration */
        
        init_declrtor = make_ast_init_declarator(declrtor, NULL);
        BINDING_COORDINATE(init_declrtor, saved_coord);
        HCC_AST_LIST_APPEND(c_init_declr_list, init_declrtor);

        declare = make_ast_declaration(declr_specifier, init_declr_list);
        ext_declr = make_ast_external_declaration(func_def, declare);
        BINDING_COORDINATE(declare, saved_coord);
        BINDING_COORDINATE(ext_declr, saved_coord);

		GET_NEXT_TOKEN;
        return ext_declr;
	}
    else if (cptk == TK_LBRACE)
    {
        /* declaration_specifiers declarator compound_statement */
        /* this is function definition */
		compound_stmt = compound_statement();
        
        func_def = make_ast_function_definition(declr_specifier, declrtor, declr_list, compound_stmt);
        ext_declr = make_ast_external_declaration(func_def, declare);
        BINDING_COORDINATE(func_def, saved_coord);
        BINDING_COORDINATE(ext_declr, saved_coord);

        return ext_declr;
    }
    else if (is_current_token_declaration_specifier_token())
    {
        /* declaration_specifiers declarator declaration_list compound_statement*/
        /* this is a function */
        while (is_current_token_declaration_specifier_token())
        {
			HCC_AST_LIST_APPEND(declr_list, declaration());
        }

		compound_stmt = compound_statement();

        func_def = make_ast_function_definition(declr_specifier, declrtor, declr_list, compound_stmt);
        ext_declr = make_ast_external_declaration(func_def, declare);
        BINDING_COORDINATE(func_def, saved_coord);
        BINDING_COORDINATE(ext_declr, saved_coord);

        return ext_declr;
    }
  
    /*
     * survive crossfire - this is a declaration
	 * declaration_specifiers init_declarator_list
     */
	declare = make_ast_declaration(declr_specifier, init_declr_list);
    ext_declr = make_ast_external_declaration(func_def, declare);
	BINDING_COORDINATE(declare, saved_coord);
    BINDING_COORDINATE(ext_declr, saved_coord);

    /* init_declarator_list */
    if (cptk == TK_COMMA || cptk == TK_ASSIGN)
    {
        if (cptk == TK_ASSIGN)
        {
            GET_NEXT_TOKEN;
            init_declrtor = make_ast_init_declarator(declrtor, initializer());
            BINDING_COORDINATE(init_declrtor, saved_coord);

            HCC_AST_LIST_APPEND(c_init_declr_list, init_declrtor);
        }

        while (cptk == TK_COMMA)
        {
            GET_NEXT_TOKEN;
            HCC_AST_LIST_APPEND(c_init_declr_list, init_declarator(storage_class));
        }

        match(TK_SEMICOLON);
    }
    else
    {
		syntax_error("illegal token detected in declaration parsing");
    }

    return ext_declr;
}

/*
type_name
        : specifier_qualifier_list
        | specifier_qualifier_list abstract_declarator
        ;
*/
t_ast_type_name* type_name()
{
	t_ast_type_name* t = NULL;
	t_coordinate saved_coord = coord;
	t_ast_abstract_declarator* abstract_declr = NULL;
	t_ast_list* list = specifiers_qualifier_list();

    if (cptk == TK_MUL || cptk == TK_LPAREN || cptk == TK_LBRACKET)
    {
		abstract_declr = abstract_declarator();
    }

	t = make_ast_type_name(list, abstract_declr);
	BINDING_COORDINATE(t, saved_coord);

	return t;
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

void semantic_check(t_ast_translation_unit* translation_unit)
{
    assert(translation_unit);
}
