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

#define __HCC_TYPE_TABLE_HASHSIZE 512

//
// table to manage types - identical type should share a single instance
//
static struct entry
{
	t_type type;
	struct entry* next;
}* type_table[__HCC_TYPE_TABLE_HASHSIZE];

//
static t_type* atomic_type(t_type* type, int code, int align, int size)
{
	struct entry* p;
	unsigned long h = (code^((unsigned long)type>>3))& __HCC_TYPE_TABLE_HASHSIZE;

	HCC_ASSERT(code >= 0 && align >= 0 && size >= 0);

	//
	// Here function and zero sized (incomplete) array
	// can't be identified simply by code, align, size, and sub type.
	// So for function and incomplete array always allocate new type for them.
	//
	// [TODO] - how about enum, struct and union?? LCC also checks the symbol table..
	//
	if (code != TYPE_FUNCTION && (code != TYPE_ARRARY || size > 0))
	{
		for (p = type_table[h]; p; p = p->next)
		{
			if (p->type.code == code && 
				p->type.align == align &&
				p->type.size == size &&
				p->type.link == type) 
			{
				return &p->type;
			}
		}
	}

	CALLOC(p, PERM);
	p->type.code = code;
	p->type.align = align;
	p->type.size = size;
	p->next = type_table[h];
	type_table[h] = p;
	
	return &p->type;
}

