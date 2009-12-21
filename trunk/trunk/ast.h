/***************************************************************

Copyright (c) 2008-2009 Michael Liang Han

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

/*
 * the overall structure of the ast nodes are pretty like what described in the Tiger book
 * and/or specified by ASDL.
*/
typedef struct hcc_ast_exp t_ast_exp;

/* kinds of expressions */
typedef enum hcc_ast_expression_kind
{
    AST_EXP_BINARY_KIND,
    AST_EXP_UNARY_KIND,
    AST_EXP_IDENTIFIER_KIND,
	AST_EXP_CONST_KIND,
    AST_EXP_FUNCTION_CALL_KIND,
    AST_EXP_SUBSCRIPT_KIND,
    AST_EXP_INDIR_KIND, /* indirect access */
    AST_EXP_POSTOP_KIND, /* post increment or post decrement */
    AST_EXP_CAST_KIND, /* type cast */
    AST_EXP_SIZEOF_KIND, /* size of expression */
    AST_EXP_CONDITION_KIND,  /* conditional expression */
    AST_EXP_ASSIGNMENT_KIND,  /* assignment expression */
    AST_EXP_COMMA_KIND /* comma expression */
} t_ast_exp_kind;


typedef enum hcc_ast_operator
{
	AST_OP_NONE,

	/* unary operators
	 * &, *, +, -, ~, !
	*/
	AST_OP_ADDR,
	AST_OP_DEREF,
	AST_OP_POS,
	AST_OP_NEGATE,
	AST_OP_INVERT,
	AST_OP_NOT,
	
	/* binary arithematic operators */
	AST_OP_ADD,
	AST_OP_SUB,
	AST_OP_MUL,
	AST_OP_DIV,
	AST_OP_MOD,
	AST_OP_BIT_AND,
	AST_OP_BIT_OR,
	AST_OP_BIT_XOR,
	AST_OP_LSHIFT,
	AST_OP_RSHIFT,

	/* relational operators */
	AST_OP_LESS,
	AST_OP_LESS_EQ,
	AST_OP_GREAT,
	AST_OP_GREAT_EQ,
	AST_OP_EQUAL,
	AST_OP_UNEQUAL,

	AST_OP_AND,
	AST_OP_OR,

    /* assignment */
    AST_OP_ASSIGN,
    AST_OP_MUL_ASSIGN,
    AST_OP_DIV_ASSIGN,
    AST_OP_MOD_ASSIGN,
    AST_OP_ADD_ASSIGN,
    AST_OP_SUB_ASSIGN,
    AST_OP_LSHIFT_ASSIGN,
    AST_OP_RSHIFT_ASSIGN,
    AST_OP_AND_ASSIGN,
    AST_OP_OR_ASSIGN,
    AST_OP_XOR_ASSIGN,

	/* misc : '->', '.', '++', '--'*/
	AST_OP_PTR,
	AST_OP_DOT,
	AST_OP_INC,
	AST_OP_DEC

} t_ast_exp_op;

typedef union hcc_ast_exp_value
{
	char sc;
	unsigned char uc;
	short ss;
	unsigned short us;
	int i;
	unsigned int ui;
	long l;
	unsigned long ul;
	float f;
	double d;
	long double ld;
	void* p;
} t_ast_exp_val;

typedef struct hcc_ast_list
{
	int size;
	void* item[1];
} t_ast_list;

/*
 * variant record represents C expression ast node
 * the enclosing record represents a generic expression ast node
 * the embeded records represent the specific expression ast node, 
 * which is specified by expression kind enum
*/
typedef struct hcc_ast_exp
{
	t_ast_exp_kind kind;

	union 
	{
		struct 
		{
			t_ast_exp_op op;
			t_ast_exp* exp;
		} ast_unary_exp;

		struct
		{
			t_ast_exp_op op;
			t_ast_exp* left;
			t_ast_exp* right;
		} ast_binary_exp;

		struct
		{
			char* name;
		} ast_id_exp; /* identifier */

		struct
		{
			t_ast_exp_val val;
		} ast_const_exp;

		struct
		{
			t_ast_exp* main;
			t_ast_exp* index;
		} ast_subscript_exp;

		struct
		{
			t_ast_exp* func;
			t_ast_list args;
		} ast_call_exp;

		struct
		{
			t_ast_exp* exp;
			t_ast_exp_op op;
			char* id;
		} ast_indir_exp; /* indirect access via post fix: dot and arrow*/

        struct
        {
            t_ast_exp* exp;
            t_ast_exp_op op;
        } ast_postop_exp; 

        struct
        {
            t_ast_exp* type; /* [FIX ME] this needs to be type declaration or such. 
                                      * type name can't be expressed as an expression.
                                      */
            t_ast_exp* exp;
        } ast_cast_exp;

        struct
        {
            t_ast_exp* exp;
            t_ast_exp* type; /* [FIX ME] Same as above */
        } ast_sizeof_exp;

        struct
        {
            /* cond_exp 
             * cond_exp ? true_exp : false_exp
             */
            t_ast_exp* cond_exp; 
            t_ast_exp* true_exp;
            t_ast_exp* false_exp;
        } ast_conditional_exp;

        struct
        {
            t_ast_exp* cond_exp;
            t_ast_exp_op op;
            t_ast_exp* assign_exp;
        } ast_assignment_exp;

        struct
        {
            t_ast_exp* comma_exp;
            t_ast_exp* assign_exp;
        } ast_comma_exp;

	} u;

} t_ast_exp;


/* ast constructors */
t_ast_exp* make_ast_id_exp(char* name);
t_ast_exp* make_ast_const_exp(t_ast_exp_val val);
t_ast_exp* make_ast_subscript_exp(t_ast_exp* main, t_ast_exp* index);
t_ast_exp* make_ast_call_exp(t_ast_exp* func, t_ast_list args);
t_ast_exp* make_ast_indir_exp(t_ast_exp* exp, t_ast_exp_op op, char* id);
t_ast_exp* make_ast_postop_exp(t_ast_exp* exp, t_ast_exp_op op);
t_ast_exp* make_ast_cast_exp(t_ast_exp* type, t_ast_exp* exp);
t_ast_exp* make_ast_sizeof_exp(t_ast_exp* type, t_ast_exp* exp);
t_ast_exp* make_ast_unary_exp(t_ast_exp* exp, t_ast_exp_op op);
t_ast_exp* make_ast_binary_exp(t_ast_exp* left, t_ast_exp_op op, t_ast_exp* right);
t_ast_exp* make_ast_conditional_exp(t_ast_exp* cond_exp, t_ast_exp* true_exp, t_ast_exp* false_exp);
t_ast_exp* make_ast_assignment_exp(t_ast_exp* cond_exp, t_ast_exp_op op, t_ast_exp* assign_exp);
t_ast_exp* make_ast_comma_exp(t_ast_exp* comma_exp, t_ast_exp* assign_exp);



#endif
