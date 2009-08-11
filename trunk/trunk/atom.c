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

#include "atom.h"
#include "arena.h"
#include "Hcc.h"
#include <assert.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

/**
 * atom table
 *
 * string is hashed and stored in buckets. a string will never be removed from the atom table.
 *
 **/
static struct atom 
{
	struct atom *link;
	int length;
	char *string;
} *buckets[4096];

const char* atom_string(const char* string)
{
	assert(string);

	// warning - convert size_t to int [tag]
	return atom_string_number(string, (int)strlen(string));
}

const char* atom_string_number(const char* string, int length)
{
	//
	// djb2 hash - [tag] research
	//
	unsigned long hash = 5381;
	int i, c;
	int k = 0;
	struct atom *p;

	assert(string);
	assert(length >= 0);

	while (c = string[k++])
	{
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}

	hash &= NUMBEROFELEMENTS(buckets) - 1;

	for (p = buckets[hash]; p; p = p->link)
	{
		if (length == p->length) 
		{
			for (i = 0; i < length && (p->string[i] == string[i]);)
			{
				i++;
			}

			if (i == length)
			{
				return p->string;
			}
		}
	}

	// allocate string into arena PERM - life time persist till compiler dies.
    p = hcc_alloc(sizeof(*p) + length + 1, PERM);
	p->length = length;
	p->string = (char *)(p + 1);

	if (length > 0)
	{
		memcpy(p->string, string, length);
	}

	p->string[length] = '\0';
	p->link = buckets[hash];
	buckets[hash] = p;

	return p->string;
}

const char* atom_int(int n)
{
	char string[43];
	char *s = string + sizeof(string);
	unsigned long m;

	if (n == LONG_MIN)
	{
		m = LONG_MAX + 1UL;
	}
	else if (n < 0)
	{
		m = -n;
	}
	else
	{
		m = n;
	}

	do
	{
		*--s = m%10 + '0';
	}
	while ((m /= 10) > 0);

	if (n < 0)
	{
		*--s = '-';
	}

	// [tag] VC warning convert from _w64 int to int possible lose of data
	return atom_string_number(s, (int)((string + sizeof(string)) - s));
}