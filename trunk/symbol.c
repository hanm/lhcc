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

#include "symbol.h"
#include "arena.h"
#include "assert.h"
#include "Hcc.h"

/*
 * scope level
 * initial 0 (global)
 * level ++ when entering scope
 * level -- when leaving scope
*/
int scope_level = GLOBAL;

#define TABLESIZE NUMBEROFELEMENTS(((t_symbol_table*)0)->buckets)

static t_symbol_table global_symbol_tables[] = {{CONSTANTS}, {GLOBAL}, {GLOBAL}, {GLOBAL} };

t_symbol_table* constants = &global_symbol_tables[0];
t_symbol_table* identifiers = &global_symbol_tables[1];
t_symbol_table* types = &global_symbol_tables[2];
t_symbol_table* externals = &global_symbol_tables[3];

t_symbol_table* make_symbol_table(int arena)
{
    t_symbol_table* table = NULL;
    CALLOC(table, arena);
    
    return table;
}

void enter_scope()
{
	scope_level ++;
	/*
	 * lazy table allocation - may save some time if there are actually no new symbol
	*/
}

void exit_scope()
{
	identifiers = identifiers->previous;
	scope_level --;
}

t_symbol* add_symbol(char* name, t_symbol_table** table, int level, int arena)
{
    t_symbol_table* tb = *table;
    struct entry* p;
    unsigned long h = (unsigned long)name&(TABLESIZE-1); //[tag] - to do need a better hashing.. 

    HCC_ASSERT(level == 0 || level >= tb->level);

    // we need a new table with deeper level than current table passed in..
    if (level > 0 && tb->level < level)
    {
        t_symbol_table* new_tb = make_symbol_table(FUNC);
        new_tb->previous = tb;
        new_tb->level = level;
	    
        if (tb)
        {
            new_tb->all_symbols = tb->all_symbols;
        }
	
        *table = new_tb;
        tb = new_tb;        
    }

    CALLOC(p, arena);

	p->symbol.name = (char *)name;
	p->symbol.scope = level;
    p->symbol.up = tb->all_symbols;
	tb->all_symbols = &p->symbol;
	p->next = tb->buckets[h];
	tb->buckets[h] = p;

	return &p->symbol;
}


struct symbol* find_symbol(char* name, t_symbol_table* table)
{
	struct entry* p = NULL;
    unsigned long h = (unsigned long)name&(TABLESIZE-1); 

    HCC_ASSERT(table);
    
	for (;;)
	{
		if (!table->buckets)
		{
			for (p = table->buckets[h]; p; p = p->next)
			{
				/*
				 * this only work if the string name coming from the atom table
				*/
				if (p->symbol.name == name) 
				{
					return &p->symbol;
				}
			}
		}

		table = table->previous;
		if (!table) break;
	}

	return NULL;
}