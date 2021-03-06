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

#include "error.h"
#include <stdio.h>
#include <string.h>

static int error_count = 0;
static int warning_count = 0;
static FILE* fp = NULL;

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

	printf("Error in file %s on line %d : expect token %s but got token %s \r\n", 
		coord.filename, coord.line, exp, actual);

    if (fp) 
    {
        fprintf(fp, "Error in file %s on line %d : expect token %s but got token %s \r\n", 
		coord.filename, coord.line, exp, actual);
    }

	error_count ++;
}

void syntax_error(char* error_msg)
{
	printf("Syntax error detected in file %s on line %d : %s \r\n", coord.filename, coord.line, error_msg);
    
    if (fp) 
    {
        fprintf(fp, "Syntax error detected in file %s on line %d : %s \r\n", coord.filename, coord.line, error_msg);
    }

	error_count ++;
}

void warning(char* warning_msg)
{
	printf("Warning in file %s on line %d : %s \r\n", coord.filename, coord.line, warning_msg);

    if (fp) 
    {
        fprintf(fp, "Warning in file %s on line %d : %s \r\n", coord.filename, coord.line, warning_msg);
    }

	warning_count ++;
}

void lexeme_error(char* error_msg)
{
	printf("Lexeme error detected in file %s on line %d : %s \r\n", coord.filename, coord.line, error_msg);

    if (fp) 
    {
        fprintf(fp, "Lexeme error detected in file %s on line %d : %s \r\n", coord.filename, coord.line, error_msg);
    }

	error_count ++;
}

void type_error(char* msg)
{
	printf("type error in file %s on line %d : %s\n", coord.filename, coord.line,msg);

    if (fp) 
    {
        fprintf(fp, "type error in file %s on line %d : %s \r\n", coord.filename, coord.line,msg);
    }

	error_count ++;
}

void semantic_error(char* msg, t_ast_coord* coord)
{
    printf("semantic error in file %s on line %d : %s\n", coord->file, coord->line,msg);

    if (fp)
    {
        fprintf(fp, "semantic error in file %s on line %d : %s \r\n", coord->file, coord->line,msg);
    }

    error_count ++;
}

void semantic_warning(char* msg, t_ast_coord* coord)
{
    printf("Semantic warning in file %s on line %d : %s\n", coord->file, coord->line,msg);

    if (fp)
    {
        fprintf(fp, "Semantic warning in file %s on line %d : %s \r\n", coord->file, coord->line,msg);
    }

    warning_count ++;
}  

void log_initialize(char* filename)
{
    fp = fopen(filename, "wb+");
    if (!fp) printf("warning - log file is not accessible");
}

void log_terminate()
{
    if (!fp) fclose(fp);
}

void log_number(int i)
{
    if (!fp)return;

    fprintf(fp, "%d \r\n", i);
}