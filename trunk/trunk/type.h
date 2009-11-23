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

#ifndef __HCC_TYPE_H
#define __HCC_TYPE_H

//
// ANSI C build in types and type qualifiers
// 
enum 
{
	TYPE_CHAR, // signed char
	TYPE_UNSIGNED_CHAR, // unsigned char
	TYPE_SHORT, // short, signed short, short int, signed short int
	TYPE_UNSIGNED_SHORT, // unsigned short, unsigned short int
	TYPE_INT, // int, signed int, or no type specifiers
	TYPE_UNSIGNED_INT, // unsigned, unsigned int
	TYPE_LONG, // long, signed long, long int, signed long int
	TYPE_UNSIGNED_LONG, // unsigned long, unsigned long int
	TYPE_LONGLONG, // [TODO] - WARNING : long long is not ANSI C type
	TYPE_UNSIGNED_LONGLONG, // AS ABOVE
	TYPE_ENUM, // enum
	TYPE_FLOAT, // float
	TYPE_DOUBLE, // double
	TYPE_LONGDOUBLE, // long double
	TYPE_PTR, // pointer
	TYPE_VOID, // void
	TYPE_STRUCT, // struct
	TYPE_UNION, // union
	TYPE_FUNCTION, // function
	TYPE_ARRARY, // array
	
	// type qualifiers
	TYPE_CONST, // constant
	TYPE_VOLATILE, // volatile
	TYPE_RESTRICT // restrict (C99)
};

//
// Type
//
typedef struct type
{
	int code; // type code 
	int align; // type alignment
	int size; // type size

	// link to "sub types" - c type system basically have two kinds of types
	// one is built in types like int, long, char, float, etc
	// the other is composed types or user defined types like typedef, struct, union, etc
	// the link field here enable type struct to extends itself linearly.
	struct type* link; 

	// point to the symbol table entry for the type
	// occasionally type needs to access the symbol table information like scope level
	// etc. this field sets up the link between them.
	void* symbolic_link;

	//
	// record/enum/function types
	//
	union  
	{
		struct record_type* record;
		struct enum_type* enums;
		struct function_type* function;
	} u;

} t_type;


//
// Member in struct or union
//
typedef struct field_type
{
	char* name; // field name.
	int offset; // field offset relative to start of the record.
	int bits; // number of bits if the field is a bit field; otherwise 0 by default.
	t_type* type; // field type.
	struct field_type* next; // link to next field in the same record.
} t_field;

//
// Record type - struct or union
//
typedef struct record_type
{
	char* name; // record name
	t_field* fields; // members in the record
} t_record;

//
// enumeration type
//
typedef struct enum_type
{
	char* name; // [TODO] - prove me...
} t_enum;

//
// parameter type
//
typedef struct paremeter_type
{
	char* name;
	t_type* type;
	int reg_qualified;

	struct parameter_type* next;
} t_param;

//
// function type
//
typedef struct function_type
{
	int prototype;
	int ellipse;

	t_param* parameter;
} t_function;

//
// build in types
//
t_type* type_char;
t_type* type_unsigned_char;
t_type* type_short;
t_type* type_unsigned_short;
t_type* type_int;
t_type* type_unsigned_int;
t_type* type_long;
t_type* type_unsigned_long;
t_type* type_longlong;
t_type* type_unsigned_longlong;
t_type* type_float;
t_type* type_double;
t_type* type_longdouble;
t_type* type_ptr;
t_type* type_void;

//
// remove the type qualifier for specified type
//
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

//
// initialize type system by initializing c build in types and install their on type symbol table
//
void type_system_initialize();

//
// remove types at specified scope level
//
void remove_types(int level);

//
// construct a pointer type whose sub type is input parameter "pointed"
//
t_type* pointer_type(t_type* pointed);

//
// dereference a pointer type
//
t_type* dereference_type(t_type* type);

//
// construct an array type with specified element type and array size
//
t_type* make_array_type(t_type* type, int size);

//
// get the ptr type out of an array type
// in ANSI C, array can be used as a pointer in some cases
//
t_type* array_to_ptr_type(t_type* type);

//
// qualify specified type with code
//
t_type* qualify_type(t_type* type, int code);

//
// construct a function type
//
t_type* make_function_type(t_type* type, t_param* parameter, int prototype, int ellipse);

//
// construct a record type
// record_type - type of record could be one of enum, struct, union
// name - name of struct/union/enum, could be null (anonymous struct/union)
//
t_type* make_record_type(int record_type, char* name);

//
// construct a field type and associate it with specified record type
//
t_field* make_field_type(t_type* field_type, char* name, t_type* record_type);

//
// check type compatibility
// 
int is_compatible_type(t_type*, t_type*); 

#endif