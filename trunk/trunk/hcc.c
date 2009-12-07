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

#include "hcc.h"
#include "clexer.h"
#include "cparser.h"
#include "hconfig.h"
#include <crtdbg.h>

static void displayhelp()
{
    	static char *msgs[] = {
            "", " [ option | file ]...\n",
            "	except for -l, options are processed left-to-right before files\n",
            "	unrecognized options are taken to be linker options\n",
            "-A	warn about nonANSI usage; 2nd -A warns more\n",
            "-b	emit expression-level profiling code; see bprint(1)\n",
            "-Bdir/	use the compiler named `dir/rcc'\n",
            "-c	compile only\n",
            "-dn	set switch statement density to `n'\n",
            "-Dname -Dname=def	define the preprocessor symbol `name'\n",
            "-E	run only the preprocessor on the named C programs and unsuffixed files\n",
            "-g	produce symbol table information for debuggers\n",
            "-help or -?	print this message on standard error\n",
            "-Idir	add `dir' to the beginning of the list of #include directories\n",	
            "-lx	search library `x'\n",
            "-M	emit makefile dependencies; implies -E\n",
            "-N	do not search the standard directories for #include files\n",
            "-n	emit code to check for dereferencing zero pointers\n",
            "-O	is ignored\n",
            "-o file	leave the output in `file'\n",
            "-P	print ANSI-style declarations for globals on standard error\n",
            "-p -pg	emit profiling code; see prof(1) and gprof(1)\n",
            "-S	compile to assembly language\n",
            "-static	specify static libraries (default is dynamic)\n",
            "-dynamic	specify dynamically linked libraries\n",
            "-t -tname	emit function tracing calls to printf or to `name'\n",
            "-target name	is ignored\n",
            "-tempdir=dir	place temporary files in `dir/'", "\n"
            "-Uname	undefine the preprocessor symbol `name'\n",
            "-v	show commands as they are executed; 2nd -v suppresses execution\n",
            "-w	suppress warnings\n",
            "-Woarg	specify system-specific `arg'\n",
            "-W[pfal]arg	pass `arg' to the preprocessor, compiler, assembler, or linker\n",
            0 };
            int i = 0;
            
            for (; msgs[i]; i++)
            {
                fprintf(stderr, "%s", msgs[i]);
            }
}

static void parsecmd(int argc, char* argv[])
{
    if (argc == 1)
    {
        fprintf(stderr, "%s\n", "command line is empty!");
        
        return;
    }

    (argv);
}

static void compile(const char* filename)
{
    int token = 0;
    static int count = 0;

    (filename);

    while (token = gettoken())
    {
        fprintf(stdout, "%s ", token);
        if (count++ == 8)fprintf(stdout, "\n");
    }
}

void test_lexer()
{
    t_scanner_context sc;
    int token = 0;
    char* path[3] = {"E:\\Program Files\\Microsoft Visual Studio 9.0\\VC\\include", 
		"C:\\Program Files\\Microsoft Visual Studio 9.0\\VC\\include", "C:\\"};

#if 1
    sc.filename = "G:\\jill.c";
#else
	sc.filename = "C:\\bloom-filter.c";
#endif

	sc.include_pathes = path;
	sc.number_of_include_pathes = 3;

	HCC_MEM_CHECK_START
    
	reset_clexer(&sc);

	initialize_parser();
	
    while ((token = get_token()) != TK_END)
    {
        /*
        printf("token : %d\n", token);
        printf("peek token : %d\n", peek_token());
        */

#ifdef HCC_TEST_LEXER_CLIENT
        switch(token)
        {
        case TK_CONST_INTEGER :
            {
                printf("integer const : %s\n", ctok); 
                break;
            }
        case TK_CONST_FLOAT :
            {
                printf("float const: %s\n", ctok);
                break;
            }
        case TK_ID:
            {
                printf("identifier: %s\n", ctok);
                break;
            }
        case TK_CONST_STRING_LITERAL:
            {
                printf("string literal : %s\n", ctok);
                break;
            }
        case TK_CONST_CHAR_LITERAL:
            {
                printf("char literal : %s\n", ctok);
                break;
            }
        default:
            {
                // others
            }
        }
#endif
        /*
        note for mapping to op tokens it can be defined like this
            
        char* token_mapping[] = 
        {
            #define TK(a, b) b
            #include "tokendef.h"
            #undef TK
        }

        the implicit ordering of token enum (indexes) binds the token enum with token string representation
        */
    }

	free_clexer();

    hcc_free_all();

	HCC_MEM_CHECK_END
}


void test_parser()
{
    t_scanner_context sc;

    char* path[3] = {"E:\\Program Files\\Microsoft Visual Studio 9.0\\VC\\include", 
		"C:\\Program Files\\Microsoft Visual Studio 9.0\\VC\\include", "C:\\"};

#if 1
	sc.filename = "G:\\Jill.c";
#else
	//sc.filename = "G:\\bloom-filter.c";
	//sc.filename = "G:\\stdlib.h";
	sc.filename = "G:\\string.h";
#endif

    sc.include_pathes = path;
    sc.number_of_include_pathes = 2;

    HCC_MEM_CHECK_START

    reset_clexer(&sc);

    initialize_parser();

    translation_unit();

    free_clexer();

    hcc_free_all();

    HCC_MEM_CHECK_END
}

int main(int argc, char* argv[])
{
    (argc), (argv);

#if 1
    test_parser();
#else
    test_lexer();
#endif

    return 0;
}