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
#include "hcc.h"
#include "type.h"

//
// Tooken coordinate
//
typedef struct token_coordinate
{
    char* file;
    unsigned long x, y;
} t_token_coordinate;

//
// symbol attribute - category
// [TODO] may expand and grow from here
//
enum {
	SYMBOL_TYPEDEF, // typedef names
	SYMBOL_IDENTIFIER, // normal identifier
	SYMBOL_FUNC, // function 
	SYMBOL_CONST, // constant
	SYMBOL_STRING, // string literals including char
	SYMBOL_LABEL, // label
	SYMBOL_ENUM_CONST // enum constant
};

//
// [TODO] - polish - remove some unneed fields or add more..
//
typedef union value
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
} t_value;


//
// Symbol
//
typedef struct symbol 
{
	char *name; // symbol name - eg, literal string for an identifier
    int storage; // stroage class - auto, register, extern, static, typedef, enum
	int scope; // symbol effective scope 
	int category; // symbol category (class)
	t_type* type; // symbol type
    t_token_coordinate coordinate;

	struct symbol* previous;

	t_value value;

} t_symbol;


//
// Symbol Table
//
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


t_symbol_table* make_symbol_table(int arena);

void enter_scope(void);

void exit_scope(void);

t_symbol* install_symbol(char* name, t_symbol_table* table);

//
// add a symbol to symbol table chain. scope_level should be no less than table's level.. in other words, hcc
// only calls this function in current or nested scope. This reflects the actual processing for parser that
// outer scope is to be processed first then inner scope..etc
//
//
t_symbol* add_symbol(char* name, t_symbol_table** table, int scope_level, int arena);

// search a symbol in symbol table - if the symbol is not there then search previous chained symbol tables if any
// (it doesn't make sense to search next chained symbol table constrained by C binding rules)
t_symbol* find_symbol(char* name, t_symbol_table* table);

//
// constant differ from normal identifier in such aspects that:
// 1. In ANSI C all constants are in the same namespace and has no scope concepts
// 2. Constants with same literal value share a single instance
//
// [TODO] - add type information
//
t_symbol* add_const(t_value val);

#endif