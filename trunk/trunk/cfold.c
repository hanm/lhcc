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
#include "ssc.h"

#define CONST_EXPRESSION(exp) ((exp->kind >= AST_EXP_CONST_FLOAT_KIND \
    && exp->kind <= AST_EXP_CONST_UNSIGNED_LONG_LONG_KIND) == 1)

static t_ast_exp* unary_expression_folding(t_ast_exp* exp)
{
    t_ast_exp* e = ssc_expression(exp->u.ast_unary_exp.exp);

    assert(exp && exp->kind == AST_EXP_UNARY_KIND);

    if (CONST_EXPRESSION(e))
    {
        switch (exp->u.ast_unary_exp.op)
        {
        case AST_OP_ADDR : /* & */
            {
                assert(0);
            }
        case AST_OP_DEREF : /* * */
            {
                assert(0);
            }
        case AST_OP_POS : /* + */ 
            {
                /* nothing to do */
            }
        case AST_OP_NEGATE : /* - */ 
            {
                printf("%s%d\n", "type", e->type->code);

                if (e->type == type_int)
                {
                    e->u.ast_const_exp.val.i = - e->u.ast_const_exp.val.i;
                }
                else if (e->type == type_long)
                {
                    e->u.ast_const_exp.val.l = - e->u.ast_const_exp.val.l;
                }
                else if (e->type == type_float)
                {
                    e->u.ast_const_exp.val.f = - e->u.ast_const_exp.val.f;
                }
                else if (e->type == type_double)
                {
                    e->u.ast_const_exp.val.d = - e->u.ast_const_exp.val.d;
                }
                
                /* FIXME - more type checks and unsigned check */ 
            }
        case AST_OP_INVERT : /* ~ */
            {
                printf("%s%d\n", "type", e->type->code);
                e->u.ast_const_exp.val.i = ~ e->u.ast_const_exp.val.i; /* [FIXME] precondition check, must be integer here */
            }
        case AST_OP_NOT : /* ! */
            {
                printf("%s%d\n", "type", e->type->code);
                e->u.ast_const_exp.val.i = ! e->u.ast_const_exp.val.i; /* [TODO] type selection and fields mapping here. */
            }
        default:
            break;
        }
    }

    return e;
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

    return exp;
}




