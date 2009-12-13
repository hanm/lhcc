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

#include "clexer.h"
#include "cparser.h"
#include "assert.h"
#include "symbol.h"

static char* tokens[] = 
{
#define TK(a, b) b,
#include "tokendef.h"
#undef TK
    "END OF TOKEN" // syntactic suguar
};

#define GET_NEXT_TOKEN cptk = get_token()

void initialize_parser()
{
	GET_NEXT_TOKEN;
}

// todo static
void match(int token)
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
    switch (cptk)
    {
    case TK_ID :
        GET_NEXT_TOKEN;
        return;
    case TK_CONST_FLOAT :
    case TK_CONST_INTEGER :
        GET_NEXT_TOKEN;
        return;
    case TK_CONST_CHAR_LITERAL :
    case TK_CONST_STRING_LITERAL:
        GET_NEXT_TOKEN;
        return;
    case TK_LPAREN :
        GET_NEXT_TOKEN;
        expression();
        match(TK_RPAREN);
        return;
    default :
        error(&coord, "expect identifier, constant, string literal or (");
        return;
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
        switch (cptk)
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
                if (cptk != TK_RPAREN)
                {
                    assignment_expression();
                    while (cptk == TK_COMMA)
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
void unary_expression()
{
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
			GET_NEXT_TOKEN;

			unary_expression();

            break;
		}
    case TK_LPAREN :
        {
            //
            // Two possibilites here - 
            // 1. (type name) unary_expression
            // 2. postfix_expression
            // The trick here is postfix_expression requires parsing the '(' itself. 
            // So here we need to peek next token instead of consume TK_LPAREN
            //
            int peek_token_code = peek_token();

            if (is_token_typename_token(peek_token_code, peek_lexeme_value.string_value))
            {
                //
                // [TODO] - well, not sure. this is a cast op, what else to do? parse type name??
                // anyways, this place looks suspicious.
                //
                GET_NEXT_TOKEN; 
                
                type_name();

                match(TK_RPAREN);

                unary_expression();
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

/*
sizeof_expression
    : SIZEOF unary_expression
	| SIZEOF '(' type_name ')'
*/
void sizeof_expression()
{
    GET_NEXT_TOKEN;
    
    if (cptk == TK_LPAREN)
    {
        GET_NEXT_TOKEN;

        if (is_token_typename_token(cptk, lexeme_value.string_value))
        {
            type_name();
        }
        else
        {
            // [NOTICE] [IMPROVE]
            unary_expression();
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

    while (cptk == TK_MUL ||
        cptk == TK_DIV ||
        cptk == TK_MOD)
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

    while (cptk == TK_ADD || cptk == TK_SUB)
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

    while (cptk == TK_LSHIFT || cptk == TK_RSHIFT)
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

    while (cptk == TK_GREAT ||
        cptk == TK_GREAT_EQ ||
        cptk == TK_LESS ||
        cptk == TK_LESS_EQ)
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

    while (cptk == TK_EQUAL || cptk == TK_UNEQUAL)
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

    while (cptk == TK_BITAND)
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

    while (cptk == TK_BITXOR)
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

    while (cptk == TK_BITOR)
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

    while (cptk == TK_AND)
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

    while (cptk == TK_OR)
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

    if (cptk == TK_QUESTION)
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

    if (cptk >= TK_ASSIGN && cptk <= TK_MOD_ASSIGN)
    {
        GET_NEXT_TOKEN;
        assignment_expression();
    }
}

void expression()
{
    //
    // empty expression. Note the implicit contract through out cparser is expression parsing doesn't consume semicolons,
    // because I want to explicit handle semicolon whenever possible which is clean and easy for debugging purpose.
    //
    if (cptk == TK_SEMICOLON)
    {
        return;
    }

    assignment_expression();

    while (cptk == TK_COMMA)
    {
        GET_NEXT_TOKEN;
        assignment_expression();
    }
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
void statement()
{
    switch (cptk)
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
    if (cptk == TK_ID && peek_token() == TK_COLON)
    {
        labeled_statement();
    }
    else if (cptk == TK_ID && strcmp(lexeme_value.string_value, "__asm") == 0)
    {
        //
        // [FIX ME] work around inline assembly parsing. this is temp solution and needs be fixed asap.
        // Ideally I need my own assembler routine to help this out.
        //
        GET_NEXT_TOKEN;

        if (cptk == TK_LBRACE)
        {
            //
            // skip __asm {} block
            //
            while (cptk != TK_RBRACE) GET_NEXT_TOKEN;
            match(TK_RBRACE); 
        }
        else
        {
            //
            // skip single __asm statement - it [TODO] can't deal with multiple asm statements at this moment
            //
             while (cptk != TK_RBRACE) GET_NEXT_TOKEN;
        }
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
    if (cptk == TK_ELSE)
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

    if (cptk != TK_SEMICOLON)
    {
        expression();
    }
    match(TK_SEMICOLON);

    if (cptk != TK_SEMICOLON)
    {
        expression();
    }
    match(TK_SEMICOLON);

    if (cptk != TK_RPAREN)
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
    if (cptk != TK_SEMICOLON)
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
	match(TK_LBRACE);

	while (cptk != TK_RBRACE && cptk != TK_END)
	{
		if (is_current_token_declaration_specifier_token())
		{
			if (cptk == TK_ID && peek_token() == TK_COLON)
			{
				// labeled statement
				statement();
			}
			else
			{
                declaration();
            }
        }
		else
		{
			statement();
		}
	}

    match(TK_RBRACE);
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

    //
    // [FIX ME] beginning of a compound statement
    // 
    if (cptk == TK_LBRACE)
    {
        return;
    }

	match(TK_SEMICOLON);
}

int declaration_specifiers()
{
    int storage_specifier = TK_AUTO;

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
		case TK_INT64:
            // "native" type specifiers
            GET_NEXT_TOKEN;
            break;
        case TK_ID:
            if (is_typedef_id(lexeme_value.string_value))
            {
                // TYPEDEF names
                GET_NEXT_TOKEN;
				break;
            }
            return storage_specifier;
        case TK_STRUCT:
        case TK_UNION:
            struct_or_union_specifier();
            break;
        case TK_ENUM:
            enum_specifier();
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
    
    //
    // look ahead is neither in declarator nor in abstract declarator, we are done
    //
    if (!is_current_token_declarator_token() && cptk != TK_LBRACKET)
    {
        return;
    }

    if (cptk == TK_MUL)
    {
        pointer();

		if (!is_current_token_declarator_token() && cptk != TK_LBRACKET)
		{
            if (cptk == TK_ID && is_typedef_id(lexeme_value.string_value))
            {
                // [FIX ME][PRIORITY FIX] -  need to update symbol table and mark this specific typedefined name out of range
                // This happens rare but it does happen
                // Typedef name is hidden by a declaration of function parameter

            }
            else
            {
                //
                // look ahead is neither a possible start of a direct declarator, nor
                // an direct abstract direclarator. 
                // This is a case where the parameter declaration is just typename*
                //
                return;
            }
		}
	}  
    
    //
    // [FIX ME] - this needs be fixed. 
	// Now it can deal with
    // int foo(const void*, int (__cdecl * _PtFuncCompare)(void *, const void *, const void *), const void*);
    //
	// but with direct declarator hard coded ...
	//
	// now only has to deal with two possible choices: 
	// direct declarator or direct abstract declarator
	//
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
		HCC_ASSERT(cptk == TK_LPAREN);
		GET_NEXT_TOKEN;
		
		if (cptk == TK_RPAREN)
		{
			// empty suffix declarator
			return;
		}
		else if (is_current_token_declaration_specifier_token())
		{
			// ( parameter_type_list) in suffix declarator
			parameter_type_list();
			match(TK_RPAREN);
		}
		else
		{
            // [FIX ME] [PRIORITY FIX]
			//declarator(storage_class);
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
        GET_NEXT_TOKEN;
        
        // parameter type list always starts with declaration specifiers
		if (is_current_token_declaration_specifier_token())
        {
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
    t_symbol* symbol = NULL;

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
			syntax_error(&coord, "direct declarator must end with an identifier");
		}

		//
		// parser has to know if an ID is typedefined type name or not to make some decisions
		// this is the place where C grammar's LL(0) is violated
		// otherwise symbol management could all be done in semantic checking phase.
		//
		if (storage_class == TK_TYPEDEF)
		{
			symbol = add_symbol(lexeme_value.string_value, &sym_table_identifiers, scope_level, FUNC);
			symbol->storage = TK_TYPEDEF;
		}
		else
		{
			// [DEBUG]
			(lexeme_value.string_value);
			(storage_class);
		}

        // [DEBUG]
        if (strcmp("sqlite3PcachePageRefcount"/*"PRKCRM_MARSHAL_HEADER"*/, lexeme_value.string_value) == 0)
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

		//
		// look ahead is neither '(' nor '[', we are done.
		// In this case abstract declarator is just a plain pointer.
		//
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

		//
		// look ahead is either ')' or parameter_type_list, so we must processing the suffix declarator.
		//
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
		syntax_error(&coord, "illegal token found in abstract declarator!");
	}
}

// this function parse declarator regardless of its type - that is, it could parse
// both declarator and abstract declarator. it should only be used when absolutely neccessary
// because a function should only do one thing well. 
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
        t_symbol* symbol = install_symbol(lexeme_value.string_value, sym_table_identifiers);
        symbol->storage = storage_class;

        GET_NEXT_TOKEN;
    }
    else if (cptk == TK_LPAREN)
    {
        all_declarator(storage_class);
    }
    else
    {
        if (cptk != TK_LBRACKET) syntax_error(&coord, "declarator error");
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
    HCC_ASSERT(cptk == TK_STRUCT || cptk == TK_UNION);

    GET_NEXT_TOKEN;
    if (cptk  == TK_ID)
    {
        // [TODO] [SYMBOL MANAGE] - install tag name into the types table?
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
    //
    // [WARNING][NON ANSI]
    //
    // This is to deal with unnamed struct / union. Unnamed struct and union is not in ANSI C
    // and portable code should avoid its usage. Limited support for unnamed struct / union
    // in hcc is simply for compatibility with other compilers. 
    //
    if (cptk == TK_SEMICOLON)
    {
        return;
    }

    if (cptk != TK_COLON)
    {
        // [FIX ME] - need to pass the correct storage class from struct_declarator
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
    int has_met_typedefined = 0;

    for(;;)
    {
        switch(cptk)
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
		case TK_INT64:
            // "native" type specifiers
            GET_NEXT_TOKEN;
            break;
        case TK_ID:
            if (!has_met_typedefined && is_typedef_id(lexeme_value.string_value))
            {
                // TYPEDEF names
                GET_NEXT_TOKEN;
                //
                // [FIX ME]? this is to short circuit specifier list parsing code 
                // otherwise we may get two typedef names straight in one line'
                // which the later might actually be redefined as a variable. without the short circuit
                // we would never have a chance to process that redefined variable.
                //
                has_met_typedefined = 1; 
                break;
            }
            return;
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
    if (cptk != TK_ID)
    {
		//
		// deal with "unclosed" enum member declaration like
		// enum { a, b, c, }
		//
		if (cptk == TK_RBRACE)
		{
			return;
		}

        syntax_error(&coord, "enumerator must be identifier!");
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
        // [FIX ME] - IS THAT NEEDED? function omit types have extern int assumed...
        declarator(TK_AUTO);

        if (cptk != TK_LBRACE)
        {
            //
            // declaration list
            //
            while (is_current_token_declaration_specifier_token())
            {
                declaration();
            }
        }
        
        //
        // function definition body
        //
        compound_statement();
        return;
    }

    storage_class = declaration_specifiers();
    
    if (cptk == TK_SEMICOLON)
    {
        //
        // an external declaration
        // warning - this would be an empty declaration with just type specifiers but no variables
        // [FIX ME] - we should initiate a warning here
        //
        GET_NEXT_TOKEN;
        return;
    }

    declarator(storage_class);
    
    //
    // here is again LL(0) 
    //
	if (cptk == TK_SEMICOLON)
	{
        //
        // got a declaration ending here which just contains one declarator
        //
		GET_NEXT_TOKEN;
        return;
	}
    else if (cptk == TK_LBRACE)
    {
        //
        // look ahead is brace next is compound statement to parse. 
        // we have a function definition here where the grammar looks like :
        // declaration_specifiers declarator compound_statement
        //
        compound_statement();
        return;
    }
    else if (is_current_token_declaration_specifier_token())
    {
        //
        // look ahead is from a declaration_list - still a function definition
        //
        // declaration_list
        // 
        while (is_current_token_declaration_specifier_token())
        {
            declaration();
        }

        compound_statement();
        return;
    }
  
    //
    // otherwise in parsing a declaration ....
    //

    //
    // init_declarator_list parsing
    //
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
		// [FIX ME]
		// This else is too harssel. Could still be parsing declarator. Should fix.
        // function definition
        if (is_current_token_declaration_specifier_token())
        {
            // declaration list
            while (is_current_token_declaration_specifier_token())
            {
                declaration();
            }
        }

        compound_statement();
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

    return (sym != NULL) && (sym->storage == TK_TYPEDEF) && (sym->scope <= scope_level);
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


