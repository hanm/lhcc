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
#include <math.h>
#include <assert.h>

#include "clexer.h"
#include "keywords.h"
#include "hcc.h"
#include "hconfig.h"
#include "assert.h"
#include "error.h"
#include "atom.h"
#include "preprocessor/mem.h"
#include "preprocessor/cpp.h"

// enable trace in lexical analysis
#define HCC_TRACE_ENABLE
#include "trace.h"

/*
 * Overview of the lexer
 * Lexer is based upon ucpp to do c preprocessing and (most) lexing; most job done in lexer is 
 * to provide more finely grained lexical elements, like integer constants, float constatns, etc.
 * (both are the same in ucpp's domain, which is defined by NUMBER)
 * 
 * another job lexer does is generating symbol for its caller (mostly parser) to install on symbol table.
 * the rule is:
 * 1. string value of an identifier is stored in ctok
 * 2. no symbol is generated for identifier, instead parser use value stored in tok to generate symbol
 * 3. number constants (integer, float) symbol is generated by lexer and stored in csym
 * 4. string literals symbol is generated by lexer and stored in csym also
 * 5. todo - build in types symbol like int, float, double, etc??
 *
 *
 */

/*
 * History
 * 10/12/2009 - add trace enable/disable to lexer
 */

static struct lexer_state ls;


static int current_token_code;
static int peek_token_code;
static int cached_token_code;

t_lexeme_value lexeme_value;
t_lexeme_value peek_lexeme_value;
t_lexeme_value cached_lexeme_value;
t_coordinate coord;

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
	0, /* number constant including integer constant and float constant. */
	TK_ID,
	0,		/* non-C characters */
	0,		/* a #pragma directive */
	0,	/* new file or #line */
	TK_CONST_STRING_LITERAL,		/* constant "xxx" */
	TK_CONST_CHAR_LITERAL,		/* constant 'xxx' */
	TK_DIV,		/*	/	*/
	TK_DIV_ASSIGN,	/*	/=	*/
	TK_SUB,		/*	-	*/
	TK_DEC,		/*	--	*/
	TK_SUB_ASSIGN,	/*	-=	*/
	TK_ARROW,		/*	->	*/
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

void initialize_clexer(t_scanner_context* sc)
{
	int i = 0;	

	assert(sc != NULL);

	current_token_code = TK_NULL;
	peek_token_code = TK_NULL;
    cached_token_code = TK_NULL;

	coord.filename = atom_string(sc->filename);
	coord.column = 0;
	coord.line = 0;

	/* initialize static tables of preprocessor ucpp */
	init_cpp();
	
	/*
	 * non-zero if the special macros (__FILE__ and others)
	 * should not be defined. This is a global flag since
	 * it affects the redefinition of such macros (which are
	 * allowed if the special macros are not defined)
	 */
	no_special_macros = 0;

	/* 
	 * This function initializes the macro table
	 * and other things; it will intialize assertions if it has a non-zero
	 * argument
	 */
	init_tables(1);
	
	
	/* init_include_path(sc->include_pathes); */
	
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

	/* [TODO] - get current dir and concatenate the names */
	ls.input = fopen(sc->filename, "rb");
	if (ls.input == NULL)
	{
		fprintf(stderr, "%s\n", "error : input file is dead");		
	}

	for (; i < sc->number_of_include_pathes; i ++) add_incpath(sc->include_pathes[i]);

    /*
     * [NOTICE] Inject macro from here
     * [FIX ME] These macros should be put in HCC header file.
     * Also these macros should be passed from command line or configuration file instead of hard coded
     */
    define_macro(&ls, "wchar_t=int");
    define_macro(&ls, "NULL=((void *)0)");
    define_macro(&ls, "size_t=unsigned int");
    
#if defined(_WIN32)
    define_macro(&ls, "_WIN32");
    define_macro(&ls, "_M_IX86=500");

    /*
     * [FIX ME] 
     * This is to work around non ANSI C extensions from Visual C++ compiler. 
     * The work around simply define the VC specific extensions as empty macro so they will be filtered out before getting to parser.
     * Not a good way as it abandons some information like calling convention from front end to back end, but is the only choice 
     * at this moment given I don't have other headers / STD C library to use.
     */
#if defined(HCC_VISUAL_STUDIO_WORK_AROUND)
    define_macro(&ls, "__cdecl=");
    define_macro(&ls, "__stdcall=");
    define_macro(&ls, "__declspec(a)=");
    define_macro(&ls, "deprecated(a)=");
    define_macro(&ls, "dllimport(a)=");
	define_macro(&ls, "__inline=");
    define_macro(&ls, "__forceinline=");

	/*
    define_macro(&ls, "IN=");
	define_macro(&ls, "PCONTEXT=int");
    define_macro(&ls, "BYTE=unsigned char");
    define_macro(&ls, "WORD=unsigned short");
    define_macro(&ls, "DWORD=unsigned long");
    */
#endif

#elif defined (_WIN64)
    define_macro(&ls, "_WIN64");
#endif

/*
    ucpp may be configured at runtime to accept alternate characters as
    possible parts of identifiers. Typical intended usage is for the '$'
    and '@' characters. The two relevant functions are set_identifier_char()
    and unset_identifier_char(). When this call is issued:
	    set_identifier_char('$');
    then for all the remaining input, the '$' character will be considered
    as just another letter, as far as identifier tokenizing is concerned. This
    is for identifiers only; numeric constants are not modified by that setting.
    This call resets things back:
	    unset_identifier_char('$');
    Those two functions modify the static table which is initialized by
    init_cpp(). You may call init_cpp() at any time to restore the table
    to its standard state.

    set '$' as an acceptable identifier char because some windows SDK header files
    (for example, specstrings.h) has macro with identifier containing $. 
*/
    set_identifier_char('$');
}

void free_clexer()
{
	wipeout();
	free_lexer_state(&ls);
}

static int identify_keyword(char* id)
{
    tKW* p = NULL;
    int retval = TK_ID;
	int index = 0;
	if (*id != '_')
	{
		/* convert *id to upper case letter if needed and calc diff as index (starting 0)
		 * deal special case "_" seperately which will generate index out of bounds (26)
		 */
		index = (*id &~0x20) - 'A'; 
	}

	/* [NON STD EXT][FIX ME]
	 * here is a hack to support __int64 extension
	 * otherwise this keyword will be omitted.
	 */
	else if (!strcmp(id, "__int64"))
	{
		return TK_INT64;
	}
  
    assert(id);

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

static int identify_integer_value(char* start, int length, int base)
{
    unsigned long value = 0;
    char* current = start;
    int i = 0;
    int overflow = 0;

    assert(start);

    for (; length > 0; length -- , current ++)
    {
        if (16 == base)
        {
            if ( (*current >= 'A' && *current <= 'F') ||
                 (*current >= 'a' && *current <= 'f'))
            {
                i = (*current & ~0x20) - 'A' + 10;
            }
            else if (HCC_ISDECIMAL_DIGIT(*current)) 
            {
                i = *current - '0';
            }
            else
            {
                lexeme_error("illegal hex character detected!");
                break;
            }

            if (value &~(~0UL >> 4))
            {
                overflow = 1;
                warning("overflow detected for hex integer!");
            }
            else
            {
                value = (value<<4) + i;
            }
        }
        else if (8 == base)
        {
            assert(HCC_ISOCT_DIGIT(*current));
        
            i = *current - '0';
            if (value &~(~0UL >> 3))
            {
                overflow = 1;
                warning("overflow detected for oct integer!");
            }
            else
            {
                value = (value<<3) + i;
            }
        }
        else
        {
            assert(base == 10);
            assert(HCC_ISDECIMAL_DIGIT(*current));
            
            i = *current - '0';

            if (value > (~0UL - i)/10)
            {
                overflow = 1;
                warning("overflow detected for decimal integer!");
            }
            else
            {
                value = value * 10 + i;
            }
        }
    }

    HCC_TRACE("value is %d\n", value);

    lexeme_value.integer_value = value;
    return TK_CONST_INTEGER;
}

static int identify_float_value(char* number)
{
    /* TODO - is this the right type? */
    long double value = 0;

    assert(number);

    if ('.' == *number)
    {
        for (;;number ++)
        {
            if (!HCC_ISDECIMAL_DIGIT(*number)) 
            {
                break;
            }
        }
    }

    if ('e' == *number || 'E' == *number)
    {
        number ++;
        if ('+' == *number || '-' == *number)
        {
            number ++;
        }

        if (HCC_ISDECIMAL_DIGIT(*number))
        {
            for (;; number ++)
            {
                if (!HCC_ISDECIMAL_DIGIT(*number))
                {
                    break;
                }
            }
        }
        else
        {
            lexeme_error("incorrect float constant format detected!");
            return 0;
        }
    }

    errno = 0;
    value = strtod(number, NULL);
    if (errno == ERANGE)
    {
        warning("float value out of range!");
    }

    fprintf(stderr, "float value %f\n", value);

	/*
     * todo - here maybe do one step further to identify that is it a double, or float?
     * right now the value is just assigned to double which is garanteed to hold on both a double and a float
     * so it doesn't hurt, so far...
	 */
    lexeme_value.double_value = value;
    return TK_CONST_FLOAT;
}

static int identify_numerical_value(char* number)
{
    int base = 10;
    char* begin = number;

    assert(number);
    
    if ('.' == *number)
    {
        return identify_float_value(number);
    }

	/*
     * [TODO] - here we need to verify the valid suffixes
     * ul, UL, uL, Ul. and detect invalid hex number, oct number, and decimal number
     * currently number like 0xXYZ will survive the crossfire which is not right
	 */
    if ('0' == *number && 
        ('x' == number[1] ||
         'X' == number[1]))
    {
        base = 16;
        begin += 2;
        number += 2;

        while (HCC_ISHEX_DIGIT(*number))
        {
            number ++;
        }
    }
    else if ('0' == *number)
    {
        base = 8;
        begin ++;
        number ++;

        while (HCC_ISOCT_DIGIT(*number))
        {
            number ++;
        }
    }
    else
    {
        number ++;
        while (HCC_ISDECIMAL_DIGIT(*number))
        {
            number ++;
        }
    }

    if (base == 16 || (*number != '.' && *number != 'e' && *number != 'E'))
    {
        return identify_integer_value(begin, (int)(number - begin), base); 
    }
    else
    {
        return identify_float_value(begin);
    }
}


static int get_token_internal()
{
    int retval;
    int r;

    /*
	 * This is the trick to implement the backup - restore feature such that
     * lexer provides its caller the capability to peek a token (without consume).
     */
	if (peek_token_code != TK_NULL)
	{
        /*
         * none NULL peek token indicates previously the peek_token is invoked.
         * so we just return the cached peek token and restore the clexer state to 
         * what it should be. 
         */
        current_token_code = peek_token_code;
		peek_token_code = TK_NULL;
        lexeme_value = peek_lexeme_value;
        return current_token_code;
	}

    retval = TK_ID;

    r = lex(&ls);

    /*
    * lex() reads the next token from the processed stream and stores it
    * into ls->ctok.
    * return value: non zero on error (including CPPERR_EOF, which is not
    * quite an error)
    */
    if (r == CPPERR_EOF)
    {
        return TK_END;
    }
    else if (r)
    {
        /*TODO*/ 
        assert(0); 
    }
	
    
    /* we print each token: its numerical value, and its
    string content; if this is a PRAGMA token, the
    string content is in fact a compressed token list,
    that we uncompress and print. */
    if (ls.ctok->type == PRAGMA) 
    {
        unsigned char *c = (unsigned char *)(ls.ctok->name);

#ifdef HCC_TRACE_ENABLE
       printf("line %ld: <#pragma>\n", ls.line); 
#endif

        for (; *c; c ++) 
        {
            int t = *c;

            if (STRING_TOKEN(t)) 
            {
            #ifdef HCC_TRACE_ENABLE
                printf("  <%2d>  ", t);
            #endif

                for (c ++; *c != PRAGMA_TOKEN_END; c ++)
                {
            #ifdef HCC_TRACE_ENABLE
                    putchar(*c);
                    putchar('\n');
            #endif
                }
            } 
            else 
            {
            #ifdef HCC_TRACE_ENABLE
                printf("  <%2d>  `%s'\n", t,
                    operators_name[t]);
            #endif
            }
        }

        retval = TK_POUND;
    } 
    else if (ls.ctok->type == CONTEXT) 
    {
#ifdef HCC_TRACE_ENABLE
        printf("new context: file '%s', line %ld\n",
            ls.ctok->name, ls.ctok->line);

        coord.filename = atom_string(ls.ctok->name);
        coord.line = ls.ctok->line;
#endif

		/* HACK! TODO */
        retval = TK_WHITESPACE;
    } 
    else if (ls.ctok->type == NEWLINE) 
    {
#ifdef HCC_TRACE_ENABLE
        printf("[newline]\n");
#endif

        retval = TK_NEWLINE;
    } 
    else 
    {
		/* todo - column and file name (should be set in translation unit) */
        coord.line = ls.ctok->line; 

        if (STRING_TOKEN(ls.ctok->type))
        {
            /* NUMBER, NAME, STRING, CHAR
             * here lexer does proper categorization for the lexeme
             * it identifies number (integer or float), string/char literal, identifier
             * the associated value of lexeme is stored in lexeme_value, 
			 * for parser usage in next stage.
            */
			switch (ls.ctok->type)
            {
            case NUMBER :
                {
                    HCC_TRACE("number const : %s\n", ls.ctok->name); 
                    retval = identify_numerical_value(ls.ctok->name);
                    break;
                }
            case NAME:
                {
					retval = identify_keyword(ls.ctok->name);

                    if (retval == TK_ID)
                    {
                        lexeme_value.string_value = atom_string(ls.ctok->name);

                        HCC_TRACE("identifier: %s\n", lexeme_value.string_value);
                    }
                    else
                    {
                        HCC_TRACE("keyword: %s\n", ls.ctok->name);
                    }

                    break;
                }
            case STRING:
                {
                    HCC_TRACE("string const : %s\n", ls.ctok->name);

                    lexeme_value.string_value = atom_string(ls.ctok->name);
                    retval = TK_CONST_STRING_LITERAL;
                    break;
                }
            case CHAR:
                {
                    HCC_TRACE("char const : %s\n", ls.ctok->name);

                    lexeme_value.string_value = atom_string(ls.ctok->name);
                    retval = TK_CONST_CHAR_LITERAL;
                    break;
                }
            }
        }
        else
        {
            retval = lexical_map[ls.ctok->type];
			lexeme_value.string_value = atom_string(STRING_TOKEN(ls.ctok->type) ? ls.ctok->name
                : operators_name[ls.ctok->type]);
#ifdef HCC_TRACE_ENABLE
            printf("line %ld: <%2d>  `%s'\n", ls.ctok->line,
                ls.ctok->type,
                STRING_TOKEN(ls.ctok->type) ? ls.ctok->name
                : operators_name[ls.ctok->type]);
#endif
        }

        /*
        printf("line %ld: <%2d>  `%s'\n", ls.ctok->line,
            ls.ctok->type,
            STRING_TOKEN(ls.ctok->type) ? ls.ctok->name
            : operators_name[ls.ctok->type]);
        */
    }

	current_token_code = retval;
    return retval;
}

int get_token()
{
    int token;

    if (cached_token_code != TK_NULL)
    {
        current_token_code = cached_token_code;
        cached_token_code = TK_NULL;
        lexeme_value = cached_lexeme_value;
        return current_token_code;
    }

    token = get_token_internal();

    /* [TODO] - tabs (vertical and horizontal) and form feed needs? */
    while (token == TK_NEWLINE ||
        token == TK_CRETURN ||
        token == TK_WHITESPACE ||
        token == TK_POUND)
    {
        token = get_token_internal();
        if (token == TK_END)
        {
            return token;
        }
    }

    /* [WORK AROUND] 
     * work around ucpp can't concat string literals
     * a better fix should be done in ucpp
     */
    while (token == TK_CONST_STRING_LITERAL)
    {
        cached_token_code = token;
        cached_lexeme_value = lexeme_value;

        token = get_token_internal();
        while (token == TK_NEWLINE ||
            token == TK_CRETURN ||
            token == TK_WHITESPACE)
        {
            token = get_token_internal();
        }

        if (token == TK_CONST_STRING_LITERAL)
        {
            char* tmp = (char*)malloc(strlen(cached_lexeme_value.string_value) + strlen(lexeme_value.string_value) + 1);
            
            strcpy(tmp, cached_lexeme_value.string_value);
            strcat(tmp, lexeme_value.string_value);
            
            lexeme_value.string_value = atom_string(tmp);
            free(tmp);
        }
        else
        {
            cached_token_code = token;
            cached_lexeme_value = lexeme_value;
            return TK_CONST_STRING_LITERAL;
        }
    }

    return token;
}

int peek_token()
{
    /* [TODO] token coordinate value save and restore */
	int backup_token = current_token_code;
    t_lexeme_value backup_lexeme_value = lexeme_value;

	peek_token_code = get_token();
    peek_lexeme_value = lexeme_value;

	current_token_code = backup_token;
    lexeme_value = backup_lexeme_value;

	return peek_token_code;
}
