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
#include <assert.h>

#include "symbol.h"
#include "arena.h"

/*
 * scope level
 * initially set to GLOABLE (enum value 3)
 * level ++ when entering scope
 * level -- when leaving scope
*/
int symbol_scope = GLOBAL;

#define TABLESIZE NUMBEROFELEMENTS(((t_symbol_table*)0)->buckets)

static t_symbol_table global_symbol_tables[] = {{CONSTANTS}, {GLOBAL}, {GLOBAL}, {GLOBAL}};

/* array holding pointers to symbols which by themselves are typedef but hidden in current scope
 * because of redeclaring as normal identifiers. 
 * the array will be reset each time exit_scope is invoked to make sure no side effect (for modifying)
 * on existing symbol tables preserves cross scope.
 *
 * [NOTE] I think 256 symbols are enough for most practical usage. If there is a bug, reconsider this later.
 */
static t_symbol* hidden_typedef_symbols[256] = {NULL};

t_symbol_table* sym_table_constants = &global_symbol_tables[0]; 
t_symbol_table* sym_table_identifiers = &global_symbol_tables[1]; 
t_symbol_table* sym_table_types = &global_symbol_tables[2]; 
t_symbol_table* sym_table_externals = &global_symbol_tables[3]; 

static void reset_hidden_typedefs()
{
    int n = 0;
    for (; n < NUMBEROFELEMENTS(hidden_typedef_symbols); n ++)
    {
        if (hidden_typedef_symbols[n] != NULL)
        {
            hidden_typedef_symbols[n]->hidden_typedef = 0;
            hidden_typedef_symbols[n] = NULL;
        }
    }
}

t_symbol_table* make_symbol_table(int arena)
{
    t_symbol_table* table = NULL;
    CALLOC(table, arena);
    
    return table;
}

void enter_scope()
{
	symbol_scope ++;
	/*
	 * I don't allocate a symbol table here because in practice when entering a scope
     * there might be no need to create a new symbol context at all
     * Many scopes are generated from C block statements where no new variable is declared/defined.
     *
     * This lazy initializatio would hopefully improve some performance since it reduced potential call
     * into the memory alloc routine (even it had a memory pool..)
	 */
}

void exit_scope()
{
    reset_hidden_typedefs();

	remove_types(symbol_scope);
	if (sym_table_types->level == symbol_scope)
	{
		sym_table_types = sym_table_types->previous;
	}

    if (symbol_scope == sym_table_identifiers->level)
    {
		sym_table_identifiers = sym_table_identifiers->previous;
    }

	assert(symbol_scope > GLOBAL);
	symbol_scope --;
}


t_symbol* add_symbol(char* name, t_symbol_table** table, int level, int arena)
{
    t_symbol_table* tb = *table;
    struct entry* p;
    unsigned long h = (unsigned long)name&(TABLESIZE-1);  /*[tag] - to do need a better hashing*/

    assert(name != NULL && table != NULL && arena >= 0);
    assert(level >= tb->level);

    /*
     * we need a new table with deeper level than current table passed in..
     * this table may not be consecutive in terms of "level" index
     * for example.. 
     * gloabl
     * void foo()
     * {{{{{int a = 0;}}}}}
     * note no symbol table will generated when entering the first four block scopes.
     */
    if (tb->level < level)
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
    p->symbol.previous = tb->all_symbols;
	tb->all_symbols = &p->symbol;
	p->next = tb->buckets[h];
	tb->buckets[h] = p;

	return &p->symbol;
}

t_symbol* install_symbol(char* name, t_symbol_table* table)
{
    struct entry* p;
    unsigned long h = (unsigned long)name&(TABLESIZE-1); 

    assert(name != NULL && table != NULL);

    CALLOC(p, FUNC);

	p->symbol.name = name;
    p->symbol.scope = table->level;
    p->symbol.previous = table->all_symbols;
	table->all_symbols = &p->symbol;
	p->next = table->buckets[h];
	table->buckets[h] = p;

	return &p->symbol;
}

struct symbol* find_symbol(char* name, t_symbol_table* table)
{
	struct entry* p = NULL;
    unsigned long h = (unsigned long)name&(TABLESIZE-1); 

    assert(table);
    
	for (;;)
	{
		/*
		 * [TO IMPROVE] Is this check really needed? The allocation of symbol table
		 * would guarantee the buckets are allocated.
		 */
		if (table->buckets)
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

t_symbol* add_const(t_symbol_value val)
{
	struct entry* p = NULL;
	unsigned long h = (unsigned long)val.ul&(TABLESIZE-1); 

	for (p = sym_table_constants->buckets[h]; p; p = p->next)
	{
		/*
		 * [TODO] - currently it just choose arbitary fields to compare
		 * need hook up with type system
		 */
		if (p->symbol.value.p == val.p)
		{
			return &p->symbol;
		}
	}

	CALLOC(p, PERM);
	p->symbol.name = "";  /* [TODO] refer LCC for reference */
	p->symbol.scope = CONSTANTS;
	p->symbol.storage = STORAGE_STATIC;
	p->symbol.value = val;
    p->symbol.defined = 1;
	p->next = sym_table_constants->buckets[h];
	p->symbol.previous = sym_table_constants->all_symbols;
	sym_table_constants->all_symbols = &p->symbol;
	/* [TODO] - type
	 * [TODO] - don't forget to check other fields recently added to symbol!!
	 */
	return &p->symbol;
}

void free_symbol_tables()
{
    /* reset all symbol table to orignial state */
    symbol_scope = GLOBAL;
    
    memset(global_symbol_tables, 0, sizeof(global_symbol_tables));
    sym_table_constants = &global_symbol_tables[0]; 
    sym_table_identifiers = &global_symbol_tables[1]; 
    sym_table_types = &global_symbol_tables[2]; 
    sym_table_externals = &global_symbol_tables[3]; 

    sym_table_constants->level = CONSTANTS;
    sym_table_identifiers->level = GLOBAL;
    sym_table_types->level = GLOBAL;
    sym_table_externals->level = GLOBAL;

    /* note thoese dangling symbol tables binding with the 4 tables avbove
     * will be deallocated all at once when the host arena is 
     * explicitly destroyed.
     */
}

void record_hidden_typedef_name(t_symbol* sym)
{
    int n = 0;
    assert(sym);

    for (; n < NUMBEROFELEMENTS(hidden_typedef_symbols); n ++)
    {
        if (hidden_typedef_symbols[n] == NULL)
        {
            hidden_typedef_symbols[n] = sym;
            break;
        }
    }

    assert(n <= NUMBEROFELEMENTS(hidden_typedef_symbols));
}