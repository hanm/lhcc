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

#include <stdio.h>
#include <limits.h>

volatile int char_min = CHAR_MIN;

/*
 * print C type length information on host compiling platform
 * http://jk-technology.com/c/inttypes.html
*/
void hcc_utility_print_type_info()
{
    /*
    // this is only C99 applicable

    printf("Size of Boolean type is %d byte(s)\n\n",
        (int)sizeof(_Bool));
    */

    printf("Number of bits in a character: %d\n",
        CHAR_BIT);
    printf("Size of character types is %d byte\n",
        (int)sizeof(char));
    printf("Signed char min: %d max: %d\n",
        SCHAR_MIN, SCHAR_MAX);
    printf("Unsigned char min: 0 max: %u\n",
        (unsigned int)UCHAR_MAX);

    printf("Default char is ");
    if (char_min < 0)
        printf("signed\n\n");
    else if (char_min == 0)
        printf("unsigned\n\n");
    else
        printf("non-standard\n\n");

    printf("Size of short int types is %d bytes\n",
        (int)sizeof(short));
    printf("Signed short min: %d max: %d\n",
        SHRT_MIN, SHRT_MAX);
    printf("Unsigned short min: 0 max: %u\n\n",
        (unsigned int)USHRT_MAX);

    printf("Size of int types is %d bytes\n",
        (int)sizeof(int));
    printf("Signed int min: %d max: %d\n",
        INT_MIN, INT_MAX);
    printf("Unsigned int min: 0 max: %u\n\n",
        (unsigned int)UINT_MAX);

    printf("Size of long int types is %d bytes\n",
        (int)sizeof(long));
    printf("Signed long min: %ld max: %ld\n",
        LONG_MIN, LONG_MAX);
    printf("Unsigned long min: 0 max: %lu\n\n",
        ULONG_MAX);

    printf("Size of long long types is %d bytes\n",
        (int)sizeof(long long));
    printf("Signed long long min: %lld max: %lld\n",
        LLONG_MIN, LLONG_MAX);
    printf("Unsigned long long min: 0 max: %llu\n",
        ULLONG_MAX);
}