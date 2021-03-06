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

#include "ast.h"

void static_semantic_check(t_ast_translation_unit* translation_unit);

t_ast_exp* ssc_expression(t_ast_exp* exp);
t_ast_exp* ssc_const_expression(t_ast_exp* exp);

t_ast_stmt* ssc_compound_stmt(t_ast_stmt* stmt);
t_ast_stmt* ssc_stmt(t_ast_stmt* stmt);

/*
implicit conversions between lvalue and rvalues and between array/function and pointer types

-	Convert between lvalue and rvalue :
	When an lvalue appears in a context that requires an rvalue, 
	the lvalue is implicitly converted to an rvalue. 
	The reverse, however, is not true: an rvalue cannot be converted to an lvalue. 
	Rvalues always have complete types or the void type.

-	Convert function and array types into pointer type
    per standard, when operands are function and/or array types, the type should be converted
	to a pointer type
*/
t_ast_exp* ssc_implicit_conversion(t_ast_exp* exp, int rvalue);


