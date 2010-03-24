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

#ifndef __HCC_HCC_HEADER
#define __HCC_HCC_HEADER

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <assert.h>

#ifndef NULL
#define NULL (void*)0
#endif

#define NUMBEROFELEMENTS(a) ((int)(sizeof(a)/sizeof(a[0])))

#define HCC_HASH_SIZE 512

#define ROUNDUP(x,n) (((x)+((n)-1))&(~((n)-1)))
#define ROUNDUP_(x,n) ((((x)+((n)-1))/(n))*(n)) // test only - bit mask is faster than div/mul

//
// type size configurations
//
#define HCC_CHAR_SIZE 1
#define HCC_SHORT_SIZE 2
#define HCC_INT_SIZE 4
#define HCC_LONG_SIZE 4
#define HCC_LONGLONG_SIZE 8
#define HCC_FLOAT_SIZE 4
#define HCC_DOUBLE_SIZE 8
#define HCC_LONG_DOUBLE_SIZE 8
#define HCC_PTR_SIZE 4

//
// arena types 
// PERM - life longs most from hcc starts to hcc ends
// FUNC - life starts from entering function scope and ends when exits function
// STMT - life starts from entering statement block and ends when exists statement block
//
enum { PERM=0, FUNC, STMT }; 

//
// scopes
//
enum { CONSTANTS=1, LABELS, GLOBAL, PARAM, LOCAL }; 

//
// storage classes
//
enum { STORAGE_AUTO=0, STORAGE_REGISTER, STORAGE_STATIC, STORAGE_EXTERN};


#ifdef _DEBUG
void llcc_test_symbol_table();
#endif

typedef struct coordinate
{
    char* filename;
    int line;
    int column;
} t_coordinate;


#endif