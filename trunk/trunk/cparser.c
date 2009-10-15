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

#include "clexer.h"
#include "cparser.h"

static char* tokens[] = 
{
#define TK(a, b) b,
#include "tokendef.h"
#undef TK
    "END OF TOKEN" // syntactic suguar
};

#define GET_NEXT_TOKEN look_ahead = get_token()

void initialize_parser()
{
	GET_NEXT_TOKEN;
}

// todo static
void match(int token)
{  
	if (look_ahead == token)
    {
        GET_NEXT_TOKEN;
    }
    else
    {
		error(tokens[token], tokens[look_ahead]);
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
void primary_expression()
{
    switch (look_ahead)
    {
    case TK_ID :
        GET_NEXT_TOKEN;
        break;
    case TK_CONST_FLOAT :
    case TK_CONST_INTEGER :
        GET_NEXT_TOKEN;
        break;
    case TK_CONST_CHAR_LITERAL :
    case TK_CONST_STRING_LITERAL:
        GET_NEXT_TOKEN;
        break;
    case TK_LPAREN :
        GET_NEXT_TOKEN;
        expression();
        match(TK_RPAREN);
    default :
        error(&coord, "expect identifier, constant, string literal or (");
    }
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
void postfix_expression()
{
    primary_expression();

    for (;;)
    {
        switch (look_ahead)
        {
        case TK_LBRACKET :
            {
                GET_NEXT_TOKEN;
                expression();
                match(TK_RBRACKET);
                break;
            }
        case TK_LPAREN :
            {
                GET_NEXT_TOKEN;
                if (look_ahead != TK_RPAREN)
                {
                    assignment_expression();
                    while (look_ahead == TK_COMMA)
                    {
                        GET_NEXT_TOKEN;
                        assignment_expression();
                    }
                }
                match(TK_RPAREN);
                break;
            }
        case TK_DOT :
        case TK_ARROW :
            {
                GET_NEXT_TOKEN;
                match(TK_ID);
            }
        case TK_INC :
        case TK_DEC :
            {
                GET_NEXT_TOKEN;
            }
        default:
            return;
        }
    }
}

/*
unary_expression
        : postfix_expression
        | '++' unary_expression
        | '--' unary_expression
        | unary_operator cast_expression
        | SIZEOF unary_expression
        | SIZEOF '(' type_name ')'
        ;
*/
void unary_expression()
{
    
}

void assignment_expression()
{
    
}

void expression()
{
    
}
