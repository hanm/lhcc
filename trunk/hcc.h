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

#ifndef __HCC_HCC_HEADER
#define __HCC_HCC_HEADER

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#ifndef NULL
#define NULL (void*)0
#endif

#define NUMBEROFELEMENTS(a) ((int)(sizeof(a)/sizeof(a[0])))

#define HCC_HASH_SIZE 512

#define ROUNDUP(x,n) (((x)+((n)-1))&(~((n)-1)))
#define ROUNDUP_(x,n) ((((x)+((n)-1))/(n))*(n)) // test only - bit mask is faster than div/mul

enum { PERM=0, FUNC, STMT }; 

enum { CONSTANTS=1, LABELS, GLOBAL, PARAM, LOCAL }; 


#ifdef _DEBUG
void llcc_test_symbol_table();
#endif

#endif