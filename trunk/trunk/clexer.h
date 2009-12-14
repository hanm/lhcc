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

#ifndef __HCC_SCANNER_H
#define __HCC_SCANNER_H

#include "symbol.h"

enum TOKEN
{
#define TK(a, b) a,
#include "tokendef.h"
#undef TK
};

#define HCC_ISDECIMAL_DIGIT(n) (n >= '0' && n <= '9')
#define HCC_ISOCT_DIGIT(n)       (n >= '0' && n <= '7')
#define HCC_ISHEX_DIGIT(n)        (HCC_ISDECIMAL_DIGIT(n) || (n >= 'A' && n <= 'F') || (n >= 'a' && n <= 'f'))

typedef struct src_coordinate
{
    char* filename;
    int line;
    int column;
} t_coordinate;

typedef struct clexer_context
{
	char* filename; /* file under compiling */
	int number_of_include_pathes; 
	char** include_pathes;
	/* extend here */
} t_scanner_context;

typedef union lexeme_value
{
	int integer_value;
	float float_value;
	long double double_value;
	char* string_value;
} t_lexeme_value;

//
// lexeme value of current lexer token
//
t_lexeme_value lexeme_value;

//
// lexeme value of peeked lexer token
//
t_lexeme_value peek_lexeme_value;

//
// lexeme value of cached token
//
t_lexeme_value cached_lexeme_value;

//
// current token coordinate (file, line, column)
// [FIX ME] - currently it has a problem to support column and file
//
t_coordinate coord;

//
// get next token
//
int get_token();

/*
 * peek next token but doesn't consume it (so lexer's internal state is kept intact in this case.. comparing with get_token)
 * there is a twist here - peek token can't be invoked straight many times
 * the invokation should be interleaved with get_token to clean up the peek status
 * an alternative is to explicit clean up peek status by caller but since it's LL(1)
 * there is no need to do so for now..
*/
int peek_token();

/*
 * reset scanner to prepare for compiling next file
 * this method will initialize internal preprocessor and lexer (currently using ucpp)
 * and reset previous lexer state if the internal lexer is already initialized
 */
void initialize_clexer(t_scanner_context* sc);

/*
 * free internal lexer state and reclaim memory used in previous lexical analysis
 */
void free_clexer();

#endif