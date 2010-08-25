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

#include "hcc.h"
#include "type.h"
#include "transform.h"

#define CONST_EXPRESSION(exp) ((exp->kind >= AST_EXP_CONST_FLOAT_KIND \
    && exp->kind <= AST_EXP_CONST_UNSIGNED_LONG_LONG_KIND) == 1)

static t_ast_exp* unary_expression_folding(t_ast_exp* exp)
{
    assert(exp && exp->kind == AST_EXP_UNARY_KIND);

    if (CONST_EXPRESSION(exp->u.ast_unary_exp.exp))
    {
        switch (exp->u.ast_unary_exp.op)
        {
        case AST_OP_ADDR : /* & */
            {
                
            }
        case AST_OP_DEREF : /* * */
            {
                
            }
        case AST_OP_POS : /* + */ 
            {

            }
        case AST_OP_NEGATE : /* - */ 
            {

            }
        case AST_OP_INVERT : /* ~ */
            {

            }
        case AST_OP_NOT : /* ! */
            {

            }
        default:
            break;
        }
    }

    return exp;
}

t_ast_exp* const_folding(t_ast_exp* exp)
{
    assert(exp);

    switch (exp->kind)
    {
    case AST_EXP_UNARY_KIND:
        {
            return unary_expression_folding(exp);
        }
    default:
        break;
    }

    return 0;
}




