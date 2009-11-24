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

#include "hcc.h"
#include "type.h"
#include "symbol.h"
#include "assert.h"
#include "arena.h"
#include "atom.h"
#include "error.h"

#define __HCC_TYPE_TABLE_HASHSIZE 512

//
// this table exits simply to provide singleton of types
// it would make sure identical type has only one runtime representation.
//
static struct type_entry
{
	t_type type;
	struct type_entry* next;
}* type_table[__HCC_TYPE_TABLE_HASHSIZE];


static t_type* atomic_type(t_type* type, int code, int align, int size, t_symbol* symbol_link)
{
	struct type_entry* p;
	unsigned long h = (code^((unsigned long)type>>3))& __HCC_TYPE_TABLE_HASHSIZE;

	HCC_ASSERT(code >= 0 && align >= 0 && size >= 0);

	//
	// Here function and zero sized (incomplete) array
	// can't be identified simply by code, align, size, and sub type.
	// So for function and incomplete array always allocate new type for them.
	//
	if (code != TYPE_FUNCTION && (code != TYPE_ARRARY || size > 0))
	{
		for (p = type_table[h]; p; p = p->next)
		{
			if (p->type.code == code && 
				p->type.align == align &&
				p->type.size == size &&
				p->type.link == type &&
				(t_symbol*)p->type.symbolic_link == symbol_link) 
			{
				return &p->type;
			}
		}
	}

	CALLOC(p, PERM);
	p->type.code = code;
	p->type.align = align;
	p->type.size = size;
	p->type.symbolic_link = symbol_link;
	p->next = type_table[h];
	type_table[h] = p;
	
	return &p->type;
}


//
// install a specific type to type symbol table
// and return a runtime representation for the type
//
static t_type* install_type_symbol(int code, char*name, int size, int align)
{
	t_symbol* symbol = add_symbol(name, &sym_table_types, GLOBAL, PERM);
	t_type* type= atomic_type(0, code, align, size, symbol);
	symbol->type = type;

	return type;
}


void type_system_initialize()
{
	type_char = install_type_symbol(TYPE_CHAR, atom_string("char"), HCC_CHAR_SIZE, HCC_CHAR_SIZE);
	
	type_unsigned_char = install_type_symbol(TYPE_UNSIGNED_CHAR, atom_string("unsigned char"), HCC_CHAR_SIZE, HCC_CHAR_SIZE);
	
	type_short = install_type_symbol(TYPE_SHORT, atom_string("short"), HCC_SHORT_SIZE, HCC_SHORT_SIZE);
	
	type_unsigned_short = install_type_symbol(TYPE_UNSIGNED_SHORT, atom_string("unsigned short"), HCC_SHORT_SIZE, HCC_SHORT_SIZE);
	
	type_int = install_type_symbol(TYPE_INT, atom_string("int"), HCC_INT_SIZE, HCC_INT_SIZE);
	
	type_unsigned_int = install_type_symbol(TYPE_UNSIGNED_INT, atom_string("unsigned int"), HCC_INT_SIZE, HCC_INT_SIZE);
	
	type_long = install_type_symbol(TYPE_LONG, atom_string("long"), HCC_LONG_SIZE, HCC_LONG_SIZE);
	
	type_unsigned_long = install_type_symbol(TYPE_UNSIGNED_LONG, atom_string("unsigned long"), HCC_LONG_SIZE, HCC_LONG_SIZE);
	
	type_longlong = install_type_symbol(TYPE_LONGLONG, atom_string("long long"), HCC_LONGLONG_SIZE, HCC_LONGLONG_SIZE);
	
	type_unsigned_longlong = install_type_symbol(TYPE_UNSIGNED_LONGLONG, atom_string("unsigned long long"), HCC_LONGLONG_SIZE, HCC_LONGLONG_SIZE);
	
	type_float = install_type_symbol(TYPE_FLOAT, atom_string("float"), HCC_FLOAT_SIZE, HCC_FLOAT_SIZE);
	
	type_double = install_type_symbol(TYPE_DOUBLE, atom_string("double"), HCC_DOUBLE_SIZE, HCC_DOUBLE_SIZE);
	
	type_longdouble = install_type_symbol(TYPE_LONGDOUBLE, atom_string("long double"), HCC_LONG_DOUBLE_SIZE, HCC_LONG_DOUBLE_SIZE);
	
	type_ptr = install_type_symbol(TYPE_PTR, atom_string("T*"), HCC_PTR_SIZE, HCC_PTR_SIZE);
	
	type_void = install_type_symbol(TYPE_VOID, atom_string("void"), 0, 0);
}


void remove_types(int level)
{
	int i = 0;

	for (i; i < __HCC_TYPE_TABLE_HASHSIZE; i ++)
	{
		struct type_entry* current_entry = NULL;
		struct type_entry** bucket = &type_table[i];

		for (;;)
		{
			current_entry = *bucket;
			if (current_entry == NULL)
			{
				break;
			}
			else if (current_entry->type.code != TYPE_FUNCTION &&
				((t_symbol*)current_entry->type.symbolic_link)->scope >= level)
			{
				// heading next item in the hash chain
				*bucket = current_entry->next;
			}
			else
			{
				// delete current item from hash chain
				bucket = &current_entry->next;
			}
		}
	}

	// [TODO] - possible optimizations here to avoid iterating hash table in some cases.
} 


t_type* pointer_type(t_type* pointed)
{
	return atomic_type(pointed, TYPE_PTR, HCC_PTR_SIZE, HCC_PTR_SIZE, type_ptr->symbolic_link); 
}

t_type* dereference_type(t_type* type)
{
	HCC_ASSERT(type != NULL);

	if (IS_PTR_TYPE(type))
	{
		type = type->link;
	}
	else
	{
		type_error("expect pointer type : dereference can only be applied to a pointer type.");
	}

	// for pointer to enum need to get the unqualified member type
	// typically this is int
	if (IS_ENUM_TYPE(type))
	{
		type = UNQUALIFY_TYPE(type)->link;
	}

	return type;
}


t_type* make_array_type(t_type* type, int size)
{
	HCC_ASSERT(type != NULL);
	HCC_ASSERT(size >= 0);
	
	if (IS_FUNCTION_TYPE(type))
	{
		type_error("illegal type usage : ANSI C doesn't have function array");
		return NULL;
	}

	if (IS_VOID_TYPE(type))
	{
		type_error("illegal type usage : array elements can't be type void");
		return NULL;
	}

	if (INT_MAX/type->size < size)
	{
		type_error("illegal array type : too many elements");
		return NULL;
	}

	return atomic_type(type, TYPE_ARRARY, type->align, size * type->size, NULL); // array type has no symbolic link to symbol table
}

t_type* array_to_ptr_type(t_type* type)
{
	HCC_ASSERT(type != NULL);

	if (IS_ARRAY_TYPE(type))
	{
		return pointer_type(type->link);
	}

	return NULL;
}


t_type* remove_type_qualifier(t_type* type)
{
	while(type != NULL)
	{ 
		if(type->code == TYPE_CONST 
			|| type->code == TYPE_VOLATILE 
			|| type->code == TYPE_RESTRICT) 
		{ 
			type = type->link;
		} 
		else 
		{ 
			break;
		} 
	}

	return type;
}

t_type* qualify_type(t_type* type, int code)
{
	HCC_ASSERT(IS_TYPE_QUALIFIERS(code));
	HCC_ASSERT(type != NULL);

	if ((IS_CONST_TYPE(type) && code == TYPE_CONST) ||
		(IS_VOLATILE_TYPE(type) && code == TYPE_VOLATILE) ||
		(IS_RESTRICT_TYPE(type) && code == TYPE_RESTRICT))
	{
		type_error("illegal type qualifer usage: duplicate type qualifier");
		return NULL;
	}

	if (IS_ARRAY_TYPE(type))
	{
		type = atomic_type(qualify_type(type, code), TYPE_ARRARY, type->align, type->size, NULL); // array has no symbolic link to symbol table
	}
	else
	{
		type = atomic_type(type, code, type->align, type->size, NULL); // [TODO] - null symbolic type link
	}

	return type;
}


t_type* make_function_type(t_type* type, t_param* parameter, int prototype, int ellipse)
{
	t_function* function = NULL;

	HCC_ASSERT(type != NULL && parameter != NULL && prototype >= 0 && ellipse >= 0);

	if (IS_ARRAY_TYPE(type) || IS_FUNCTION_TYPE(type))
	{
		// function can't have either array or function as its return type
		type_error("illegal function return type");
		return NULL;
	}

	// [TODO] function type align and size ; currently it's the same with ptr 
	type = atomic_type(type, TYPE_FUNCTION, type_ptr->align, type_ptr->size, NULL);

	CALLOC(function, sizeof *function);
	function->ellipse = ellipse;
	function->prototype = prototype;
	function->parameter = parameter;
	
	type->u.function = function;

	return type;
}


t_type* make_record_type(int record_type, char* name)
{
    t_symbol* symbol = NULL;
    t_record* record = NULL;
    static int a = 0; // for anonymous record hcc generate the name

    HCC_ASSERT(record_type == TYPE_ENUM || record_type == TYPE_STRUCT || record_type == TYPE_UNION);

    if (name == NULL)
    {
        name = atom_int(a ++);
    }
    else
    {
        symbol = find_symbol(name, sym_table_types);
        if (symbol != NULL)
        {
            // this implicitly means a new record type is only created in current scope (scope_level)
            if (symbol->scope == scope_level || symbol->scope == PARAM && scope_level == PARAM + 1)
            {
                if (symbol->type->code == record_type && !symbol->defined)
                {
                    return symbol->type;               
                }
                else if (symbol->type->code == record_type && symbol->defined)
                {
                    type_error("\'record\' type redefinition");
                    return NULL;
                }
            }
        }
    }

    // after cross fire either the type is missing or not satisfy our needs
    // so create a new one and set up links with sym table
    // the fields of record will be added later and size/align will be adjusted accordingly.
    symbol = add_symbol(name, &sym_table_types, scope_level, PERM);
    symbol->type = atomic_type(NULL, record_type, 0, 0, symbol); // a new record type has align 0 and size 0
    
    // bind with record 
    CALLOC(record, sizeof *record);
    record->name = name;
    record->fields = NULL;
    symbol->type->u.record = record;
    
    return symbol->type;
}


t_field* make_field_type(t_type* field_type, char* name, t_type* record_type)
{
    t_field* current = NULL;
    t_field** next = NULL;

    // [TODO] - support unnamed bit field ; at this moment field requires a name
    HCC_ASSERT(field_type != NULL && name != NULL && record_type != NULL);

    next = &record_type->u.record->fields;
    current = *next;

    for (; current; next = &current->next, current = *next)
    {
        if (current->name == name) type_error("duplicate field name");
    }

    CALLOC(current, sizeof *current);
    current->name = name;
    current->type = field_type;
    current->bits = 0;
    current->offset = 0;
    current->next = NULL;

    return current;
}


static int is_compatible_function(t_type* type1, t_type* type2)
{
    t_param* p1 = NULL;
    t_param* p2 = NULL;

    HCC_ASSERT(type1 != NULL && type2 != NULL && 
        type1->code == TYPE_FUNCTION && 
        type2->code == TYPE_FUNCTION &&
        type1->u.function != NULL &&
        type2->u.function != NULL);

    // incompatible return type
    if (!is_compatible_type(type1->link, type2->link))
    {
        return 0;
    }
    
    // both functions have no prototype
    // [TODO] they are compatible??
    if (!type1->u.function->prototype && !type2->u.function->prototype)
    {
        return 1;
    }

    p1 = type1->u.function->parameter;
    p2 = type2->u.function->parameter;

    if (p1 && p2)
    {
        // both have prototype - iterate and check
        // [TODO] stupid cast
        for (; p1 && p2; p1 = (t_param*)p1->next, p2 = (t_param*)p2->next)
        {
            if (!is_compatible_type(p1->type, p2->type))
            {
                return 0;
            }
        }

        return 1;

    }
    else if (!p1 && !p2)
    {
        // both have no prototype - implicitly compatible?? [TODO]
        return 1;
    }
    else
    {
        if (is_variadic_function(p1 ? type1 : type2))
        {
            return 0;
        }

        // either p1 or p2 has to have prototype when the flow hits here..
        if (!p1)
        {
            p1 = p2;
        }

        for (; p1; p1 = (t_param*)p1->next)
        {
            type1 = UNQUALIFY_TYPE(p1->type);
            if (promote_type(type1) != type1)
            {
                return 0;
            }
        }

        return 1;
    }
}


int is_compatible_type(t_type* type1, t_type* type2)
{
    if (type1 == type2)
	{
		return 1;
	}
    
    type1 = UNQUALIFY_TYPE(type1);
    type2 = UNQUALIFY_TYPE(type2);

	if (type1->code != type2->code)
	{
		return 0;
	}

	if (type1->code == TYPE_PTR)
	{
		return is_compatible_type(type1->link, type2->link);
	}
	else if (type1->code == TYPE_ARRARY)
	{
		return is_compatible_type(type1->link, type2->link) &&
			(type1->size == type2->size || type1->size == 0 || type2->size == 0);
	}
	else if (type1->code == TYPE_FUNCTION)
	{
		return is_compatible_function(type1, type2);
	}

    return 0;
}

t_type* promote_type(t_type* type)
{
    HCC_ASSERT(type != NULL);
    
    if (type->code <= TYPE_UNSIGNED_SHORT)
    {
        return type_int;
    }
    else if (type->code == TYPE_FLOAT)
    {
        return type_double;
    }

    return type;
}

int is_variadic_function(t_type* type)
{
    // [TODO] here we need to make sure the ellipse flag is properly set
    if (IS_FUNCTION_TYPE(type) && type->u.function->ellipse)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


t_type* composite_type(t_type* type1, t_type* type2)
{
    (type1);
    (type2);
    return NULL;
}