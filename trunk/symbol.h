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

#ifndef __HCC_SYMBOL_H
#define __HCC_SYMBOL_H

#include "hcc.h"
#include "type.h"

extern int symbol_scope;

enum {
	SYMBOL_TYPEDEF, /* typedef names */
	SYMBOL_IDENTIFIER, /* normal identifier */
	SYMBOL_FUNC, /* function */ 
	SYMBOL_CONST, /* constant */
	SYMBOL_STRING, /* string literals including char */
	SYMBOL_LABEL, /* label */
	SYMBOL_ENUM_CONST /* enum constant */
};

/*
 * symbol value
 */
typedef union union_value
{
	char sc;
	unsigned char uc;
	short ss;
	unsigned short us;
	int i;
	unsigned int ui;
	long l;
	unsigned long ul;
	float f;
	double d;
	long double ld;
	void* p;
} t_symbol_value;


/*
 * symbol struct
 */
typedef struct symbol 
{
	char *name; /* symbol name - eg, literal string for an identifier */
    int storage; /* stroage class - auto, register, extern, static, typedef, enum */
	int scope; /* symbol effective scope */
	t_type* type; /* symbol type */
    int defined; /* symbol definition flag */

    int hidden_typedef;
    t_coordinate coordinate;

	struct symbol* previous;

	t_symbol_value value;

} t_symbol;


/*
 * Symbol Table
 */
typedef struct symbol_table
{
	int level;
    struct symbol_table* previous;
    struct entry 
    {
        t_symbol symbol;
		struct entry *next;
	} *buckets[HCC_HASH_SIZE];
	
    struct symbol* all_symbols;
} t_symbol_table;

extern t_symbol_table* sym_table_constants; /* store constants, string literals */
extern t_symbol_table* sym_table_identifiers; /* store identifiers */
extern t_symbol_table* sym_table_types;/* store types */
extern t_symbol_table* sym_table_externals; /* []*/

t_symbol_table* make_symbol_table(int arena);

void enter_scope(void);

void exit_scope(void);

void record_hidden_typedef_name(t_symbol* sym);

t_symbol* install_symbol(char* name, t_symbol_table* table);

/*
 * add a symbol to symbol table chain. symbol_scope should be no less than table's level.. in other words, hcc
 * only calls this function in current or nested scope. This reflects the actual processing for parser that
 * outer scope is to be processed first then inner scope..etc
 *
 */
t_symbol* add_symbol(char* name, t_symbol_table** table, int symbol_scope, int arena);

/* search a symbol in symbol table of current and enclosing scope - if the symbol is not there then search previous chained symbol tables if any
 * (it doesn't make sense to search next chained symbol table constrained by C binding rules)
 * 
*/
t_symbol* find_symbol(char* name, t_symbol_table* table);

/*
 * constant differ from normal identifier in such aspects that:
 * 1. In ANSI C all constants are in the same namespace and has no scope concepts
 * 2. Constants with same literal value share a single instance
 *
 * [TODO] - add type information
 */
t_symbol* add_const(t_symbol_value val);

/* free symbol tables for current parsing session
 * the symbols allocated for the parsing session will be freed and tables restored to orignial state
*/
void free_symbol_tables();

#endif