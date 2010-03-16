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

/* Semantic check for declarations - implementations */
static void sc_declaration_specifiers(t_ast_declaration_specifier* spec)
{   
    t_ast_list* qualifiers;
    t_ast_list* specifiers;
    t_ast_type_qualifier* q;
    t_ast_type_specifier* s;
    int f = 0; /* qualifier flag */
    int g = 0; /* type specifier flag */
	int long_long = 0; /* long long type as a special case */

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
				if ( (g & (1 << ntype)) != 0)
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

                break;
            }
        case AST_TYPE_SPECIFIER_STRUCT_OR_UNION:
            {
                break;
            }
        case AST_TYPE_SPECIFIER_TYPEDEF:
            {
                break;
            }
        case AST_TYPE_SPECIFIER_ENUM:
            {
                break;
            }
        default:
            break;
        }  
    } /* end iteration */

	if (long_long)
	{
		spec->type = type_longlong;
	}

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

