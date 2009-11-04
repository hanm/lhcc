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
#include "assert.h"

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
                break;
            }
        case TK_INC :
        case TK_DEC :
            {
                GET_NEXT_TOKEN;
                break;
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

unary_operator
        : '&'
        | '*'
        | '+'
        | '-'
        | '~'
        | '!'
        ;

cast_expression
        : unary_expression
        | '(' type_name ')' cast_expression
        ;
*/
void unary_expression()
{
	switch (look_ahead)
	{
	case TK_INC :
	case TK_DEC :
	case TK_BITAND :
	case TK_MUL :
	case TK_ADD :
	case TK_SUB :
	case TK_COMP :
	case TK_NOT :
		{
			GET_NEXT_TOKEN;

			unary_expression();

            break;
		}
    case TK_LPAREN :
        {
            GET_NEXT_TOKEN;
            if (is_typedef_name(look_ahead))
            {
                // todo - parse type name
                GET_NEXT_TOKEN;
                match(TK_RPAREN);
            }
            else
            {
                postfix_expression();
            }

            break;
        }
    case TK_SIZEOF :
        {
            sizeof_expression();
            break;
        }
    default :
        postfix_expression();
	}
}

void sizeof_expression()
{
    GET_NEXT_TOKEN;
    
    if (look_ahead == TK_LPAREN)
    {
        GET_NEXT_TOKEN;
        if (is_typedef_name(look_ahead))
        {
            // todo - parse typename
            GET_NEXT_TOKEN;
        }
        else
        {
            expression();
        }

        match(TK_RPAREN);
    }
    else
    {
        unary_expression();
    }
}

/*

multiplicative_expression
        : cast_expression
        | multiplicative_expression '*' cast_expression
        | multiplicative_expression '/' cast_expression
        | multiplicative_expression '%' cast_expression
        ;
*/
void mul_expression()
{
    unary_expression();

    while (look_ahead == TK_MUL ||
        look_ahead == TK_DIV ||
        look_ahead == TK_MOD)
    {
        GET_NEXT_TOKEN;
        unary_expression();
    }
}

/*
additive_expression
        : multiplicative_expression
        | additive_expression '+' multiplicative_expression
        | additive_expression '-' multiplicative_expression
        ;
*/
void add_expression()
{
    mul_expression();

    while (look_ahead == TK_ADD || look_ahead == TK_SUB)
    {
        GET_NEXT_TOKEN;
        mul_expression();
    }
}

/*
shift_expression
        : additive_expression
        | shift_expression '<<' additive_expression
        | shift_expression '>>' additive_expression
        ;
*/
void shift_expression()
{
    add_expression();

    while (look_ahead == TK_LSHIFT || look_ahead == TK_RSHIFT)
    {
        GET_NEXT_TOKEN;
        add_expression();
    }
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
void rel_expression()
{
    shift_expression();

    while (look_ahead == TK_GREAT ||
        look_ahead == TK_GREAT_EQ ||
        look_ahead == TK_LESS ||
        look_ahead == TK_LESS_EQ)
    {
        GET_NEXT_TOKEN;
        shift_expression();
    }
}

/*
equality_expression
        : relational_expression
        | equality_expression '==' relational_expression
        | equality_expression '!=' relational_expression
        ;
*/
void eql_expression()
{
    rel_expression();

    while (look_ahead == TK_EQUAL || look_ahead == TK_UNEQUAL)
    {
        GET_NEXT_TOKEN;
        rel_expression();
    }
}

/*
and_expression
        : equality_expression
        | and_expression '&' equality_expression
        ;
*/
void and_expression()
{
    eql_expression();

    while (look_ahead == TK_BITAND)
    {
        GET_NEXT_TOKEN;
        eql_expression();
    }
}

/*
exclusive_or_expression
        : and_expression
        | exclusive_or_expression '^' and_expression
        ;
*/
void xor_expression()
{
    and_expression();

    while (look_ahead == TK_BITXOR)
    {
        GET_NEXT_TOKEN;
        and_expression();
    }
}

/*
inclusive_or_expression
        : exclusive_or_expression
        | inclusive_or_expression '|' exclusive_or_expression
        ;
*/
void or_expression()
{
    xor_expression();

    while (look_ahead == TK_BITOR)
    {
        GET_NEXT_TOKEN;
        xor_expression();
    }
}

/*
logical_and_expression
        : inclusive_or_expression
        | logical_and_expression '&&' inclusive_or_expression
        ;
*/
void logical_and_expression()
{
    or_expression();

    while (look_ahead == TK_AND)
    {
        GET_NEXT_TOKEN;
        or_expression();
    }
}  

/*
logical_or_expression
        : logical_and_expression
        | logical_or_expression '||' logical_and_expression
        ;
*/
void logical_or_expression()
{
    logical_and_expression();

    while (look_ahead == TK_OR)
    {
        GET_NEXT_TOKEN;
        logical_and_expression();
    }
}

/*
conditional_expression
        : logical_or_expression
        | logical_or_expression '?' expression ':' conditional_expression
        ;
*/
void conditional_expression()
{
    logical_or_expression();

    if (look_ahead == TK_QUESTION)
    {
        GET_NEXT_TOKEN;
        expression();
        match(TK_COLON);
        conditional_expression();
    }
}

void constant_expression()
{
    conditional_expression(); 
}

/*
assignment_expression
        : conditional_expression
        | unary_expression assignment_operator assignment_expression
        ;
*/
void assignment_expression()
{
    //
    // todo - this sounds not conform with grammar described above
    // the parser always asusme it encurs a conditional expression
    // 
    conditional_expression();

    if (look_ahead >= TK_ASSIGN && look_ahead <= TK_MOD_ASSIGN)
    {
        GET_NEXT_TOKEN;
        assignment_expression();
    }
}

void expression()
{
    //
    // todo - guard here?
    // 
    assignment_expression();

    while (look_ahead == TK_COMMA)
    {
        GET_NEXT_TOKEN;
        assignment_expression();
    }
}

// declarations



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
void statement()
{
    switch (look_ahead)
    {
    case TK_ID :
        expression_statement();
        break;
    case TK_SWITCH :
        switch_statement();
        break;
    case TK_CASE :
        case_statement();
        break;
    case TK_DEFAULT :
        default_statement();
        break;
    case TK_IF :
        if_statement();
        break;
    case TK_DO :
        do_while_statement();
        break;
    case TK_WHILE :
        while_statement();
        break;
    case TK_FOR :
        for_statement();
        break;
    case TK_BREAK :
        break_statement();
        break;
    case TK_CONTINUE :
        continue_statement();
        break;
    case TK_RETURN :
        return_statement();
        break;
    case TK_GOTO :
        goto_statement();
        break;
    case TK_LBRACE :
        compound_statement();
        break;
    default:
        expression_statement();
    }
}

/*
expression_statement
        : ';'
        | expression ';'
        ;
*/
void expression_statement()
{
    if (look_ahead == TK_ID && peek_token() == TK_COLON)
    {
        labeled_statement();
    }
    else
    {
        expression();
        match(TK_SEMICOLON);
    }
}

/*
labeled_statement
        : IDENTIFIER ':' statement
        | CASE constant_expression ':' statement
        | DEFAULT ':' statement
        ;
*/
void labeled_statement()
{
    match(TK_ID); match(TK_COLON);
    statement();
}

void case_statement()
{
    match(TK_CASE); 
    constant_expression();
    match(TK_COLON);
    statement();
}

void default_statement()
{
    match(TK_DEFAULT);
    match(TK_COLON);
    statement();
}

/*
selection_statement
        : IF '(' expression ')' statement
        | IF '(' expression ')' statement ELSE statement
        | SWITCH '(' expression ')' statement
        ;
*/
void switch_statement()
{
    match(TK_SWITCH);

    match(TK_LPAREN);
    expression();
    match(TK_RPAREN);

    statement();
}

void if_statement()
{
    match(TK_IF);
    match(TK_LPAREN);
    
    expression();
    
    match(TK_RPAREN);
    statement();
    if (look_ahead == TK_ELSE)
    {
        GET_NEXT_TOKEN;
        statement();
    }
}

/*
iteration_statement
        : WHILE '(' expression ')' statement
        | DO statement WHILE '(' expression ')' ';'
        | FOR '(' expression_statement expression_statement ')' statement
        | FOR '(' expression_statement expression_statement expression ')' statement
        ;
*/
void do_while_statement()
{
    match(TK_DO);
    
    statement();
    
    match(TK_WHILE);
    match(TK_LPAREN);
    expression();
    match(TK_RPAREN);
    match(TK_SEMICOLON);
}

void while_statement()
{
    match(TK_WHILE);
    match(TK_LPAREN);
    expression();
    match(TK_RPAREN);

    statement();
}

void for_statement()
{
    match(TK_FOR);
    match(TK_LPAREN);

    if (look_ahead != TK_SEMICOLON)
    {
        expression();
    }
    match(TK_SEMICOLON);

    if (look_ahead != TK_SEMICOLON)
    {
        expression();
    }
    match(TK_SEMICOLON);

    if (look_ahead != TK_RPAREN)
    {
        expression();
    }

    match(TK_RPAREN);

    statement();
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

void goto_statement()
{
    match(TK_GOTO);
    match(TK_ID);
    match(TK_SEMICOLON);
}

void continue_statement()
{
    match(TK_CONTINUE);
    match(TK_SEMICOLON);
}

void break_statement()
{
    match(TK_BREAK);
    match(TK_SEMICOLON);
}

void return_statement()
{
    match(TK_RETURN);
    if (look_ahead != TK_SEMICOLON)
    {
        expression();
    }
    match(TK_SEMICOLON);
}

/*
compound_statement
        : '{' '}'
        | '{' statement_list '}'
        | '{' declaration_list '}'
        | '{' declaration_list statement_list '}'
        ;
*/
void compound_statement()
{

}

/*
declaration
        : declaration_specifiers ';'
        | declaration_specifiers init_declarator_list ';'
        ;
*/
void declaration()
{
    
}

void declaration_specifiers()
{
    for(;;)
    {
        switch(look_ahead)
        {
        case TK_AUTO:
        case TK_REGISTER:
        case TK_EXTERN:
        case TK_STATIC:
        case TK_TYPEDEF:
            // storage specifiers
            GET_NEXT_TOKEN;
            break;
        case TK_CONST:
        case TK_VOLATILE:
            // TODO - C99 restrict
            // type qualifiers
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
            // "native" type specifiers
            GET_NEXT_TOKEN;
            break;
        case TK_ID:
            if (is_typedef(lexeme_value.s))
            {
                // TYPEDEF names
                GET_NEXT_TOKEN;
            }
            break;
        case TK_STRUCT:
        case TK_UNION:
            struct_or_union_specifier();
            break;
        case TK_ENUM:
            enum_specifier();
            break;
        default:
            return;
        }
    }
}

void init_declarator_list()
{

}

void init_declarator()
{

}

void initializer()
{

}

void initializer_list()
{

}


void declarator()
{

}

void direct_declarator()
{

}

void abstract_declarator()
{

}

void direct_abstract_declarator()
{

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
    HCC_ASSERT(look_ahead == TK_STRUCT || look_ahead == TK_UNION);

    GET_NEXT_TOKEN;
    if (look_ahead  == TK_ID)
    {
        GET_NEXT_TOKEN;
    }

    if (look_ahead == TK_LBRACE)
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
        // TODO - this should exclude storage specifiers
        declaration_specifiers();
        struct_declarator();
        
        while (look_ahead == TK_COMMA)
        {
            GET_NEXT_TOKEN;
            struct_declarator();
        }

        match(TK_SEMICOLON);
    }
    while (look_ahead != TK_RBRACE);
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
    if (look_ahead != TK_COLON)
    {
        declarator();
    }

    if (look_ahead == TK_COLON)
    {
        GET_NEXT_TOKEN;
        constant_expression();
    }
}

void enum_specifier()
{
    
}

int is_typedef_name(int token)
{
    if (token >= TK_FLOAT && token <= TK_EXTERN) return 1;

    if (token == TK_ID)
    {
        // todo - here need to look up symbol table and check type flag.
        // for now just return false
        return 0;
    }

    return 0;
}

int is_typedef(char* name)
{
    // TODO
    (name);
    return 0;
}
