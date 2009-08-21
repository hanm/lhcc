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

#ifndef __HCC_SYMBOL_H
#define __HCC_SYMBOL_H

typedef struct token_coordinate
{
    char* file;
    unsigned long x, y;
} t_token_coordinate;

typedef struct symbol 
{
	char *name;
	int scope;
    t_token_coordinate coordinate;

	struct symbol* up;
} t_symbol;

typedef struct symbol_table
{
	int level;
    struct symbol_table* previous;
    struct entry 
    {
        t_symbol symbol;
		struct entry *next;
	} *buckets[256];
	
    struct symbol* all_symbols;
} t_symbol_table;

t_symbol_table* make_symbol_table(int arena);
void enter_scope(void);
void exit_scope(void);
t_symbol* add_symbol(char* name, t_symbol_table** table, int scope_level, int arena);
t_symbol* find_symbol(char* name, t_symbol_table* table);

#endif