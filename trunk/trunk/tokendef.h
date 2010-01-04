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

/****************************************************************

							~~~~~~~~~~~TK Category ~~~~~~~~~~~

															identifier
															keyword
															constant
															string-literal
															operator
															punctuator

*****************************************************************/

/* Padding fields for enum defined in ucpp */
TK(TK_WHITESPACE, "")

/* 
	KEYWORDS
	
	auto double int struct
	break else long switch
	char extern return union
	const float short unsigned
	continue for signed void
	default goto sizeof volatile
	do if static while register static
    enum typedef
*/
TK(TK_FLOAT,  "float")
TK(TK_DOUBLE,  "double")
TK(TK_CHAR,  "char")
TK(TK_SHORT, "short")
TK(TK_INT,  "int")
TK(TK_SIGNED,       "signed")
TK(TK_UNSIGNED,   "unsigned")
TK(TK_VOID,      "void")
TK(TK_LONG,   "long")


TK(TK_STRUCT,     "struct")
TK(TK_UNION,     "union")
TK(TK_ENUM,     "enum")

TK(TK_CONST,     "const")
TK(TK_VOLATILE, "volatile")

TK(TK_SIZEOF,        "sizeof")
TK(TK_TYPEDEF,       "typedef") /* 16 */

TK(TK_AUTO,         "auto")
TK(TK_REGISTER,       "register")
TK(TK_STATIC,       "static")
TK(TK_EXTERN,      "extern")

TK(TK_SWITCH,       "switch")
TK(TK_CASE,         "case")
TK(TK_BREAK,        "break")
TK(TK_CONTINUE,     "continue")
TK(TK_DEFAULT,     "default")
TK(TK_DO,           "do")
TK(TK_WHILE,       "while")
TK(TK_FOR,           "for")
TK(TK_IF,           "if")
TK(TK_ELSE,         "else")
TK(TK_GOTO,      "goto")
TK(TK_RETURN,      "return")

/* identifier */
TK(TK_ID, "identifier") /* 33 */

/* constants 
 * 
 * The suffixes [lL] traditionally indicate integer constants of type long. These suffixes
 * are allowed, but are superfluous, because int and long are the same size normally.
 *
 * An integer constant can also be suffixed with uU, in which case its type is
 * unsigned. (One or both of uU and lL can appear.) An integer constant also has type
 * unsigned if its value cannot be represented as an int. Otherwise, the type of an
 * integer constant is int. 
 * 
 * The ll, LL, lL, and Ll suffixes indicate a long long constant (a 64-bit integral type). 
 * Note that long long is not a strict ANSI C type.
 */
TK(TK_CONST_INTEGER,     "int const")
TK(TK_CONST_UNSIGNED_INTEGER,     "unsigned int const")
TK(TK_CONST_LONG_INTEGER,     "long int const")
TK(TK_CONST_UNSIGNED_LONG_INTEGER,     "unsigned long int const")
TK(TK_CONST_LONG_LONG,     "long long const")
TK(TK_CONST_UNSIGNED_LONG_LONG,     "unsigned long long const")

TK(TK_CONST_FLOAT,    "float const")
TK(TK_CONST_DOUBLE, "double const")
TK(TK_CONST_LONG_DOUBLE, "long double const")

TK(TK_CONST_STRING_LITERAL,    "string literal")
TK(TK_CONST_CHAR_LITERAL,   "char literal")

/* operator */
TK(TK_COMMA,         ",")
TK(TK_QUESTION,      "?")
TK(TK_COLON,         ":")
TK(TK_ASSIGN,        "=")
TK(TK_BITOR_ASSIGN,  "|=")
TK(TK_BITXOR_ASSIGN, "^=")
TK(TK_BITAND_ASSIGN, "&=")
TK(TK_LSHIFT_ASSIGN, "<<=")
TK(TK_RSHIFT_ASSIGN, ">>=")
TK(TK_ADD_ASSIGN,    "+=")
TK(TK_SUB_ASSIGN,    "-=")
TK(TK_MUL_ASSIGN,    "*=")
TK(TK_DIV_ASSIGN,    "/=")
TK(TK_MOD_ASSIGN,    "%=")
TK(TK_OR,            "||")
TK(TK_AND,           "&&")
TK(TK_BITOR,         "|")
TK(TK_BITXOR,        "^")
TK(TK_BITAND,        "&")
TK(TK_EQUAL,         "==")
TK(TK_UNEQUAL,       "!=")
TK(TK_GREAT,         ">")
TK(TK_LESS,          "<")
TK(TK_GREAT_EQ,      ">=")
TK(TK_LESS_EQ,       "<=")
TK(TK_LSHIFT,        "<<")
TK(TK_RSHIFT,        ">>")
TK(TK_ADD,           "+")
TK(TK_SUB,           "-")
TK(TK_MUL,           "*")
TK(TK_DIV,           "/")
TK(TK_MOD,           "%")
TK(TK_INC,           "++")
TK(TK_DEC,           "--")
TK(TK_NOT,           "!")
TK(TK_COMP,          "~")
TK(TK_DOT,           ".")
TK(TK_ARROW,       "->")
TK(TK_LPAREN,        "(")
TK(TK_RPAREN,        ")")
TK(TK_LBRACKET,      "[")
TK(TK_RBRACKET,      "]")

/* punctuators */
TK(TK_LBRACE,        "{")
TK(TK_RBRACE,        "}")
TK(TK_SEMICOLON,     ";")
TK(TK_ELLIPSE,       "...")
TK(TK_POUND,         "#")
TK(TK_CRETURN, "\r")
TK(TK_NEWLINE,       "\n")

TK(TK_END,           "EOF")

/* extension to ANSI C for type int 64*/
TK(TK_INT64, "__int64")

TK(TK_NULL, "null")