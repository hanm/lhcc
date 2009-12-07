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

#include "error.h"
#include <string.h>

static int error_count = 0;

void error(char* expect_token, char* actual_token)
{
	char* exp;
	char* actual;

	if (strcmp(expect_token, "\r") == 0)
	{
		exp = "carriage return";
	}
	else if (strcmp(expect_token, "\n") == 0)
	{
		exp = "new line";
	}
	else
	{
		exp = expect_token;
	}

	if (strcmp(actual_token, "\r") == 0)
	{
		actual = "carriage return";
	}
	else if (strcmp(actual_token, "\n") == 0)
	{
		actual = "new line";
	}
	else
	{
		actual = actual_token;
	}

	printf("Error : expect token %s but got token %s \n", exp, actual);

	error_count ++;
}

void syntax_error(t_coordinate* coordinate, char* error_msg)
{
    printf("Syntax Error on line: %d : %s", coordinate->line, error_msg);

	error_count ++;
}

void warning(t_coordinate* coordinate, char* warning_msg)
{
    printf("Warning : line %d %s", coordinate->line, warning_msg);
}

void lexeme_error(t_coordinate* coordinate, char* error_msg)
{
    printf("Lexeme error : line %d %s", coordinate->line, error_msg);

	error_count ++;
}

void type_error(char* msg)
{
	printf("type error : %s\n", msg);

	error_count ++;
}