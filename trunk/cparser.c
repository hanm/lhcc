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

#define GET_NEXT_TOKEN cparser_token = get_token()

void initialize_parser()
{
	GET_NEXT_TOKEN;
}

// todo static
void match(int token)
{  
	if (cparser_token == token)
    {
        GET_NEXT_TOKEN;
    }
    else
    {
		error(tokens[token], tokens[cparser_token]);
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
    switch (cparser_token)
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
        switch (cparser_token)
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
                if (cparser_token != TK_RPAREN)
                {
                    assignment_expression();
                    while (cparser_token == TK_COMMA)
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
	switch (cparser_token)
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
            if (is_typedef_name(cparser_token))
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
    
    if (cparser_token == TK_LPAREN)
    {
        GET_NEXT_TOKEN;
        if (is_typedef_name(cparser_token))
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

    while (cparser_token == TK_MUL ||
        cparser_token == TK_DIV ||
        cparser_token == TK_MOD)
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

    while (cparser_token == TK_ADD || cparser_token == TK_SUB)
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

    while (cparser_token == TK_LSHIFT || cparser_token == TK_RSHIFT)
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

    while (cparser_token == TK_GREAT ||
        cparser_token == TK_GREAT_EQ ||
        cparser_token == TK_LESS ||
        cparser_token == TK_LESS_EQ)
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

    while (cparser_token == TK_EQUAL || cparser_token == TK_UNEQUAL)
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

    while (cparser_token == TK_BITAND)
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

    while (cparser_token == TK_BITXOR)
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

    while (cparser_token == TK_BITOR)
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

    while (cparser_token == TK_AND)
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

    while (cparser_token == TK_OR)
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

    if (cparser_token == TK_QUESTION)
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

    if (cparser_token >= TK_ASSIGN && cparser_token <= TK_MOD_ASSIGN)
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

    while (cparser_token == TK_COMMA)
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
    switch (cparser_token)
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
    if (cparser_token == TK_ID && peek_token() == TK_COLON)
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
    if (cparser_token == TK_ELSE)
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

    if (cparser_token != TK_SEMICOLON)
    {
        expression();
    }
    match(TK_SEMICOLON);

    if (cparser_token != TK_SEMICOLON)
    {
        expression();
    }
    match(TK_SEMICOLON);

    if (cparser_token != TK_RPAREN)
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
    if (cparser_token != TK_SEMICOLON)
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
	declaration_specifiers();
	if (cparser_token == TK_SEMICOLON)
	{
		GET_NEXT_TOKEN;
		return;
	}

	// TODO - function defination
	init_declarator();

	while (cparser_token == TK_COMMA)
	{
		GET_NEXT_TOKEN;
		init_declarator();
	}

	match(TK_SEMICOLON);
}

void declaration_specifiers()
{
    for(;;)
    {
        switch(cparser_token)
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

/*
init_declarator
	: declarator
	| declarator '=' initializer
	;
*/
void init_declarator()
{
	declarator();

	if (cparser_token == TK_ASSIGN)
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
	if (cparser_token == TK_LBRACE)
	{
		GET_NEXT_TOKEN;
		
		initializer();
		while (cparser_token == TK_COMMA)
		{
			GET_NEXT_TOKEN;
			if (cparser_token == TK_RBRACE) break;
			initializer();
		}

		match(TK_RBRACE);
	}
	else
	{
		assignment_expression();
	}
}


void declarator()
{

}


/*
direct_declarator
	: IDENTIFIER
	| '(' declarator ')'
	| direct_declarator '[' constant_expression ']'
	| direct_declarator '[' ']'
	| direct_declarator '(' parameter_type_list ')'
	| direct_declarator '(' identifier_list ')'
	| direct_declarator '(' ')'
	;

*/
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
    HCC_ASSERT(cparser_token == TK_STRUCT || cparser_token == TK_UNION);

    GET_NEXT_TOKEN;
    if (cparser_token  == TK_ID)
    {
        GET_NEXT_TOKEN;
    }

    if (cparser_token == TK_LBRACE)
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
        
        while (cparser_token == TK_COMMA)
        {
            GET_NEXT_TOKEN;
            struct_declarator();
        }

        match(TK_SEMICOLON);
    }
    while (cparser_token != TK_RBRACE);
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
    if (cparser_token != TK_COLON)
    {
        declarator();
    }

    if (cparser_token == TK_COLON)
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
    for(;;)
    {
        switch(cparser_token)
        {
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
        } // end switch
    } // end for
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

    if (cparser_token == TK_ID)
    {
        GET_NEXT_TOKEN;
        flag = 1;
    }

    if (cparser_token == TK_LBRACE)
    {
        GET_NEXT_TOKEN;
        enumerator();
        while (cparser_token == TK_COMMA)
        {
            GET_NEXT_TOKEN;
            enumerator();
        }

        match(TK_RBRACE);
        flag = 1;
    }

    if (0 == flag)
    {
        syntax_error(&coord, "Error when parsing enum specifier : { or identifier expected");
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
    if (cparser_token != TK_ID)
    {
        syntax_error(&coord, "enumerator must be identifier!");
        return;
    }

    GET_NEXT_TOKEN;
    if (cparser_token == TK_ASSIGN)
    {
        GET_NEXT_TOKEN;
        constant_expression();
    }
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
