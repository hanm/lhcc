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

#ifndef __HCC_AST_H
#define __HCC_AST_H

// sample ast
/*
typedef struct Exp
{
  enum { int_exp, true_exp, false_exp, variable_exp,
         binary_op, unary_op, function_call,
         record_construction, projection } tag;
  union { int                                      integer;
          string                                   variable;
          struct { string           oper;
                   struct Exp*      left;
                   struct Exp*      right; }       binary;
          struct { string           oper;
                   struct Exp*      uexp; }        unary;
          struct { string           name;
                   struct Exp_list* arguments; }   call;
          struct rec { string       attribute;
                       struct Exp*  value;
                       struct rec*  next; }        record;
          struct { struct Exp*  value;
                   string attribute; }             project;
      } op;
} ast;
*/
#endif
