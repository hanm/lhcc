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

#include "arena.h"
#include "hcc.h"

struct block {
	struct block *next;
	char *limit;
	char *avail;
};

union align {
	int i;
	void* vp;
	long l;
	long* lp;
	char *p;
	float t;
	double d;
	long double ld;
	int (*f)(void);
};

union header {
	struct block b;
	union align a;
};

static struct block
	 first[] = {  { NULL },  { NULL },  { NULL } },
	*arena[] = { &first[0], &first[1], &first[2] };

static struct block *freeblocks;

void* hcc_alloc(unsigned long n, unsigned a)
{
    struct block *ap;

    assert(a < NUMBEROFELEMENTS(arena));
	assert(n > 0);

	ap = arena[a];
	n = ROUNDUP(n, sizeof (union align));

	while (n > (unsigned)(ap->limit - ap->avail)) 
    {
		/* [IMPROVE] - this could be optimized - the orignial impl in the book "C interfaces and implementations"
		 * will never stop remove free blocks from the list and put them
		 * into the arena if the free block it checks is not big enough for this allocation.
		 * as a result these removd free blocks are wasted and never gonna be used.
		 */
		if ((ap->next = freeblocks) != NULL) 
        {
			freeblocks = freeblocks->next;
			ap = ap->next;
		} 
        else
        {
            unsigned m = sizeof (union header) + n + ROUNDUP(10*1024, sizeof (union align));
            ap->next = malloc(m);
			memset(ap->next, 0, m);
            ap = ap->next;
            if (ap == NULL) 
            {
                exit(1);
            }
            ap->limit = (char *)ap + m;
        }

		ap->avail = (char *)((union header *)ap + 1);
		ap->next = NULL;
		arena[a] = ap;
	}
	ap->avail += n;
	return ap->avail - n;
}

void hcc_free_arena(unsigned a)
{
    assert(a < NUMBEROFELEMENTS(arena));
	arena[a]->next = freeblocks;
	freeblocks = first[a].next;
	first[a].next = NULL;
	arena[a] = &first[a];
}

void hcc_deallocate_all()
{
    int arenas = NUMBEROFELEMENTS(arena);
    int n = 0;
    struct block* cblock = NULL;
    struct block* nblock = NULL;

    for (n; n < arenas; n ++)
    {
		cblock = &first[n];

        while (cblock != NULL)
        {
            nblock = cblock->next;
            if (cblock->limit != NULL)
            {
                free(cblock);
                cblock = NULL;
            }
            cblock = nblock;
        }

        arena[n] = &first[n];
    }

    while (freeblocks != NULL)
    {
        nblock = freeblocks->next;
        free(freeblocks);
        freeblocks = nblock;
    }
}