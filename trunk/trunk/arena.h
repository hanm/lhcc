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

#ifndef __HCC_ALLOC_H
#define __HCC_ALLOC_H

/* allocate a memory block with length n bytes in arena a */
void* hcc_alloc(unsigned long n, unsigned a);

/* free a specific arena indexed by a by putting all memory blocks in the arena to free block list 
 * so these blocks can be reused by other arena or the same arena later.
 * note it doesn't really free all memory allocated for the arena.
 */
void hcc_free_arena(unsigned a);

/* free all arenas and deallocate memory allocated for arenas. */
void hcc_deallocate_all();

/*
 * allocate object p in arena a
 */
#define ALLOC(p,a) ((p) = hcc_alloc(sizeof *(p), (a)))

/*
 * allocate object p and initialize it to zero in arena a
 */
#define CALLOC(p,a) memset(ALLOC((p),(a)), 0, sizeof *(p))

#endif