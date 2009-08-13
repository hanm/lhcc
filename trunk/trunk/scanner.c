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

    init_cpp();
    (i); // TODO 
    no_special_macros = 0;
    emit_defines = emit_assertions = 0;

    init_tables(1);

    /* step 4 -- no default include path */
    init_include_path(0);

    /* step 5 -- no need to reset the two emit_* variables set in 2 */
    emit_dependencies = 0;

    /* step 6 -- we work with stdin, this is not a real filename */
    set_init_filename("[stdin]", 0);

    /* step 7 -- we make sure that assertions are on, and pragma are
    handled */
    init_lexer_state(&ls);
    init_lexer_mode(&ls);
    ls.flags |= HANDLE_ASSERTIONS | HANDLE_PRAGMA | LINE_NUM;

    /* step 8 -- input is from stdin */
    ls.input = stdin;

    /* step 9 -- we do not have any macro to define, but we add any
    argument as an include path */
 //   for (i = 1; i < argc; i ++) add_incpath(argv[i]);

    /* step 10 -- we are a lexer and we want CONTEXT tokens */
    enter_file(&ls, ls.flags);

    /* read tokens until end-of-input is reached -- errors (non-zero
    return values different from CPPERR_EOF) are ignored */
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