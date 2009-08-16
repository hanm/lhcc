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

#ifndef __HCC_TK_DEF
#define __HCC_TK_DEF

/****************************************************************

							~~~~~~~~~~~TK Category ~~~~~~~~~~~

															identifier
															keyword
															constant
															string-literal
															operator
															punctuator

*****************************************************************/

/* identifier */
TK(ID, 32, "identifier");

/* 
	KEYWORDS
	
	auto double int struct
	break else long switch
	char extern return union
	const float short unsigned
	continue for signed void
	default goto sizeof volatile
	do if static while

	extended in this compiler - register, static, enum, typedef

	C99 restrict and ?? TODO
*/
TK(FLOAT,     1,  "float")
TK(DOUBLE,    2, "double")
TK(CHAR,      3, "char")
TK(SHORT,     4,  "short")
TK(INT,       5, "int")
TK(SIGNED,      6,    "signed")
TK(UNSIGNED,  7,   "unsigned")
TK(VOID,      8, "void")
TK(LONG,     9, "long")
// 10 ~ 19 left undefined extensible in future

TK(STRUCT,    20,  "struct")
TK(UNION,    21, "union")
TK(ENUM,     22, "enum")
// 23 ~ 29 left undefined extensible in future

TK(CONST,    30, "const")
TK(VOLATILE, 31, "volatile")

TK(SIZEOF,      40,   "sizeof")
TK(TYPEDEF,     41,   "typedef")

TK(AUTO,        50,  "auto")
TK(REGISTER,    51,    "register")
TK(STATIC,      52,     "static")
TK(EXTERN,     53 ,     "extern")

TK(SWITCH,      56,   "switch")
TK(CASE,        57,  "case")
TK(BREAK,       58,    "break")
TK(CONTINUE,    59,  "continue")
TK(DEFAULT,     60,  "default")
TK(DO,          61,      "do")
TK(WHILE,       62,    "while")
TK(FOR,         63,     "for")
TK(IF,          64,   "if")
TK(ELSE,        65,    "else")
TK(GOTO,        66,   "goto")
TK(RETURN,      67,     "return")

//operators
TK(COMMA,         ",")
TK(QUESTION,      "?")
TK(COLON,         ":")
TK(ASSIGN,        "=")
TK(BITOR_ASSIGN,  "|=")
TK(BITXOR_ASSIGN, "^=")
TK(BITAND_ASSIGN, "&=")
TK(LSHIFT_ASSIGN, "<<=")
TK(RSHIFT_ASSIGN, ">>=")
TK(ADD_ASSIGN,    "+=")
TK(SUB_ASSIGN,    "-=")
TK(MUL_ASSIGN,    "*=")
TK(DIV_ASSIGN,    "/=")
TK(MOD_ASSIGN,    "%=")
TK(OR,            "||")
TK(AND,           "&&")
TK(BITOR,         "|")
TK(BITXOR,        "^")
TK(BITAND,        "&")
TK(EQUAL,         "==")
TK(UNEQUAL,       "!=")
TK(GREAT,         ">")
TK(LESS,          "<")
TK(GREAT_EQ,      ">=")
TK(LESS_EQ,       "<=")
TK(LSHIFT,        "<<")
TK(RSHIFT,        ">>")
TK(ADD,           "+")
TK(SUB,           "-")
TK(MUL,           "*")
TK(DIV,           "/")
TK(MOD,           "%")
TK(INC,           "++")
TK(DEC,           "--")
TK(NOT,           "!")
TK(COMP,          "~")
TK(DOT,           ".")
TK(DEREFERENCE,       "->")
TK(LPAREN,        "(")
TK(RPAREN,        ")")
TK(LBRACKET,      "[")
TK(RBRACKET,      "]")

//punctuators
TK(LBRACE,        "{")
TK(RBRACE,        "}")
TK(SEMICOLON,     ";")
TK(ELLIPSE,       "...")
TK(POUND,         "#")
TK(CRETURN, "\r")
TK(NEWLINE,       "\n")

TK(END,           "EOF")

#endif