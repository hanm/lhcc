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
#include "error.h"

/* Semantic check for declarations - prototypes */
static void sc_declaration_specifiers(t_ast_declaration_specifier* spec);
static void sc_outer_declaration(t_ast_declaration* declr);
/*
 * semantic check for type specifier list (composed of a chain of native types)
 * return the composed type if everything conforms to spec, or signal semantic error
 *
 * param mask the bit mask represent type specifier list
 * param coord coordinate for firing logging
 * param unsign/long_long two special flags indicates the type is signed/unsigned or long_long
*/
static t_type* sc_native_type_specifiers(int mask, t_ast_coord* coord, int unsign, int long_long);

void semantic_check(t_ast_translation_unit* translation_unit)
{
    t_ast_list *ext_declr_list;
	t_ast_external_declaration* ext_declr;

    assert(translation_unit);

    ext_declr_list = translation_unit->ext_declaration_list;

    while(!HCC_AST_LIST_IS_END(ext_declr_list))
    {
		ext_declr = ext_declr_list->item;
        ext_declr_list = ext_declr_list->next;

		if (ext_declr->fun_def)
		{
			/* check function */
		}
		else
		{
			sc_outer_declaration(ext_declr->u.declr);
		}
    }
}

/* Semantic check for declarations - implementations */
static void sc_declaration_specifiers(t_ast_declaration_specifier* spec)
{   
    t_ast_list* qualifiers;
    t_ast_list* specifiers;
    t_ast_type_qualifier* q;
    t_ast_type_specifier* s;
    int f = 0; /* qualifier flag */
    int g = 0; /* bit mask for type specifier list */
    int h = 0; /* bit mask for different kinds of type specifiers 
                    * from MSB to LSB: native type, struct, enum, typedef.
                   */
	int long_long = 0; /* long long type as a special case */
	int unsign = 0;

    assert(spec);   

    qualifiers = spec->type_qualifier_list;
    while(!HCC_AST_LIST_IS_END(qualifiers))
    {
        q = (t_ast_type_qualifier*)qualifiers->item;
        qualifiers = qualifiers->next;

        if (q->kind == AST_TYPE_CONST && !(f & 0x01))
        {
            f |= 0x01;
        }
        else if (q->kind == AST_TYPE_VOLATILE && !(f & 0x10))
        {
            f |= 0x10;
        }
		else
		{
			semantic_warning("duplicate type qualifiers are ignored!", &q->coord);
		}
    }

    specifiers = spec->type_specifier_list;
    while(!HCC_AST_LIST_IS_END(specifiers))
    {
        s = (t_ast_type_specifier*)specifiers->item; 
        specifiers = specifiers->next;

        switch (s->kind)
        {
        case AST_TYPE_SPECIFIER_NATIVE:
            {
				int ntype = s->u.native_type;
				if ((g & (1 << ntype)))
				{
					if (ntype <= AST_NTYPE_SHORT || ntype == AST_NTYPE_INT64)
					{
						semantic_error("duplicate type specifier is illegal", &s->coord);
					}
					else if (ntype == AST_NTYPE_SIGNED || ntype == AST_NTYPE_UNSIGNED)
					{
						semantic_warning("duplicate signedness specifier is ignored", &s->coord);
					}
					else if (ntype == AST_NTYPE_LONG)
					{
						g &= ~ (1 << AST_NTYPE_LONG);
						if (long_long)
						{
							semantic_error("too many long long specifier", &s->coord);
						}
						long_long = 1;
					}
				}
				else
				{
					g |= (1 << s->u.native_type);
				}

                h |= 0x08;
                break;
            }
        case AST_TYPE_SPECIFIER_STRUCT_OR_UNION:
            {
                h |= 0x04;
                break;
            }
        case AST_TYPE_SPECIFIER_ENUM:
            {
                h |= 0x02;
                break;
            }
        case AST_TYPE_SPECIFIER_TYPEDEF:
            {
                t_symbol* sym = find_symbol(s->u.type_def, sym_table_identifiers);
                assert(sym && sym->storage == TK_TYPEDEF);
                spec->type = sym->type;
                assert(!sym->type); /* TODO - this validation should be reverted later. */

                h |= 0x01;
                break;
            }
        default:
            break;
        }  
    }

    if ((h & (h -1)))
    {
        semantic_error("illegal type usage detected - likely you mixed use typedef/enum/struct/union/other types together", &spec->coord);
        return;
    }
    else if ( h != 8)
    {
        /* enum, struct/union, typedef already validates their types in specific sub routines */
        return;
    }

	if ((g & ( 1 << AST_NTYPE_UNSIGNED)) && (g & ( 1 << AST_NTYPE_SIGNED)))
	{
		semantic_error("unsigned and signed can't be mixed used together", &spec->coord);
	}
	else if ( g & ( 1 << AST_NTYPE_UNSIGNED))
	{
		unsign = 1;
	}

    spec->type = sc_native_type_specifiers(g, &spec->coord, unsign, long_long);
}

/* http://www.mers.byu.edu/docs/standardC/declare.html */

/* specification of declaration checking */
/*
Outer Declaration

You write an outer declaration as one of the declarations that make up a translation unit. 
An outer declaration is one that is not contained within another declaration or function definition:
You can omit the declarator only for a structure, union, or enumeration declaration that declares a tag. 
You must write a name within the declarator of any other outer declarator.
Variable array can't be declared externally (at file scope)

Storage specifiers : extern, static, typdef (auto, and register is forbidden)
Storage specifiers are omittable (implicit external declaration assumed extern)

Type specifiers are omittable (implicit assumed as int)
*/

/*
 semantic check for global declarations
 */
static void sc_outer_declaration(t_ast_declaration* declr)
{
    t_ast_declaration_specifier* specifiers;
    t_ast_storage_specifier* storage_specifier;
    
    assert(declr);

    specifiers = declr->declr_specifiers;
    storage_specifier = specifiers->storage_specifier;

    if (storage_specifier && 
        (storage_specifier->kind == AST_STORAGE_AUTO ||
            specifiers->storage_specifier->kind == AST_STORAGE_REGISTER))
    {
        semantic_error("global declaration can't have register or auto storage specifier!", &specifiers->coord);
    }    

	sc_declaration_specifiers(specifiers);
}



static t_type* sc_native_type_specifiers(int mask, t_ast_coord* coord, int unsign, int long_long)
{
      /* list of possible type specifiers from C99 standard.
		void
		char
		signed char
		unsigned char
		short, signed short, short int, or signed short int
		unsigned short, or unsigned short int
		int, signed, or signed int
		unsigned, or unsigned int
		long, signed long, long int, or signed long int
		unsigned long, or unsigned long int
		long long, signed long long, long long int, or
		signed long long int
		unsigned long long, or unsigned long long int
		float
		double
		long double
		_Bool
		float _Complex
		double _Complex
		long double _Complex
		struct or union specifier *
		enum specifier
		typedef name
    */

    t_type* type;

    /* void */
    if (( mask & (1 << AST_NTYPE_VOID)))
    {
        if (mask & ~(1 << AST_NTYPE_VOID))
        {
            semantic_error("void can't be used with other types", coord);
        }
        else
        {
            type = type_void;
        }
    }
     /* char, signed char, unsigned char */
    else if ( (mask & ( 1 << AST_NTYPE_CHAR)))
    {
        int t = mask & ~(1 << AST_NTYPE_CHAR);
        if (t)
        {
            if ( ( t & ( 1 << AST_NTYPE_SIGNED))  && !( t & ~( 1 << AST_NTYPE_SIGNED)))
            {
                type = type_signed_char;
            }
            else if ( (t & ( 1<< AST_NTYPE_UNSIGNED)) && !( t & ~( 1 << AST_NTYPE_UNSIGNED)))
            {
                type = type_unsigned_char;
            }
            else
            {
                semantic_error("char can only be decorated by signed or unsigned", coord);
            }
        }
        else
        {
            type = type_char;
        }
    }
    /* short, signed short, short int, or signed short int */
	/* unsigned short, unsigned short int */
    else if ( (mask & ( 1 << AST_NTYPE_SHORT)))
    {
		int m = (1 << AST_NTYPE_SIGNED) | 
				(1 << AST_NTYPE_UNSIGNED) | 
				(1 << AST_NTYPE_INT) | 
				(1 << AST_NTYPE_SHORT);
		
		if ( mask & ~m)
		{
			semantic_error("illegal short type", coord);
		}
		else
		{
			type = unsign ? type_unsigned_short : type_short;
		}
	}
    /* all kinds of long :)

        long, signed long, long int, or signed long int
        unsigned long, or unsigned long int
        long long, signed long long, long long int, or
        signed long long int
        unsigned long long, or unsigned long long int

        and long double :)
    */
    else if ( (mask & ( 1 << AST_NTYPE_LONG)))
    {
        int m = (1 << AST_NTYPE_SIGNED) | 
				(1 << AST_NTYPE_UNSIGNED) | 
				(1 << AST_NTYPE_INT) | 
                (1 << AST_NTYPE_LONG);

        int n = (1 << AST_NTYPE_LONG) | 
            (1 << AST_NTYPE_DOUBLE);

        if ( mask & ~m)
        {
            if ( mask & ~n)
            {
                semantic_error("illegal long type", coord);
            }
            
            if ( mask & (1 << AST_NTYPE_DOUBLE))
            {
                type = type_longdouble;
            }
        }
        else
        {
            if (unsign)
            {
                if (long_long)
                {
                    type = type_unsigned_longlong;
                }
                else
                {
                    type = type_unsigned_long;
                }
            }
            else
            {
                if (long_long)
                {
                    type = type_longlong;
                }
                else
                {
                    type = type_long;
                }
            }
        }
    } /* end various long long*/
	/*
	  unsigned int, signed int, int
	*/
	else if(mask & ( 1 << AST_NTYPE_INT))
	{
		int m = (1 << AST_NTYPE_SIGNED) | 
				(1 << AST_NTYPE_UNSIGNED) | 
				(1 << AST_NTYPE_INT);

		if (mask & ~m)
		{
			semantic_error("illegal int type", coord);
		}
		else
		{
			type = unsign ? type_unsigned_int : type_int;
		}
	}
	/* float */
	else if(mask & ( 1 << AST_NTYPE_FLOAT))
	{
		if (mask & ~( 1 << AST_NTYPE_FLOAT))
		{
			semantic_error("illegal float type", coord);
		}
		else
		{
			type = type_float;	
		}
	}
	else if(mask & ( 1 << AST_NTYPE_DOUBLE))
	{
		if (mask & ~( 1 << AST_NTYPE_DOUBLE))
		{
			semantic_error("illegal double type", coord);
		}
		else
		{
			type = type_double;	
		}
	}
    else if (mask & ( 1 << AST_NTYPE_INT64)) /* [WARNING] non std extension - int64 type is not part of C99 or C90/95*/
    {
        int m = ( 1 << AST_NTYPE_INT64) |
            ( 1 << AST_NTYPE_UNSIGNED) |
            ( 1 << AST_NTYPE_SIGNED);

        if (mask & ~m)
        {
            semantic_error("illegal int64 type usage", coord);
        }
        else
        {
            type = unsign ? type_unsigned_int64 : type_int64;
        }
    }
	else
	{
		int m = (1 << AST_NTYPE_SIGNED) |
				(1 << AST_NTYPE_UNSIGNED);

		if (mask & ~m)
		{
			semantic_error("unrecognized type", coord);
            type = type_int;
		}
		else
		{
			type = unsign ? type_unsigned_int : type_int;
		}
	}

    return type;
}