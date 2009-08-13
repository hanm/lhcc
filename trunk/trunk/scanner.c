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
#include "hcc.h"
#include "preprocessor/mem.h"
#include "preprocessor/cpp.h"

int gettoken()
{
	return 0;
}

void setupscanner()
{
    int i, r;
        
    struct lexer_state ls;

    char* include_path[] = {"E:\\Program Files\\Microsoft Visual Studio 9.0\\VC\\include", 0};
    
    char* compile_files[] = {"G:\\Share\\bloom-filter.h", "G:\\Share\\bloom-filter.c", 0};
    
    init_cpp();

    (i); // TODO 

    no_special_macros = 0;
    emit_defines = emit_assertions = 0;

    init_tables(1);

    init_include_path(include_path);

    emit_dependencies = 0;

    set_init_filename("G:\\Share\\bloom-filter.h", 1);

    init_lexer_state(&ls);
    init_lexer_mode(&ls);
    ls.flags |= HANDLE_ASSERTIONS | HANDLE_PRAGMA | LINE_NUM;

    ls.input = fopen("G:\\Share\\bloom-filter.h", "rb");

    for (i = 0; i < 2; i ++) add_incpath(compile_files[i]);

    enter_file(&ls, ls.flags);

    while ((r = lex(&ls)) < CPPERR_EOF) {
        if (r) {
            /* error condition -- no token was retrieved */
            continue;
        }
        /* we print each token: its numerical value, and its
        string content; if this is a PRAGMA token, the
        string content is in fact a compressed token list,
        that we uncompress and print. */
        if (ls.ctok->type == PRAGMA) {
            unsigned char *c = (unsigned char *)(ls.ctok->name);

            printf("line %ld: <#pragma>\n", ls.line);
            for (; *c; c ++) {
                int t = *c;

                if (STRING_TOKEN(t)) {
                    printf("  <%2d>  ", t);
                    for (c ++; *c != PRAGMA_TOKEN_END;
                        c ++) putchar(*c);
                        putchar('\n');
                } else {
                    printf("  <%2d>  `%s'\n", t,
                        operators_name[t]);
                }
            }
        } else if (ls.ctok->type == CONTEXT) {
            printf("new context: file '%s', line %ld\n",
                ls.ctok->name, ls.ctok->line);
        } else if (ls.ctok->type == NEWLINE) {
            printf("[newline]\n");
        } else {
            printf("line %ld: <%2d>  `%s'\n", ls.ctok->line,
                ls.ctok->type,
                STRING_TOKEN(ls.ctok->type) ? ls.ctok->name
                : operators_name[ls.ctok->type]);
        }
    }

    /* give back memory and exit */
    wipeout();
    free_lexer_state(&ls);
#ifdef MEM_DEBUG
    report_leaks();
#endif
}