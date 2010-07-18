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

#ifndef __HCC_AST_H
#define __HCC_AST_H

typedef struct hcc_ast_exp t_ast_exp;
typedef struct hcc_ast_stmt t_ast_stmt;

typedef struct hcc_ast_declarator t_ast_declarator;
typedef struct hcc_ast_abstract_declarator t_ast_abstract_declarator;
typedef struct hcc_ast_all_declarator t_ast_all_declarator;
typedef struct hcc_ast_type_name t_ast_type_name;

typedef struct type t_type;

typedef struct hcc_ast_list
{
    void* item;
    struct hcc_ast_list* next;
} t_ast_list;

t_ast_list* make_ast_list_entry();

#define HCC_AST_LIST_APPEND(l, n)   (l)->item = n; \
                (l)->next = make_ast_list_entry(); \
                (l) = (l)->next;

#define HCC_AST_LIST_IS_END(l) ((!(l)->item) && (!(l)->next)) ? 1 : 0

/* WARNING - there might be a compiler out there which checks array bondary
 * which renders this trick useless
 */
typedef struct hcc_ast_array
{
	int size;
	void *items[1];
} t_ast_array;

t_ast_array* make_ast_array(int size, int arena);

#define HCC_AST_ARRAY_GET(a, i) (a)->items[(i)]
#define HCC_AST_ARRAY_LEN(a) ((a) == NULL ? 0 : (a)->size)
#define HCC_AST_ARRAY_SET(a, i, v) { \
        assert((a) && i < (a)->size); \
        (a)->items[i] = (v); \
}


/**************************** Expressions *****************************************/

typedef struct hcc_ast_coordinate
{
	char* file;
	unsigned long line, column;
} t_ast_coord;

typedef enum hcc_ast_expression_kind
{
    AST_EXP_BINARY_KIND,
    AST_EXP_UNARY_KIND,
    AST_EXP_IDENTIFIER_KIND,
    AST_EXP_FUNCTION_CALL_KIND,
    AST_EXP_SUBSCRIPT_KIND,
    AST_EXP_INDIR_KIND, /* indirect access */
    AST_EXP_POSTOP_KIND, /* post increment or post decrement */
    AST_EXP_CAST_KIND, /* type cast */
    AST_EXP_SIZEOF_KIND, /* size of expression */
    AST_EXP_CONDITION_KIND,  /* conditional expression */
    AST_EXP_ASSIGNMENT_KIND,  /* assignment expression */
    AST_EXP_COMMA_KIND, /* comma expression */

    /* constant expression kind */
    AST_EXP_CONST_FLOAT_KIND,
    AST_EXP_CONST_DOUBLE_KIND,
    AST_EXP_CONST_LONG_DOUBLE_KIND,
    AST_EXP_CONST_INTEGER_KIND,
	AST_EXP_CONST_LONG_INTEGER_KIND,
	AST_EXP_CONST_LONG_LONG_KIND,
	AST_EXP_CONST_UNSIGNED_INTEGER_KIND,
	AST_EXP_CONST_UNSIGNED_LONG_INTEGER_KIND,
	AST_EXP_CONST_UNSIGNED_LONG_LONG_KIND,

    /* string literal expression kind */
    AST_EXP_LITERAL_STRING_KIND,
    AST_EXP_LITERAL_STRING_WIDE_KIND

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
    AST_OP_BIT_AND_ASSIGN,
    AST_OP_BIT_OR_ASSIGN,
    AST_OP_BIT_XOR_ASSIGN,

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
	long long ll;
	unsigned long long ull;
	float f;
	double d;
	long double ld;
	void* p;
} t_ast_exp_val;


/*
 * variant record represents C expression ast node
 * the enclosing record represents a generic expression ast node
 * the embeded records represent the specific expression ast node, 
 * which is specified by expression kind enum
*/
typedef struct hcc_ast_exp
{
	t_ast_exp_kind kind;
	t_ast_coord coord;

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
			t_ast_list* args;
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
			t_ast_type_name* type; 
            t_ast_exp* exp;
        } ast_cast_exp;

        struct
        {
            t_ast_exp* exp;
			t_ast_type_name* type; 
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

    t_type* type;
	
	int has_lvalue;
	int no_rvalue;

} t_ast_exp;

t_ast_exp* make_ast_id_exp(char* name);
t_ast_exp* make_ast_const_exp(t_ast_exp_val val, t_ast_exp_kind kind);
t_ast_exp* make_ast_subscript_exp(t_ast_exp* main, t_ast_exp* index);
t_ast_exp* make_ast_call_exp(t_ast_exp* func, t_ast_list* args);
t_ast_exp* make_ast_indir_exp(t_ast_exp* exp, t_ast_exp_op op, char* id);
t_ast_exp* make_ast_postop_exp(t_ast_exp* exp, t_ast_exp_op op);
t_ast_exp* make_ast_cast_exp(t_ast_type_name* type, t_ast_exp* exp);
t_ast_exp* make_ast_sizeof_exp(t_ast_type_name* type, t_ast_exp* exp);
t_ast_exp* make_ast_unary_exp(t_ast_exp* exp, t_ast_exp_op op);
t_ast_exp* make_ast_binary_exp(t_ast_exp* left, t_ast_exp_op op, t_ast_exp* right);
t_ast_exp* make_ast_conditional_exp(t_ast_exp* cond_exp, t_ast_exp* true_exp, t_ast_exp* false_exp);
t_ast_exp* make_ast_assignment_exp(t_ast_exp* cond_exp, t_ast_exp_op op, t_ast_exp* assign_exp);
t_ast_exp* make_ast_comma_exp(t_ast_exp* comma_exp, t_ast_exp* assign_exp);


/**************************** Statements *****************************************/

typedef struct hcc_ast_stmt_list 
{
	t_ast_stmt* current;
	
	struct hcc_ast_stmt_list* next;

} t_ast_stmt_list;


typedef enum hcc_ast_statement_kind
{
    AST_STMT_LABEL_KIND,
	AST_STMT_CASE_KIND,
	AST_STMT_DEFAULT_KIND,
    AST_STMT_COMPOUND_KIND,
    AST_STMT_EXPRESSION_KIND,
    AST_STMT_IF_KIND,
    AST_STMT_SWITCH_KIND,
    AST_STMT_DO_KIND, 
    AST_STMT_FOR_KIND,
    AST_STMT_WHILE_KIND,
    AST_STMT_GOTO_KIND,
    AST_STMT_CONTINUE_KIND, 
    AST_STMT_BREAK_KIND, 
    AST_STMT_RETURN_KIND
} t_ast_stmt_kind;

typedef struct hcc_ast_stmt
{
	t_ast_stmt_kind kind;
	t_ast_coord coord;

	union 
	{
		struct 
		{
			char* label_name;
			t_ast_stmt* stmt;
		} ast_label_stmt;

		struct
		{
            t_ast_list* stmts;
            t_ast_list* declrs;
		} ast_compound_stmt;
		
		struct
		{
			t_ast_exp* exp;
		} ast_expression_stmt;

		struct
		{
			t_ast_exp* test_exp;
			t_ast_stmt* then_stmt;
			t_ast_stmt* else_stmt;
		} ast_if_stmt;

		struct
		{
			t_ast_exp* test_exp;
			t_ast_stmt* stmt;
		} ast_switch_stmt;

		struct
		{
			t_ast_exp* const_exp;
			t_ast_stmt* stmt;
		} ast_case_stmt;

		struct
		{
			t_ast_stmt* stmt;
		} ast_default_stmt;

		struct
		{
			t_ast_stmt* body_stmt;
			t_ast_exp* test_exp;
		} ast_do_stmt;

		struct
		{
			t_ast_exp* test_exp;
			t_ast_stmt* body_stmt;
		} ast_while_stmt;

		struct
		{
			t_ast_stmt* init_exp_stmt;
			t_ast_stmt* test_exp_stmt;
			t_ast_exp* post_test_exp;
			t_ast_stmt* body_stmt;
		} ast_for_stmt;

		struct
		{
			char* label_name;
		} ast_goto_stmt;

		struct
		{
			int i; /* [TODO] fill */
		} ast_continue_stmt;

		struct
		{
			int i; /* [TODO] fill */
		} ast_break_stmt;

		struct
		{
			t_ast_exp* exp;
		} ast_return_stmt;

	} u;

} t_ast_stmt;

t_ast_stmt* make_ast_label_stmt(char* label_name, t_ast_stmt* stmt);
t_ast_stmt* make_ast_expression_stmt(t_ast_exp* exp);
t_ast_stmt* make_ast_if_stmt(t_ast_exp* test_exp, t_ast_stmt* then_stmt, t_ast_stmt* else_stmt);
t_ast_stmt* make_ast_case_stmt(t_ast_exp* const_exp, t_ast_stmt* body_stmt);
t_ast_stmt* make_ast_switch_stmt(t_ast_exp* test_exp, t_ast_stmt* switch_stmt);
t_ast_stmt* make_ast_default_stmt(t_ast_stmt* stmt);
t_ast_stmt* make_ast_do_stmt(t_ast_stmt* body_stmt, t_ast_exp* test_exp);
t_ast_stmt* make_ast_while_stmt(t_ast_exp* test_exp, t_ast_stmt* body_stmt);
t_ast_stmt* make_ast_for_stmt(t_ast_stmt* init_exp_stmt, t_ast_stmt* test_exp_stmt, t_ast_exp* post_test_exp, t_ast_stmt* body_stmt);
t_ast_stmt* make_ast_goto_stmt(char* label_name);
t_ast_stmt* make_ast_continue_stmt();
t_ast_stmt* make_ast_break_stmt();
t_ast_stmt* make_ast_return_stmt(t_ast_exp* return_exp);
t_ast_stmt* make_ast_compound_stmt(t_ast_list* stmts, t_ast_list* declrs);


/******************* Declarations *****************/

typedef struct hcc_ast_enumerator
{
	t_ast_coord coord;
	char* id;
	int value;
	t_ast_exp* exp;
} t_ast_enumerator;

typedef struct hcc_ast_enum_specifier
{
    t_ast_coord coord;
	char* id;
	t_ast_list* enumerator_list;

    int scope;
} t_ast_enum_specifier;

typedef struct hcc_ast_typedef
{
    t_ast_coord coord;
    char* name;
    void* symbol; /* ptr to symbol table entry */
} t_ast_typedef;

typedef struct hcc_ast_struct_or_union_specifier
{
    t_ast_coord coord;
    int is_struct; /* set to 1 for struct, 0 for union */
    char* name;
    t_ast_list* struct_declr_list;

	int scope;
} t_ast_struct_or_union_specifier;

typedef enum 
{
    AST_NTYPE_VOID,
    AST_NTYPE_CHAR,
    AST_NTYPE_INT,
    AST_NTYPE_FLOAT,
    AST_NTYPE_DOUBLE,
    AST_NTYPE_SHORT,
    AST_NTYPE_LONG,
    AST_NTYPE_SIGNED,
    AST_NTYPE_UNSIGNED,
	AST_NTYPE_INT64 /* WARNING - NON STD USED */
} t_ast_native_type_kind;

typedef enum
{
    AST_TYPE_SPECIFIER_NATIVE,
    AST_TYPE_SPECIFIER_STRUCT_OR_UNION,
    AST_TYPE_SPECIFIER_ENUM,
    AST_TYPE_SPECIFIER_TYPEDEF
} t_ast_type_specifier_kind;

typedef struct hcc_ast_type_specifier
{
    t_ast_coord coord;
    t_ast_type_specifier_kind kind;

    union 
    {
        t_ast_native_type_kind native_type;
        t_ast_struct_or_union_specifier* struct_union_specifier;
        t_ast_enum_specifier* enum_specifier;
        char* type_def;
    } u;
} t_ast_type_specifier;

typedef enum
{
	AST_TYPE_CONST,
	AST_TYPE_VOLATILE
} t_ast_type_qualifier_kind;

typedef struct hcc_ast_type_qualifier
{
	t_ast_coord coord;
	t_ast_type_qualifier_kind kind;
} t_ast_type_qualifier;

typedef enum
{
	AST_STORAGE_TYPEDEF,
	AST_STORAGE_EXTERN,
	AST_STORAGE_STATIC,
	AST_STORAGE_AUTO,
	AST_STORAGE_REGISTER,
    AST_STORAGE_NA /* sentinel */
} t_ast_storage_specifier_kind;

typedef struct hcc_ast_storage_specifier
{
	t_ast_coord coord;
	t_ast_storage_specifier_kind kind;
} t_ast_storage_specifier;

typedef struct hcc_ast_declr_specifier
{
	t_ast_coord coord;
    t_ast_storage_specifier* storage_specifier;
    t_ast_list* type_qualifier_list;
	t_ast_list* type_specifier_list;

	int storage_class;
	
	struct type* type;
} t_ast_declaration_specifier;

typedef struct hcc_ast_pointer
{
	t_ast_coord coord;
	t_ast_list* type_qualifier_list;
	struct hcc_ast_pointer* pointer;
} t_ast_pointer;

typedef struct hcc_ast_param_type_list
{
	t_ast_coord coord;
	t_ast_list* parameter_list;
	int has_ellipsis;
} t_ast_param_type_list;

typedef enum
{
    AST_SUFFIX_DECLR_SUBSCRIPT,
    AST_SUFFIX_DECLR_PARAMETER
} t_ast_suffix_declarator_kind;

typedef struct hcc_ast_suffix_declarator
{
	t_ast_coord coord;
    t_ast_suffix_declarator_kind kind;

    union 
    {
        struct 
        {
            t_ast_exp* const_exp;
        } subscript;

        struct
        {
            t_ast_param_type_list* param_type_list;
            t_ast_list* identifier_list;
        } parameter;
    } u;

} t_ast_suffix_declarator;


typedef struct hcc_ast_direct_declarator
{
    t_ast_coord coord;

    char* id;
    t_ast_declarator* declarator;
} t_ast_direct_declarator;

typedef struct hcc_ast_declarator
{
    t_ast_coord coord;

    t_ast_direct_declarator* direct_declarator;
    t_ast_pointer* pointer;
    t_ast_list* suffix_delcr_list;
} t_ast_declarator;

typedef struct hcc_ast_direct_abstract_declarator
{
    t_ast_coord coord;
    
    t_ast_abstract_declarator* abstract_declr;
    t_ast_suffix_declarator* suffix_declr;
} t_ast_direct_abstract_declarator;

typedef struct hcc_ast_abstract_declarator
{
    t_ast_coord coord;

    t_ast_pointer* pointer;
    t_ast_direct_abstract_declarator* direct_abstract_declarator;
    t_ast_list* suffix_list;
} t_ast_abstract_declarator;

typedef struct hcc_ast_struct_declarator
{
    t_ast_coord coord;

    t_ast_declarator* declarator;
    t_ast_exp* const_exp;
} t_ast_struct_declarator;

typedef struct hcc_ast_struct_declaration
{
	t_ast_coord coord;

	t_ast_list* specifier_qualifier_list;
	t_ast_list* struct_declarator_list;
} t_ast_struct_declaration;

typedef struct hcc_ast_type_name
{
    t_ast_coord coord;

    t_ast_list* specifier_qualifier_list;
    t_ast_abstract_declarator* abstract_declarator;
} t_ast_type_name;

typedef struct hcc_ast_initializer
{
    t_ast_coord coord;

    t_ast_exp* assign_exp;
    t_ast_list* initializer_list;
    int comma_ending;
} t_ast_initializer;

typedef struct hcc_ast_parameter_declaration
{
    t_ast_coord coord;

    t_ast_declaration_specifier* declr_specifiers;
    t_ast_direct_declarator* dir_declr;
    t_ast_direct_abstract_declarator* dir_abstract_declr;
	t_ast_all_declarator* all_declr;
    t_ast_pointer* ptr;
    t_ast_list* suffix_declr_list;
} t_ast_parameter_declaration;

typedef struct hcc_ast_init_declarator
{
    t_ast_coord coord;

    t_ast_declarator* declarator;
    t_ast_initializer* initializer;
} t_ast_init_declarator;

typedef struct hcc_ast_declaration
{
    t_ast_coord coord;

    t_ast_declaration_specifier* declr_specifiers;
    t_ast_list* init_declr_list;
} t_ast_declaration;

typedef struct hcc_ast_all_declarator
{
	t_ast_coord coord;
	
	t_ast_pointer* pointer;
	char* id;
	t_ast_all_declarator* all_declr;
	t_ast_list* suffix_declr_list;
} t_ast_all_declarator;

typedef struct hcc_ast_function_definition
{
	t_ast_coord coord;

	t_ast_declaration_specifier* declr_specifier;
	t_ast_declarator* declarator;
	t_ast_list* declr_list;
	t_ast_stmt* compound_stmt;
} t_ast_function_definition;

typedef struct hcc_ast_external_declaration
{
	t_ast_coord coord;

	int fun_def;
	union
	{
		t_ast_function_definition* func_def;
		t_ast_declaration* declr;
	}u;
} t_ast_external_declaration;

typedef struct hcc_ast_translation_unit
{
    t_ast_coord coord;

    t_ast_list *ext_declaration_list;
} t_ast_translation_unit;


t_ast_enumerator* make_ast_enumerator(char*id, t_ast_exp* exp);
t_ast_enum_specifier* make_ast_enum_specifier(char* id, t_ast_list* enumerator_list);
t_ast_typedef* make_ast_typedef(char*id, void* symbol);
t_ast_struct_or_union_specifier* make_ast_struct_union_specifier(int is_struct, char* name, t_ast_list* struct_declr_list);
t_ast_type_specifier* make_ast_type_specifier_template(); 
t_ast_type_specifier* make_ast_type_specifier_native_type(t_ast_native_type_kind native_type);
t_ast_type_specifier* make_ast_type_specifier_struct_union(t_ast_struct_or_union_specifier* specifier);
t_ast_type_specifier* make_ast_type_specifier_enum(t_ast_enum_specifier* specifier);
t_ast_type_specifier* make_ast_type_specifier_typedef(char* id);
t_ast_type_qualifier* make_ast_type_qualifer(t_ast_type_qualifier_kind kind);
t_ast_storage_specifier* make_ast_storage_specifier(t_ast_storage_specifier_kind kind);
t_ast_declaration_specifier* make_ast_declaration_specifier();

t_ast_pointer* make_ast_pointer(t_ast_list* list, t_ast_pointer* pointer);

t_ast_suffix_declarator* make_ast_subscript_declarator(t_ast_exp* exp); 
t_ast_suffix_declarator* make_ast_parameter_list_declarator(t_ast_param_type_list* param_type_list, t_ast_list* id_list);
t_ast_direct_declarator* make_ast_direct_declarator(char* id, t_ast_declarator* declarator);
t_ast_declarator* make_ast_declarator(t_ast_pointer* pointer, t_ast_direct_declarator* direct_declarator, t_ast_list* list);
t_ast_direct_abstract_declarator* make_ast_direct_abstract_declarator(t_ast_suffix_declarator* suffix_declr, t_ast_abstract_declarator* abstract_declr);
t_ast_abstract_declarator* make_ast_abstract_declarator(t_ast_pointer* pointer, t_ast_direct_abstract_declarator* direct_abstract_declarator, t_ast_list* suffix_list);
t_ast_struct_declarator* make_ast_struct_declarator(t_ast_declarator* declarator, t_ast_exp* const_exp);
t_ast_struct_declaration* make_ast_struct_declaration(t_ast_list* specifier_qualifier_list, t_ast_list* struct_declr_list);

t_ast_type_name* make_ast_type_name(t_ast_list* list, t_ast_abstract_declarator* abstract_declr);
t_ast_initializer* make_ast_initializer(t_ast_exp* assign_exp, t_ast_list* initializer_list, int comma_ending);
t_ast_parameter_declaration* make_ast_parameter_declaration(t_ast_declaration_specifier* specifier, t_ast_direct_declarator* dir_declr, t_ast_direct_abstract_declarator* dir_abstract_declr, t_ast_all_declarator* all_declr, t_ast_pointer* ptr, t_ast_list* suffix_declr_list);
t_ast_init_declarator* make_ast_init_declarator(t_ast_declarator* declarator, t_ast_initializer* initializer);
t_ast_declaration* make_ast_declaration(t_ast_declaration_specifier* declr_specifier, t_ast_list* init_declr_list);
t_ast_all_declarator* make_ast_all_declarator(t_ast_pointer* ptr, char* id, t_ast_all_declarator* all_declr, t_ast_list* suffix_declr_list);

t_ast_param_type_list* make_ast_parameter_type_list(t_ast_list* list, int ellipsis);

t_ast_function_definition* make_ast_function_definition(t_ast_declaration_specifier* declr_specifier,
	t_ast_declarator* declarator,
	t_ast_list* declr_list,
	t_ast_stmt* compound_stmt);

t_ast_external_declaration* make_ast_external_declaration(t_ast_function_definition* func_def, t_ast_declaration* declar);

t_ast_translation_unit* make_ast_translation_unit(t_ast_list *ext_declaration_list);

#endif
