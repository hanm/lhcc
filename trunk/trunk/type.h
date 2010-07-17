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

#ifndef __HCC_TYPE_H
#define __HCC_TYPE_H

/***********************************************
 * ANSI C build in types and type qualifiers
 ************************************************/
typedef enum
{
	TYPE_CHAR, /* char */
    TYPE_SIGNED_CHAR, /* signed char */
	TYPE_UNSIGNED_CHAR, /* unsigned char */
	TYPE_SHORT, /* short, signed short, short int, signed short int */
	TYPE_UNSIGNED_SHORT, /* unsigned short, unsigned short int */
	TYPE_INT, /* int, signed int, or no type specifiers */
	TYPE_UNSIGNED_INT, /* unsigned, unsigned int */
	TYPE_LONG, /* long, signed long, long int, signed long int */
	TYPE_UNSIGNED_LONG, /* unsigned long, unsigned long int */
	TYPE_LONGLONG, /* [TODO] - WARNING : long long is not ANSI C type */
	TYPE_UNSIGNED_LONGLONG, /* AS ABOVE */
	TYPE_ENUM, /* enum */
	TYPE_FLOAT, /* float */
	TYPE_DOUBLE, /* double */
	TYPE_LONGDOUBLE, /* long double */
	TYPE_PTR, /* pointer */
	
	/*scalar type ends */
	
	TYPE_VOID, /* void */
	TYPE_STRUCT, /* struct */
	TYPE_UNION, /* union */
	TYPE_FUNCTION, /* function */
	TYPE_ARRARY, /* array */
	
	/* type qualifiers */
	TYPE_CONST, /* constant */
	TYPE_VOLATILE, /* volatile */
	TYPE_RESTRICT, /* restrict (C99) */

	/* extended types [FIX ME] */
	TYPE_INT64, /* int 64 */
    TYPE_UNSIGNED_INT64
} t_type_kind;

typedef struct type
{
	int code; /* type code */
	int align; /* type alignment */
	int size; /* type size */

	/* link to other types - for example unsigned long 
	 * where unsigned in type and long is type->link
	 * this allows types be expanded linearly
	 */
	struct type* link; 

	/*
	 * link to symbol information of the type
	 * sometimes type needs to access symbol table for information 
	 * like scope level
	 */
	void* symbolic_link;

	/*
	 * user defined types
	 */
	union  
	{
		struct tag_trait* tag;
		struct func_trait* function;
	} u;

} t_type;

/*
 * members in struct / union
 * it's a linked list
 */
typedef struct field_type
{
	char* name; /* field name */
	int offset; /* field offset relative to start of the record */
	int bits; /* number of bits if the field is a bit field; otherwise 0 by default */
	t_type* type; /* field type */
	struct field_type* next; /* link to next field in the same record */
} t_record_field;

/*
 * struct / union / enum tag traits
 * determined by tag name only
 */
typedef struct tag_trait
{
	char* tag; 
	t_record_field* fields; 
} t_tag;

/*
 * function parameter type
 * also a linked list
 */
typedef struct paremeter_type
{
	char* name;
	t_type* type;
	int reg_qualified;

	struct parameter_type* next;
} t_param;

/*
 * function trait is decided by its parameter and return value
 */
typedef struct func_trait
{
	int prototype;
	int ellipse;

	t_param* parameter;
} t_function;

/*
 * ANSI C Defined Types
 */
extern t_type* type_char;
extern t_type* type_signed_char;
extern t_type* type_unsigned_char;
extern t_type* type_short;
extern t_type* type_unsigned_short;
extern t_type* type_int;
extern t_type* type_unsigned_int;
extern t_type* type_long;
extern t_type* type_unsigned_long;
extern t_type* type_longlong;
extern t_type* type_unsigned_longlong;
extern t_type* type_int64; /* non std extension */
extern t_type* type_unsigned_int64; /* non std extension */
extern t_type* type_float;
extern t_type* type_double;
extern t_type* type_longdouble;
extern t_type* type_ptr;
extern t_type* type_void;


t_type* remove_type_qualifier(t_type* type);

#define IS_TYPE_QUALIFIERS(t) ((t) == TYPE_CONST \
	|| (t) == TYPE_VOLATILE \
	|| (t) == TYPE_RESTRICT)

#define QUALIFIED_TYPE(t) ((t)->code == TYPE_CONST \
	|| (t)->code == TYPE_VOLATILE \
	|| (t)->code == TYPE_RESTRICT)

#define UNQUALIFY_TYPE(t) (QUALIFIED_TYPE(t)?remove_type_qualifier(t):(t))

#define IS_VOLATILE_TYPE(t) ((t)->code == TYPE_VOLATILE)

#define IS_CONST_TYPE(t) ((t)->code == TYPE_CONST)

#define IS_RESTRICT_TYPE(t) ((t)->code == TYPE_RESTRICT)

#define IS_ARRAY_TYPE(t) (UNQUALIFY_TYPE(t)->code == TYPE_ARRARY)

#define IS_RECORD_TYPE(t) (UNQUALIFY_TYPE(t)->code == TYPE_STRUCT \
	|| UNQUALIFY_TYPE(t)->code == TYPE_UNION)

#define IS_STRUCT_TYPE(t) (UNQUALIFY_TYPE(t)->code == TYPE_STRUCT)

#define IS_UNION_TYPE(t) (UNQUALIFY_TYPE(t)->code == TYPE_UNION)

#define IS_FUNCTION_TYPE(t) (UNQUALIFY_TYPE(t)->code == TYPE_FUNCTION)

#define IS_PTR_TYPE(t) (UNQUALIFY_TYPE(t)->code == TYPE_PTR)

#define IS_CHAR_TYPE(t) (UNQUALIFY_TYPE(t)->code == TYPE_CHAR \
	|| UNQUALIFY_TYPE(t)->code == TYPE_UNSIGNED_CHAR)

#define IS_INTEGER_TYPE(t) (UNQUALIFY_TYPE(t)->code <= TYPE_ENUM \
	&& UNQUALIFY_TYPE(t)->code >= TYPE_CHAR)

#define IS_ARITHMETIC_TYPE(t) (UNQUALIFY_TYPE(t)->code <= TYPE_LONGDOUBLE \
	&& UNQUALIFY_TYPE(t)->code >= TYPE_CHAR)

#define IS_ENUM_TYPE(t) (UNQUALIFY_TYPE(t)->code == TYPE_ENUM)

#define IS_VOID_TYPE(t) (UNQUALIFY_TYPE(type)->code == TYPE_VOID)

#define IS_SCALAR_TYPE(t) (UNQUALIFY_TYPE(type)->code <= TYPE_PTR)

/* below for arithmetic type conversion */
#define IS_LONGDOUBLE_TYPE(t) (UNQUALIFY_TYPE(t)->code == TYPE_LONGDOUBLE)

#define IS_DOUBLE_TYPE(t) (UNQUALIFY_TYPE(t)->code == TYPE_DOUBLE)

#define IS_FLOAT_TYPE(t) (UNQUALIFY_TYPE(t)->code == TYPE_FLOAT)

#define IS_UNSIGNED_LONGLONG_TYPE(t) (UNQUALIFY_TYPE(t)->code == TYPE_UNSIGNED_LONGLONG)

#define IS_LONGLONG_TYPE(t) (UNQUALIFY_TYPE(t)->code == TYPE_LONGLONG)

#define IS_UNSIGNED_LONG_TYPE(t) (UNQUALIFY_TYPE(t)->code == TYPE_UNSIGNED_LONG)

#define IS_LONG_TYPE(t) (UNQUALIFY_TYPE(t)->code == TYPE_LONG)

#define IS_UNSIGNED_INT_TYPE(t) (UNQUALIFY_TYPE(t)->code == TYPE_UNSIGNED_INT)

 
/*
 * initialize type system by initializing c build in types and install their on type table
 */
void type_system_initialize();

/*
 * remove types at specified scope level
 */
void remove_types(int level);

/*
 * construct a pointer type whose sub type is input parameter "pointed"
 */
t_type* pointer_type(t_type* pointed);

/*
 * dereference a pointer type
 */
t_type* dereference_type(t_type* type);

/*
 * construct an array type with specified element type and array size
 */
t_type* make_array_type(t_type* type, int size);

/*
 * get the ptr type out of an array type
 * in ANSI C, array can be used as a pointer in some cases
 */
t_type* array_to_ptr_type(t_type* type);

/*
 * qualify specified type with code
 */
t_type* qualify_type(t_type* type, int code);

/*
 * construct a function type
 */
t_type* make_function_type(t_type* type, t_param* parameter, int prototype, int ellipse);

/*
 * construct a record type
 * record_type - struct, union, enum
 * tag - tag of the record. could be null
 * scope - applicable scope for this type
 */
t_type* make_record_type(t_type_kind kind, char* tag, int scope);

/*
 * construct a field type and associate it with specified record type
 */
t_record_field* make_field_type(t_type* field_type, char* name, t_type* record_type);

/*
 * Check type compatibility
 *
 * To determine whether or not an implicit conversion is permissible, ANSI C
 * introduced the concept of compatible types. After promotion, using the appropriate
 * set of promotion rules, two non-pointer types are compatible if they have the same
 * size, signedness, and integer or float characteristic, or, in the case of aggregates, are of
 * the same structure or union type
 * 
 * Two arithmetic types are compatible only if they are the same type.
 *
 * Pointers are compatible if they point to compatible types. No default promotion rules
 * apply to pointers
 */
int is_compatible_type(t_type*, t_type*); 

/*
 * promote type according to default argument promotion rules: 
 * short and char types (whether signed or unsigned) are passed as ints, 
 * other integral quantities are not changed, and floating
 * point quantities are passed as doubles. These rules are also used for arguments in the
 * variable-argument portion of a function whose prototype ends in ellipses (?
 */
t_type* promote_type(t_type* type);

/*
 * check if a function is a function of variable arity
 */
int is_variadic_function(t_type* type);

/*
 * composite a new type from compatible types
 */
t_type* composite_type(t_type* type1, t_type* type2);

/*
 * check if two types have same type qualifier
 * for example, const volatile restrict would be treat the same set of qualifier as restrict volatile const.. etc.
 */ 
int has_same_type_qualifier(t_type* type1, t_type* type2);

/*
 * usual arithmetic converstion
 * calculate the result type based on two operator types (more operators can be recursively apply this)
 * ANSI type conversion rules:
 *
 * (in the order of "precedence")
 * long double -> long double
 * double -> double
 * float -> float
 * 
 * ---- integer promotions -----
 * 
 * unsigned long long -> unsigned long long
 * long long
 * unsigned long
 * long
 * unsigned int
 * otherwise -> int
*/
t_type* arithmetic_conversion(t_type* t1, t_type* t2);


#endif