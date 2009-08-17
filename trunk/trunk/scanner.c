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

#include "scanner.h"
#include "keywords.h"
#include "hcc.h"
#include "assert.h"
#include "preprocessor/mem.h"
#include "preprocessor/cpp.h"

static char** ptr_includefiles;
static char** ptr_compilefiles;
static struct lexer_state ls;

/* 
	Lexical Map
	
	key - value of the "token type" unnamed enum defined in ucpp preprocessor 
	value - value of the TOKEN enum defined in this file

	this map is used to bridge between lexer and preprocessor
*/
static unsigned char lexical_map[256] = 
{
	TK_WHITESPACE,
	TK_NEWLINE, 
	0, /* comment left undefined */
	TK_CONSTTODO,
	TK_ID,
	0,		/* non-C characters */
	0,		/* a #pragma directive */
	0,	/* new file or #line */
	TK_CONSTTODO,		/* constant "xxx" */
	TK_CONSTTODO,		/* constant 'xxx' */
	TK_DIV,		/*	/	*/
	TK_DIV_ASSIGN,	/*	/=	*/
	TK_SUB,		/*	-	*/
	TK_DEC,		/*	--	*/
	TK_SUB_ASSIGN,	/*	-=	*/
	TK_DEREFERENCE,		/*	->	*/
	TK_ADD,		/*	+	*/
	TK_INC,		/*	++	*/
	TK_ADD_ASSIGN,		/*	+=	*/
	TK_LESS,		/*	<	*/
	TK_LESS_EQ,		/*	<=	*/
	TK_LSHIFT,		/*	<<	*/
	TK_LSHIFT_ASSIGN,		/*	<<=	*/
	TK_GREAT,		/*	>	*/
	TK_GREAT_EQ,		/*	>=	*/
	TK_RSHIFT,		/*	>>	*/
	TK_RSHIFT_ASSIGN,		/*	>>=	*/
	TK_ASSIGN,		/*	=	*/
	TK_EQUAL,		/*	==	*/
#ifdef CAST_OP
	CAST,		/*	=>	*/
#endif
	TK_COMP,		/*	~	*/
	TK_UNEQUAL,		/*	!=	*/
	TK_BITAND,		/*	&	*/
	TK_AND,		/*	&&	*/
	TK_BITAND_ASSIGN,		/*	&=	*/
	TK_BITOR,		/*	|	*/
	TK_OR,		/*	||	*/
	TK_BITOR_ASSIGN,		/*	|=	*/
	TK_MOD,		/*	%	*/
	TK_MOD_ASSIGN,		/*	%=	*/
	TK_MUL,		/*	*	*/
	TK_MUL_ASSIGN,		/*	*=	*/
	TK_BITXOR,		/*	^	*/
	TK_BITXOR_ASSIGN,		/*	^=	*/
	TK_NOT,		/*	!	*/
	TK_LBRACE,		/*	{	*/
	TK_RBRACE,		/*	}	*/
	TK_LBRACKET,		/*	[	*/
	TK_RBRACKET,		/*	]	*/
	TK_LPAREN,		/*	(	*/
	TK_RPAREN,		/*	)	*/
	TK_COMMA,		/*	,	*/
	TK_QUESTION,		/*	?	*/
	TK_SEMICOLON,		/*	;	*/
	TK_COLON,		/*	:	*/
	TK_DOT,		/*	.	*/
	TK_ELLIPSE,		/*	...	*/
	0,		/*	#	*/
	0,		/*	##	*/

	0,	/* optional space to separate tokens in text output */

	0,			/* there begin digraph tokens */

	/* for DIG_*, do not change order, unless checking undig() in cpp.c */
	0,	/*	<:	*/
	0,	/*	:>	*/
	0,	/*	<%	*/
	0,	/*	%>	*/
	0,	/*	%:	*/
	0,	/*	%:%:	*/

	0,		/* digraph tokens end here */

	0,		/* reserved words will go there */

	0,	/* special token for representing macro arguments */

	TK_ADD,	/* unary + */
	TK_SUB		/* unary - */

};

void reset_scanner(t_scanner_context* sc)
{
	// include file loop counter
	int i = 0;
	
	int r = 0;
	(r);
	assert(sc);

	//
	// clean previous states and memory if neccessary
	//
	//wipeout();
	free_lexer_state(&ls);

	//
	// initialize static tables of preprocessor ucpp
	//
	init_cpp();
	
	/*
	  non-zero if the special macros (__FILE__ and others)
	  should not be defined. This is a global flag since
	  it affects the redefinition of such macros (which are
	  allowed if the special macros are not defined)
	*/
	no_special_macros = 0;

	/* 
	  This function initializes the macro table
	  and other things; it will intialize assertions if it has a non-zero
	  argument
	*/
	init_tables(1);
	
	//
	// reset include path
	//
	// init_include_path(sc->include_pathes);
	
	/*
	these are the set of "emit" macros use to debug or produce analysis information during
	compilation

	emit_dependencies
		set to 1 if dependencies should be emitted during
		preprocessing
		set to 2 if dependencies should also be emitted for
		system include files
	
	emit_defines
		set to non-zero if #define macro definitions should be
		emitted when macros are defined
	
	emit_assertions
		set to non-zero if #define macro definitions should be
		emitted when macros are defined
	
	emit_output
		the FILE * where the above items are sent if one of the
		three emit_ variables is set to non zero
	*/
	emit_dependencies = 0;
	emit_defines = 0;
	emit_assertions = 0;
	emit_output = 0;
	
	set_init_filename(sc->filename, 1);

	init_lexer_state(&ls);
	init_lexer_mode(&ls);
	ls.flags |= HANDLE_ASSERTIONS | HANDLE_PRAGMA | LINE_NUM | CPLUSPLUS_COMMENTS;

	ls.input = fopen(sc->filename, "rb");
	if (ls.input == NULL)
	{
		fprintf(stderr, "%s\n", "error : input file is dead");		
	}

	for (; i < sc->number_of_include_pathes; i ++) add_incpath(sc->include_pathes[i]);
}

void free_scanner()
{
	wipeout();
	free_lexer_state(&ls);
}

static int identify_keyword(char* id)
{
    tKW* p = NULL;
    int retval = TK_ID;
    int index = (*id &~0x20) - 'A'; // convert *id to upper case letter if needed and calc diff as index (starting 0)
  
    if (index >= 26)
    {
        return TK_ID;
    }

    p = kw_table[index];
    while (p->name)
    {
        if ((size_t)p->len == strlen(id) && strncmp(id, p->name, strlen(id)) == 0)
        {
            retval = p->token;
            break;
        }

        p++;
    }

    return retval; 
}

int gettoken()
{
    char* token = NULL;
    int retval = TK_ID;
    int r = lex(&ls);

    /*
    * lex() reads the next token from the processed stream and stores it
    * into ls->ctok.
    * return value: non zero on error (including CPPERR_EOF, which is not
    * quite an error)
    */
    if (r == CPPERR_EOF)
    {
        // end of file
        return TK_END;
    }
    else if (r)
    {
        // error - TODO what kind of error?
        assert(0); // TODO
    }
    
    /* we print each token: its numerical value, and its
    string content; if this is a PRAGMA token, the
    string content is in fact a compressed token list,
    that we uncompress and print. */
    if (ls.ctok->type == PRAGMA) 
    {
        unsigned char *c = (unsigned char *)(ls.ctok->name);

        printf("line %ld: <#pragma>\n", ls.line);
        for (; *c; c ++) 
        {
            int t = *c;

            if (STRING_TOKEN(t)) 
            {
                printf("  <%2d>  ", t);
                for (c ++; *c != PRAGMA_TOKEN_END;
                    c ++) putchar(*c);
                    putchar('\n');
            } 
            else 
            {
                printf("  <%2d>  `%s'\n", t,
                    operators_name[t]);
            }
        }
    } 
    else if (ls.ctok->type == CONTEXT) 
    {
        printf("new context: file '%s', line %ld\n",
            ls.ctok->name, ls.ctok->line);
    } 
    else if (ls.ctok->type == NEWLINE) 
    {
        printf("[newline]\n");
    } 
    else 
    {
        if (STRING_TOKEN(ls.ctok->type))
        {
            token = ls.ctok->name;

            /* NUMBER, NAME, STRING, CHAR*/
            /* Lexer is not interested in bunch, pragma and context */
            switch (ls.ctok->type)
            {
            case NUMBER :
                {
                    printf("number const : %s\n", ls.ctok->name);
                    retval = TK_CONSTTODO;
                    break;
                }
            case NAME:
                {
                    retval = identify_keyword(token);

                    if (retval == TK_ID)
                    {
                        printf("identifier: %s\n", token);
                    }
                    else
                    {
                        printf("keyword: %s\n", token);
                    }

                    break;
                }
            case STRING:
                {
                    printf("string const : %s\n", ls.ctok->name);
                    retval = TK_CONSTTODO;
                    break;
                }
            case CHAR:
                {
                    printf("char const : %s\n", ls.ctok->name);
                    retval = TK_CONSTTODO;
                    break;
                }
            }
        }
        else
        {
            retval = lexical_map[ls.ctok->type];

            printf("line %ld: <%2d>  `%s'\n", ls.ctok->line,
                ls.ctok->type,
                STRING_TOKEN(ls.ctok->type) ? ls.ctok->name
                : operators_name[ls.ctok->type]);
        }

        /*
        printf("line %ld: <%2d>  `%s'\n", ls.ctok->line,
            ls.ctok->type,
            STRING_TOKEN(ls.ctok->type) ? ls.ctok->name
            : operators_name[ls.ctok->type]);
*/
    }

    return retval;
}
