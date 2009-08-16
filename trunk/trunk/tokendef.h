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
TK(ID, "identifier");

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
TK(FLOAT,  "float")
TK(DOUBLE,  "double")
TK(CHAR,  "char")
TK(SHORT, "short")
TK(INT,  "int")
TK(SIGNED,       "signed")
TK(UNSIGNED,   "unsigned")
TK(VOID,      "void")
TK(LONG,   "long")


TK(STRUCT,     "struct")
TK(UNION,     "union")
TK(ENUM,     "enum")

TK(CONST,     "const")
TK(VOLATILE, "volatile")

TK(SIZEOF,        "sizeof")
TK(TYPEDEF,       "typedef")

TK(AUTO,         "auto")
TK(REGISTER,       "register")
TK(STATIC,       "static")
TK(EXTERN,      "extern")

TK(SWITCH,       "switch")
TK(CASE,         "case")
TK(BREAK,        "break")
TK(CONTINUE,     "continue")
TK(DEFAULT,     "default")
TK(DO,           "do")
TK(WHILE,       "while")
TK(FOR,           "for")
TK(IF,           "if")
TK(ELSE,         "else")
TK(GOTO,      "goto")
TK(RETURN,      "return")

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