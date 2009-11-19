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

#include "hcc.h"
#include "type.h"
#include "symbol.h"
#include "assert.h"
#include "arena.h"
#include "atom.h"

#define __HCC_TYPE_TABLE_HASHSIZE 512

//
// this table exits simply to provide singleton of types
// it would make sure identical type has only one runtime representation.
//
static struct type_entry
{
	t_type type;
	struct type_entry* next;
}* type_table[__HCC_TYPE_TABLE_HASHSIZE];


static t_type* atomic_type(t_type* type, int code, int align, int size, t_symbol* symbol_link)
{
	struct type_entry* p;
	unsigned long h = (code^((unsigned long)type>>3))& __HCC_TYPE_TABLE_HASHSIZE;

	HCC_ASSERT(code >= 0 && align >= 0 && size >= 0);

	//
	// Here function and zero sized (incomplete) array
	// can't be identified simply by code, align, size, and sub type.
	// So for function and incomplete array always allocate new type for them.
	//
	if (code != TYPE_FUNCTION && (code != TYPE_ARRARY || size > 0))
	{
		for (p = type_table[h]; p; p = p->next)
		{
			if (p->type.code == code && 
				p->type.align == align &&
				p->type.size == size &&
				p->type.link == type &&
				(t_symbol*)p->type.symbolic_link == symbol_link) 
			{
				return &p->type;
			}
		}
	}

	CALLOC(p, PERM);
	p->type.code = code;
	p->type.align = align;
	p->type.size = size;
	p->type.symbolic_link = symbol_link;
	p->next = type_table[h];
	type_table[h] = p;
	
	return &p->type;
}


//
// install a specific type to type symbol table
// and return a runtime representation for the type
//
static t_type* install_type_symbol(int code, char*name, int size, int align)
{
	t_symbol* symbol = add_symbol(name, &sym_table_types, GLOBAL, PERM);
	t_type* type= atomic_type(0, code, align, size, symbol);
	symbol->type = type;

	return type;
}


void type_system_initialize()
{
	// [TODO] double check type and alignments for Type
	type_char = install_type_symbol(TYPE_CHAR, atom_string("char"), 1, 4);
	type_unsigned_char = install_type_symbol(TYPE_UNSIGNED_CHAR, atom_string("unsigned char"), 1, 4);
	type_short = install_type_symbol(TYPE_SHORT, atom_string("short"), 2, 4);
	type_unsigned_short = install_type_symbol(TYPE_UNSIGNED_SHORT, atom_string("unsigned short"), 2, 4);
	type_int = install_type_symbol(TYPE_INT, atom_string("int"), 4, 4);
	type_unsigned_int = install_type_symbol(TYPE_UNSIGNED_INT, atom_string("unsigned int"), 4, 4);
	type_long = install_type_symbol(TYPE_LONG, atom_string("long"), 4, 4);
	type_unsigned_long = install_type_symbol(TYPE_UNSIGNED_LONG, atom_string("unsigned long"), 4, 4);
	type_longlong = install_type_symbol(TYPE_LONGLONG, atom_string("long long"), 8, 8);
	type_unsigned_longlong = install_type_symbol(TYPE_UNSIGNED_LONGLONG, atom_string("unsigned long long"), 8, 8);
	type_float = install_type_symbol(TYPE_FLOAT, atom_string("float"), 4, 4);
	type_double = install_type_symbol(TYPE_DOUBLE, atom_string("double"), 4, 4);
	type_longdouble = install_type_symbol(TYPE_LONGDOUBLE, atom_string("long double"), 8, 8);
	type_ptr = install_type_symbol(TYPE_PTR, atom_string("T*"), 4, 4);
	type_void = install_type_symbol(TYPE_VOID, atom_string("void"), 0, 0);
}


void remove_types(int level)
{
	int i = 0;

	for (i; i < __HCC_TYPE_TABLE_HASHSIZE; i ++)
	{
		struct type_entry* current_entry = NULL;
		struct type_entry** bucket = &type_table[i];

		for (;;)
		{
			current_entry = *bucket;
			if (current_entry == NULL)
			{
				break;
			}
			else if (current_entry->type.code == TYPE_FUNCTION ||
				((t_symbol*)current_entry->type.symbolic_link)->scope >= level)
			{
				*bucket = current_entry->next;
			}
		}
	}
}