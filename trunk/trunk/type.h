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
// ANSI C build in types
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
	TYPE_FLOAT, // float
	TYPE_DOUBLE, // double
	TYPE_LONGDOUBLE, // long double
	TYPE_PTR, // pointer
	TYPE_VOID, // void
	TYPE_STRUCT, // struct
	TYPE_UNION, // union
	TYPE_FUNCTION, // function
	TYPE_ARRARY, // array
	TYPE_ENUM // enum
};

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
	struct field* next; // link to next field in the same record.
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
} t_param;

//
// function type
//
typedef struct function_type
{
	int prototype;
	int ellipse;
	t_param* param[1];
} t_function;

int type_equal(t_type* type_a, t_type* type_b);

#endif